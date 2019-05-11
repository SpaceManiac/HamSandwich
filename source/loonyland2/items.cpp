#include "items.h"
#include "display.h"
#include "editor.h"
#include "gallery.h"
#include "options.h"

item_t itemInfo[MAX_ITMS]={
	{"None",0,0,0,0,0},
	{"Pine1",1,0,0,0,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Pine2",1,0,1,1,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Pine3",1,0,2,2,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Cabin1",1,0,3,3,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Cabin2",1,0,4,4,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Post1",1,0,5,5,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Post2",1,0,6,6,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Post3",1,0,7,7,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Axe",1,0,10,10,IF_SHADOW|IF_GET},
	{"Magic Axe",1,0,11,11,IF_SHADOW|IF_GET},
	{"Golden Axe",1,0,15,15,IF_SHADOW|IF_GET},
	{"Amulet",1,0,13,13,IF_SHADOW|IF_GET},
	{"Magic Amulet",1,0,14,14,IF_SHADOW|IF_GET},
	{"Golden Amulet",1,0,12,12,IF_SHADOW|IF_GET},
	{"Parka",1,0,16,16,IF_SHADOW|IF_GET},
	{"Magic Parka",1,0,17,17,IF_SHADOW|IF_GET},
	{"Golden Parka",1,0,18,18,IF_SHADOW|IF_GET},
	{"Coin",1,0,9,9,IF_SHADOW|IF_GET},
	{"Skill Scroll",1,0,32,32,IF_SHADOW|IF_GET},
	{"Pole",1,0,8,8,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Barrel",1,0,69,69,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Cauldron",1,100,167,172,IF_SHADOW|IF_GET},	// for alchemy
	{"Buried Asb.",1,0,19,19,0},
	{"Buried Stone",1,0,19,19,0},
	{"Cave",1,0,21,21,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Rock",1,0,28,28,IF_OBSTACLE},
	{"Rockbig",1,0,31,31,IF_OBSTACLE|IF_TALL|IF_SHADOW},
	{"Rocksml",1,0,29,29,0},
	{"Rocksml2",1,0,30,30,0},
	{"Ice Spike",1,0,26,26,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Deadplant",1,0,22,22,IF_SHADOW|IF_OBSTACLE},
	{"Deadplantsml",1,0,23,23,IF_SHADOW},
	{"Deadplantsml2",1,0,24,24,IF_SHADOW},
	{"Allusionary",1,0,159,159,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"ARU Sign",1,0,34,34,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Biz Sign",1,0,35,35,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Bed",1,0,36,36,IF_SHADOW|IF_OBSTACLE},
	{"Bed2",1,0,37,37,IF_SHADOW|IF_OBSTACLE},
	{"Bookcase",1,0,38,38,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Table",1,0,39,39,IF_SHADOW|IF_OBSTACLE},
	{"Table2",1,0,40,40,IF_SHADOW|IF_OBSTACLE},
	{"Chair",1,0,41,41,IF_SHADOW|IF_OBSTACLE},
	{"Chair2",1,0,42,42,IF_SHADOW|IF_OBSTACLE},
	{"Chair3",1,0,43,43,IF_SHADOW|IF_OBSTACLE},
	{"Chair4",1,0,44,44,IF_SHADOW|IF_OBSTACLE},
	{"Stool",1,0,45,45,IF_SHADOW|IF_OBSTACLE},
	{"Dresser",1,0,46,46,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Tencoin",1,0,188,188,IF_SHADOW|IF_GET},
	{"Logpile",1,0,48,48,IF_SHADOW|IF_OBSTACLE},
	{"Stump",1,0,49,49,IF_SHADOW|IF_OBSTACLE},
	{"Choppable",1,0,50,50,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Walktree",1,0,51,51,IF_WALK|IF_BOTTOM},
	{"Invisowalk",1,0,45,45,IF_WALK|IF_INVIS},
	{"Exit",1,128,52,61,IF_GLOW},
	{"Invisoblock",1,0,45,45,IF_OBSTACLE|IF_TALL|IF_INVIS|IF_LIGHTOK},
	{"Oven",1,0,62,62,IF_OBSTACLE|IF_TALL|IF_SHADOW},
	{"Pine stump",1,0,63,63,0},
	{"Sign",1,0,64,64,IF_OBSTACLE|IF_TALL|IF_SHADOW},
	{"WaterRockWalk",1,0,99,99,IF_WALK|IF_BOTTOM},
	{"WaterRockWalk2",1,0,100,100,IF_WALK|IF_BOTTOM},
	{"WaterRock",1,0,101,101,IF_OBSTACLE|IF_BOTTOM},
	{"WaterRock2",1,0,102,102,IF_OBSTACLE|IF_BOTTOM},
	// chests
	{"Chest-Snowman",1,0,121,121,IF_OBSTACLE},
	{"Chest-Pine",1,0,127,127,IF_OBSTACLE},
	{"Chest-Mitten",1,0,115,115,IF_OBSTACLE},
	{"Chest-Flake",1,0,109,109,IF_OBSTACLE},
	{"Chest-Bear",1,0,103,103,IF_OBSTACLE},
	// doors
	{"Door-Snowman",1,0,148,148,IF_OBSTACLE|IF_TALL},
	{"Door-Pine",1,0,153,153,IF_OBSTACLE|IF_TALL},
	{"Door-Mitten",1,0,143,143,IF_OBSTACLE|IF_TALL},
	{"Door-Flake",1,0,138,138,IF_OBSTACLE|IF_TALL},
	{"Door-Bear",1,0,133,133,IF_OBSTACLE|IF_TALL},
	// open doors
	{"OpenDoor-Snowman",1,0,152,152,0},
	{"OpenDoor-Pine",1,0,157,157,0},
	{"OpenDoor-Mitten",1,0,147,147,0},
	{"OpenDoor-Flake",1,0,142,142,0},
	{"OpenDoor-Bear",1,0,137,137,0},
	// cave rocks
	{"CaveRocksml",1,0,163,163,0},
	{"CaveRocksml2",1,0,164,164,0},
	{"CaveRock",1,0,160,160,IF_OBSTACLE|IF_TALL|IF_SHADOW},
	{"CaveRock2",1,0,161,161,IF_OBSTACLE|IF_TALL|IF_SHADOW},
	{"CaveRock3",1,0,162,162,IF_OBSTACLE|IF_TALL|IF_SHADOW},
	{"Clock",1,50,173,178,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Crack",1,0,187,187,IF_BOTTOM},
	{"MtnRock",1,0,189,189,IF_SHADOW|IF_OBSTACLE},
	{"MtnRock2",1,0,190,190,IF_SHADOW|IF_OBSTACLE},
	{"MtnRock3",1,0,192,192,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"MtnRockSml",1,0,191,191,0},
	{"NonEvilSign",1,0,193,193,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"OnionBanner",1,0,194,194,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"BunnyBanner",1,0,195,195,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Roped Stump",1,0,197,917,IF_SHADOW|IF_BOTTOM},
	{"Supplies",1,0,198,198,IF_SHADOW|IF_OBSTACLE|IF_GET},
	{"Supplies2",1,0,199,199,IF_SHADOW|IF_OBSTACLE|IF_GET},
	{"Supply Crack",1,0,187,187,IF_BOTTOM},
	{"Geyser",1,0,222,222,IF_BOTTOM|IF_STEAM},
	{"Purple Rock",1,0,226,226,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Purple Rock2",1,0,227,227,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Purple Pebble",1,0,228,228,IF_SHADOW|IF_OBSTACLE},
	{"Purple Pebble2",1,0,229,229,IF_SHADOW|IF_OBSTACLE},
	{"Shroom",1,0,230,230,IF_SHADOW|IF_OBSTACLE},
	{"Shroom2",1,0,231,231,IF_SHADOW|IF_OBSTACLE},
	{"ShroomBig",1,0,232,232,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"ShroomBig2",1,0,233,233,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"ShroomPatch",1,0,234,234,IF_SHADOW},
	{"ShroomPatch2",1,0,235,235,IF_SHADOW},
	{"Upgeyser",1,0,187,187,IF_BOTTOM|IF_STEAM},
	{"PurpleDark",1,0,236,236,IF_OBSTACLE|IF_TALL},
	{"PurpleDark2",1,0,237,237,IF_OBSTACLE|IF_TALL},
	{"Crystal",1,0,238,238,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Crystal2",1,0,239,239,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Crystal3",1,0,240,240,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Crystal4",1,0,241,241,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Undercrack",1,0,242,242,IF_BOTTOM},
	{"Soundingtree",1,0,243,243,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Vinewall",1,0,255,255,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Vinewall2",1,0,256,256,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	{"Vinewall3",1,0,257,257,IF_SHADOW|IF_OBSTACLE|IF_TALL},
	//////// inventory items
	{"Rope",1,0,165,165,IF_SHADOW|IF_GET},
	// potions
	{"Brain Juice",1,0,79,79,IF_SHADOW|IF_GET},
	{"Mudder's Milk",1,0,80,80,IF_SHADOW|IF_GET},
	{"Stone Soup",1,0,81,81,IF_SHADOW|IF_GET},
	{"Life-Ade",1,0,82,82,IF_SHADOW|IF_GET},
	{"Red Cow",1,0,83,83,IF_SHADOW|IF_GET},
	{"Soula Cola",1,0,84,84,IF_SHADOW|IF_GET},
	{"Dr. Deflector",1,0,85,85,IF_SHADOW|IF_GET},
	{"Alien Blood",1,0,86,86,IF_SHADOW|IF_GET},
	{"Fear Beer",1,0,87,87,IF_SHADOW|IF_GET},
	{"Jolting Soda",1,0,88,88,IF_SHADOW|IF_GET},
	{"Sparkling Water",1,0,89,89,IF_SHADOW|IF_GET},
	{"Smart Sauce",1,0,90,90,IF_SHADOW|IF_GET},
	{"Luckenbrau",1,0,91,91,IF_SHADOW|IF_GET},
	{"Eau De Monster",1,0,92,92,IF_SHADOW|IF_GET},
	{"Healenbrau",1,0,93,93,IF_SHADOW|IF_GET},
	{"Magicola",1,0,94,94,IF_SHADOW|IF_GET},
	{"Stamino",1,0,95,95,IF_SHADOW|IF_GET},
	{"Rocket Sauce",1,0,96,96,IF_SHADOW|IF_GET},
	{"Explosivo",1,0,97,97,IF_SHADOW|IF_GET},
	{"Cool-Ade",1,0,98,98,IF_SHADOW|IF_GET},

	// herbs
	{"Asbestos",1,0,20,20,IF_SHADOW|IF_GET},
	{"Bubbleweed",1,0,70,70,IF_SHADOW|IF_GET},
	{"Cilantro",1,0,71,71,IF_SHADOW|IF_GET},
	{"Devil Grass",1,0,72,72,IF_SHADOW|IF_GET},
	{"Elderberry",1,0,73,73,IF_SHADOW|IF_GET},
	{"Frostbane",1,0,25,25,IF_SHADOW|IF_GET},

	// junk items
	{"Log",1,0,27,27,IF_SHADOW|IF_GET},
	{"Gear",1,0,65,65,IF_SHADOW|IF_GET},
	{"Tin Can",1,0,66,66,IF_SHADOW|IF_GET},
	{"Stuffing",1,0,67,67,IF_SHADOW|IF_GET},
	{"Sock",1,0,68,68,IF_SHADOW|IF_GET},
	{"Ice Cube",1,0,185,185,IF_SHADOW|IF_GET},
	{"Seed",1,0,184,184,IF_SHADOW|IF_GET},
	{"Rock",1,0,221,221,IF_SHADOW|IF_GET},
	{"Ectoplasm",1,0,220,220,IF_SHADOW|IF_GET},
	{"Claw",1,0,224,224,IF_SHADOW|IF_GET},
	// mining items (also considered junk)
	{"Silver",1,0,183,183,IF_SHADOW|IF_GET},
	{"Gold",1,0,182,182,IF_SHADOW|IF_GET},
	{"Ruby",1,0,181,181,IF_SHADOW|IF_GET},
	{"Emerald",1,0,180,180,IF_SHADOW|IF_GET},
	{"Diamond",1,0,179,179,IF_SHADOW|IF_GET},
	// inscribed lenses
	{"Lens",1,0,203,203,IF_SHADOW|IF_GET},
	{"Lens",1,0,204,204,IF_SHADOW|IF_GET},
	{"Lens",1,0,205,205,IF_SHADOW|IF_GET},
	{"Lens",1,0,206,206,IF_SHADOW|IF_GET},
	{"Lens",1,0,207,207,IF_SHADOW|IF_GET},
	{"Lens",1,0,208,208,IF_SHADOW|IF_GET},
	{"Lens",1,0,209,209,IF_SHADOW|IF_GET},
	{"Lens",1,0,210,210,IF_SHADOW|IF_GET},
	{"Lens",1,0,211,211,IF_SHADOW|IF_GET},
	{"Lens",1,0,212,212,IF_SHADOW|IF_GET},
	{"Lens",1,0,213,213,IF_SHADOW|IF_GET},
	{"Lens",1,0,214,214,IF_SHADOW|IF_GET},
	{"Lens",1,0,215,215,IF_SHADOW|IF_GET},
	// quest items
	{"Snowman Key",1,0,78,78,IF_SHADOW|IF_GET},
	{"Pine Key",1,0,77,77,IF_SHADOW|IF_GET},
	{"Mitten Key",1,0,76,76,IF_SHADOW|IF_GET},
	{"Snowflake Key",1,0,75,75,IF_SHADOW|IF_GET},
	{"Bear Key",1,0,74,74,IF_SHADOW|IF_GET},
	{"Sharpening Stone",1,0,164,164,IF_SHADOW|IF_GET},
	{"Binding Crystal",1,0,166,166,IF_SHADOW|IF_GET},
	{"Shovel",1,0,33,33,IF_SHADOW|IF_GET},
	{"Ice Skates",1,0,201,201,IF_SHADOW|IF_GET},
	{"Wing Boots",1,0,202,202,IF_SHADOW|IF_GET},
	{"Present",1,0,47,47,IF_SHADOW|IF_GET},
	{"Asbestos Undies",1,0,200,200,IF_SHADOW|IF_GET},
	{"Tool Box",1,0,158,158,IF_SHADOW|IF_GET},
	{"Letter",1,0,186,186,IF_SHADOW|IF_GET},
	{"Sounding Crystal",1,0,196,196,IF_SHADOW|IF_GET},
	{"Glasses",1,0,216,216,IF_SHADOW|IF_GET},
	{"Super Magical Glasses",1,0,217,217,IF_SHADOW|IF_GET},
	{"Souvenir Sword",1,0,219,219,IF_SHADOW|IF_GET},
	{"Package",1,0,218,218,IF_SHADOW|IF_GET},
	{"Candy",1,0,223,223,IF_SHADOW|IF_GET},
	{"Stolen Goods",1,0,218,218,IF_SHADOW|IF_GET},
	{"Book",1,0,225,225,IF_SHADOW|IF_GET},
	{"Thing Of Majig",1,0,252,252,IF_SHADOW|IF_GET},
	{"Thing Of Mabob",1,0,253,253,IF_SHADOW|IF_GET},
	{"Watch Of Makalit",1,0,254,254,IF_SHADOW|IF_GET},
	{"Energy Barrier",1,128,244,251,IF_OBSTACLE|IF_GLOW},
	{"Ice Block",1,0,258,258,IF_SHADOW|IF_GLOW|IF_OBSTACLE|IF_TALL|IF_LIGHTOK},
	{"Ice Melt",1,0,259,259,IF_SHADOW|IF_GLOW},

	{"Deadvine",1,0,260,260,IF_BOTTOM},
	{"Deadvine",1,0,261,261,IF_BOTTOM},
	{"Deadvine",1,0,262,262,IF_BOTTOM},

	{"Control Crystal",1,0,166,166,IF_SHADOW|IF_GET},
	{"Castle Switch",1,0,263,263,IF_SHADOW|IF_OBSTACLE|IF_TALL|IF_LIGHTOK},	// 267 is end of animation
	{"Artaxe",1,0,268,268,IF_SHADOW},
	{"Artammy",1,0,269,269,IF_SHADOW},
	{"Artparka",1,0,270,270,IF_SHADOW},
	{"Gallery Image",1,0,32,32,IF_SHADOW|IF_GET},
	{"Artifact Bit",1,128,244,251,IF_GLOW|IF_GET},
};
// 268=artaxe, 269=artamulet, 270=artparka

sprite_set_t *itmSpr[2];
static	int itemAnim;

void InitItems(void)
{
	itmSpr[1]=new sprite_set_t("graphics\\items.jsp");
	itemAnim=0;
}

void ExitItems(void)
{
	delete itmSpr[1];
}

char *ItemName(byte itm)
{
	return itemInfo[itm].name;
}

void UpdateItems(void)
{
	itemAnim++;
	if(itemAnim>4095)
		itemAnim=0;
}

void RenderItem(int x,int y,int z,byte type,byte info,char bright)
{
	int frm;
	word flags;

	if(type==0 || type>=MAX_ITMS)
		return;

	if((itemInfo[type].flags&IF_INVIS) && !editing)
		return;	// don't draw invis
	if(itemInfo[type].rate>0)
		frm=(itemAnim*itemInfo[type].rate/256)%(itemInfo[type].spr2-itemInfo[type].spr+1);
	else
		frm=0;

	if(type>=IT_CHEST1 && type<=IT_CHEST5)
		frm+=info/2;	// they animate
	if(type>=IT_DOOR1 && type<=IT_DOOR5)
		frm+=info/2;	// they animate

	if(type==IT_CASTLESWITCH)
		frm+=info/2;

	frm+=itemInfo[type].spr;

	flags=DISPLAY_DRAWME;
	if(itemInfo[type].flags&IF_GLOW)
		flags|=DISPLAY_GLOW;
	if(itemInfo[type].flags&IF_BOTTOM)
		flags|=DISPLAY_BOTTOM;

	if(type==IT_CLOCK)
		flags|=DISPLAY_CLOCK;

	if(type==IT_PICTURE)
		OffSprDraw(x,y,z,1,2,bright-8+(rand()%8),itmSpr[itemInfo[type].sprset]->GetSprite(frm),flags);
	else
		SprDraw(x,y,z,255,bright,itmSpr[itemInfo[type].sprset]->GetSprite(frm),flags);
	if(itemInfo[type].flags&IF_SHADOW)
		SprDraw(x,y,0,255,bright,itmSpr[itemInfo[type].sprset]->GetSprite(frm),DISPLAY_DRAWME|DISPLAY_SHADOW);
}

void RenderClock(int x,int y,MGLDraw *mgl)
{
	// 0,-27
	byte hour,min;
	int xx,yy;

	hour=(byte)((player.playClock/(60*30))%12);
	min=(byte)((player.playClock/30)%60);

	xx=x+3+Cosine(hour*256/12-64)*6/FIXAMT;
	yy=y-28+Sine(hour*256/12-64)*6/FIXAMT;
	DrawLine(x-1,y-28-1,xx-1,yy-1,8);
	DrawLine(x-1,y-28,xx-1,yy,8);
	DrawLine(x,y-28-1,xx,yy-1,8);
	DrawLine(x,y-28,xx,yy,8);

	xx=x+3+Cosine(min*256/60-64)*8/FIXAMT;
	yy=y-28+Sine(min*256/60-64)*8/FIXAMT;
	DrawLine(x,y-28,xx,yy,0);
}

void InstaRenderItem(int x,int y,byte type,char bright,MGLDraw *mgl)
{
	int frm;

	if(type==0 || type>=MAX_ITMS)
		return;

	if(itemInfo[type].rate>0)
		frm=(itemAnim*itemInfo[type].rate/256)%(itemInfo[type].spr2-itemInfo[type].spr+1);
	else
		frm=0;
	frm+=itemInfo[type].spr;

	itmSpr[itemInfo[type].sprset]->GetSprite(frm)->DrawBright(x,y,mgl,bright);
}

void InstaRenderItem2(int x,int y,byte type,int anim,char bright,MGLDraw *mgl)
{
	int frm;

	if(type==0 || type>=MAX_ITMS)
		return;

	if(itemInfo[type].rate>0)
		frm=(anim*256/itemInfo[type].rate)%(itemInfo[type].spr2-itemInfo[type].spr+1);
	else
		frm=0;

	frm+=itemInfo[type].spr;

	itmSpr[itemInfo[type].sprset]->GetSprite(frm)->DrawBright(x,y,mgl,bright);
}

dword ItemFlags(int type)
{
	return itemInfo[type].flags;
}

void ChestPop(byte type,int mapx,int mapy)
{
	int i;
	byte level;
	bullet_t *b;
	byte money;
	int drops;
	int x,y;

#ifdef DIRECTORS
	x=0;
	for(i=0;i<50;i++)
		if(player.var[VAR_CHEST+i])
			x++;
	if(x==50)
	{
		picToDrop=5;
		if(opt.galPix[picToDrop-1]==0 && player.var[VAR_TEMP+15]==0)
		{
			bullet_t *b;

			b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,64,BLT_ITEM,0);
			if(b)
			{
				player.var[VAR_TEMP+15]=1;
				b->anim=IT_PICTURE;
				b->target=picToDrop;
				NewMessage("All chests opened!",0);
			}
		}
		picToDrop=0;
	}
#endif
	x=(mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
	y=(mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	level=(type-IT_CHEST1+1)*10;	// 10 levels per chest quality
	if(player.var[VAR_MADCAP])
		level+=50;

	money=3+(rand()%((level/5)+4));

	drops=3;
	while(drops--)
	{
		b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
		if(b)
		{
			EquipCreateItem(level,&type,&b->target,&b->owner);
			b->anim=type;
		}
	}
	for(i=0;i<money;i++)
	{
		b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
		if(b)
		{
			b->anim=IT_COIN;
			b->timer=30*10;
		}
	}
}
