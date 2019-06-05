#include "monsteredit.h"
#include "dialogbits.h"
#include "filedialog.h"
#include "items.h"
#include "tool.h"
#include "edithelp.h"
#include "guy.h"

#define MMODE_NORMAL	0	// doing nothing (just viewing monsters, click to select, select all you like)
#define MMODE_SELECT	1	// selecting a monster (pick one and you're done)
#define MMODE_HELP		2

#define ID_EXIT		1
#define ID_MOREMONS 2
#define ID_GROUPS	150
#define ID_PICKTHEME 200
#define ID_PICKMONS  500

static byte mode,curTheme;
static world_t *world;
static byte curMons;
static byte rememberMode;
static byte helpRemember;

static byte monsList[256];
static word monsInList,monsStart,monsShown;
static byte realClick;

static byte saveCurMons=1,saveCurTheme=0;

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

static void MakeMonsterList(void)
{
	int i,pos;
	byte b,swapped;

	ClearButtons(ID_PICKMONS,ID_PICKMONS+50);
	ClearButtons(ID_MOREMONS,ID_MOREMONS);

	monsStart=0;
	pos=0;
	monsInList=0;
	// get all items which match this theme
	for(i=0;i<NUM_MONSTERS;i++)
	{
		if(MonsterTheme(i)&(1<<curTheme))
		{
			monsList[pos]=i;
			monsInList=pos+1;
			pos++;
		}
	}

	// now sort them alphabetically, simple bubble sort
	swapped=1;
	while(swapped)
	{
		swapped=0;
		for(i=0;i<monsInList-1;i++)
		{
			if(strcmp(MonsterName(monsList[i]),MonsterName(monsList[i+1]))>0)
			{
				b=monsList[i+1];
				monsList[i+1]=monsList[i];
				monsList[i]=b;
				swapped=1;
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

static void PickThemeClick(int id)
{
	RadioOn(id,ID_PICKTHEME,ID_PICKTHEME+50);
	curTheme=id-ID_PICKTHEME;
	MakeMonsterList();
}

static void MonsterEditSetupButtons(void)
{
	ClearButtons();

	// theme selection
	MakeButton(BTN_RADIO,ID_PICKTHEME+0,0,2+158*0,2+18*0,156,16,"Goodguys",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+1,0,2+158*1,2+18*0,156,16,"Badguys",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+2,0,2+158*2,2+18*0,156,16,"Spooky",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+3,0,2+158*3,2+18*0,156,16,"Zombies",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+4,0,2+158*0,2+18*1,156,16,"Bats",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+5,0,2+158*1,2+18*1,156,16,"Spiders",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+6,0,2+158*2,2+18*1,156,16,"Pumpkins",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+7,0,2+158*3,2+18*1,156,16,"Zoids",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+8,0,2+158*0,2+18*2,156,16,"Bosses",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+9,0,2+158*1,2+18*2,156,16,"Mini-Bosses",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+10,0,2+158*2,2+18*2,156,16,"Traps",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+11,0,2+158*3,2+18*2,156,16,"Overworld",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+12,0,2+158*0,2+18*3,156,16,"Tomb of Mirrors",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+13,0,2+158*1,2+18*3,156,16,"Tomb of Flames",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+14,0,2+158*2,2+18*3,156,16,"Tomb of Sparks",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+15,0,2+158*3,2+18*3,156,16,"Tomb of Darkness",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+16,0,2+158*0,2+18*4,156,16,"Tomb of Din",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+17,0,2+158*1,2+18*4,156,16,"Tomb of Bones",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+18,0,2+158*2,2+18*4,156,16,"Vehicles",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+19,0,2+158*3,2+18*4,156,16,"Body Parts",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+20,0,2+158*0,2+18*5,156,16,"Flying",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+21,0,2+158*1,2+18*5,156,16,"Non-Hollow",PickThemeClick);
	//MakeButton(BTN_RADIO,ID_PICKTHEME+22,0,2+158*2,2+18*5,156,16,"Bad Graphics",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+23,0,2+158*3,2+18*5,156,16,"AI Only",PickThemeClick);
	//MakeButton(BTN_RADIO,ID_PICKTHEME+24,0,2+158*0,2+18*6,156,16,"Aquatic",PickThemeClick);
	//MakeButton(BTN_RADIO,ID_PICKTHEME+25,0,2+158*1,2+18*6,156,16,"Undersea",PickThemeClick);
	//MakeButton(BTN_RADIO,ID_PICKTHEME+26,0,2+158*2,2+18*6,156,16,"Flying",PickThemeClick);
	//MakeButton(BTN_RADIO,ID_PICKTHEME+27,0,2+158*3,2+18*6,156,16,"Body Parts",PickThemeClick);

	// delete, move, and exit
	MakeButton(BTN_NORMAL,ID_EXIT,0,480,460,158,14,"Exit Monster List",ExitClick);
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
				return;
			}
			break;
		case MMODE_HELP:
			if(!EditHelpKey(k))
				mode=helpRemember;
			break;

	}
}

// see monsnotes.cpp for this
extern const char* MonsterNotes(byte type);
extern int MonsterNotesPercent();

void MonsterEdit_Render(int mouseX,int mouseY,MGLDraw *mgl)
{
	char s[32];

	mgl->ClearScreen();

	mgl->FillBox(161,129,639,479,32*3+6);

	SetSpriteConstraints(161,130,639,479);
	InstaRenderMonster(520,380,curMons,0,mgl);
	SetSpriteConstraints(0,0,639,479);

	mgl->FillBox(161,268,470,479,32*1+4);
	mgl->Box(160,268,470,479,32*1+16);

	mgl->FillBox(0,128,639,128,31);
	mgl->FillBox(160,129,160,479,31);
	mgl->FillBox(0,129,159,479,32*1+4);

	CenterPrint(317,270,MonsterName(curMons),0,1);
	mgl->FillBox(170,286,460,286,31);

	// if(profile.progress.scanned[curMons])
	if(MonsterFlags(curMons,curMons)&MF_NOHIT)
		Print(164,464,"Life: Unhittable",0,1);
	else if(MonsterFlags(curMons,curMons)&MF_INVINCIBLE)
		Print(164,464,"Life: Invincible",0,1);
	else
	{
		sprintf(s,"Life: %d",MonsterHP(curMons));
		Print(164,464,s,0,1);
	}
	PrintRect(164,290,470,479,13,MonsterNotes(curMons),1);
	if(!WorthXP(curMons))
		Print(300,464,"No XP",0,1);
	if(DoesRespawn(curMons))
		Print(370,464,"Respawns",0,1);

	sprintf(s,"Notes: %d%%",MonsterNotesPercent());
	Print(4+158*3,4+18*6,s,0,1);

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
