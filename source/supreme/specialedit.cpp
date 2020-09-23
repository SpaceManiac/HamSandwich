#include "winpch.h"
#include "specialedit.h"
#include "dialogbits.h"
#include "filedialog.h"
#include "items.h"
#include "textdialog.h"
#include "monsteredit.h"
#include "itemedit.h"
#include "vars.h"
#include "terrainedit.h"
#include "soundedit.h"
#include "edithelp.h"
#include "viewdialog.h"
#include "shop.h"

// Originally 14
#define TRG_H 14
#define EFF_H 14
#define TRG_W 128
#define EFF_W 128
#define TRG_L 24
#define EFF_L 24

#define SMODE_NORMAL	0
#define SMODE_USES		1
#define SMODE_PICKTRIG	2
#define SMODE_PICKGUY	3
#define SMODE_VALUE		4
#define SMODE_VALUE2	5
#define SMODE_PICKXY	6
#define SMODE_PICKRECT	7
#define SMODE_PICKRECT2	8
#define SMODE_PICKITEM	9
#define SMODE_FVALUE	10
#define SMODE_PICKFLOOR	11
#define SMODE_PICKEFF	12
#define SMODE_MESSAGE	13
#define SMODE_PICKSOUND	14
#define SMODE_PICKXY2	15
#define SMODE_PICKFLOOR2 16
#define SMODE_PICKBMP	17
#define SMODE_PICKITEM2	18
#define SMODE_PICKRECT3	19
#define SMODE_PICKGUY2	20
#define SMODE_FVALUE2	21
#define SMODE_PICKSONG	22
#define SMODE_FVALUE3	23
#define SMODE_PICKITEM3	24
#define SMODE_HELP		25
#define SMODE_PICKRECT3T 26

#define ID_EXIT		1
#define ID_USES		2
#define ID_TRGUP	3
#define ID_TRGDN	4
#define ID_EFFUP	5
#define ID_EFFDN	6

#define ID_TRIG0	100
#define ID_TRIG1	200
#define ID_TRIG2	300
#define ID_TRIG3	400
#define ID_TRIG4	500

#define ID_EFF0		600
#define ID_EFF1		700
#define ID_EFF2		800
#define ID_EFF3		900
#define ID_EFF4		1000

#define OFS_NOT		0
#define OFS_ANDOR	1
#define OFS_SELECT	2
#define OFS_CUSTOM	10

static byte mode;
static special_t spcl;
static byte curTrig,curEff,effMode;
static byte rightClick,prevType;
static byte specialNum;
static int selectY;
static byte previousMap;
static byte helpRemember;
static byte trgStart,effStart;

static char trigName[][32]={
	"Unused",
	"Step On/Near",
	"Step In Rect",
	"Have Item",
	"Shoot Item/Wall",
	"Monsters Alive",
	"Monster Killed",
	"Floor Is Tile",
	"Passed Levels",
	"Passed Level",
	"Variables",
	"Timed",
	"Delayed",
	"Random",
	"Chain Off Other",
	"Tiles In Rect",
	"Monster Life",
	"Step On Tile",
	"Get Item At",
	"Item On Map",
	"Monster Awake",
	"Items In Level",
	"Compare Areas",
	"Compare Vars",
	"Monster In Rect",
	"Item In Rect",
	"Difficulty",
	"Keypress",
	"Playing As",
	"Monster Color",
	"Equation",
	"Var Equation",
	"Bullet In Rect",
	"Check Timer",
	"Monster Hurt",
	"Game Mode",
	"Monster Status",
	"Rage Activated",
	"Blt @ Item/Wall",
	"Monster Age",
	"Stand On Tile",
};

static char effName[][32]={
	"Unused",
	"Message",
	"Sound",
	"Song",
	"Win Level",
	"Goto Level",
	"Teleport",
	"Change Tiles",
	"Summon Monster",
	"Light/Dark",
	"Show File",
	"Change Items",
	"Copy Map",
	"Kill Monsters",
	"Change Monsters",
	"Change Teams",
	"Delete Special",
	"Set Variable",
	"Light Rect",
	"Level Flags",
	"Old Toggle",
	"Set Life",
	"Force Weapon",
	"Tag Target",
	"Tag Monster",
	"Monster Item",
	"Tile Var",
	"Change Life",
	"Monster AI",
	"Monster Name",
	"Monster Color",
	"Monster Bright",
	"Play As",
	"Monster Sprites",
	"Item Sprites",
	"Variable Bar",
	"Summon Bullet",
	"Change Bullet",
	"Change Timer",
	"Camera @ Point",
	"Camera @ Monstr",
	"DynWater Color",
	"DynWater BMP",
	"Transfrm Player",
	"Mark As Boss",
	"Goto Lvl. (NoFX)",
};

static char lvlFlagName[][16]={
	"Snowing",
	"Raining",
	"Hub Level",
	"Secret Level",
	"Torch Lit",
	"Lantern Lit",
	"Star Background",
	"Underwater",
	"Underlava",
	"Stealth",
	"Wavy",
	"Oxygen Meter",
	"Dynamic Water",
	"Dynamic Lava",
	"Var 0 Bar",
};

static char wpnName[][16]={
	"None",
	"Missiles",
	"AK-8087",
	"Bombs",
	"Toaster",
	"Power Armor",
	"Big Axe",
	"Zap Wand",
	"Spears",
	"Machete",
	"Mines",
	"Turrets",
	"Mind Control",
	"Reflect",
	"Jetpack",
	"Swapgun",
	"Torch",
	"Scanner",
	"Mini-Sub",
	"Freeze Ray",
	"Stopwatch",
	"Boomerang",
	"Potted Cactus",
	"Bionic Arm",
	"Water Gun",
	"Megaphone",
	"Cucurbinator",
	"Death Ray",
	"Whoopie Cushion",
	"Abyssinator",
	"Medic Kit",
	"Magic Wand",
	"Burninator",
	"B's Fav. Gun",
	"AK-Blaster",
	"Snow Blower",
	"Confuse Ray",
	"Grenadier"};

static char bulletName[][32]={
	"Anything",
	"Hammer",
	"Bouncy Hammer",
	"Missile",
	"Flame",
	"AK-8087 Shot",
	"Acid",
	"Cherry Bomb",
	"Explosion",
	"Red Bullet",
	"Megabeam Source",
	"Megabeam Part",
	"Megabeam End",
	"Evil Flame",
	"Spore",
	"Mushroom",
	"Grenade",
	"Grenade Boom",
	"SDZ Shockwave",
	"Missile Boom",
	"Snowball",
	"Big Snowball",
	"Ice Spike",
	"Rock",
	"Cactus Spine",
	"Evil Hammer",
	"Power Shell",
	"Big Axe",
	"Lightning",
	"Spear",
	"Machete",
	"Landmine",
	"Evil Spear",
	"Orbiter",
	"Green Bullet",
	"Ball Lightning",
	"Zap Wand Shock",
	"Mind Control",
	"Reflect Shield",
	"Swap Gun",
	"Water Shot",
	"Orbit Bomber",
	"Harpoon",
	"Scanner",
	"Scanner Shot",
	"Torpedo",
	"Dirt Spike",
	"Paper",
	"Scanner Lock",
	"Bubble",
	"Freeze Ray",
	"Bubble Pop",
	"Harmless Boom",
	"Cheese Hammer",
	"Evil Freeze",
	"Lunachick Ray",
	"Bouncy Lunachick",
	"Floaty Flame",
	"Sitting Flame",
	"Evil Sitting Flame",
	"Deadly Laser",
	"Evil Green Bullet",
	"Weather Orbiter",
	"Good Water Shot",
	"Orbit Poison",
	"Wind Missile",
	"Evil Face",
	"Homing LunaBullet",
	"Rainbow LunaBullet",
	"Eye Guy Wave",
	"Laser Beam",
	"Laser Beam End",
	"Slug Slime",
	"Red Grenade",
	"Red Grenade Boom",
	"Big Yellow Bullet",
	"Mega Explosion",
	"Bouncy Energy Bullet",
	"Rocket",
	"Skull",
	"Mystic Wand",
	"Boomerang",
	"Fart Cloud",
	"PUMPKIN!",
	"Hotfoot Flame",
	"All-Directional Flame",
	"Bouncy Mystic Wand",
	"Black Hole Shot",
	"Black Hole",
	"Alien Egg",
	"Flying Pie",
	"Red Bullet (ZigZag)",
	"Red Bullet (CW)",
	"Red Bullet (CCW)",
	"Red Bullet (CW2)",
	"Red Bullet (CCW2)",
	"Homing Cherry Bomb",
	"Flame Wall",
	"Claw Shot",
	"Ice Shard",
	"Hot Pants",
	"Witch Wand",
	"Confusion Shot",
	"Health Spore",
	"BIG Rock",
	"Red Shockwave",
	"Freeze Snowball",
	"Evil LunaChick",
};
#define MAX_BULLETS MAX_BULTYPES

static void SetupTriggerButtons(int t,int y);
static void SetupEffectButtons(int t,int y);

static char *WeaponName(byte n)
{
	return wpnName[n];
}

static void SpecialEditSetupButtons(void);
static void PageClick(int id)
{
	switch(id) {
		case ID_TRGUP:
			if (trgStart > 0) --trgStart;
			break;
		case ID_TRGDN:
			if (trgStart < NUM_TRIGGERS - 5) ++trgStart;
			break;
		case ID_EFFUP:
			if (effStart > 0) --effStart;
			break;
		case ID_EFFDN:
			if (effStart < NUM_EFFECTS - 5) ++effStart;
			break;
	}
	SpecialEditSetupButtons();
}

static void ExitClick(int id)
{
	if(rightClick)
		return;

	if(!CheckSpecial(&spcl))
		spcl.x=255;
	memcpy(GetSpecial(specialNum),&spcl,sizeof(special_t));
	SetEditMode(EDITMODE_EDIT);
	MakeNormalSound(SND_MENUCLICK);
	PickedTile(-1);
}

static void UsesClick(int id)
{
	if(rightClick)
	{
		spcl.uses=0;
		MakeNormalSound(SND_MENUCLICK);
		return;
	}

	MakeNormalSound(SND_MENUCLICK);
	mode=SMODE_USES;
	InitTextDialog("Enter # of uses, 0-255 (0=unlimited)","",3);
}

static void NotClick(int id)
{
	int t;

	t=trgStart + id/100-1;

	spcl.trigger[t].flags^=TF_NOT;

	SetButtonState(id,((spcl.trigger[t].flags&TF_NOT)!=0)*CHECK_ON);

}

static void ChooseTriggerClick(int id)
{
	if(rightClick)
	{
		spcl.trigger[trgStart + id/100-1].type=TRG_NONE;
		MakeNormalSound(SND_MENUCLICK);
		return;
	}

	MakeNormalSound(SND_MENUCLICK);
	mode=SMODE_PICKTRIG;
	curTrig=(trgStart + id/100)-1;
	prevType=spcl.trigger[curTrig].type;
	selectY=(curTrig*38+30)-(TRG_H*TRG_L)/2;
	if(selectY<0)
		selectY=0;
	if(selectY+TRG_H*MAX_TRIGGER>475)
		selectY=475-TRG_H*TRG_L;
}

static void ChooseEffectClick(int id)
{
	if(rightClick)
	{
		spcl.effect[effStart + (id-ID_EFF0)/100].type=EFF_NONE;
		MakeNormalSound(SND_MENUCLICK);
		return;
	}

	MakeNormalSound(SND_MENUCLICK);
	mode=SMODE_PICKEFF;
	curEff=effStart + (id-ID_EFF0)/100;
	prevType=spcl.effect[curEff].type;

	selectY=(curEff*38+264)-(EFF_H*EFF_L)/2;
	if(selectY<0)
		selectY=0;
	if(selectY+EFF_H*EFF_MAX>475)
		selectY=475-EFF_H*EFF_L;
}

static void MonsterClick(int id)
{
	if(id<ID_EFF0)
	{
		effMode=0;
		curTrig=trgStart + id/100-1;
	}
	else
	{
		effMode=1;
		curEff=effStart + (id-ID_EFF0)/100;
	}

	if(rightClick)
	{
		SetEditMode(EDITMODE_PICKENEMY);
		MonsterEdit_Init(EDITMODE_SPECIAL,EditorGetWorld());
		mode=SMODE_PICKGUY;
	}
	else
	{
		if(effMode==0)
		{
			switch(spcl.trigger[curTrig].value)
			{
				case MONS_GOODGUY:
					spcl.trigger[curTrig].value=MONS_BADGUY;
					break;
				case MONS_BADGUY:
					spcl.trigger[curTrig].value=MONS_ANYBODY;
					break;
				case MONS_ANYBODY:
					spcl.trigger[curTrig].value=MONS_NONPLAYER;
					break;
				case MONS_NONPLAYER:
					spcl.trigger[curTrig].value=MONS_TAGGED;
					break;
				case MONS_TAGGED:
					spcl.trigger[curTrig].value=MONS_BOSS;
					break;
				case MONS_BOSS:
					spcl.trigger[curTrig].value=MONS_PLAYER;
					break;
				case MONS_PLAYER:
					spcl.trigger[curTrig].value=MONS_BOUAPHA;
					break;
				default:	// if on a specific monster
					spcl.trigger[curTrig].value=MONS_GOODGUY;
					break;
			}
			SetupTriggerButtons((curTrig-trgStart),(curTrig-trgStart)*38+30);
		}
		else
		{
			switch(spcl.effect[curEff].value)
			{
				case MONS_GOODGUY:
					spcl.effect[curEff].value=MONS_BADGUY;
					break;
				case MONS_BADGUY:
					spcl.effect[curEff].value=MONS_ANYBODY;
					break;
				case MONS_ANYBODY:
					spcl.effect[curEff].value=MONS_NONPLAYER;
					break;
				case MONS_NONPLAYER:
					spcl.effect[curEff].value=MONS_TAGGED;
					break;
				case MONS_TAGGED:
					spcl.effect[curEff].value=MONS_BOSS;
					break;
				case MONS_BOSS:
					spcl.effect[curEff].value=MONS_PLAYER;
					break;
				case MONS_PLAYER:
					spcl.effect[curEff].value=MONS_BOUAPHA;
					break;
				default:	// if on a specific monster
					spcl.effect[curEff].value=MONS_GOODGUY;
					break;
			}
			SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
		}
	}
}

static void Monster2Click(int id)
{
	effMode=1;
	curEff=effStart + (id-ID_EFF0)/100;

	SetEditMode(EDITMODE_PICKENEMY);
	MonsterEdit_Init(EDITMODE_SPECIAL,EditorGetWorld());
	mode=SMODE_PICKGUY2;
}

static void SoundClick(int id)
{
	curEff=effStart + (id-ID_EFF0)/100;

	SetEditMode(EDITMODE_SOUND);
	SoundEdit_InitPicker(EDITMODE_SPECIAL,EditorGetWorld());
	mode=SMODE_PICKSOUND;
}

static void MessageClick(int id)
{
	curEff=effStart + (id-ID_EFF0)/100;

	if(rightClick)
	{
		spcl.effect[curEff].text[0]='\0';
		MakeNormalSound(SND_MENUCLICK);
		return;
	}

	MakeNormalSound(SND_MENUCLICK);
	mode=SMODE_MESSAGE;
	InitTextDialog("Enter a message!","",31);
}

static void EquationClick(int id)
{
	if(id<ID_EFF0)
		curEff=trgStart + (id-ID_TRIG0)/100;
	else
		curEff=effStart + (id-ID_EFF0)/100;

	if(rightClick)
	{
		spcl.effect[curEff].text[0]='\0';
		MakeNormalSound(SND_MENUCLICK);
		return;
	}

	MakeNormalSound(SND_MENUCLICK);
	mode=SMODE_MESSAGE;
	InitTextDialog("Enter some mathematical gizmos!","",31);
}

static void NumberClick(int id)
{
	if(id<ID_EFF0)
	{
		effMode=0;
		curTrig=trgStart + (id/100)-1;
	}
	else
	{
		effMode=1;
		curEff=effStart + (id-ID_EFF0)/100;
	}
	// enter a number for value
	if(rightClick)
	{
		if(!effMode)
			spcl.trigger[curTrig].value=0;
		else
			spcl.effect[curEff].value=0;
		MakeNormalSound(SND_MENUCLICK);
		return;
	}

	MakeNormalSound(SND_MENUCLICK);
	mode=SMODE_VALUE;
	InitTextDialog("Enter a number!","",16);
}

static void FloatClick(int id)
{
	curTrig=trgStart + (id/100)-1;
	// enter a number for value
	if(rightClick)
	{
		spcl.trigger[curTrig].value=0;
		MakeNormalSound(SND_MENUCLICK);
		return;
	}

	MakeNormalSound(SND_MENUCLICK);
	mode=SMODE_FVALUE;
	InitTextDialog("Enter a number (decimals are ok)!","",16);
}

static void Float2Click(int id)
{
	curTrig=trgStart + (id/100)-1;
	// enter a number for value
	if(rightClick)
	{
		spcl.trigger[curTrig].value=0;
		MakeNormalSound(SND_MENUCLICK);
		return;
	}

	MakeNormalSound(SND_MENUCLICK);
	mode=SMODE_FVALUE2;
	InitTextDialog("Enter a number (decimals are ok)!","",16);
}

static void Float3Click(int id)
{
	curTrig=trgStart + (id/100)-1;
	// enter a number for value
	if(rightClick)
	{
		spcl.trigger[curTrig].value2=0;
		MakeNormalSound(SND_MENUCLICK);
		return;
	}

	MakeNormalSound(SND_MENUCLICK);
	mode=SMODE_FVALUE3;
	InitTextDialog("Enter a number (decimals are ok)!","",16);
}

static void Number2Click(int id)
{
	if(id<ID_EFF0)
	{
		effMode=0;
		curTrig=trgStart + (id/100)-1;
	}
	else
	{
		effMode=1;
		curEff=effStart + (id-ID_EFF0)/100;
	}
	// enter a number for value2
	if(rightClick)
	{
		if(!effMode)
		{
			spcl.trigger[curTrig].value2=0;
			SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
		}
		else
		{
			spcl.effect[curEff].value2=0;
			SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
		}
		MakeNormalSound(SND_MENUCLICK);
		return;
	}

	MakeNormalSound(SND_MENUCLICK);
	mode=SMODE_VALUE2;
	InitTextDialog("Enter a number!","",16);
}

static void XYClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	// click on a target spot
	if(id<ID_EFF0)
	{
		effMode=0;
		curTrig=trgStart + (id/100)-1;
	}
	else
	{
		effMode=1;
		curEff=effStart + (id-ID_EFF0)/100;
	}
	if(rightClick)
	{
		if(effMode==0)
		{
			spcl.trigger[curTrig].x=spcl.x;
			spcl.trigger[curTrig].y=spcl.y;
			SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
		}
		else
		{
			spcl.effect[curEff].x=spcl.x;
			spcl.effect[curEff].y=spcl.y;
			SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
		}
		MakeNormalSound(SND_MENUCLICK);
		return;
	}
	else
	{
		mode=SMODE_PICKXY;
		SetEditMode(EDITMODE_PICKSPOT);
	}
}

static void XY2Click(int id)
{
	// clicking on a target in a different map!

	MakeNormalSound(SND_MENUCLICK);
	// click on a target spot
	effMode=1;
	curEff=effStart + (id-ID_EFF0)/100;
	if(rightClick)
	{
		spcl.effect[curEff].x=255;
		spcl.effect[curEff].y=255;
		SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
		MakeNormalSound(SND_MENUCLICK);
		return;
	}
	else
	{
		previousMap=EditorGetMapNum();
		EditorSelectMap(spcl.effect[curEff].value);
		mode=SMODE_PICKXY2;
		SetEditMode(EDITMODE_PICKSPOT);
	}
}

static void XY3Click(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	// click on a target spot
	if(id<ID_EFF0)
	{
		effMode=0;
		curTrig=trgStart + (id/100)-1;
	}
	else
	{
		effMode=1;
		curEff=effStart + (id-ID_EFF0)/100;
	}
	if(rightClick)
	{
		if(effMode==0)
		{
			spcl.trigger[curTrig].x=255;
			spcl.trigger[curTrig].y=255;
			SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
			return;
		}
		else
		{
			spcl.effect[curEff].x=255;
			spcl.effect[curEff].y=255;
			SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
			return;
		}
	}
	else
	{
		mode=SMODE_PICKXY;
		SetEditMode(EDITMODE_PICKSPOT);
	}
}

static void ItemClick(int id)
{
	curTrig=trgStart + id/100-1;

	SetEditMode(EDITMODE_ITEM);
	ItemEdit_Init(EDITMODE_SPECIAL,EditorGetWorld(),1);
	mode=SMODE_PICKITEM;
	MakeNormalSound(SND_MENUCLICK);
}

static void KeypressClick(int id)
{
	curTrig=trgStart + id/100-1;

	spcl.trigger[curTrig].value*=2;
	if(spcl.trigger[curTrig].value>CONTROL_B2)
		spcl.trigger[curTrig].value=1;

	MakeNormalSound(SND_MENUCLICK);
	SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
}

static void PlayAsClick(int id)
{
	if(id<ID_EFF0)
	{
		curTrig=trgStart + id/100-1;

		spcl.trigger[curTrig].value++;
		if(spcl.trigger[curTrig].value>PLAY_LOONY)
			spcl.trigger[curTrig].value=PLAY_BOUAPHA;

		MakeNormalSound(SND_MENUCLICK);
		SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
	}
	else
	{
		curEff=effStart + (id-ID_EFF0)/100;
		MakeNormalSound(SND_MENUCLICK);

		spcl.effect[curEff].value++;
		if(spcl.effect[curEff].value>PLAY_LOONY)
			spcl.effect[curEff].value=PLAY_BOUAPHA;

		SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
	}
}

static void Item2Click(int id)
{
	curEff=effStart + (id-ID_EFF0)/100;
	MakeNormalSound(SND_MENUCLICK);

	if(rightClick)
	{
		spcl.effect[curEff].value=0;
		SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
		return;
	}
	SetEditMode(EDITMODE_ITEM);
	ItemEdit_Init(EDITMODE_SPECIAL,EditorGetWorld(),1);
	mode=SMODE_PICKITEM2;
}

static void MonsItemClick(int id)
{
	curEff=effStart + (id-ID_EFF0)/100;
	MakeNormalSound(SND_MENUCLICK);

	if(rightClick)
	{
		if(spcl.effect[curEff].value2==ITM_NONE)
			spcl.effect[curEff].value2=ITM_RANDOM;
		else if(spcl.effect[curEff].value2==ITM_RANDOM)
			spcl.effect[curEff].value2=ITM_HAMMERUP;
		else
			spcl.effect[curEff].value2=ITM_NONE;

		SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
		return;
	}
	SetEditMode(EDITMODE_ITEM);
	ItemEdit_Init(EDITMODE_SPECIAL,EditorGetWorld(),1);
	mode=SMODE_PICKITEM3;
}

static void TileClick(int id)
{
	if(id<ID_EFF0)
	{
		effMode=0;
		curTrig=trgStart + id/100-1;
	}
	else
	{
		effMode=1;
		curEff=effStart + (id-ID_EFF0)/100;
	}

	SetEditMode(EDITMODE_TERRAIN);
	TerrainEdit_InitPicker(EDITMODE_SPECIAL,EditorGetWorld());
	mode=SMODE_PICKFLOOR;
}

static void LevelNameClick(int id)
{
	if(!rightClick)
	{
		if(id<ID_EFF0)
		{
			effMode=0;
			curTrig=trgStart + id/100-1;
			spcl.trigger[curTrig].value++;
			if(spcl.trigger[curTrig].value>=EditorGetWorld()->numMaps)
				spcl.trigger[curTrig].value=0;
			SetupTriggerButtons(curTrig,curTrig*38+30);
		}
		else
		{
			effMode=1;
			curEff=effStart + (id-ID_EFF0)/100;
			spcl.effect[curEff].value++;
			if(spcl.effect[curEff].value>=EditorGetWorld()->numMaps)
				spcl.effect[curEff].value=0;
			SetupEffectButtons(curEff,curEff*38+264);
		}
	}
	else
	{
		if(id<ID_EFF0)
		{
			effMode=0;
			curTrig=trgStart + id/100-1;
			spcl.trigger[curTrig].value--;
			if(spcl.trigger[curTrig].value<0)
				spcl.trigger[curTrig].value=EditorGetWorld()->numMaps-1;
			SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
		}
		else
		{
			effMode=1;
			curEff=effStart + (id-ID_EFF0)/100;
			spcl.effect[curEff].value--;
			if(spcl.effect[curEff].value<0)
				spcl.effect[curEff].value=EditorGetWorld()->numMaps-1;
			SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
		}
	}
}

static void Tile2Click(int id)
{
	if(id<ID_EFF0)
	{
		effMode=0;
		curTrig=trgStart + id/100-1;
	}
	else
	{
		effMode=1;
		curEff=effStart + (id-ID_EFF0)/100;
	}

	if(rightClick)
	{
		if(effMode==1)
			spcl.effect[curEff].value2=0;
		else
			spcl.trigger[curTrig].value2=0;
		MakeNormalSound(SND_MENUCLICK);
		SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
		return;
	}
	SetEditMode(EDITMODE_TERRAIN);
	TerrainEdit_InitPicker(EDITMODE_SPECIAL,EditorGetWorld());
	mode=SMODE_PICKFLOOR2;
}

static void RectClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	// click on a target rectangle
	curTrig=trgStart + (id/100)-1;
	if(rightClick)
	{
		spcl.trigger[curTrig].x=spcl.x;
		spcl.trigger[curTrig].y=spcl.y;
		spcl.trigger[curTrig].value2=(spcl.x+spcl.y*256);
		MakeNormalSound(SND_MENUCLICK);
		return;
	}
	else
	{
		mode=SMODE_PICKRECT;
		SetEditMode(EDITMODE_PICKRSPOT);
	}
}

static void Rect2Click(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	// click on a target rectangle
	curEff=effStart + (id-ID_EFF0)/100;
	effMode=1;

	mode=SMODE_PICKRECT2;
	SetEditMode(EDITMODE_PICKRSPOT);
}

static void Rect3Click(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	// click on a target spot
	curEff=effStart + (id-ID_EFF0)/100;
	effMode=1;

	mode=SMODE_PICKRECT3;
	SetEditPickerRect((((word)spcl.effect[curEff].value)%256)-spcl.effect[curEff].x,
					  (((word)spcl.effect[curEff].value)/256)-spcl.effect[curEff].y);
	SetEditMode(EDITMODE_PICKSPOT2);
}

static void Rect3TClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	// click on a target spot
	curTrig=trgStart + id/100-1;
	effMode=0;

	mode=SMODE_PICKRECT3T;
	SetEditPickerRect((((word)spcl.trigger[curTrig].value2)%256)-spcl.trigger[curTrig].x,
					  (((word)spcl.trigger[curTrig].value2)/256)-spcl.trigger[curTrig].y);
	SetEditMode(EDITMODE_PICKSPOT2);
}

static void LessMoreClick(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	t=trgStart + id/100-1;

	if(spcl.trigger[t].flags&TF_LESS)
	{
		spcl.trigger[t].flags&=(~TF_LESS);
		spcl.trigger[t].flags|=TF_MORE;
	}
	else if(spcl.trigger[t].flags&TF_MORE)
	{
		spcl.trigger[t].flags&=(~TF_MORE);
	}
	else
	{
		spcl.trigger[t].flags|=TF_LESS;
	}
	SetupTriggerButtons(t-trgStart,(t-trgStart)*38+30);
}

static void LessClick(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	t=id/100-1;

	spcl.trigger[trgStart + t].flags^=TF_LESS;
	SetupTriggerButtons(t,t*38+30);
}

static void PlayerTargetClick(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	t=(id-ID_EFF0)/100;

	spcl.effect[effStart + t].flags^=EF_PLAYER;
	SetupEffectButtons(t,t*38+264);
}

static void PlayerTarget2Click(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	t=effStart + (id-ID_EFF0)/100;

	spcl.effect[t].flags++;
	if(spcl.effect[t].flags>EF_TAGGED)
		spcl.effect[t].flags=0;
	SetupEffectButtons(t-effStart,(t-effStart)*38+264);
}

static void ContiguousClick(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	t=effStart + (id-ID_EFF0)/100;

	if(spcl.effect[t].flags&EF_ALL)
	{
		spcl.effect[t].flags&=(~EF_ALL);	// go to single
	}
	else if((spcl.effect[t].flags&(EF_ALL|EF_CONTIGUOUS))==0)
	{
		spcl.effect[t].flags|=EF_CONTIGUOUS;	// go to contiguous
	}
	else
	{
		spcl.effect[t].flags&=(~EF_CONTIGUOUS);
		spcl.effect[t].flags|=EF_ALL;	// go to all
	}

	SetupEffectButtons(t-effStart,(t-effStart)*38+264);
}

static void ToggleClick(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	t=(id-ID_EFF0)/100;

	spcl.effect[effStart+t].flags^=EF_TOGGLE;
	SetupEffectButtons(t,t*38+264);
}

static void TeamClick(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	t=effStart + (id-ID_EFF0)/100;

	spcl.effect[t].value2++;
	if(spcl.effect[t].value2>2)
		spcl.effect[t].value2=0;
	SetupEffectButtons(t-effStart,(t-effStart)*38+264);
}

static void VarClick(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	if(id<ID_EFF0)
	{
		effMode=0;
		t=trgStart + id/100-1;
	}
	else
	{
		effMode=1;
		t=effStart + (id-ID_EFF0)/100;
	}

	if(rightClick)
	{
		if(!effMode)
			spcl.trigger[t].value^=VAR_LOCAL;
		else
			spcl.effect[t].value^=VAR_LOCAL;
	}
	else
	{
		if(!effMode)
		{
			spcl.trigger[t].value++;
			if(spcl.trigger[t].value==VAR_LOCAL)
				spcl.trigger[t].value=VAR_GLOBAL;
			else if(spcl.trigger[t].value==VAR_LOCAL+NUM_VARS)
				spcl.trigger[t].value=VAR_LOCAL;
		}
		else
		{
			spcl.effect[t].value++;
			if(spcl.effect[t].value==VAR_LOCAL)
				spcl.effect[t].value=VAR_GLOBAL;
			else if(spcl.effect[t].value==VAR_LOCAL+NUM_VARS)
				spcl.effect[t].value=VAR_LOCAL;
		}
	}
	if(!effMode)
		SetupTriggerButtons(t-trgStart,(t-trgStart)*38+30);
	else
		SetupEffectButtons(t-effStart,(t-effStart)*38+264);
}

static void Var2Click(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	if(id<ID_EFF0)
	{
		effMode=0;
		t=trgStart + id/100-1;
	}
	else
	{
		effMode=1;
		t=effStart + (id-ID_EFF0)/100;
	}

	if(rightClick)
	{
		if(!effMode)
			spcl.trigger[t].value2^=VAR_LOCAL;
		else
			spcl.effect[t].value2^=VAR_LOCAL;
	}
	else
	{
		if(!effMode)
		{
			spcl.trigger[t].value2++;
			if(spcl.trigger[t].value2==VAR_LOCAL)
				spcl.trigger[t].value2=VAR_GLOBAL;
			else if(spcl.trigger[t].value2==VAR_LOCAL+NUM_VARS)
				spcl.trigger[t].value2=VAR_LOCAL;
		}
		else
		{
			spcl.effect[t].value2++;
			if(spcl.effect[t].value2==VAR_LOCAL)
				spcl.effect[t].value2=VAR_GLOBAL;
			else if(spcl.effect[t].value2==VAR_LOCAL+NUM_VARS)
				spcl.effect[t].value2=VAR_LOCAL;
		}
	}
	if(!effMode)
		SetupTriggerButtons(t-trgStart,(t-trgStart)*38+30);
	else
		SetupEffectButtons(t-effStart,(t-effStart)*38+264);
}

static void LevelFlagClick(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	effMode=1;
	t=effStart + (id-ID_EFF0)/100;

	spcl.effect[t].value++;
	if(spcl.effect[t].value==NUM_LVL_FLAGS)
		spcl.effect[t].value=0;

	SetupEffectButtons(t-effStart,(t-effStart)*38+264);
}

static void AndOrClick(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	t=id/100-1;
	ClearButtons(id,id);

	spcl.trigger[trgStart + t].flags^=TF_AND;
	if(spcl.trigger[trgStart + t].flags&TF_AND)
		MakeButton(BTN_NORMAL,ID_TRIG0+OFS_ANDOR+100*t,0,600,30+t*38+10,30,14,"AND",AndOrClick);
	else
		MakeButton(BTN_NORMAL,ID_TRIG0+OFS_ANDOR+100*t,0,600,30+t*38+10,30,14,"OR",AndOrClick);
}

static void NoFXClick(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	effMode=1;
	t=(id-ID_EFF0)/100;

	spcl.effect[effStart+t].flags^=EF_NOFX;
	SetupEffectButtons(t,t*38+264);
}

static void ColorClick1(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	if(id<ID_EFF0)
	{
		effMode=0;
		t=trgStart + id/100-1;

		spcl.trigger[t].value2++;
		if(spcl.trigger[t].value2>7)
			spcl.trigger[t].value2=0;
		SetupTriggerButtons(t-trgStart,(t-trgStart)*38+30);
	}
	else
	{
		effMode=1;
		t=effStart + (id-ID_EFF0)/100;
		spcl.effect[t].value2++;
		if((spcl.effect[t].value2%256)>7)
			spcl.effect[t].value2=(spcl.effect[t].value2/256)*256;
		SetupEffectButtons(t-effStart,(t-effStart)*38+264);
	}
}

static void ColorClick2(int id)
{
	int t;

	MakeNormalSound(SND_MENUCLICK);

	effMode=1;
	t=effStart + (id-ID_EFF0)/100;

	spcl.effect[t].value2+=256;
	if((spcl.effect[t].value2/256)>7)
		spcl.effect[t].value2=(spcl.effect[t].value2%256);
	SetupEffectButtons(t-effStart,(t-effStart)*38+264);
}

static void PicNameClick(int id)
{
	curEff=effStart + (id-ID_EFF0)/100;

	mode=SMODE_PICKBMP;
	InitFileDialog("user/*.*",FM_LOAD|FM_EXIT|FM_NOWAVS,"");
}
static void PicNameClick2(int id)
{
	curEff=effStart + (id-ID_EFF0)/100;

	mode=SMODE_PICKBMP;
	InitFileDialog("user/*.*",FM_LOAD|FM_EXIT|FM_PICSONLY,"");
}

static void SongClick(int id)
{
	curEff=effStart + (id-ID_EFF0)/100;

	if(rightClick)
	{
		strcpy(spcl.effect[curEff].text,"");
	}
	else
	{
		mode=SMODE_PICKSONG;
		InitFileDialog("music/*.ogg",FM_LOAD|FM_EXIT|FM_PLAYSONGS,spcl.effect[curEff].text);
	}
}

static void DiffyClick(int id)
{
	byte t;

	MakeNormalSound(SND_MENUCLICK);

	t=trgStart + id/100-1;
	ClearButtons(id,id);

	spcl.trigger[t].value++;
	if(spcl.trigger[t].value>4)
		spcl.trigger[t].value=0;

	SetupTriggerButtons(t-trgStart,(t-trgStart)*38+30);
}
static void ModeClick(int id)
{
	byte t;

	MakeNormalSound(SND_MENUCLICK);

	t=trgStart + id/100-1;
	ClearButtons(id,id);

	spcl.trigger[t].value++;
	if(spcl.trigger[t].value2>2)
		spcl.trigger[t].value2=0;

	SetupTriggerButtons(t-trgStart,(t-trgStart)*38+30);
}

static void CondClick(int id)
{
	byte t;

	MakeNormalSound(SND_MENUCLICK);

	t=trgStart + id/100-1;
	ClearButtons(id,id);

	spcl.trigger[t].value++;
	if(spcl.trigger[t].value2>5)
		spcl.trigger[t].value2=0;

	SetupTriggerButtons(t-trgStart,(t-trgStart)*38+30);
}

static void PicModeClick(int id)
{
	curEff=effStart + (id-ID_EFF0)/100;

	spcl.effect[curEff].value++;
	if(spcl.effect[curEff].value>=TEXTFILE_MAX)
		spcl.effect[curEff].value=0;

	SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
}

static void WeaponClick(int id)
{
	curEff=effStart+(id-ID_EFF0)/100;

	spcl.effect[curEff].value++;
	if(spcl.effect[curEff].value>=MAX_WEAPONS)
		spcl.effect[curEff].value=0;

	if(rightClick)
		spcl.effect[curEff].value=0;

	SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
}

static void Toggle2Click(int id)
{
	curEff=effStart+(id-ID_EFF0)/100;

	spcl.effect[curEff].value2=1-spcl.effect[curEff].value2;

	SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
}

static void BulletClick(int id)
{
	curEff=effStart+(id-ID_EFF0)/100;

	if(rightClick)
	{
		spcl.effect[curEff].value2--;
		if(spcl.effect[curEff].value2<=0)
			spcl.effect[curEff].value2=MAX_BULLETS-1;
	}
	else
	{
		spcl.effect[curEff].value2++;
		if(spcl.effect[curEff].value2>=MAX_BULLETS)
			spcl.effect[curEff].value2=1;
	}

	SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
}

static void Bullet1Click(int id)
{
	if (id < ID_EFF0)
	{
		effMode=0;
		curTrig=trgStart + id/100-1;

		if(rightClick)
		{
			spcl.trigger[curTrig].value--;
			if(spcl.trigger[curTrig].value<0)
				spcl.trigger[curTrig].value=MAX_BULLETS-1;
		}
		else
		{
			spcl.trigger[curTrig].value++;
			if(spcl.trigger[curTrig].value>=MAX_BULLETS)
				spcl.trigger[curTrig].value=0;
		}
		SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
	}
	else
	{
		effMode=1;
		curEff=effStart+(id-ID_EFF0)/100;

		if(rightClick)
		{
			spcl.effect[curEff].value--;
			if(spcl.effect[curEff].value<0)
				spcl.effect[curEff].value=MAX_BULLETS-1;
		}
		else
		{
			spcl.effect[curEff].value++;
			if(spcl.effect[curEff].value>=MAX_BULLETS)
				spcl.effect[curEff].value=0;
		}
		SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
	}
}

static void SetupTriggerButtons(int t,int y)
{
	char s[32];
	trigger_t trigger = spcl.trigger[t+trgStart];
	effect_t effect = spcl.effect[t+trgStart];

	ClearButtons(ID_TRIG0+100*t,ID_TRIG0+100*t+99);
	// not button
	MakeButton(BTN_RADIO,ID_TRIG0+OFS_NOT+100*t,((trigger.flags&TF_NOT)!=0)*CHECK_ON,
				   4,y+10,30,14,"NOT",NotClick);

	// select trigger button
	MakeButton(BTN_NORMAL,ID_TRIG0+OFS_SELECT+100*t,0,40,y,50,14,"Select",ChooseTriggerClick);

	// and/or button
	if(trgStart+t<NUM_TRIGGERS-1)	// last one has no button for this
	{
		if(trigger.flags&TF_AND)
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_ANDOR+100*t,0,600,y+10,30,14,"AND",AndOrClick);
		else
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_ANDOR+100*t,0,600,y+10,30,14,"OR",AndOrClick);
	}

	// individual specialties
	switch(trigger.type)
	{
		case TRG_NONE:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+100*t,0,40,y+17,1,1,"Unused",NULL);
			break;
		case TRG_STEP:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,55,y+17,140,14,MonsterName(spcl.trigger[t].value),MonsterClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,200,y+17,1,1,"steps within",NULL);
			sprintf(s,"%d",trigger.value2);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,300,y+17,40,14,s,Number2Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+4+100*t,0,350,y+17,1,1,"tiles of",NULL);
			sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+5+100*t,0,410,y+17,70,14,s,XYClick);
			break;
		case TRG_STEPRECT:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,55,y+17,140,14,MonsterName(trigger.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,200,y+17,1,1,"steps inside the area: ",NULL);
			sprintf(s,"(%d,%d)-(%d,%d)",trigger.x,trigger.y,((word)trigger.value2)%256,((word)trigger.value2)/256);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,370,y+17,150,14,s,RectClick);
			break;
		case TRG_HAVEITEM:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If player has",NULL);
			sprintf(s,"%d",trigger.value2);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,150,y+17,40,14,s,Number2Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,194,y+17,1,1,"of Item",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,255,y+17,140,14,GetItem(trigger.value)->name,ItemClick);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,404,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,404,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,404,y+17,80,14,"Exactly",LessMoreClick);
			break;
		case TRG_PASSLEVELS:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If player has passed",NULL);
			sprintf(s,"%d",trigger.value);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,200,y+17,40,14,s,NumberClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,244,y+17,1,1,"levels",NULL);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,294,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,294,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,294,y+17,80,14,"Exactly",LessMoreClick);
			break;
		case TRG_PASSLEVEL:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If player has passed level",NULL);
			sprintf(s,"%s",EditorGetWorld()->map[trigger.value]->name);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,240,y+17,200,14,s,LevelNameClick);
			break;
		case TRG_SHOOT:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If item/wall at",NULL);
			sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,150,y+17,70,14,s,XYClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,224,y+17,1,1,"is hit by",NULL);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,299,y+17,50,14,"Good",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,299,y+17,50,14,"Evil",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,299,y+17,50,14,"Any",LessMoreClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+4+100*t,0,353,y+17,1,1,"shots",NULL);
			break;
		case TRG_TIMED:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Trigger once every",NULL);
			sprintf(s,"%0.2f",(float)trigger.value/30.0f);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,190,y+17,60,14,s,FloatClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,254,y+17,1,1,"seconds after",NULL);
			sprintf(s,"%0.2f",(float)trigger.value2/30.0f);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,360,y+17,60,14,s,Float3Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+4+100*t,0,424,y+17,1,1,"seconds elapse",NULL);
			break;
		case TRG_DELAY:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Trigger continuously after",NULL);
			sprintf(s,"%0.2f",(float)trigger.value/30.0f);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,250,y+17,60,14,s,FloatClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,314,y+17,1,1,"seconds have passed",NULL);
			break;
		case TRG_MONSTER:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If there are",NULL);
			sprintf(s,"%d",trigger.value2);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,140,y+17,40,14,s,Number2Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,184,y+17,1,1,"of Monster",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,270,y+17,140,14,MonsterName(trigger.value),MonsterClick);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,414,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,414,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,414,y+17,80,14,"Exactly",LessMoreClick);
			break;
		case TRG_CHAIN:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If the special at",NULL);
			sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,164,y+17,70,14,s,XYClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,238,y+17,1,1,"is triggered",NULL);
			break;
		case TRG_KILL:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If a",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,75,y+17,140,14,MonsterName(trigger.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,219,y+17,1,1,"is beaten",NULL);
			break;
		case TRG_VAR:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If variable",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,120,y+17,30,14,VarName(trigger.value),VarClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,154,y+17,1,1,"is",NULL);
			sprintf(s,"%d",trigger.value2);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,169,y+17,40,14,s,Number2Click);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,213,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,213,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,213,y+17,80,14,"Exactly",LessMoreClick);
			break;
		case TRG_FLOOR:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If the floor at",NULL);
			sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,180,y+17,70,14,s,XYClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,254,y+17,1,1,"is",NULL);
			sprintf(s,"%c%c",(char)(trigger.value%128),(char)(trigger.value/128));
			MakeButton(BTN_TILE,ID_TRIG0+OFS_CUSTOM+3+100*t,0,270,y+2,34,26,s,TileClick);
			break;
		case TRG_RANDOM:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Random Chance:",NULL);
			sprintf(s,"%0.2f%%",(float)trigger.value/(float)FIXAMT);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,170,y+17,50,14,s,Float2Click);
			break;
		case TRG_FLOORRECT:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If the floor at",NULL);
			sprintf(s,"(%d,%d)-(%d,%d)",trigger.x,trigger.y,((word)trigger.value2)%256,((word)trigger.value2)/256);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,147,y+17,140,14,s,RectClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,291,y+17,1,1,"is entirely",NULL);
			sprintf(s,"%c%c",(char)(trigger.value%128),(char)(trigger.value/128));
			MakeButton(BTN_TILE,ID_TRIG0+OFS_CUSTOM+3+100*t,0,362,y+2,34,26,s,TileClick);
			break;
		case TRG_LIFE:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,60,y+17,140,14,MonsterName(trigger.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,204,y+17,1,1,"at",NULL);
			if(trigger.x==255)
				sprintf(s,"Anywhere");
			else
				sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,230,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+4+100*t,0,304,y+17,1,1,"has",NULL);
			sprintf(s,"%d",trigger.value2);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+5+100*t,0,330,y+17,40,14,s,Number2Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+6+100*t,0,374,y+17,1,1,"life",NULL);

			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+7+100*t,0,404,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+7+100*t,0,404,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+7+100*t,0,404,y+17,80,14,"Exactly",LessMoreClick);
			break;
		case TRG_STANDTILE:
		case TRG_STEPTILE:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,55,y+17,140,14,MonsterName(trigger.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,200,y+17,1,1,"steps on the tile",NULL);
			sprintf(s,"%c%c",(char)(trigger.value2%128),(char)(trigger.value2/128));
			MakeButton(BTN_TILE,ID_TRIG0+OFS_CUSTOM+3+100*t,0,342,y+2,34,26,s,Tile2Click);
			break;
		case TRG_GETITEM:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If player gets item at",NULL);
			sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,204,y+17,70,14,s,XYClick);
			break;
		case TRG_ITEM:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If the item at",NULL);
			sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,160,y+17,70,14,s,XYClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,234,y+17,1,1,"is",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,250,y+17,140,14,GetItem(trigger.value)->name,ItemClick);
			break;
		case TRG_AWAKE:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,60,y+17,140,14,MonsterName(trigger.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,204,y+17,1,1,"at",NULL);
			if(trigger.x==255)
				sprintf(s,"Anywhere");
			else
				sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,230,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+4+100*t,0,304,y+17,1,1,"is",NULL);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+7+100*t,0,334,y+17,100,14,"All Awake",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+7+100*t,0,334,y+17,100,14,"All Asleep",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+7+100*t,0,334,y+17,100,14,"Some Awake",LessMoreClick);
			break;
		case TRG_ITEMS:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If there are",NULL);
			sprintf(s,"%d",trigger.value2);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,150,y+17,40,14,s,Number2Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,194,y+17,1,1,"of Item",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,255,y+17,140,14,GetItem(trigger.value)->name,ItemClick);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,404,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,404,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,404,y+17,80,14,"Exactly",LessMoreClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+5+100*t,0,488,y+17,1,1,"in the level",NULL);
			break;
		case TRG_COMPMAP:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,54,y+17,1,1,"If map at",NULL);
			sprintf(s,"(%d,%d)-(%d,%d)",trigger.x,trigger.y,((word)trigger.value2)%256,((word)trigger.value2)/256);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,134,y+17,140,14,s,RectClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+4+100*t,0,278,y+17,1,1,"matches",NULL);
			sprintf(s,"(%d,%d)-(%d,%d)",((word)trigger.value)%256,((word)trigger.value)/256,
				(((word)trigger.value)%256)+((((word)trigger.value2)%256)-trigger.x),
				(((word)trigger.value)/256)+((((word)trigger.value2)/256)-trigger.y)
				);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+5+100*t,0,338,y+17,140,14,s,Rect3TClick);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,484,y+17,100,14,"No Monsters",LessClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,484,y+17,100,14,"Exactly",LessClick);
			break;
		case TRG_COMPVAR:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If variable",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,120,y+17,30,14,VarName(trigger.value),VarClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,154,y+17,1,1,"is",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,169,y+17,30,14,VarName(trigger.value2),Var2Click);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,213,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,213,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,213,y+17,80,14,"Exactly",LessMoreClick);
			break;
		case TRG_MONSINRECT:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If any",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,90,y+17,140,14,MonsterName(trigger.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,235,y+17,1,1,"are in the area: ",NULL);
			sprintf(s,"(%d,%d)-(%d,%d)",trigger.x,trigger.y,((word)trigger.value2)%256,((word)trigger.value2)/256);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,370,y+17,150,14,s,RectClick);
			break;
		case TRG_ITEMRECT:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If any",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,90,y+17,140,14,GetItem(trigger.value)->name,ItemClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,235,y+17,1,1,"are in the area: ",NULL);
			sprintf(s,"(%d,%d)-(%d,%d)",trigger.x,trigger.y,((word)trigger.value2)%256,((word)trigger.value2)/256);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,370,y+17,150,14,s,RectClick);
			break;
		case TRG_DIFFICULTY:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If difficulty is",NULL);
			if(trigger.value==0)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,160,y+17,80,14,"Wimpy",DiffyClick);
			else if(trigger.value==1)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,160,y+17,80,14,"Normal",DiffyClick);
			else if(trigger.value==2)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,160,y+17,80,14,"Hard",DiffyClick);
			else if(trigger.value==3)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,160,y+17,80,14,"Lunatic",DiffyClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,160,y+17,80,14,"Jamulio",DiffyClick);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,243,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,243,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,243,y+17,80,14,"Exactly",LessMoreClick);
			break;
		case TRG_KEYPRESS:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If player",NULL);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,120,y+17,80,14,"Taps",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,120,y+17,80,14,"Releases",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,120,y+17,80,14,"Is holding",LessMoreClick);

			if(trigger.value==CONTROL_UP)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,80,14,"Up",KeypressClick);
			else if(trigger.value==CONTROL_DN)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,80,14,"Down",KeypressClick);
			else if(trigger.value==CONTROL_LF)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,80,14,"Left",KeypressClick);
			else if(trigger.value==CONTROL_RT)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,80,14,"Right",KeypressClick);
			else if(trigger.value==CONTROL_B1)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,80,14,"Fire",KeypressClick);
			else if(trigger.value==CONTROL_B2)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,80,14,"Special",KeypressClick);
			break;
		case TRG_PLAYAS:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If player is playing as",NULL);
			if(trigger.value==PLAY_BOUAPHA)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Bouapha",PlayAsClick);
			else if(trigger.value==PLAY_LUNATIC)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Dr. Lunatic",PlayAsClick);
			else if(trigger.value==PLAY_HAPPY)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Happy Stick Man",PlayAsClick);
			else if(trigger.value==PLAY_SHROOM)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Shtupid Shroom",PlayAsClick);
			else if(trigger.value==PLAY_LUNACHIK)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Lunachick",PlayAsClick);
			else if(trigger.value==PLAY_MECHA)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Mechabouapha",PlayAsClick);
			else if(trigger.value==PLAY_WOLF)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Wolfman",PlayAsClick);
			else if(trigger.value==PLAY_WIZ)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Wild Wizard",PlayAsClick);
			else if(trigger.value==PLAY_MYSTIC)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Kid Mystic",PlayAsClick);
			else if(trigger.value==PLAY_LOONY)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Young Loony",PlayAsClick);
			break;
		case TRG_MONSCOLOR:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,60,y+17,140,14,MonsterName(trigger.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,204,y+17,1,1,"at",NULL);
			if(trigger.x==255)
				sprintf(s,"Anywhere");
			else
				sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,230,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+4+100*t,0,304,y+17,1,1,"is painted",NULL);
			s[1]='\0';
			s[0]=trigger.value2;
			MakeButton(BTN_COLOR,ID_TRIG0+OFS_CUSTOM+5+100*t,0,390,y+17,14,14,s,ColorClick1);
			break;
		case TRG_EQUATION:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,60,y+17,250,14,effect.text,EquationClick);

			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,313,y+17,1,1,"is",NULL);

			sprintf(s,"%d",trigger.value);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,333,y+17,40,14,s,NumberClick);

			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,393,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,393,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,393,y+17,80,14,"Exactly",LessMoreClick);
			break;
		case TRG_EQUVAR:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,60,y+17,250,14,effect.text,EquationClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,313,y+17,1,1,"is var",NULL);

			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,363,y+17,30,14,VarName(trigger.value),VarClick);

			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,396,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,396,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,396,y+17,80,14,"Exactly",LessMoreClick);
			break;
		case TRG_BULLETRECT:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If any",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,90,y+17,140,14,bulletName[trigger.value],Bullet1Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,235,y+17,1,1,"are in the area: ",NULL);
			sprintf(s,"(%d,%d)-(%d,%d)",trigger.x,trigger.y,((word)trigger.value2)%256,((word)trigger.value2)/256);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,370,y+17,150,14,s,RectClick);
			break;
		case TRG_TIMER:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If the timer has",NULL);
			sprintf(s,"%d",trigger.value);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,200-30,y+17,40,14,s,NumberClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,244-30,y+17,1,1,"seconds left",NULL);
			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,294+24,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,294+24,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+4+100*t,0,294+24,y+17,80,14,"Exactly",LessMoreClick);
			break;
		case TRG_HURT: //Hurt
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,60,y+17,140,14,MonsterName(trigger.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,204,y+17,1,1,"at",NULL);
			if(trigger.x==255)
				sprintf(s,"Anywhere");
			else
				sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,230,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+4+100*t,0,304,y+17,1,1,"is hurt",NULL);
			break;
		case TRG_XTRAMODE: //hyper/supreme mode
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If gamemode",NULL);
			if(trigger.value==0)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,150,y+17,80,14,"Hyper",ModeClick);
			else if(trigger.value==1)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,150,y+17,80,14,"Supreme",ModeClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,240,y+17,1,1,"is activated",NULL);
			break;
		case TRG_CONDITION:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,60,y+17,140,14,MonsterName(trigger.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,204,y+17,1,1,"at",NULL);
			if(trigger.x==255)
				sprintf(s,"Anywhere");
			else
				sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,230,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+4+100*t,0,304,y+17,1,1,"is",NULL);
			if(trigger.value2==0)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,316,y+17,80,14,"Poisoned",CondClick);
			else if(trigger.value2==1)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,316,y+17,80,14,"Frozen",CondClick);
			else if(trigger.value2==2)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,316,y+17,80,14,"Ignited",CondClick);
			else if(trigger.value2==3)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,316,y+17,80,14,"Weakened",CondClick);
			else if(trigger.value2==4)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,316,y+17,80,14,"Strength'd",CondClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,316,y+17,80,14,"Confused",CondClick);
			break;
		case TRG_RAGED:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If the player is raging",NULL);
			break;
		case TRG_SHOTBULLET:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If item/wall at",NULL);
			sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,150,y+17,70,14,s,XYClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,224,y+17,1,1,"is hit by",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,316,y+17,140,14,bulletName[trigger.value],Bullet1Click);
			break;
		case TRG_MONSAGE:
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"If",NULL);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+1+100*t,0,60,y+17,140,14,MonsterName(trigger.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+2+100*t,0,204,y+17,1,1,"at",NULL);
			if(trigger.x==255)
				sprintf(s,"Anywhere");
			else
				sprintf(s,"%d, %d",trigger.x,trigger.y);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+3+100*t,0,230,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+4+100*t,0,304,y+17,1,1,"is",NULL);
			sprintf(s,"%0.2f",(float)trigger.value2/30.0f);
			MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+5+100*t,0,330,y+17,40,14,s,Float3Click);
			MakeButton(BTN_STATIC,ID_TRIG0+OFS_CUSTOM+6+100*t,0,374,y+17,1,1,"seconds",NULL);

			if(trigger.flags&TF_LESS)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+7+100*t,0,462,y+17,80,14,"Or Less",LessMoreClick);
			else if(trigger.flags&TF_MORE)
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+7+100*t,0,462,y+17,80,14,"Or More",LessMoreClick);
			else
				MakeButton(BTN_NORMAL,ID_TRIG0+OFS_CUSTOM+7+100*t,0,462,y+17,80,14,"Exactly",LessMoreClick);
			break;
	}
}

static void SetupEffectButtons(int t,int y)
{
	char s[32];
	effect_t effect = spcl.effect[t+effStart];

	ClearButtons(ID_EFF0+100*t,ID_EFF0+100*t+99);

	// select effect button
	MakeButton(BTN_NORMAL,ID_EFF0+OFS_SELECT+100*t,0,40,y,50,14,"Select",ChooseEffectClick);

	// individual specialties
	switch(effect.type)
	{
		case EFF_NONE:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Unused",NULL);
			break;
		case EFF_MESSAGE:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Show message",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,160,y+17,250,14,effect.text,MessageClick);
			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+2+100*t,0,520,y+17,65,14,"No Snd",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+2+100*t,0,520,y+17,65,14,"Play Snd",NoFXClick);
			if(effect.flags&EF_TOGGLE)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,455,y+17,60,14,"Large",ToggleClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,455,y+17,60,14,"Normal",ToggleClick);
			break;
		case EFF_SOUND:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Play sound",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,150,y+17,250,14,GetSoundInfo(effect.value)->name,SoundClick);
			break;
		case EFF_SONG:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Play song",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,140,y+17,150,14,effect.text,SongClick);
			break;
		case EFF_WINLEVEL:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Win level and go to",NULL);
			sprintf(s,"%s",EditorGetWorld()->map[effect.value]->name);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,200,y+17,200,14,s,LevelNameClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,404,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"AUTO");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,422,y+17,70,14,s,XY2Click);
			break;
		case EFF_GOTOMAP:
		case EFF_GOTOMAP2:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Go to",NULL);
			sprintf(s,"%s",EditorGetWorld()->map[effect.value]->name);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,100,y+17,200,14,s,LevelNameClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,304,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"AUTO");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,322,y+17,70,14,s,XY2Click);
			break;
		case EFF_TELEPORT:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Teleport",NULL);
			if(effect.flags&EF_PLAYER)
				strcpy(s,"Player");
			else if(effect.flags&EF_TAGGED)
				strcpy(s,"Tagged");
			else
				strcpy(s,"Target");
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,100,y+17,100,14,s,PlayerTarget2Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,204,y+17,1,1,"to",NULL);
			sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,222,y+17,70,14,s,XYClick);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+4+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+4+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_CHANGETILE:
			if(effect.flags&EF_TOGGLE)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,60,14,"Toggle",ToggleClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,60,14,"Change",ToggleClick);

			if(effect.flags&EF_CONTIGUOUS)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,110,14,"Touching Tiles",ContiguousClick);
			else if(effect.flags&EF_ALL)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,110,14,"All Same Tiles",ContiguousClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,110,14,"Single Tile",ContiguousClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,214,y+17,1,1,"at",NULL);
			sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,230,y+17,70,14,s,XYClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,304,y+17,1,1,"to floor",NULL);
			sprintf(s,"%c%c",(char)(effect.value%128),(char)(effect.value/128));
			MakeButton(BTN_TILE,ID_EFF0+OFS_CUSTOM+5+100*t,0,366,y+2,34,26,s,TileClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+6+100*t,0,404,y+17,1,1,"& wall",NULL);
			sprintf(s,"%c%c",(char)(effect.value2%128),(char)(effect.value2/128));
			MakeButton(BTN_TILE2,ID_EFF0+OFS_CUSTOM+7+100*t,0,456,y+2,34,26,s,Tile2Click);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_SUMMON:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Summon",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,248,y+17,1,1,"at",NULL);
			sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,268,y+17,70,14,s,XYClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,342,y+17,1,1,"with",NULL);
			if(effect.value2==ITM_RANDOM)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,375,y+17,120,14,"Random",MonsItemClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,375,y+17,120,14,GetItem(effect.value2)->name,MonsItemClick);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_LIGHT:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Make",NULL);
			if(effect.flags&EF_PERMLIGHT)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,80,y+17,80,14,"Permanent",PlayerTargetClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,80,y+17,80,14,"Temporary",PlayerTargetClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,164,y+17,1,1,"light at",NULL);
			sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,223,y+17,70,14,s,XYClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,297,y+17,1,1,"at brightness",NULL);
			sprintf(s,"%d",effect.value);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,399,y+17,40,14,s,NumberClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+6+100*t,0,443,y+17,1,1,"and radius",NULL);
			sprintf(s,"%d",effect.value2);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+7+100*t,0,520,y+17,40,14,s,Number2Click);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+1,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+1,65,14,"Play FX",NoFXClick);
			break;
		case EFF_PICTURE:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Show image",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,140,y+17,250,14,effect.text,PicNameClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,394,y+17,1,1,"mode",NULL);
			switch(effect.value)
			{
				case TEXTFILE_NORMAL:
					strcpy(s,"Standard");
					break;
				case TEXTFILE_YERFDOG:
					strcpy(s,"Yerfdog");
					break;
				case TEXTFILE_COMPUTER:
					strcpy(s,"Computer");
					break;
			}
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,435,y+17,70,14,s,PicModeClick);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_ITEM:
			if(effect.flags&EF_TOGGLE)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,60,14,"Toggle",ToggleClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,60,14,"Change",ToggleClick);

			if(effect.flags&EF_CONTIGUOUS)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,110,14,"Touching Items",ContiguousClick);
			else if(effect.flags&EF_ALL)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,110,14,"All Same Items",ContiguousClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,110,14,"Single Item",ContiguousClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,214,y+17,1,1,"at",NULL);
			sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,230,y+17,70,14,s,XYClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,304,y+17,1,1,"to",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,336,y+17,120,14,GetItem(effect.value)->name,Item2Click);
			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+4+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+4+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_SWAPMAP:
			if(effect.flags&EF_TOGGLE)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,60,14,"Swap",ToggleClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,60,14,"Copy",ToggleClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,104,y+17,1,1,"map from",NULL);
			sprintf(s,"(%d,%d)-(%d,%d)",effect.x,effect.y,((word)effect.value)%256,((word)effect.value)/256);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,184,y+17,140,14,s,Rect2Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,328,y+17,1,1,"to",NULL);
			sprintf(s,"(%d,%d)-(%d,%d)",((word)effect.value2)%256,((word)effect.value2)/256,
				(((word)effect.value2)%256)+((((word)effect.value)%256)-effect.x),
				(((word)effect.value2)/256)+((((word)effect.value)/256)-effect.y)
				);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,348,y+17,150,14,s,Rect3Click);
			break;
		case EFF_KILLMONS:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Destroy",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,248,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,268,y+17,70,14,s,XY3Click);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+4+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+4+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_CHANGEMONS:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Change",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,248,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,268,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,342,y+17,1,1,"to",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,360,y+17,140,14,MonsterName(effect.value2),Monster2Click);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_CHANGETEAM:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Change",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,248,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,268,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,342,y+17,1,1,"to",NULL);
			switch(effect.value2)
			{
				case 0:
					MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,360,y+17,140,14,"Good",TeamClick);
					break;
				case 1:
					MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,360,y+17,140,14,"Evil",TeamClick);
					break;
				case 2:
					MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,360,y+17,140,14,"Toggle",TeamClick);
					break;
			}
			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_DELETESPCL:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Delete special at",NULL);
			sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,188,y+17,70,14,s,XYClick);
			break;
		case EFF_VAR:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Set variable",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,130,y+17,30,14,VarName(effect.value),VarClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,164,y+17,1,1,"to",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,184,y+17,250,14,effect.text,EquationClick);
			break;
		case EFF_LIGHTRECT:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Make",NULL);
			if(effect.flags&EF_PERMLIGHT)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,80,y+17,80,14,"Permanent",PlayerTargetClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,80,y+17,80,14,"Temporary",PlayerTargetClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,164,y+17,1,1,"light in",NULL);
			sprintf(s,"(%d,%d)-(%d,%d)",effect.x,effect.y,((word)effect.value)%256,((word)effect.value)/256);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,223,y+17,140,14,s,Rect2Click);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,367,y+17,1,1,"at brightness",NULL);
			sprintf(s,"%d",effect.value2);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,469,y+17,40,14,s,Number2Click);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+1,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+1,65,14,"Play FX",NoFXClick);
			break;
		case EFF_LEVELFLAG:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Change level flag",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,170,y+17,140,14,lvlFlagName[effect.value],LevelFlagClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,314,y+17,1,1,"to",NULL);
			switch(effect.value2)
			{
				case 0:
					MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,360,y+17,60,14,"On",TeamClick);
					break;
				case 1:
					MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,360,y+17,60,14,"Off",TeamClick);
					break;
				case 2:
					MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,360,y+17,60,14,"Toggle",TeamClick);
					break;
			}
			break;
		case EFF_OLDTOGGLE:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,60,14,"OldToggle tiles at",NULL);

			sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,220,y+17,70,14,s,XYClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,304,y+17,1,1,"to floor",NULL);
			sprintf(s,"%c%c",(char)(effect.value%128),(char)(effect.value/128));
			MakeButton(BTN_TILE,ID_EFF0+OFS_CUSTOM+5+100*t,0,366,y+2,34,26,s,TileClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+6+100*t,0,404,y+17,1,1,"& wall",NULL);
			sprintf(s,"%c%c",(char)(effect.value2%128),(char)(effect.value2/128));
			MakeButton(BTN_TILE2,ID_EFF0+OFS_CUSTOM+7+100*t,0,464,y+2,34,26,s,Tile2Click);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_LIFE:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Set",NULL);
			if(effect.flags&EF_PERMLIGHT)
				strcpy(s,"Max");
			else
				strcpy(s,"Current");
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,70,y+17,65,14,s,PlayerTargetClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,140,y+17,1,1,"life of",NULL);

			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,200,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,343,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,360,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,430,y+17,1,1,"to",NULL);
			sprintf(s,"%d",effect.value2);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,450,y+17,40,14,s,Number2Click);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_WEAPON:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Force player's weapon to",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,224,y+17,140,14,WeaponName(effect.value),WeaponClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,370,y+17,1,1,"and",NULL);

			if(effect.value2==0)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,400,y+17,40,14,"don't",Toggle2Click);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,400,y+17,40,14,"do",Toggle2Click);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,444,y+17,1,1,"reload if same",NULL);
			break;
		case EFF_TAGTARGET:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Tag the targeted monster.",NULL);
			break;
		case EFF_TAGMONS:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Tag",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,248,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,268,y+17,70,14,s,XY3Click);
		case EFF_TAGBOSS:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Mark",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,248,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,268,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,343,y+17,1,1,"as boss.",NULL);
			break;
		case EFF_MONSITEM:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,50,y+17,1,1,"Give",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,84,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,228,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,248,y+17,75,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,327,y+17,1,1,"the item",NULL);
			if(effect.value2==ITM_RANDOM)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,390,y+17,110,14,"Random",MonsItemClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,390,y+17,110,14,GetItem(effect.value2)->name,MonsItemClick);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_TILEVAR:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,60,14,"Change",NULL);
			if(effect.flags&EF_CONTIGUOUS)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,110,14,"Touching Tiles",ContiguousClick);
			else if(effect.flags&EF_ALL)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,110,14,"All Same Tiles",ContiguousClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,110,14,"Single Tile",ContiguousClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,214,y+17,1,1,"at",NULL);
			sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,230,y+17,70,14,s,XYClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,304,y+17,1,1,"to floor",NULL);

			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,384,y+17,30,14,VarName(effect.value),VarClick);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_LIFEAMT:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Chg life of",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,124,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,268,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,288,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,362,y+17,1,1,"by",NULL);
			sprintf(s,"%d",effect.value2);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,380,y+17,40,14,s,Number2Click);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_AI:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Change",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,94,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,238,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,258,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,332,y+17,1,1,"to AI",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,375,y+17,140,14,MonsterName(effect.value2),Monster2Click);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_NAME:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Change",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,94,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,238,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,258,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,332,y+17,1,1,"to Name",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,405,y+17,140,14,effect.text,MessageClick);
			break;
		case EFF_COLOR:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Change",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,94,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,238,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,258,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,332,y+17,1,1,"to Color Swap",NULL);

			s[1]='\0';
			s[0]=effect.value2%256;
			MakeButton(BTN_COLOR,ID_EFF0+OFS_CUSTOM+5+100*t,0,440,y+17,14,14,s,ColorClick1);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,456,y+17,1,1,"->",NULL);
			s[0]=effect.value2/256;
			MakeButton(BTN_COLOR,ID_EFF0+OFS_CUSTOM+5+100*t,0,485,y+17,14,14,s,ColorClick2);
			break;
		case EFF_MONSBRIGHT:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Change",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,94,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,238,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,258,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,332,y+17,1,1,"to Brightness",NULL);
			sprintf(s,"%d",effect.value2);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,430,y+17,40,14,s,Number2Click);
			break;
		case EFF_PLAYAS:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Force player to play as",NULL);
			if(effect.value==PLAY_BOUAPHA)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Bouapha",PlayAsClick);
			else if(effect.value==PLAY_LUNATIC)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Dr. Lunatic",PlayAsClick);
			else if(effect.value==PLAY_HAPPY)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Happy Stick Man",PlayAsClick);
			else if(effect.value==PLAY_SHROOM)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Shtupid Shroom",PlayAsClick);
			else if(effect.value==PLAY_LUNACHIK)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Lunachick",PlayAsClick);
			else if(effect.value==PLAY_MECHA)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Mechabouapha",PlayAsClick);
			else if(effect.value==PLAY_WOLF)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Wolfman",PlayAsClick);
			else if(effect.value==PLAY_WIZ)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Wild Wizard",PlayAsClick);
			else if(effect.value==PLAY_MYSTIC)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Kid Mystic",PlayAsClick);
			else if(effect.value==PLAY_LOONY)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,213,y+17,140,14,"Young Loony",PlayAsClick);
			break;
		case EFF_MONSGRAPHICS:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Change",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,94,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,238,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,258,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,332,y+17,1,1,"to Graphics",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,430,y+17,140,14,effect.text,PicNameClick);
			break;
		case EFF_ITEMGRAPHICS:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Use custom item sprites",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,240,y+17,250,14,effect.text,PicNameClick);
			break;
		case EFF_VARBAR:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Set",NULL);
			if(effect.flags&EF_PERMLIGHT)
				strcpy(s,"Max");
			else
				strcpy(s,"Current");
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,70,y+17,65,14,s,PlayerTargetClick);

			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,140,y+17,1,1,"Variable bar to value of",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,330,y+17,30,14,VarName(effect.value),VarClick);

			s[1] = '\0';
			s[0] = effect.value2;
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,400,y+17,1,1,"Color",NULL);
			MakeButton(BTN_COLOR,ID_EFF0+OFS_CUSTOM+5+100*t,0,440,y+17,14,14,s,ColorClick1);
			break;
		case EFF_MAKEBULLET:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Summon",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,140,14,bulletName[effect.value2],BulletClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,248,y+17,1,1,"at",NULL);
			if(effect.x==255)
				sprintf(s,"Tagged");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,268,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,342,y+17,1,1,"facing",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,400,y+17,95,14,effect.text,MessageClick);

			if(BulletFacingType(effect.value2))
				sprintf(s,"Uses 0-%d",BulletFacingType(effect.value2));
			else
				sprintf(s,"Not used");
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+7+100*t,0,400,y,1,1,s,NULL);

			if(effect.flags&EF_PERMLIGHT)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"Good",PlayerTargetClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"Evil",PlayerTargetClick);
			break;
		case EFF_CHANGEBULLET:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Change",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,104,y+17,140,14,bulletName[effect.value],Bullet1Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,248,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,268,y+17,70,14,s,XY3Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,342,y+17,1,1,"to",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,360,y+17,140,14,bulletName[effect.value2],BulletClick);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_CHANGETIMER:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Add",NULL);
			sprintf(s,"%d",effect.value);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,80,y+17,40,14,s,NumberClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,128,y+17,1,1,"second(s) to the timer",NULL);
			break;
		case EFF_CAMPOINT:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,60,14,"Focus camera on",NULL);
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,258,y+17,70,14,s,XY3Click);
			break;
		case EFF_CAMINST:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,60,14,"Focus camera on",NULL);;
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,94,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,238,y+17,1,1,"at",NULL);
			if(effect.x==255)
				strcpy(s,"Anywhere");
			else
				sprintf(s,"%d, %d",effect.x,effect.y);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+3+100*t,0,258,y+17,70,14,s,XY3Click);
			break;
		case EFF_CHARTRANS:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Transform player into",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,194,y+17,140,14,MonsterName(effect.value),MonsterClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,340,y+17,1,1,"with",NULL);
			sprintf(s,"%d",effect.value2);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+5+100*t,0,370,y+17,40,14,s,Number2Click);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+4+100*t,0,418,y+17,1,1,"HP",NULL);
			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"No FX",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,520,y+17,65,14,"Play FX",NoFXClick);
			break;
		case EFF_DYNAMICCOL:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Change color of dynamic",NULL);
			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,226,y+17,65,14,"lava",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+6+100*t,0,226,y+17,65,14,"water",NoFXClick);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,300,y+17,1,1,"to color ",NULL);
			s[1]='\0';
			s[0]=effect.value2%256;
			MakeButton(BTN_COLOR,ID_EFF0+OFS_CUSTOM+5+100*t,0,364,y+17,14,14,s,ColorClick1);
			break;
		case EFF_DYNAMICSCRN:
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+0+100*t,0,40,y+17,1,1,"Use image",NULL);
			MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+1+100*t,0,114,y+17,250,14,effect.text,PicNameClick2);
			MakeButton(BTN_STATIC,ID_EFF0+OFS_CUSTOM+2+100*t,0,369,y+17,1,1,"for",NULL);

			if(effect.flags&EF_NOFX)
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,395,y+17,65,14,"lava",NoFXClick);
			else
				MakeButton(BTN_NORMAL,ID_EFF0+OFS_CUSTOM+8+100*t,0,395,y+17,65,14,"water",NoFXClick);
			break;
	}
}

static void SpecialEditSetupButtons(void)
{
	int i;

	ClearButtons();

	// delete, move, and exit
	MakeButton(BTN_NORMAL,ID_EXIT,0,480,460,158,14,"Exit Special Editor",ExitClick);
	MakeButton(BTN_NORMAL,ID_USES,0,480,224,40,14,"Uses",UsesClick);

	// trigger/effect pages
	MakeButton(BTN_NORMAL,ID_TRGUP,0,5,224,40,14,"Up",PageClick);
	MakeButton(BTN_NORMAL,ID_TRGDN,0,50,224,40,14,"Down",PageClick);
	MakeButton(BTN_NORMAL,ID_EFFUP,0,5,460,40,14,"Up",PageClick);
	MakeButton(BTN_NORMAL,ID_EFFDN,0,50,460,40,14,"Down",PageClick);

	for(i=0;i<5;i++)
	{
		SetupTriggerButtons(i,30+i*38);
		SetupEffectButtons(i,264+i*38);
	}
}

void SpecialEdit_Init(int spclNum)
{
	GetDisplayMGL()->MouseTap();
	mode=SMODE_NORMAL;
	memcpy(&spcl,GetSpecial(spclNum),sizeof(special_t));
	trgStart=effStart=0;
	SpecialEditSetupButtons();
	specialNum=spclNum;
}

void SpecialEdit_Exit(void)
{
	memcpy(GetSpecial(specialNum),&spcl,sizeof(special_t));
}

void SpecialEdit_Update(int mouseX,int mouseY,MGLDraw *mgl)
{
	switch(mode)
	{
		case SMODE_NORMAL:
			if(mgl->MouseTap())
			{
				rightClick=0;
				CheckButtons(mouseX,mouseY);
			}
			if(mgl->RMouseTap())
			{
				rightClick=1;
				CheckButtons(mouseX,mouseY);
			}
			break;
		case SMODE_PICKBMP:
		case SMODE_PICKSONG:
			if(mgl->MouseTap())
			{
				FileDialogClick(mouseX,mouseY);
			}
			if(FileDialogCommand()==FM_LOAD)
			{
				strcpy(spcl.effect[curEff].text,GetFilename(""));
				mode=SMODE_NORMAL;
				ReturnToSong();
				SpecialEditSetupButtons();
			}
			else if(FileDialogCommand()==FM_EXIT)
			{
				mode=SMODE_NORMAL;
				ReturnToSong();
			}
			break;
		case SMODE_USES:
		case SMODE_VALUE:
		case SMODE_FVALUE:
		case SMODE_FVALUE2:
		case SMODE_FVALUE3:
		case SMODE_VALUE2:
		case SMODE_MESSAGE:
			if(mgl->MouseTap())
			{
				TextDialogClick(mouseX,mouseY);
			}
			if(TextDialogCommand()!=0)
			{
				if(TextDialogCommand()==TM_OKAY)
				{
					if(mode==SMODE_MESSAGE)
					{
						strcpy(spcl.effect[curEff].text,GetText());
						SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
						SetupTriggerButtons(curEff-effStart,(curEff-effStart)*38+30);

					}
					else if(mode==SMODE_VALUE)
					{
						if(effMode)
						{
							spcl.effect[curEff].value=atoi(GetText());
							SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
						}
						else
						{
							spcl.trigger[curTrig].value=atoi(GetText());
							SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
						}
					}
					else if(mode==SMODE_VALUE2)
					{
						if(effMode)
						{
							spcl.effect[curEff].value2=atoi(GetText());
							SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
						}
						else
						{
							spcl.trigger[curTrig].value2=atoi(GetText());
							SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
						}
					}
					else if(mode==SMODE_FVALUE)
					{
						spcl.trigger[curTrig].value=(short)(atof(GetText())*30.0f);
						SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
					}
					else if(mode==SMODE_FVALUE2)
					{
						spcl.trigger[curTrig].value=(short)(atof(GetText())*(float)FIXAMT);
						SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
					}
					else if(mode==SMODE_FVALUE3)
					{
						spcl.trigger[curTrig].value2=(short)(atof(GetText())*30.0f);
						SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
					}
					else if(mode==SMODE_USES)
						spcl.uses=atoi(GetText());
				}
				mode=SMODE_NORMAL;
			}
			break;
		case SMODE_PICKTRIG:
			if(mgl->MouseDown())
			{
				if(((mouseY-selectY)/TRG_H + floor((mouseX-40)/TRG_W)*TRG_L)<MAX_TRIGGER)
					spcl.trigger[curTrig].type=(mouseY-selectY)/TRG_H + floor((mouseX-40)/TRG_W)*TRG_L;
				else
					spcl.trigger[curTrig].type=TRG_NONE;
			}
			else
			{
				if(((mouseY-selectY)/TRG_H + floor((mouseX-40)/TRG_W)*TRG_L)<MAX_TRIGGER)
					spcl.trigger[curTrig].type=(mouseY-selectY)/TRG_H + floor((mouseX-40)/TRG_W)*TRG_L;
				else
					spcl.trigger[curTrig].type=TRG_NONE;
				mode=SMODE_NORMAL;
				if(prevType!=spcl.trigger[curTrig].type)
				{
					DefaultTrigger(&spcl.trigger[curTrig],spcl.x,spcl.y);
					if(spcl.trigger[curTrig].type==TRG_EQUATION || spcl.trigger[curTrig].type==TRG_EQUVAR)
					{
						if(spcl.effect[curTrig].type!=EFF_MESSAGE && spcl.effect[curTrig].type!=EFF_SONG &&
								spcl.effect[curTrig].type!=EFF_PICTURE && spcl.effect[curTrig].type!=EFF_VAR &&
								spcl.effect[curTrig].type!=EFF_NAME && spcl.effect[curTrig].type!=EFF_MONSGRAPHICS &&
								spcl.effect[curTrig].type!=EFF_ITEMGRAPHICS)
							spcl.effect[curTrig].text[0]='\0';
					}
				}
				SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
				MakeNormalSound(SND_MENUSELECT);
			}
			break;
		case SMODE_PICKEFF:
			if(mgl->MouseDown())
			{
				if(((mouseY-selectY)/EFF_H + floor((mouseX-40)/EFF_W)*EFF_L)<EFF_MAX)
					spcl.effect[curEff].type=(mouseY-selectY)/EFF_H + floor((mouseX-40)/EFF_W)*EFF_L;
				else
					spcl.effect[curEff].type=EFF_NONE;
			}
			else
			{
				if(((mouseY-selectY)/EFF_H + floor((mouseX-40)/EFF_W)*EFF_L)<EFF_MAX)
					spcl.effect[curEff].type=(mouseY-selectY)/EFF_H + floor((mouseX-40)/EFF_W)*EFF_L;
				else
					spcl.effect[curEff].type=EFF_NONE;
				mode=SMODE_NORMAL;
				if(prevType!=spcl.effect[curEff].type)
					DefaultEffect(&spcl.effect[curEff],spcl.x,spcl.y,(byte)(spcl.trigger[curEff].type==TRG_EQUATION || spcl.trigger[curEff].type==TRG_EQUVAR));
				SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
				MakeNormalSound(SND_MENUSELECT);
			}
			break;
		case SMODE_PICKGUY:
			if(effMode==0)
			{
				if(EditorGetLastPick()!=-1)
					spcl.trigger[curTrig].value=EditorGetLastPick();
			}
			else
			{
				if(EditorGetLastPick()!=-1)
					spcl.effect[curEff].value=EditorGetLastPick();
			}
			mode=SMODE_NORMAL;
			SpecialEditSetupButtons();
			break;
		case SMODE_PICKGUY2:
			if(EditorGetLastPick()!=-1)
				spcl.effect[curEff].value2=EditorGetLastPick();
			mode=SMODE_NORMAL;
			SpecialEditSetupButtons();
			break;
		case SMODE_PICKITEM:
			if(EditorGetLastPick()!=-1)
				spcl.trigger[curTrig].value=EditorGetLastPick();
			mode=SMODE_NORMAL;
			SpecialEditSetupButtons();
			break;
		case SMODE_PICKITEM2:
			if(EditorGetLastPick()!=-1)
				spcl.effect[curEff].value=EditorGetLastPick();
			mode=SMODE_NORMAL;
			SpecialEditSetupButtons();
			break;
		case SMODE_PICKITEM3:
			if(EditorGetLastPick()!=-1)
				spcl.effect[curEff].value2=EditorGetLastPick();
			mode=SMODE_NORMAL;
			SpecialEditSetupButtons();
			break;
		case SMODE_PICKFLOOR:
			if(!effMode)
			{
				if(EditorGetLastPick()!=-1)
					spcl.trigger[curTrig].value=EditorGetLastPick();
			}
			else
			{
				if(EditorGetLastPick()!=-1)
					spcl.effect[curEff].value=EditorGetLastPick();
			}
			mode=SMODE_NORMAL;
			SpecialEditSetupButtons();
			break;
		case SMODE_PICKFLOOR2:
			if(!effMode)
			{
				if(EditorGetLastPick()!=-1)
					spcl.trigger[curTrig].value2=EditorGetLastPick();
			}
			else
			{
				if(EditorGetLastPick()!=-1)
					spcl.effect[curEff].value2=EditorGetLastPick();
			}
			mode=SMODE_NORMAL;
			SpecialEditSetupButtons();
			break;
		case SMODE_PICKSOUND:
			if(EditorGetLastPick()!=-1)
				spcl.effect[curEff].value=EditorGetLastPick();
			mode=SMODE_NORMAL;
			SpecialEditSetupButtons();
			break;
		case SMODE_HELP:
			if(mgl->MouseTap())
			{
				if(EditHelpClick(mouseX,mouseY))
					mode=helpRemember;
			}
			break;

	}
}

void SpecialEdit_Key(char k)
{
	switch(mode)
	{
		case SMODE_NORMAL:
			if(k==27)
			{
				SetEditMode(EDITMODE_EDIT);
				if(!CheckSpecial(&spcl))
					spcl.x=255;
				memcpy(GetSpecial(specialNum),&spcl,sizeof(special_t));
				PickedTile(-1);
				return;
			}
			break;
		case SMODE_PICKBMP:
		case SMODE_PICKSONG:
			FileDialogKey(k);
			break;
		case SMODE_USES:
		case SMODE_VALUE:
		case SMODE_FVALUE:
		case SMODE_FVALUE2:
		case SMODE_FVALUE3:
		case SMODE_VALUE2:
		case SMODE_MESSAGE:
			TextDialogKey(k);
			break;
		case SMODE_HELP:
			if(!EditHelpKey(k))
				mode=helpRemember;
			break;

	}
}

void SpecialEdit_Render(int mouseX,int mouseY,MGLDraw *mgl)
{
	char s[16];
	int i,j;

	mgl->ClearScreen();

	Print(2,2,"Triggers",0,1);
	Print(2,242,"Effects",0,1);
	Print(500,2,"SPECIAL EDIT",0,1);
	sprintf(s,"#%03d",specialNum);
	Print(600,2,s,0,1);
	DrawFillBox(0,240,639,240,31);

	// # of uses
	if(spcl.uses>0)
		sprintf(s,"%d",spcl.uses);
	else
		strcpy(s,"Unlimited");
	Print(524,226,s,0,1);

	for(i=0;i<5;i++)
	{
		if(spcl.trigger[trgStart+i].type==TRG_NONE)
			DrawBox(2,30+i*38-2,590,30+i*38+34,16);
		else
		{
			DrawFillBox(2,30+i*38-2,590,30+i*38+34,32*1+4);
			DrawBox(2,30+i*38-2,590,30+i*38+34,32*1+16);
		}

		if(spcl.effect[effStart+i].type==EFF_NONE)
			DrawBox(2,264+i*38-2,590,264+i*38+34,16);
		else
		{
			DrawFillBox(2,264+i*38-2,590,264+i*38+34,32*1+4);
			DrawBox(2,264+i*38-2,590,264+i*38+34,32*1+16);
		}
	}

	// trigger page numbers
	sprintf(s,"%d-%d/%d",trgStart+1,trgStart+5,NUM_TRIGGERS);
	Print(95,226,s,0,1);
	sprintf(s,"%d-%d/%d",effStart+1,effStart+5,NUM_EFFECTS);
	Print(95,462,s,0,1);

	if(mode==SMODE_NORMAL)
		RenderButtons(mouseX,mouseY,mgl);
	else
		RenderButtons(-1,-1,mgl);

	switch(mode)
	{
		case SMODE_PICKBMP:
		case SMODE_PICKSONG:
			RenderFileDialog(mouseX,mouseY,mgl);
			break;
		case SMODE_USES:
		case SMODE_VALUE:
		case SMODE_VALUE2:
		case SMODE_FVALUE:
		case SMODE_FVALUE2:
		case SMODE_FVALUE3:
		case SMODE_MESSAGE:
			RenderTextDialog(mouseX,mouseY,mgl);
			break;
		case SMODE_PICKTRIG:
			DrawFillBox(40,selectY,40+floor(MAX_TRIGGER/TRG_L)*TRG_W+TRG_W,selectY+TRG_L*TRG_H+2,0);
			DrawBox(40,selectY,40+floor(MAX_TRIGGER/TRG_L)*TRG_W+TRG_W,selectY+TRG_L*TRG_H+2,31);
			for(j=0;j<floor(MAX_TRIGGER/TRG_L)+1;j++)
			{
				for(i=0;i<TRG_L;i++)
				{
					if(spcl.trigger[curTrig].type==j*TRG_L+i)
						DrawFillBox(40+TRG_W*j+1,selectY+1+i*TRG_H,40+TRG_W*(j+1)-1,selectY+1+i*TRG_H+TRG_H,32*1+8);
					if(j*TRG_L+i < MAX_TRIGGER)
					Print(40+TRG_W*j+2,selectY+2+i*TRG_H,trigName[j*TRG_L+i],0,1);
				}
			}
			break;
		case SMODE_PICKEFF:
			DrawFillBox(40,selectY,40+floor(EFF_MAX/EFF_L)*EFF_W*2,selectY+EFF_L*EFF_H+2,0);
			DrawBox(40,selectY,40+floor(EFF_MAX/EFF_L)*EFF_W*2,selectY+EFF_L*EFF_H+2,31);
			for(j=0;j<floor(EFF_MAX/EFF_L)+1;j++)
			{
				for(i=0;i<EFF_L;i++)
				{
					if(spcl.effect[curEff].type==j*EFF_L+i)
						DrawFillBox(40+EFF_W*j+1,selectY+1+i*EFF_H,40+EFF_W*(j+1)-1,selectY+1+i*EFF_H+EFF_H,32*1+8);
					if(j*EFF_L+i < EFF_MAX)
					Print(40+EFF_W*j+2,selectY+2+i*EFF_H,effName[j*EFF_L+i],0,1);
				}
			}
			break;
		case SMODE_HELP:
			RenderEditHelp(mouseX,mouseY,mgl);
			break;
	}
}

void SetSpecialCoords(int x,int y)
{
//	if(x==255)
	//	return;

	if(mode==SMODE_PICKXY2)
	{
		EditorSelectMap(previousMap);
	}

	if(mode==SMODE_PICKRECT3)
	{
		if(x>=0 && y>=0)
			spcl.effect[curEff].value2=x+y*256;
		mode=SMODE_NORMAL;
		SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
	}
	else if(mode==SMODE_PICKRECT3T)
	{
		if(x>=0 && y>=0)
			spcl.trigger[curTrig].value=x+y*256;
		mode=SMODE_NORMAL;
		SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
	}
	else
	{
		if(effMode==0)
		{
			mode=SMODE_NORMAL;

			if(x<0 || y<0)
				return;

			spcl.trigger[curTrig].x=(byte)x;
			spcl.trigger[curTrig].y=(byte)y;
			SetupTriggerButtons(curTrig-trgStart,(curTrig-trgStart)*38+30);
		}
		else
		{
			mode=SMODE_NORMAL;

			if(x<0)
			{
				//spcl.effect[curEff].x=255;
			}
			else
			{
				spcl.effect[curEff].x=(byte)x;
				spcl.effect[curEff].y=(byte)y;
				SetupEffectButtons(curEff-effStart,(curEff-effStart)*38+264);
			}
		}
	}
}

void SetSpecialRect(int x,int y,int x2,int y2)
{
	if(x<0 || y<0)
	{
		mode=SMODE_NORMAL;
		return;
	}

	if(mode==SMODE_PICKRECT)
	{
		spcl.trigger[curTrig].x=(byte)x;
		spcl.trigger[curTrig].y=(byte)y;
		spcl.trigger[curTrig].value2=x2+y2*256;
		SetupTriggerButtons(curTrig,curTrig*38+30);
	}
	else
	{
		spcl.effect[curEff].x=(byte)x;
		spcl.effect[curEff].y=(byte)y;
		spcl.effect[curEff].value=x2+y2*256;
		SetupEffectButtons(curEff,curEff*38+264);
	}
	mode=SMODE_NORMAL;
}

void SpecialEdit_Help(void)
{
	if(mode==SMODE_HELP)
		return;

	helpRemember=mode;
	InitEditHelp(HELP_SPECIALEDIT);
	mode=SMODE_HELP;
}
