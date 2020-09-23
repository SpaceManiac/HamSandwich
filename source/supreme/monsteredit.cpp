#include "monsteredit.h"
#include "dialogbits.h"
#include "filedialog.h"
#include "items.h"
#include "monsnotes.h"
#include "tool.h"
#include "edithelp.h"
#include <vector>

#define MMODE_NORMAL	0	// doing nothing (just viewing monsters, click to select, select all you like)
#define MMODE_SELECT	1	// selecting a monster (pick one and you're done)
#define MMODE_HELP		2

#define ID_EXIT		1
#define ID_MOREMONS 2
#define ID_SURPRISE 3
#define ID_GROUPS	150
#define ID_PICKTHEME 200
#define ID_PICKGROUP 1200
#define ID_PICKCOLOR 1300
#define ID_PICKSIZE 1400
#define ID_PICKMONS  500
#define ID_MORETHEME 800

static byte mode,curTheme,curGroup,curColor;
static world_t *world;
static dword curMons;
static byte rememberMode;
static byte helpRemember;
static byte themePage;
static byte targetSizeMin=0;
static byte targetSizeMax=255;

static dword themes[]={MT_GOOD,MT_EVIL,MT_FOREST,MT_DESERT,MT_ARCTIC,MT_CAVES,MT_SPACE,
					   MT_MANSION,MT_ISLAND,MT_URBAN,MT_UNDERSEA,MT_ASYLUM,MT_TRAP,MT_HUMAN,MT_ANIMAL,
					   MT_VEGGIE,MT_PUMPKIN,MT_ZOMBIE,MT_SKELETON,MT_VAMPIRE,MT_ZOID,
					   MT_ALIEN,MT_GENERATE,MT_VEHICLE,MT_FLYING,MT_AQUATIC,MT_SPIDER,
					   MT_BITS,MT_BOSS,MT_MINIBOSS,MT_WACKY};
static dword groups[]={MG_ALL,MG_DRL,MG_EXPANDO,MG_FUN,MG_SUPREME,MG_MYSTIC,MG_HOLLOW,MG_LOONY,MG_NEW};
static dword colors[]={MC_ALL,MC_GREY,MC_GREEN,MC_BROWN,MC_BLUE,MC_RED,MC_YELLOW,MC_VIOLET,MC_AQUA,MC_GLOW};

static std::vector<dword> monsList;
static word monsInList,monsStart,monsShown;
static byte realClick;

static dword saveCurMons=1,saveCurTheme=0,saveCurGroup=0,saveCurColor=0;

static void SetupMonsterDisplay(void);
static void MakeMonsterList(void);

static void ExitClick(int id)
{
	PurgeMonsterSprites();
	SetEditMode(EDITMODE_EDIT);
	MakeNormalSound(SND_MENUCLICK);
	PickedTile(-1);
	SetEditMode(rememberMode);
	saveCurMons=curMons;
	saveCurTheme=curTheme;
	saveCurGroup=curGroup;
	saveCurColor=curColor;
}

static void PickMonsterClick(int id)
{
	PurgeMonsterSprites();
	RadioOn(id,ID_PICKMONS,ID_PICKMONS+50);
	curMons=monsList[monsStart+(id-ID_PICKMONS)];
	SetupMonsterDisplay();
	if(realClick)
	{
		PurgeMonsterSprites();
		SetEditMode(rememberMode);
		PickedTile(curMons);
		MakeNormalSound(SND_MENUCLICK);
		saveCurMons=curMons;
		saveCurTheme=curTheme;
		saveCurGroup=curGroup;
		saveCurColor=curColor;
	}
}

static void SurpriseClick(int id)
{
	PurgeMonsterSprites();
	curMons=Random(NUM_MONSTERS);
	SetupMonsterDisplay();
	if(realClick)
	{
		PurgeMonsterSprites();
		SetEditMode(rememberMode);
		PickedTile(curMons);
		MakeNormalSound(SND_MENUCLICK);
		saveCurMons=curMons;
		saveCurTheme=curTheme;
		saveCurGroup=curGroup;
		saveCurColor=curColor;
	}
}

static void MoreMonstersClick(int id)
{
	int i,pos;

	monsStart+=monsShown;
	if(monsStart>=monsInList)
		monsStart=0;

	ClearButtons(ID_PICKMONS,ID_PICKMONS+50);
	ClearButtons(ID_MOREMONS,ID_MOREMONS);

	// now make buttons for the first N, which is however many fit on the screen
	pos=130;
	monsShown=0;
	for(i=0;i<monsInList-monsStart;i++)
	{
		MakeButton(BTN_RADIO,ID_PICKMONS+i,0,2,pos,156,16,MonsterName(monsList[i+monsStart]),PickMonsterClick);
		monsShown++;
		pos+=18;
		if(pos>480-30)
			break;
	}
	MakeButton(BTN_NORMAL,ID_MOREMONS,0,2,pos,156,16,"More Monsters...",MoreMonstersClick);
}


static void PickThemeClick(int id)
{
	RadioOn(id,ID_PICKTHEME,ID_PICKTHEME+50);
	curTheme=id-ID_PICKTHEME;
	MakeMonsterList();
}

static void PickGroupClick(int id)
{
	RadioOn(id,ID_PICKGROUP,ID_PICKGROUP+50);
	curGroup=id-ID_PICKGROUP;
	MakeMonsterList();
}

static void PickColorClick(int id)
{
	RadioOn(id,ID_PICKCOLOR,ID_PICKCOLOR+50);
	curColor=id-ID_PICKCOLOR;
	MakeMonsterList();
}

static void PickSizeClick(int id)
{
	RadioOn(id,ID_PICKSIZE,ID_PICKSIZE+50);
	if(id-ID_PICKSIZE==1)
	{
		targetSizeMin=0;
		targetSizeMax=8;
	}
	else if(id-ID_PICKSIZE==2)
	{
		targetSizeMin=9;
		targetSizeMax=23;
	}
	else if(id-ID_PICKSIZE==3)
	{
		targetSizeMin=24;
		targetSizeMax=39;
	}
	else if(id-ID_PICKSIZE==4)
	{
		targetSizeMin=40;
		targetSizeMax=79;
	}
	else if(id-ID_PICKSIZE==5)
	{
		targetSizeMin=80;
		targetSizeMax=255;
	}
	else
	{
		targetSizeMin=0;
		targetSizeMax=255;
	}
	MakeMonsterList();
}

static void MoreThemesClick(int id)
{
	ClearButtons(ID_PICKTHEME,ID_PICKTHEME+50);
	ClearButtons(ID_PICKGROUP,ID_PICKGROUP+50);
	ClearButtons(ID_PICKCOLOR,ID_PICKCOLOR+50);
	ClearButtons(ID_PICKSIZE,ID_PICKSIZE+50);
	themePage = 1-themePage;
	
	if(themePage==1)
	{
		MakeButton(BTN_RADIO,ID_PICKTHEME+28,0,2+158*0,2+18*0,156,16,"Bosses",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+29,0,2+158*1,2+18*0,156,16,"Mini-Bosses",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+30,0,2+158*2,2+18*0,156,16,"Wacky?!?",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKGROUP+0,0,2+158*3,2+18*0,156,16,"All Groups",PickGroupClick);
		MakeButton(BTN_RADIO,ID_PICKGROUP+1,0,2+158*0,2+18*1,156,16,"Dr. Lunatic",PickGroupClick);
		MakeButton(BTN_RADIO,ID_PICKGROUP+2,0,2+158*1,2+18*1,156,16,"Expando Pak",PickGroupClick);
		MakeButton(BTN_RADIO,ID_PICKGROUP+3,0,2+158*2,2+18*1,156,16,"Fun Pack",PickGroupClick);
		MakeButton(BTN_RADIO,ID_PICKGROUP+4,0,2+158*3,2+18*1,156,16,"Supreme",PickGroupClick);
		MakeButton(BTN_RADIO,ID_PICKGROUP+5,0,2+158*0,2+18*2,156,16,"Kid Mystic",PickGroupClick);
		MakeButton(BTN_RADIO,ID_PICKGROUP+6,0,2+158*1,2+18*2,156,16,"Sleepless Hollow",PickGroupClick);
		MakeButton(BTN_RADIO,ID_PICKGROUP+7,0,2+158*2,2+18*2,156,16,"Loonyland",PickGroupClick);
		MakeButton(BTN_RADIO,ID_PICKGROUP+8,0,2+158*3,2+18*2,156,16,"V8 Exclusive",PickGroupClick);
		MakeButton(BTN_RADIO,ID_PICKCOLOR+0,0,2+158*0,2+18*3,156,16,"All Colors",PickColorClick);
		MakeButton(BTN_RADIO,ID_PICKCOLOR+1,0,2+158*1,2+18*3,156,16,"Grey",PickColorClick);
		MakeButton(BTN_RADIO,ID_PICKCOLOR+2,0,2+158*2,2+18*3,156,16,"Green",PickColorClick);
		MakeButton(BTN_RADIO,ID_PICKCOLOR+3,0,2+158*3,2+18*3,156,16,"Brown",PickColorClick);
		MakeButton(BTN_RADIO,ID_PICKCOLOR+4,0,2+158*0,2+18*4,156,16,"Blue",PickColorClick);
		MakeButton(BTN_RADIO,ID_PICKCOLOR+5,0,2+158*1,2+18*4,156,16,"Red",PickColorClick);
		MakeButton(BTN_RADIO,ID_PICKCOLOR+6,0,2+158*2,2+18*4,156,16,"Yellow",PickColorClick);
		MakeButton(BTN_RADIO,ID_PICKCOLOR+7,0,2+158*3,2+18*4,156,16,"Violet",PickColorClick);
		MakeButton(BTN_RADIO,ID_PICKCOLOR+8,0,2+158*0,2+18*5,156,16,"Aqua",PickColorClick);
		MakeButton(BTN_RADIO,ID_PICKCOLOR+9,0,2+158*1,2+18*5,156,16,"Glowing",PickColorClick);
		MakeButton(BTN_RADIO,ID_PICKSIZE+0,0,2+158*2,2+18*5,156,16,"All Sizes",PickSizeClick);
		MakeButton(BTN_RADIO,ID_PICKSIZE+1,0,2+158*3,2+18*5,156,16,"Tiny",PickSizeClick);
		MakeButton(BTN_RADIO,ID_PICKSIZE+2,0,2+158*0,2+18*6,156,16,"Small",PickSizeClick);
		MakeButton(BTN_RADIO,ID_PICKSIZE+3,0,2+158*1,2+18*6,156,16,"Medium",PickSizeClick);
		MakeButton(BTN_RADIO,ID_PICKSIZE+4,0,2+158*2,2+18*6,156,16,"Large",PickSizeClick);
		MakeButton(BTN_RADIO,ID_PICKSIZE+5,0,2+158*3,2+18*6,156,16,"Huge",PickSizeClick);
	}
	else
	{
		MakeButton(BTN_RADIO,ID_PICKTHEME+0,0,2+158*0,2+18*0,156,16,"Goodguys",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+1,0,2+158*1,2+18*0,156,16,"Badguys",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+2,0,2+158*2,2+18*0,156,16,"Spooky Forest",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+3,0,2+158*3,2+18*0,156,16,"Dusty Desert",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+4,0,2+158*0,2+18*1,156,16,"Icy Mountain",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+5,0,2+158*1,2+18*1,156,16,"Cavernous Caves",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+6,0,2+158*2,2+18*1,156,16,"Space Station",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+7,0,2+158*3,2+18*1,156,16,"Haunted Mansion",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+8,0,2+158*0,2+18*2,156,16,"Pygmy Island",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+9,0,2+158*1,2+18*2,156,16,"Urban",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+10,0,2+158*2,2+18*2,156,16,"Undersea",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+11,0,2+158*3,2+18*2,156,16,"Crazy Asylum",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+12,0,2+158*0,2+18*3,156,16,"Traps & Puzzles",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+13,0,2+158*1,2+18*3,156,16,"Humanoid",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+14,0,2+158*2,2+18*3,156,16,"Animals",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+15,0,2+158*3,2+18*3,156,16,"Vegetation",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+16,0,2+158*0,2+18*4,156,16,"Pumpkins",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+17,0,2+158*1,2+18*4,156,16,"Zombies",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+18,0,2+158*2,2+18*4,156,16,"Skeletons",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+19,0,2+158*3,2+18*4,156,16,"Vampires",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+20,0,2+158*0,2+18*5,156,16,"Serpents",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+21,0,2+158*1,2+18*5,156,16,"Far Out",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+22,0,2+158*2,2+18*5,156,16,"Generators",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+23,0,2+158*3,2+18*5,156,16,"Vehicles",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+24,0,2+158*0,2+18*6,156,16,"Flying",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+25,0,2+158*1,2+18*6,156,16,"Aquatic",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+26,0,2+158*2,2+18*6,156,16,"Spiders",PickThemeClick);
		MakeButton(BTN_RADIO,ID_PICKTHEME+27,0,2+158*3,2+18*6,156,16,"Bits & Pieces",PickThemeClick);
	}
	
}

static void MakeMonsterList(void)
{
	int i,pos;

	ClearButtons(ID_PICKMONS,ID_PICKMONS+50);
	ClearButtons(ID_MOREMONS,ID_MOREMONS);

	monsStart=0;
	pos=0;
	monsInList=0;
	monsList.clear();

	// get all items which match this theme
	for(i=0;i<NUM_MONSTERS;i++)
	{
		if(MonsterTheme(i)&themes[curTheme] &&(curGroup==0||MonsterGroup(i)&groups[curGroup])&&(curColor==0||MonsterColor(i)&colors[curColor])&&(MonsterSize(i)>=targetSizeMin&&MonsterSize(i)<=targetSizeMax))
		{
			monsList.push_back(i);
			monsInList=pos+1;
			pos++;
		}
	}

	// now sort them alphabetically, simple bubble sort
	if (curTheme!=1)
	{
		byte swapped=1;
		while(swapped)
		{
			swapped=0;
			for(i=0;i<monsInList-1;i++)
			{
				if(strcmp(MonsterName(monsList[i]),MonsterName(monsList[i+1]))>0)
				{
					dword b=monsList[i+1];
					monsList[i+1]=monsList[i];
					monsList[i]=b;
					swapped=1;
				}
			}
		}
	}

	// now make buttons for the first N, which is however many fit on the screen
	pos=130;
	monsShown=0;
	for(i=0;i<monsInList-monsStart;i++)
	{
		if(pos>480-30)
		{
			MakeButton(BTN_NORMAL,ID_MOREMONS,0,2,pos,156,16,"More Monsters...",MoreMonstersClick);
			break;
		}
		MakeButton(BTN_RADIO,ID_PICKMONS+i,0,2,pos,156,16,MonsterName(monsList[i+monsStart]),PickMonsterClick);
		monsShown++;
		pos+=18;
	}

}

static void MonsterEditSetupButtons(void)
{
	ClearButtons();
	themePage=0;

	// theme selection
	MakeButton(BTN_RADIO,ID_PICKTHEME+0,0,2+158*0,2+18*0,156,16,"Goodguys",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+1,0,2+158*1,2+18*0,156,16,"Badguys",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+2,0,2+158*2,2+18*0,156,16,"Spooky Forest",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+3,0,2+158*3,2+18*0,156,16,"Dusty Desert",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+4,0,2+158*0,2+18*1,156,16,"Icy Mountain",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+5,0,2+158*1,2+18*1,156,16,"Cavernous Caves",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+6,0,2+158*2,2+18*1,156,16,"Space Station",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+7,0,2+158*3,2+18*1,156,16,"Haunted Mansion",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+8,0,2+158*0,2+18*2,156,16,"Pygmy Island",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+9,0,2+158*1,2+18*2,156,16,"Urban",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+10,0,2+158*2,2+18*2,156,16,"Undersea",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+11,0,2+158*3,2+18*2,156,16,"Crazy Asylum",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+12,0,2+158*0,2+18*3,156,16,"Traps & Puzzles",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+13,0,2+158*1,2+18*3,156,16,"Humanoid",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+14,0,2+158*2,2+18*3,156,16,"Animals",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+15,0,2+158*3,2+18*3,156,16,"Vegetation",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+16,0,2+158*0,2+18*4,156,16,"Pumpkins",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+17,0,2+158*1,2+18*4,156,16,"Zombies",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+18,0,2+158*2,2+18*4,156,16,"Skeletons",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+19,0,2+158*3,2+18*4,156,16,"Vampires",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+20,0,2+158*0,2+18*5,156,16,"Serpents",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+21,0,2+158*1,2+18*5,156,16,"Far Out",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+22,0,2+158*2,2+18*5,156,16,"Generators",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+23,0,2+158*3,2+18*5,156,16,"Vehicles",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+24,0,2+158*0,2+18*6,156,16,"Flying",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+25,0,2+158*1,2+18*6,156,16,"Aquatic",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+26,0,2+158*2,2+18*6,156,16,"Spiders",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+27,0,2+158*3,2+18*6,156,16,"Bits & Pieces",PickThemeClick);

	// delete, move, and exit
	MakeButton(BTN_NORMAL,ID_EXIT,0,480,460,158,14,"Exit Monster List",ExitClick);
	MakeButton(BTN_NORMAL,ID_MORETHEME,0,480,460-16,158,14,"More Themes & Filters",MoreThemesClick);
	MakeButton(BTN_NORMAL,ID_SURPRISE,0,480,460-32,158,14,"Surprise Me!",SurpriseClick);
}

static void SetupMonsterDisplay(void)
{
}

void MonsterEdit_Init(byte modeFrom,world_t *wrld)
{
	rememberMode=modeFrom;
	GetDisplayMGL()->MouseTap();
	world=wrld;
	mode=MMODE_NORMAL;
	curMons=saveCurMons;
	curTheme=saveCurTheme;
	curGroup=saveCurGroup;
	curColor=saveCurColor;
	MonsterEditSetupButtons();
	SetupMonsterDisplay();
	RadioOn(ID_PICKTHEME+curTheme,ID_PICKTHEME,ID_PICKTHEME+50);
	MakeMonsterList();
}

void MonsterEdit_Exit(void)
{
}

void MonsterEdit_Update(int mouseX,int mouseY,MGLDraw *mgl)
{
	int i;

	// just hovering on any monster buttons 'selects' the monster
	realClick=0;
	for(i=0;i<50;i++)
		CheckButton(mouseX,mouseY,ID_PICKMONS+i);

	if(mgl->MouseTap())
	{
		realClick=1;
		if(mode==MMODE_HELP)
		{
			if(EditHelpClick(mouseX,mouseY))
				mode=helpRemember;
		}
		else
			CheckButtons(mouseX,mouseY);
	}
}

void MonsterEdit_Key(char k)
{
	switch(mode)
	{
		case MMODE_NORMAL:
			if(k==27)
			{
				if(WhichTool()==TOOL_SPECIAL)
					SetEditMode(EDITMODE_SPECIAL);
				else
					SetEditMode(EDITMODE_EDIT);
				PickedTile(-1);
				saveCurMons=curMons;
				saveCurTheme=curTheme;
				saveCurGroup=curGroup;
				saveCurColor=curColor;
				return;
			}
			break;
		case MMODE_HELP:
			if(!EditHelpKey(k))
				mode=helpRemember;
			break;

	}
}

void MonsterEdit_Render(int mouseX,int mouseY,MGLDraw *mgl)
{
	char s[32];

	mgl->ClearScreen();

	mgl->FillBox(161,129,639,479,32*3+6);

	SetSpriteConstraints(161,130,639,479);
	InstaRenderMonster(520,380,curMons,1,mgl);
	SetSpriteConstraints(0,0,639,479);

	mgl->FillBox(161,268,470,479,32*1+4);
	mgl->Box(160,268,470,479,32*1+16);

	mgl->FillBox(0,128,639,128,31);
	mgl->FillBox(160,129,160,479,31);
	mgl->FillBox(0,129,159,479,32*1+4);

	CenterPrint(317,270,MonsterName(curMons),0,1);
	mgl->FillBox(170,286,460,286,31);
	if(profile.progress.scanned[curMons])
	{
		PrintRect(164,290,470,479,13,MonsterNotes(curMons),1);

		if(MonsterFlags(curMons,curMons)&MF_NOHIT)
			Print(164,464,"Life: Unhittable",0,1);
		else if(MonsterFlags(curMons,curMons)&MF_INVINCIBLE)
			Print(164,464,"Life: Invincible",0,1);
		else
		{
			sprintf(s,"Life: %d",MonsterHP(curMons));
			Print(164,464,s,0,1);
		}
	}
	else
		PrintRect(164,290,470,479,13,"NO DATA ON MONSTER!",1);
	sprintf(s,"ID: %d",curMons);
	Print(364,464,s,0,1);
	
	RenderButtons(mouseX,mouseY,mgl);

	if(mode==MMODE_HELP)
		RenderEditHelp(mouseX,mouseY,mgl);
}

void MonsterEdit_Help(void)
{
	if(mode==MMODE_HELP)
		return;
	helpRemember=mode;
	mode=MMODE_HELP; 
	InitEditHelp(HELP_MONSPICK);
}
