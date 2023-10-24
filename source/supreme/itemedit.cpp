#include "itemedit.h"
#include <stdlib.h>
#include "dialogbits.h"
#include "filedialog.h"
#include "items.h"
#include "tool.h"
#include "textdialog.h"
#include "monsteredit.h"
#include "soundedit.h"
#include "terrainedit.h"
#include "vars.h"
#include "edithelp.h"
#include "player.h"

#define IMODE_NORMAL	0	// doing nothing
#define IMODE_SELECT	1	// selecting an item (for instance to define what item this one becomes)
#define IMODE_DELETE	3	// asking yes/no on item deletion
#define IMODE_RESET		4	// asking yes/no on item reset
#define IMODE_PICKEFFECT 5	// selecting an item effect
#define IMODE_NAME		6	// typing in a name
#define IMODE_EFFNUM	7	// typing in a number for an item effect
#define IMODE_PICKMONS	8	// picking a monster
#define IMODE_PICKSND	9	// picking a sound
#define IMODE_MESSAGE	10	// typing in a message
#define IMODE_RARITY	11	// entering the item's rarity
#define IMODE_PICKTILE	12	// picking a tile
#define IMODE_HELP		13	// item edit help

#define ID_EXIT		1
#define ID_DELETE	2
#define ID_NEW		3
#define ID_RESET	4
#define ID_MOREITEMS 5
#define ID_NAME		6
#define ID_NAME2	7
#define ID_PIC		8
#define ID_FLAGS	10
#define ID_THEMES	50
#define ID_TRIGGERS 100
#define ID_GROUPS	150
#define ID_PICKTHEME 200
#define ID_PICKITEM  500

#define ID_BRIGHT	600
#define ID_FROMCOL	601
#define ID_TOCOL	602
#define ID_ITEMEFF	603
#define ID_ITEMEFFMOD 604
#define ID_ITEMSND	605
#define ID_ITEMMSG	606
#define ID_RARITY	607
#define ID_PICKSPR	608

static word flags[]={IF_SHADOW,IF_GLOW,IF_SOLID,IF_BULLETPROOF,IF_PICKUP,IF_LOONYCOLOR,IF_TILE,IF_USERJSP,IF_BUBBLES};
static dword themes[]={IT_PICKUP,IT_DECOR,IT_OBSTACLE,IT_BULLETPROOF,IT_DOOR,IT_TREE,IT_ROCK,
				  IT_CRATE,IT_SIGN,IT_WEAPON,IT_POWERUP,IT_KEY,IT_COLLECT,IT_FOOD,IT_ENTRANCE,
				  IT_CHAIR,IT_CUSTOM};
static word trigs[]={ITR_GET,ITR_SHOOT,ITR_PLAYERBUMP,ITR_ENEMYBUMP,ITR_FRIENDBUMP,ITR_CHOP,
				  ITR_MINECART,ITR_ALWAYS};

#define IEBTN_NONE	0
#define IEBTN_NUM	1	// numerical
#define IEBTN_ITEM	2	// choose an item name
#define IEBTN_MONS	3	// choose a monster
#define IEBTN_KEYCH 4	// choose a keychain
#define IEBTN_COLOR	5	// choose a color
#define IEBTN_KEYCOLOR 6	// choose a key color
#define IEBTN_GIVETAKE 7	// toggle give or take (-1 or 1)
#define IEBTN_POWERUP  8	// choose from the powerup list
#define IEBTN_WEAPON   9	// choose from the weapon list
#define IEBTN_VAR	   10	// choose a variable (right click toggles G or V, left click cycles num)
#define IEBTN_DIR	   11	// choose a direction

struct itemEff_t
{
	char name[32];
	char btnTxt[32];
	int buttonMin;
	int buttonMax;
	byte buttonType;
};

static const itemEff_t itemEff[]={
	{"No Effect","",0,0,IEBTN_NONE},
	{"Explode into particles","Color",0,7,IEBTN_COLOR},
	{"Heal/Harm Target","Amount",-9999,9999,IEBTN_NUM},
	{"Give Weapon","Weapon",0,0,IEBTN_WEAPON},
	{"Give Hammers","Number",-5,5,IEBTN_NUM},
	{"Give Pants","Number",-4,4,IEBTN_NUM},
	{"Give Yellow Keys","Number",-3,3,IEBTN_NUM},
	{"Give Red Key","Give/Take",-1,1,IEBTN_GIVETAKE},
	{"Give Green Key","Give/Take",-1,1,IEBTN_GIVETAKE},
	{"Give Blue Key","Give/Take",-1,1,IEBTN_GIVETAKE},
	{"Give LoonyKey","Give/Take",-1,1,IEBTN_GIVETAKE},
	{"Give Coins","Number",-9999,9999,IEBTN_NUM},
	{"Give Candles","Number",-9999,9999,IEBTN_NUM},
	{"Give Oxygen","Amount",-128,128,IEBTN_NUM},
	{"Rage Up/Down","Amount",-128,128,IEBTN_NUM},
	{"Radioactive Chinese","",0,0,IEBTN_NONE},
	{"Become Different Item","Item",0,0,IEBTN_ITEM},
	{"Summon Monster","Monster",0,0,IEBTN_MONS},
	{"Power Up Player","Power-up",0,0,IEBTN_POWERUP},
	{"Chinese Food","",0,0,IEBTN_NONE},
	{"Give Brains","Number",-9999,9999,IEBTN_NUM},
	{"Give Keychain","Keychain",0,0,IEBTN_KEYCH},
	{"Act as Door","Color",0,0,IEBTN_KEYCOLOR},
	{"Give Orbiters","Number",-50,50,IEBTN_NUM},
	{"Give Orbit Bs","Number",-50,50,IEBTN_NUM},
	{"Get Pushed","",0,0,IEBTN_NONE},
	{"Increment","Variable",0,15,IEBTN_VAR},
	{"Decrement","Variable",0,15,IEBTN_VAR},
	{"Move","Direction",0,3,IEBTN_DIR},
};

static const char colorName[8][16]={
	"Grey",
	"Green",
	"Brown",
	"Blue",
	"Red",
	"Yellow",
	"Purple",
	"Aqua",
};

static const char keyColorName[4][16]={
	"Yellow",
	"Red",
	"Green",
	"Blue",
};

static const char keychainName[4][16]={
	"Pumpkin",
	"Hammer",
	"Rocket",
	"Squash",
};

static const char directionName[4][16]={
	"Right",
	"Down",
	"Left",
	"Up",
};

static byte itemList[256];
static word itemsInList,itemStart=0,itemsShown;
static byte justPicking;
static byte mode,curTheme;
static world_t *world;
static byte curItem;
static byte backColor;
static byte realClick;

static byte saveCurTheme=0,saveCurItem=1,rememberMode,helpRemember;
static word saveItemStart=0;

static byte canDrag=1;

static void ItemSetFlags(void);
static void MakeItemList(void);

static void FlagClick(int id)
{
	byte b;

	MakeNormalSound(SND_MENUCLICK);

	b=GetButtonState(id);

	if(b==CHECK_ON)	// if it's on
	{
		// shut it
		if(id<ID_THEMES)
			GetItem(curItem)->flags&=(~flags[id-ID_FLAGS]);
		else if(id<ID_TRIGGERS)
			GetItem(curItem)->theme&=(~themes[id-ID_THEMES]);
		else
			GetItem(curItem)->trigger&=(~trigs[id-ID_TRIGGERS]);
		SetButtonState(id,CHECK_OFF);

		if(GetItem(curItem)->theme==0)	// can't be on zero themes, it'd be unpickable
			GetItem(curItem)->theme=IT_CUSTOM;	// so throw it into custom
	}
	else
	{
		// turn them on
		if(id<ID_THEMES)
			GetItem(curItem)->flags|=flags[id-ID_FLAGS];
		else if(id<ID_TRIGGERS)
			GetItem(curItem)->theme|=themes[id-ID_THEMES];
		else
			GetItem(curItem)->trigger|=trigs[id-ID_TRIGGERS];
		SetButtonState(id,CHECK_ON);
	}

	if(id==ID_FLAGS+6)	// TILE flag
	{
		// prevent it from using an illegal sprite
		if(!(GetItem(curItem)->flags&IF_TILE))
		{
			if(GetItem(curItem)->sprNum>=NumItemSprites())
				GetItem(curItem)->sprNum=0;
		}
		else
		{
			GetItem(curItem)->flags&=(~IF_USERJSP); // can't combine tile and JSP
		}
	}
	else if(id==ID_FLAGS+7)	// USERJSP flag
	{
		// prevent it from using an illegal sprite
		if(GetItem(curItem)->flags&IF_USERJSP)
		{
			int numCust = NumCustomSprites();
			if (numCust == 0)
			{
				MakeNormalSound(SND_TURRETBZZT);
				GetItem(curItem)->flags&=(~IF_USERJSP);
				SetButtonState(id,CHECK_OFF);
			}
			else
			{
				GetItem(curItem)->flags&=(~IF_TILE); // can't combine tile and JSP
				if(GetItem(curItem)->sprNum>=numCust)
					GetItem(curItem)->sprNum=0;
			}
		}
		else
		{
			if(GetItem(curItem)->sprNum>=NumItemSprites())
				GetItem(curItem)->sprNum=0;
		}
	}
	ItemSetFlags();
}

static void DeleteClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);

	if(curItem<NUM_ORIGINAL_ITEMS)
	{
		// reset, don't delete
		memcpy(GetItem(curItem),GetBaseItem(curItem),sizeof(item_t));
	}
	else
	{
		// delete the item
		DeleteItem(curItem);
		AddMapGuys(EditorGetMap());
		ToolItemWasDeleted(curItem);
		curItem=1;
	}
	ItemSetFlags();
}

static void NewClick(int id)
{
	int i;

	i=NewItem();

	if(i==-1)
		MakeNormalSound(SND_MENUCLICK);
	else
	{
		MakeNormalSound(SND_MENUSELECT);
		curItem=i;
		curTheme=16;
		MakeItemList();
		ItemSetFlags();
	}
}

static void CopyClick(int id)
{
	int i;
	char s[64];

	i=NewItem();

	if(i==-1)
		MakeNormalSound(SND_MENUCLICK);
	else
	{
		MakeNormalSound(SND_MENUSELECT);
		memcpy(GetItem(i),GetItem(curItem),sizeof(item_t));
		GetItem(i)->theme=IT_CUSTOM;	// copy everything but the theme
		sprintf(s,"Copy of %s",GetItem(curItem)->name);
		SDL_strlcpy(GetItem(i)->name, s, sizeof(GetItem(i)->name));
		curItem=i;
		curTheme=16;
		MakeItemList();
		ItemSetFlags();
	}
}

static void ExitClick(int id)
{
	if(mode==IMODE_NORMAL)
	{
		SetEditMode(EDITMODE_EDIT);
		MakeNormalSound(SND_MENUCLICK);
	}
	else if(mode==IMODE_SELECT)
	{
		if(justPicking)
		{
			PickedTile(-1);
			if(WhichTool()!=TOOL_SPECIAL)
				SetEditMode(EDITMODE_EDIT);
			else
				SetEditMode(EDITMODE_SPECIAL);
			MakeNormalSound(SND_MENUCLICK);
		}
		else
			mode=IMODE_NORMAL;
	}
	saveCurTheme=curTheme;
	saveCurItem=curItem;
	saveItemStart=itemStart;
}

static void PickItemClick(int id)
{
	RadioOn(id,ID_PICKITEM,ID_PICKITEM+50);
	curItem=itemList[itemStart+(id-ID_PICKITEM)];
	ItemSetFlags();
	if(mode==IMODE_NORMAL)
	{
	}
	else if(realClick && mode==IMODE_SELECT)
	{
		if(justPicking)
		{
			saveCurTheme=curTheme;
			saveCurItem=curItem;
			saveItemStart=itemStart;
			SetEditMode(rememberMode);
			PickedTile(curItem);
			MakeNormalSound(SND_MENUCLICK);
		}
		else
		{
			mode=IMODE_NORMAL;
			GetItem(saveCurItem)->effectAmt=curItem;
			curItem=saveCurItem;
			curTheme=saveCurTheme;
			itemStart=saveItemStart;
			ItemSetFlags();
		}
	}
}

static void MoreItemsClick(int id)
{
	int i,pos;

	itemStart+=itemsShown;
	if(itemStart>=itemsInList)
		itemStart=0;

	ClearButtons(ID_PICKITEM,ID_PICKITEM+50);
	ClearButtons(ID_MOREITEMS,ID_MOREITEMS);

	// now make buttons for the first N, which is however many fit on the screen
	pos=100;
	itemsShown=0;
	for(i=0;i<itemsInList-itemStart;i++)
	{
		MakeButton(BTN_RADIO,ID_PICKITEM+i,0,2,pos,156,16,GetItem(itemList[i+itemStart])->name,PickItemClick);
		itemsShown++;
		pos+=18;
		if(pos>480-30)
			break;
	}
	MakeButton(BTN_NORMAL,ID_MOREITEMS,0,2,pos,156,16,"More Items...",MoreItemsClick);
}

static void MakeItemList(void)
{
	int i,pos;
	byte b,swapped;

	ClearButtons(ID_PICKITEM,ID_PICKITEM+50);
	ClearButtons(ID_MOREITEMS,ID_MOREITEMS);

	pos=0;
	itemsInList=0;
	// get all items which match this theme
	for(i=0;i<NumItems();i++)
	{
		if(GetItem(i)->theme&themes[curTheme])
		{
			itemList[pos]=i;
			itemsInList=pos+1;
			pos++;
		}
	}

	// now sort them alphabetically, simple bubble sort
	swapped=1;
	while(swapped)
	{
		swapped=0;
		for(i=0;i<itemsInList-1;i++)
		{
			if(strcmp(GetItem(itemList[i])->name,GetItem(itemList[i+1])->name)>0)
			{
				b=itemList[i+1];
				itemList[i+1]=itemList[i];
				itemList[i]=b;
				swapped=1;
			}
		}
	}

	// now make buttons for the first N, which is however many fit on the screen
	pos=100;
	itemsShown=0;
	if(itemStart>=itemsInList)
		itemStart=0;
	for(i=itemStart;i<itemsInList;i++)
	{
		MakeButton(BTN_RADIO,ID_PICKITEM+i-itemStart,(curItem==itemList[i])*CHECK_ON,2,pos,156,16,GetItem(itemList[i])->name,PickItemClick);
		itemsShown++;
		pos+=18;
		if(pos>480-30)
			break;
	}
	if(itemStart>0 || pos>480-30)
		MakeButton(BTN_NORMAL,ID_MOREITEMS,0,2,pos,156,16,"More Items...",MoreItemsClick);
}

static void PickThemeClick(int id)
{
	RadioOn(id,ID_PICKTHEME,ID_PICKTHEME+50);
	curTheme=id-ID_PICKTHEME;
	itemStart=0;
	MakeItemList();
}

static void NameClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	mode=IMODE_NAME;
	InitTextDialog("Enter item name",GetItem(curItem)->name,31);
}

static void MessageClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	mode=IMODE_MESSAGE;
	InitTextDialog("Enter message",GetItem(curItem)->msg,31);
}

static void RarityClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	mode=IMODE_RARITY;
	InitTextDialog("Enter item's rarity (0-255), 0=never dropped","",3);
}

static void BrightClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	GetItem(curItem)->bright=0;
}

static void ColorClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);

	if(id==ID_FROMCOL)
	{
		GetItem(curItem)->fromColor++;
		if(GetItem(curItem)->fromColor==8)
			GetItem(curItem)->fromColor=0;
	}
	else
	{
		GetItem(curItem)->toColor++;
		if(GetItem(curItem)->toColor==8)
			GetItem(curItem)->toColor=0;
	}
}

static void ItemEditSetupButtons(void)
{
	ClearButtons();

	// theme selection
	MakeButton(BTN_RADIO,ID_PICKTHEME+0,0,2+158*0,2+18*0,156,16,"Pick-ups",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+1,0,2+158*1,2+18*0,156,16,"Decorations",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+2,0,2+158*2,2+18*0,156,16,"Obstacles",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+3,0,2+158*3,2+18*0,156,16,"Bulletproof",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+4,0,2+158*0,2+18*1,156,16,"Doors",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+5,0,2+158*1,2+18*1,156,16,"Vegetation",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+6,0,2+158*2,2+18*1,156,16,"Rocks",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+7,0,2+158*3,2+18*1,156,16,"Crates, Etc.",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+8,0,2+158*0,2+18*2,156,16,"Signs",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+9,0,2+158*1,2+18*2,156,16,"Weapons",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+10,0,2+158*2,2+18*2,156,16,"Power-Ups",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+11,0,2+158*3,2+18*2,156,16,"Keys",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+12,0,2+158*0,2+18*3,156,16,"Collectibles",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+13,0,2+158*1,2+18*3,156,16,"Food",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+14,0,2+158*2,2+18*3,156,16,"Entrances",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+15,0,2+158*3,2+18*3,156,16,"Chairs",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+16,0,2+158*0,2+18*4,156,16,"Extras",PickThemeClick);

	// delete, move, and exit
	if(mode!=IMODE_SELECT)
	{
		MakeButton(BTN_NORMAL,ID_NEW,0,480,460-100,158,14,"Add New Item",NewClick);
		if(curItem<NUM_ORIGINAL_ITEMS)
			MakeButton(BTN_NORMAL,ID_DELETE,0,480,460-80,158,14,"Reset Item",DeleteClick);
		else
			MakeButton(BTN_NORMAL,ID_DELETE,0,480,460-80,158,14,"Delete Item",DeleteClick);
		MakeButton(BTN_NORMAL,ID_RESET,0,480,460-60,158,14,"Copy Item",CopyClick);
	}

	if(mode==IMODE_SELECT)
		MakeButton(BTN_NORMAL,ID_EXIT,0,480,460,158,14,"Cancel Selection",ExitClick);
	else
		MakeButton(BTN_NORMAL,ID_EXIT,0,480,460,158,14,"Exit Item Editor",ExitClick);

	RadioOn(ID_PICKTHEME+curTheme,ID_PICKTHEME,ID_PICKTHEME+50);
}

static void EffectClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);

	GetItem(curItem)->effect++;
	if(GetItem(curItem)->effect>=IE_MAX)
		GetItem(curItem)->effect=0;
	GetItem(curItem)->effectAmt=1;
	ItemSetFlags();
}

static void EffectRightClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);

	GetItem(curItem)->effect--;
	if(GetItem(curItem)->effect>=IE_MAX)
		GetItem(curItem)->effect=IE_MAX-1;
	GetItem(curItem)->effectAmt=1;
	ItemSetFlags();
}

static void TileClick(int id)
{
	mode=IMODE_PICKTILE;
	SetEditMode(EDITMODE_TERRAIN);
	TerrainEdit_InitPicker(EDITMODE_ITEM,EditorGetWorld());
}

static void SpriteClick(int id)
{
	int max = (GetItem(curItem)->flags & IF_USERJSP) ? NumCustomSprites() : NumItemSprites();
	GetItem(curItem)->sprNum++;
	if(GetItem(curItem)->sprNum>=max)
		GetItem(curItem)->sprNum=0;
}

static void SpriteRightClick(int id)
{
	int max = (GetItem(curItem)->flags & IF_USERJSP) ? NumCustomSprites() : NumItemSprites();
	GetItem(curItem)->sprNum--;
	if(GetItem(curItem)->sprNum>=max)
		GetItem(curItem)->sprNum=max-1;
}

static void EffModClick(int id)
{
	char text[64];

	MakeNormalSound(SND_MENUCLICK);
	switch(itemEff[GetItem(curItem)->effect].buttonType)
	{
		case IEBTN_NONE:
			break;
		case IEBTN_NUM:
			mode=IMODE_EFFNUM;
			sprintf(text,"Enter a number (%d to %d)",itemEff[GetItem(curItem)->effect].buttonMin,
				itemEff[GetItem(curItem)->effect].buttonMax);
			InitTextDialog(text,"",5);
			break;
		case IEBTN_ITEM:
			mode=IMODE_SELECT;
			saveCurTheme=curTheme;
			saveCurItem=curItem;
			saveItemStart=itemStart;
			break;
		case IEBTN_MONS:
			mode=IMODE_PICKMONS;
			SetEditMode(EDITMODE_PICKENEMY);
			MonsterEdit_Init(EDITMODE_ITEM,EditorGetWorld());
			break;
		case IEBTN_KEYCH:
		case IEBTN_DIR:
			GetItem(curItem)->effectAmt++;
			if(GetItem(curItem)->effectAmt>3)
				GetItem(curItem)->effectAmt=0;
			break;
		case IEBTN_WEAPON:
			GetItem(curItem)->effectAmt++;
			if(GetItem(curItem)->effectAmt>=MAX_WEAPONS)
				GetItem(curItem)->effectAmt=0;	// can give no weapon to disarm
			break;
		case IEBTN_COLOR:
			GetItem(curItem)->effectAmt++;
			if(GetItem(curItem)->effectAmt>7)
				GetItem(curItem)->effectAmt=0;
			break;
		case IEBTN_KEYCOLOR:
			GetItem(curItem)->effectAmt++;
			if(GetItem(curItem)->effectAmt>3)
				GetItem(curItem)->effectAmt=0;
			break;
		case IEBTN_GIVETAKE:
			GetItem(curItem)->effectAmt=-GetItem(curItem)->effectAmt;
			break;
		case IEBTN_POWERUP:
			if(GetItem(curItem)->effectAmt>0)
				GetItem(curItem)->effectAmt=-GetItem(curItem)->effectAmt;
			else
				GetItem(curItem)->effectAmt=-GetItem(curItem)->effectAmt+1;
			if(GetItem(curItem)->effectAmt>=MAX_POWERUP)
				GetItem(curItem)->effectAmt=1;
			break;
		case IEBTN_VAR:
			GetItem(curItem)->effectAmt++;
			if(GetItem(curItem)->effectAmt==8)
				GetItem(curItem)->effectAmt=0;
			if(GetItem(curItem)->effectAmt==16)
				GetItem(curItem)->effectAmt=8;
			break;
	}
	if(mode==IMODE_NORMAL)
		ItemSetFlags();
}

static void EffModRightClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	switch(itemEff[GetItem(curItem)->effect].buttonType)
	{
		case IEBTN_NONE:
			break;
		case IEBTN_NUM:
			GetItem(curItem)->effectAmt=0;
			break;
		case IEBTN_ITEM:
			GetItem(curItem)->effectAmt=0;	// null item
			break;
		case IEBTN_MONS:
			GetItem(curItem)->effectAmt=MONS_BOUAPHA;
			break;
		case IEBTN_KEYCH:
		case IEBTN_DIR:
			GetItem(curItem)->effectAmt--;
			if(GetItem(curItem)->effectAmt<0)
				GetItem(curItem)->effectAmt=3;
			break;
		case IEBTN_WEAPON:
			GetItem(curItem)->effectAmt--;
			if(GetItem(curItem)->effectAmt<0)
				GetItem(curItem)->effectAmt=MAX_WEAPONS-1;
			break;
		case IEBTN_COLOR:
			GetItem(curItem)->effectAmt--;
			if(GetItem(curItem)->effectAmt<0)
				GetItem(curItem)->effectAmt=7;
			break;
		case IEBTN_KEYCOLOR:
			GetItem(curItem)->effectAmt--;
			if(GetItem(curItem)->effectAmt<0)
				GetItem(curItem)->effectAmt=3;
			break;
		case IEBTN_GIVETAKE:
			GetItem(curItem)->effectAmt=-GetItem(curItem)->effectAmt;
			break;
		case IEBTN_POWERUP:
			GetItem(curItem)->effectAmt--;
			if(GetItem(curItem)->effectAmt<1)
				GetItem(curItem)->effectAmt=MAX_POWERUP-1;
			break;
		case IEBTN_VAR:
			GetItem(curItem)->effectAmt^=8;
			break;
	}
	ItemSetFlags();
}

static void SoundClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	mode=IMODE_PICKSND;
	SetEditMode(EDITMODE_SOUND);
	SoundEdit_InitPicker(EDITMODE_ITEM,EditorGetWorld());
}

static void SetupEffect(void)
{
	const itemEff_t *ie;
	char s[64];

	ie=&itemEff[GetItem(curItem)->effect];

	MakeButton(BTN_STATIC,ID_NAME2,0,218,130+16*16,128,15,ie->name,NULL);

	switch(ie->buttonType)
	{
		case IEBTN_NONE:
			// no modifier
			break;
		case IEBTN_NUM:
			// enter a number
			MakeButton(BTN_NORMAL,ID_ITEMEFFMOD,0,174,132+17*16,80,15,itemEff[GetItem(curItem)->effect].btnTxt,EffModClick);
			sprintf(s,"%d",GetItem(curItem)->effectAmt);
			MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,s,NULL);
			break;
		case IEBTN_ITEM:
			// choose an item
			MakeButton(BTN_NORMAL,ID_ITEMEFFMOD,0,174,132+17*16,80,15,itemEff[GetItem(curItem)->effect].btnTxt,EffModClick);
			if(GetItem(GetItem(curItem)->effectAmt))
				MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,GetItem(GetItem(curItem)->effectAmt)->name,NULL);
			else
				MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,"None",NULL);
			break;
		case IEBTN_MONS:
			// choose a monster
			MakeButton(BTN_NORMAL,ID_ITEMEFFMOD,0,174,132+17*16,80,15,itemEff[GetItem(curItem)->effect].btnTxt,EffModClick);
			MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,MonsterName(GetItem(curItem)->effectAmt),NULL);
			break;
		case IEBTN_KEYCH:
			// choose a keychain
			MakeButton(BTN_NORMAL,ID_ITEMEFFMOD,0,174,132+17*16,80,15,itemEff[GetItem(curItem)->effect].btnTxt,EffModClick);
			MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,keychainName[GetItem(curItem)->effectAmt],NULL);
			break;
		case IEBTN_WEAPON:
			// choose a weapon
			MakeButton(BTN_NORMAL,ID_ITEMEFFMOD,0,174,132+17*16,80,15,itemEff[GetItem(curItem)->effect].btnTxt,EffModClick);
			MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,GetWeaponName(GetItem(curItem)->effectAmt),NULL);
			break;
		case IEBTN_COLOR:
			// choose a color
			MakeButton(BTN_NORMAL,ID_ITEMEFFMOD,0,174,132+17*16,80,15,itemEff[GetItem(curItem)->effect].btnTxt,EffModClick);
			MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,colorName[GetItem(curItem)->effectAmt],NULL);
			break;
		case IEBTN_KEYCOLOR:
			// choose a key color
			MakeButton(BTN_NORMAL,ID_ITEMEFFMOD,0,174,132+17*16,80,15,itemEff[GetItem(curItem)->effect].btnTxt,EffModClick);
			MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,keyColorName[GetItem(curItem)->effectAmt],NULL);
			break;
		case IEBTN_GIVETAKE:
			// toggle give or take (-1 or 1)
			MakeButton(BTN_NORMAL,ID_ITEMEFFMOD,0,174,132+17*16,80,15,itemEff[GetItem(curItem)->effect].btnTxt,EffModClick);
			if(GetItem(curItem)->effectAmt>0)
				MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,"Give",NULL);
			else
				MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,"Take",NULL);
			break;
		case IEBTN_POWERUP:
			// choose from the powerup list
			MakeButton(BTN_NORMAL,ID_ITEMEFFMOD,0,174,132+17*16,80,15,itemEff[GetItem(curItem)->effect].btnTxt,EffModClick);
			if(GetItem(curItem)->effectAmt>0)
				MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,GetPowerupName(GetItem(curItem)->effectAmt),NULL);
			else
			{
				sprintf(s,"Remove %s",GetPowerupName(-GetItem(curItem)->effectAmt));
				MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,s,NULL);
			}
			break;
		case IEBTN_VAR:
			// choose a variable
			MakeButton(BTN_NORMAL,ID_ITEMEFFMOD,0,174,132+17*16,80,15,itemEff[GetItem(curItem)->effect].btnTxt,EffModClick);
			MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,VarName(GetItem(curItem)->effectAmt),NULL);
			break;
		case IEBTN_DIR:
			// choose a direction
			MakeButton(BTN_NORMAL,ID_ITEMEFFMOD,0,174,132+17*16,80,15,itemEff[GetItem(curItem)->effect].btnTxt,EffModClick);
			MakeButton(BTN_STATIC,ID_NAME2,0,256,132+17*16,80,15,directionName[GetItem(curItem)->effectAmt],NULL);
			break;
	}
}

static void ItemSetFlags(void)
{
	int i;
	char t[32];

	ItemEditSetupButtons();

	MakeButton(BTN_NORMAL,ID_NAME,0,164,100,38,16,"Name",NameClick);
	MakeButton(BTN_STATIC,ID_NAME2,0,204,100,100,16,GetItem(curItem)->name,NameClick);

	MakeButton(BTN_CHECK,ID_FLAGS+0,CHECK_OFF,164,118+16*0,100,15,"Shadow",FlagClick);
	MakeButton(BTN_CHECK,ID_FLAGS+1,CHECK_OFF,164,118+16*1,100,15,"Glowing",FlagClick);
	MakeButton(BTN_CHECK,ID_FLAGS+2,CHECK_OFF,164,118+16*2,100,15,"Impassable",FlagClick);
	MakeButton(BTN_CHECK,ID_FLAGS+3,CHECK_OFF,164,118+16*3,100,15,"Bulletproof",FlagClick);
	MakeButton(BTN_CHECK,ID_FLAGS+4,CHECK_OFF,164,118+16*4,100,15,"Can Pick Up",FlagClick);
	MakeButton(BTN_CHECK,ID_FLAGS+5,CHECK_OFF,164,118+16*5,100,15,"Loony Color",FlagClick);
	MakeButton(BTN_CHECK,ID_FLAGS+6,CHECK_OFF,164,118+16*6,100,15,"Use Tile Image",FlagClick);
	MakeButton(BTN_CHECK,ID_FLAGS+7,CHECK_OFF,164,118+16*7,100,15,"Use Custom JSP",FlagClick);
	MakeButton(BTN_CHECK,ID_FLAGS+8,CHECK_OFF,314,118+16*0,100,15,"Bubbles",FlagClick);
	MakeButton(BTN_STATIC,ID_NAME2,0,164,130+16*7,100,16,"Triggers",NULL);

	MakeButton(BTN_CHECK,ID_TRIGGERS+0,CHECK_OFF,174,130+16*8,100,15,"Get",FlagClick);
	MakeButton(BTN_CHECK,ID_TRIGGERS+1,CHECK_OFF,174,130+16*9,100,15,"Shoot",FlagClick);
	MakeButton(BTN_CHECK,ID_TRIGGERS+2,CHECK_OFF,174,130+16*10,100,15,"Player Bump",FlagClick);
	MakeButton(BTN_CHECK,ID_TRIGGERS+3,CHECK_OFF,174,130+16*11,100,15,"Badguy Bump",FlagClick);
	MakeButton(BTN_CHECK,ID_TRIGGERS+4,CHECK_OFF,174,130+16*12,100,15,"Goodguy Bump",FlagClick);
	MakeButton(BTN_CHECK,ID_TRIGGERS+5,CHECK_OFF,174,130+16*13,100,15,"Machete Chop",FlagClick);
	MakeButton(BTN_CHECK,ID_TRIGGERS+6,CHECK_OFF,174,130+16*14,100,15,"Minecart Hit",FlagClick);
	MakeButton(BTN_CHECK,ID_TRIGGERS+7,CHECK_OFF,174,130+16*15,100,15,"Always",FlagClick);

	MakeButton(BTN_NORMAL,ID_ITEMEFF,0,164,130+16*16,50,15,"Effect",EffectClick);
	SetupEffect();

	MakeButton(BTN_NORMAL,ID_ITEMSND,0,164,134+18*16,50,15,"Sound",SoundClick);
	MakeButton(BTN_STATIC,ID_NAME2,0,216,134+18*16,50,15,GetSoundInfo(GetItem(curItem)->sound)->name,NULL);

	MakeButton(BTN_NORMAL,ID_ITEMMSG,0,164,135+19*16,50,15,"Msg",MessageClick);
	MakeButton(BTN_STATIC,ID_NAME2,0,216,135+19*16,50,15,GetItem(curItem)->msg,NULL);

	MakeButton(BTN_NORMAL,ID_RARITY,0,164,136+20*16,50,15,"Rarity",RarityClick);
	sprintf(t,"%d in %d (%0.2f%%)",GetItem(curItem)->rarity,GetTotalRarity(),(float)GetItem(curItem)->rarity*100.0f/(float)GetTotalRarity());
	MakeButton(BTN_STATIC,ID_NAME2,0,216,136+20*16,50,15,t,NULL);

	if(GetItem(curItem)->flags&IF_TILE)
		MakeButton(BTN_NORMAL,ID_PICKSPR,0,480,140,120,15,"Choose Tile",TileClick);
	else
		MakeButton(BTN_NORMAL,ID_PICKSPR,0,480,140,120,15,"Change Sprite",SpriteClick);
	MakeButton(BTN_NORMAL,ID_BRIGHT,0,480,180,80,16,"Bright",BrightClick);
	MakeButton(BTN_STATIC,ID_NAME2,0,480,165,10,10,"Replace",NULL);
	MakeButton(BTN_NORMAL,ID_FROMCOL,0,536,160,16,16,"",ColorClick);
	MakeButton(BTN_STATIC,ID_NAME2,0,554,165,10,10,"With",NULL);
	MakeButton(BTN_NORMAL,ID_TOCOL,0,590,160,16,16,"",ColorClick);

	for(i=0;i<9;i++)
		if(GetItem(curItem)->flags&flags[i])
			SetButtonState(ID_FLAGS+i,CHECK_ON);
	/*
	for(i=0;i<17;i++)
		if(GetItem(curItem)->theme&themes[i])
			SetButtonState(ID_THEMES+i,CHECK_ON);
			*/

	for(i=0;i<8;i++)
		if(GetItem(curItem)->trigger&trigs[i])
			SetButtonState(ID_TRIGGERS+i,CHECK_ON);

	MakeItemList();
}

void ItemEdit_Init(byte modeFrom,world_t *wrld,byte picking)
{
	rememberMode=modeFrom;
	GetDisplayMGL()->MouseTap();
	world=wrld;
	if(picking)
	{
		justPicking=1;
		mode=IMODE_SELECT;
	}
	else
	{
		justPicking=0;
		mode=IMODE_NORMAL;
	}
	curItem=saveCurItem;
	itemStart=saveItemStart;
	curTheme=saveCurTheme;
	if(curItem>=NumItems())
	{
		curItem=ITM_HAMMERUP;
		curTheme=0;
	}
	backColor=1;
	realClick=0;
	ItemSetFlags();
}

void ItemEdit_Exit(void)
{
}

void ItemEdit_Update(int mouseX,int mouseY,MGLDraw *mgl)
{
	int i;
	byte wasInRect;

	switch(mode)
	{
		case IMODE_NORMAL:
			if(!mgl->MouseDown())
				canDrag=1;
			if(mgl->MouseTap())
			{
				realClick=1;
				CheckButtons(mouseX,mouseY);
				if(PointInRect(mouseX,mouseY,470,200,479,358))
				{
					backColor++;
					if(backColor==8)
						backColor=0;
				}
			}
			if(mgl->RMouseTap())
			{
				CheckButtonCallback(mouseX,mouseY,ID_ITEMEFF,EffectRightClick);
				CheckButtonCallback(mouseX,mouseY,ID_ITEMEFFMOD,EffModRightClick);
				if(!(GetItem(curItem)->flags&IF_TILE))
					CheckButtonCallback(mouseX,mouseY,ID_PICKSPR,SpriteRightClick);
			}

			wasInRect=0;
			// dragging sprite around
			if(mgl->MouseDown() && PointInRect(mouseX,mouseY,481,201,637,357) && canDrag)
			{
				GetItem(curItem)->xofs=(mouseX-560);
				GetItem(curItem)->yofs=(mouseY-280);
				wasInRect=1;
			}
			// right mouse click to center the object
			if(mgl->RMouseTap() && PointInRect(mouseX,mouseY,481,201,637,357))
			{
				GetItem(curItem)->xofs=0;
				GetItem(curItem)->yofs=0;
			}

			if(mgl->MouseDown() && PointInRect(mouseX,mouseY,565,180,565+63,196) && canDrag)
			{
				GetItem(curItem)->bright=mouseX-565-31;
				wasInRect=1;
			}
			if(!wasInRect && mgl->MouseDown())
				canDrag=0;
			break;
		case IMODE_SELECT:
			realClick=0;
			for(i=0;i<50;i++)
				CheckButton(mouseX,mouseY,ID_PICKITEM+i);	// select by highlighting

			if(mgl->MouseTap())
			{
				realClick=1;
				for(i=0;i<20;i++)
					CheckButton(mouseX,mouseY,ID_PICKTHEME+i);
				for(i=0;i<50;i++)
					if(CheckButton(mouseX,mouseY,ID_PICKITEM+i))
						i=50;

				if(PointInRect(mouseX,mouseY,470,200,479,358))
				{
					backColor++;
					if(backColor==8)
						backColor=0;
				}
				CheckButton(mouseX,mouseY,ID_EXIT);
				CheckButton(mouseX,mouseY,ID_MOREITEMS);

			}
			break;
		case IMODE_NAME:
		case IMODE_EFFNUM:
		case IMODE_MESSAGE:
		case IMODE_RARITY:
			if(mgl->MouseTap())
			{
				TextDialogClick(mouseX,mouseY);
			}
			if(TextDialogCommand()!=0)
			{
				if(TextDialogCommand()==TM_OKAY)
				{
					if(mode==IMODE_NAME)
						strcpy(GetItem(curItem)->name,GetText());
					else if(mode==IMODE_MESSAGE)
						strcpy(GetItem(curItem)->msg,GetText());
					else if(mode==IMODE_EFFNUM)
					{
						GetItem(curItem)->effectAmt=atoi(GetText());
						if(GetItem(curItem)->effectAmt<itemEff[GetItem(curItem)->effect].buttonMin)
							GetItem(curItem)->effectAmt=itemEff[GetItem(curItem)->effect].buttonMin;
						if(GetItem(curItem)->effectAmt>itemEff[GetItem(curItem)->effect].buttonMax)
							GetItem(curItem)->effectAmt=itemEff[GetItem(curItem)->effect].buttonMax;
					}
					else if(mode==IMODE_RARITY)
					{
						i=atoi(GetText());
						if(i<0)
							i=0;
						if(i>255)
							i=255;
						GetItem(curItem)->rarity=(byte)i;
						SetupRandomItems();
					}
					ItemSetFlags();
				}
				mode=IMODE_NORMAL;
			}
			break;
		case IMODE_PICKMONS:
			if(EditorGetLastPick()!=-1)
				GetItem(curItem)->effectAmt=EditorGetLastPick();
			mode=IMODE_NORMAL;
			ItemSetFlags();
			break;
		case IMODE_PICKSND:
			if(EditorGetLastPick()!=-1)
				GetItem(curItem)->sound=EditorGetLastPick();
			mode=IMODE_NORMAL;
			ItemSetFlags();
			break;
		case IMODE_PICKTILE:
			if(EditorGetLastPick()!=-1)
				GetItem(curItem)->sprNum=EditorGetLastPick();
			mode=IMODE_NORMAL;
			ItemSetFlags();
			break;
		case IMODE_HELP:
			if(mgl->MouseTap())
			{
				if(EditHelpClick(mouseX,mouseY))
				{
					mode=helpRemember;
				}
			}
			break;
	}
}

void ItemEdit_Key(char k)
{
	switch(mode)
	{
		case IMODE_NORMAL:
			if(k==27)
			{
				saveCurTheme=curTheme;
				saveCurItem=curItem;
				saveItemStart=itemStart;
				SetEditMode(EDITMODE_EDIT);
				return;
			}
			break;
		case IMODE_SELECT:
			if(k==27)
			{
				if(justPicking)
				{
					PickedTile(-1);
					if(WhichTool()!=TOOL_SPECIAL)
						SetEditMode(EDITMODE_EDIT);
					else
						SetEditMode(EDITMODE_SPECIAL);
				}
				else
					mode=IMODE_NORMAL;

			}
			break;
		case IMODE_NAME:
		case IMODE_EFFNUM:
		case IMODE_MESSAGE:
		case IMODE_RARITY:
			TextDialogKey(k);
			break;
		case IMODE_HELP:
			if(!EditHelpKey(k))
				mode=helpRemember;
			break;

	}
}

void ItemEdit_Render(int mouseX,int mouseY,MGLDraw *mgl)
{
	mgl->ClearScreen();

	// crosshairs to center the image on
	mgl->FillBox(480,200,638,358,32*backColor+8);
	mgl->FillBox(480,280,638,280,32*backColor+12);
	mgl->FillBox(560,200,560,358,32*backColor+12);
	mgl->Box(480,200,638,358,31);

	// the background color chooser
	mgl->FillBox(470,200,479,358,32*backColor+16);
	mgl->Box(470,200,480,358,31);

	// the sprite and its position
	SetSpriteConstraints(481,201,637,357);
	InstaRenderItem(560,280,curItem,0,mgl);
	ClearSpriteConstraints();
	mgl->Box(560-TILE_WIDTH/2,280-TILE_HEIGHT/2,560+TILE_WIDTH/2-1,280+TILE_HEIGHT/2-1,32*backColor+18);

	// misc. lines and the background behind the buttons
	mgl->FillBox(0,95,639,95,31);
	mgl->FillBox(160,95,160,479,31);
	mgl->FillBox(0,96,159,479,32*1+4);

	switch(mode)
	{
		case IMODE_NORMAL:
			RenderButtons(mouseX,mouseY,mgl);
			break;
		case IMODE_SELECT:
			int i;

			if(justPicking)
				RenderButtons(-1,-1,mgl);
			for(i=0;i<50;i++)
			{
				RenderButton(mouseX,mouseY,ID_PICKTHEME+i,mgl);
				RenderButton(mouseX,mouseY,ID_PICKITEM+i,mgl);
			}
			RenderButton(mouseX,mouseY,ID_EXIT,mgl);
			RenderButton(mouseX,mouseY,ID_MOREITEMS,mgl);
			Print(400,82,"Click on an item to select it!",0,1);
			break;
		case IMODE_NAME:
		case IMODE_EFFNUM:
		case IMODE_MESSAGE:
		case IMODE_RARITY:
			RenderButtons(-1,-1,mgl);
			RenderTextDialog(mouseX,mouseY,mgl);
			break;
		case IMODE_HELP:
			RenderButtons(-1,-1,mgl);
			RenderEditHelp(mouseX,mouseY,mgl);
			break;
	}

	if(mode!=IMODE_HELP && (mode!=IMODE_SELECT || justPicking))
	{
		// brightness knob 560,180
		mgl->FillBox(565,187,565+63,188,16);
		mgl->FillBox(565+GetItem(curItem)->bright+31-4,180,565+GetItem(curItem)->bright+31+4,196,16);
		mgl->Box(565+GetItem(curItem)->bright+31-4,180,565+GetItem(curItem)->bright+31+4,196,31);

		// colors for color-shifting guys
		mgl->FillBox(538,162,538+12,162+12,GetItem(curItem)->fromColor*32+16);
		mgl->FillBox(592,162,592+12,162+12,GetItem(curItem)->toColor*32+16);
	}
}

void ItemEditHelp(void)
{
	if (mode == IMODE_HELP)
		return;

	helpRemember=mode;
	if(mode==IMODE_SELECT)
		InitEditHelp(HELP_ITEMPICK);
	else if(mode!=IMODE_HELP)
		InitEditHelp(HELP_ITEMEDIT);
	mode=IMODE_HELP;
}
