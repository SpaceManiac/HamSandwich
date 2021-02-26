#include "items.h"
#include "display.h"

item_t itemInfo[MAX_ITMS]={
	//{item sheet, Rate?, sprite/anim start, sprite/anim end, flags}
	{0,0,0,0,0},
	// pickups
	{0,128,15,22,IF_GET|IF_GLOW|IF_SHADOW},	// tiny heart
	{0,128,7,14,IF_GET|IF_GLOW|IF_SHADOW},	// big heart
	{0,64,0,6,IF_GET|IF_GLOW},	// super heart
	{0,128,23,30,IF_GET|IF_SHADOW},	// little gem
	{0,128,31,38,IF_GET|IF_SHADOW},	// big gem
	{0,128,39,46,IF_GET|IF_GLOW},	// huge gem
	{0,128,47,62,IF_GET|IF_GLOW},	// fire power up
	{0,128,63,70,IF_GET|IF_GLOW},	// range up
	{0,128,103,114,IF_GET|IF_GLOW},	// fire rate up
	{0,128,71,77,IF_GET|IF_SHADOW},	// key
	{0,64,79,86,IF_GET|IF_SHADOW},	// key2
	{0,64,87,94,IF_GET|IF_SHADOW},	// key3
	{0,64,95,102,IF_GET|IF_SHADOW},	// key4
	{0,256,115,124,IF_GET|IF_SHADOW},	// weapon bomb
	{0,256,125,134,IF_GET|IF_SHADOW},	// weapon lightning
	{0,256,135,144,IF_GET|IF_SHADOW},	// weapon ice
	{0,256,145,154,IF_GET|IF_SHADOW},	// weapon cactus
	{0,256,155,164,IF_GET|IF_SHADOW},	// weapon boomerang
	{0,256,165,174,IF_GET|IF_SHADOW},	// weapon whoopee
	{0,256,175,184,IF_GET|IF_SHADOW},	// weapon hot pants
	{0,128,185,195,IF_GET|IF_SHADOW},	// potion
	// doors
	{1,0,0,0,IF_OBSTACLE|IF_TALL},	// door
	{1,0,4,4,IF_OBSTACLE|IF_TALL},	// skull door
	{1,0,12,12,IF_OBSTACLE|IF_TALL},	// bat door
	{1,0,8,8,IF_OBSTACLE|IF_TALL},	// pumpkin door
	// open doors
	{1,0,3,3,0},	// door
	{1,0,7,7,0},	// skull door
	{1,0,15,15,0},	// bat door
	{1,0,11,11,0},	// pumpkin door
	// trees
	{1,0,16,16,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// dead tree (tree1)
	{1,0,17,17,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// evil tree smiling (tree2)
	// stone objects
	{1,0,18,18,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// lake cave
	// chairs & tables
	{1,0,71,71,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// table
	{1,0,72,72,IF_OBSTACLE|IF_SHADOW},			// stool
	{1,0,73,73,IF_OBSTACLE|IF_SHADOW},			// chair
	{1,0,74,74,IF_OBSTACLE|IF_SHADOW},			// chair2
	{1,0,75,75,IF_OBSTACLE|IF_SHADOW},			// chair3
	{1,0,76,76,IF_OBSTACLE|IF_SHADOW},			// chair4
	{1,0,77,77,IF_OBSTACLE|IF_SHADOW},			// chair5
	{1,0,78,78,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// bigchair
	{1,0,79,79,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// bigchair2
	{1,0,80,80,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// bigchair3
	{1,0,81,81,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// bigchair4
	// mystery orb
	{1,64,55,70,IF_GET|IF_GLOW|IF_SHADOW},	// mystery orb
	{1,0,38,38,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// orb stand
	{1,64,39,54,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// orb stand with orb
	// save gem
	{1,128,28,37,IF_OBSTACLE|IF_TALL|IF_GLOW},	// savegem
	// bottles n' stuff, pure scenery
	{1,0,82,82,IF_SHADOW},	// bottle
	{1,0,83,83,IF_SHADOW},	// bottle2
	{1,0,84,84,IF_SHADOW},	// bottle3
	{1,0,85,85,IF_SHADOW},	// bottle4
	{1,0,86,86,IF_SHADOW},	// bottle5
	{1,0,87,87,IF_SHADOW},	// bottle6
	{1,0,88,88,IF_SHADOW},	// bottle7
	{1,0,89,89,IF_SHADOW},	// bottle8
	{1,0,90,90,IF_SHADOW},	// bottle9
	{1,0,91,91,IF_SHADOW},	// bottle10
	{1,0,92,92,IF_SHADOW},	// bottle11
	// signs
	{1,0,94,94,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// sign
	{1,0,95,95,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// sign2
	{1,0,96,96,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// sign3
	// mansion gear
	{1,0,97,97,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// knight
	{1,0,98,98,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// knight2
	{1,128,99,114,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// clock
	{1,64,115,124,IF_OBSTACLE|IF_SHADOW},	// candle
	{1,0,125,125,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// bookcase
	{1,0,126,126,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// bed
	{1,0,127,127,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// log
	{1,0,128,128,IF_OBSTACLE|IF_SHADOW},	// log2
	{1,0,129,129,IF_OBSTACLE|IF_SHADOW},	// log3
	{1,0,130,130,IF_SHADOW},	// twig
	{1,0,131,131,0},	// rock
	{1,0,132,132,IF_OBSTACLE},	// rockpile
	{1,0,133,133,IF_OBSTACLE|IF_PUSHABLE|IF_SHADOW|IF_LIGHTOK},	// barrel
	{1,0,134,134,IF_OBSTACLE|IF_SHADOW|IF_LIGHTOK},	// barrel2
	{1,0,135,135,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// tree3
	{1,0,136,136,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// tree4
	{1,16,137,140,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// tree5
	{1,0,162,162,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// tree6
	{1,0,163,163,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// tree7
	{0,96,265,276,IF_GET|IF_SHADOW},	// shroom
	{0,0,197,197,IF_GET|IF_SHADOW},	// daisy
	{0,72,198,209,IF_GET},			// silver
	{1,48,141,144,IF_SHADOW},		// magicplant
	{1,0,149,149,IF_WALK|IF_SHADOW},	// walkplant
	{1,0,150,150,IF_OBSTACLE|IF_SHADOW},	// switch1on
	{1,0,157,157,IF_OBSTACLE|IF_SHADOW},	// switch1off
	{1,0,158,158,IF_OBSTACLE|IF_SHADOW},	// switch2
	{1,0,166,166,0},	// grass
	{1,0,167,167,0},	// grass2
	{1,0,168,168,0},	// grass3
	{1,0,171,171,IF_OBSTACLE|IF_TALL|IF_SHADOW|IF_DESTROY},	// cattail
	{1,0,170,170,0},	// lilrock
	{1,0,169,169,IF_OBSTACLE|IF_SHADOW},	// bigrock
	{1,0,165,165,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// boomrock
	{1,0,164,164,IF_OBSTACLE|IF_TALL|IF_SHADOW|IF_LIGHTOK},	// torch
	{1,128,196,206,0},	// gas source
	{1,0,172,172,IF_OBSTACLE|IF_TALL|IF_PUSHABLE|IF_LIGHTOK},	// gaspipe-lr
	{1,0,176,176,IF_OBSTACLE|IF_TALL|IF_PUSHABLE|IF_LIGHTOK},	// gaspipe-ud
	{1,0,180,180,IF_OBSTACLE|IF_TALL|IF_PUSHABLE|IF_LIGHTOK},	// gaspipe-dr
	{1,0,184,184,IF_OBSTACLE|IF_TALL|IF_PUSHABLE|IF_LIGHTOK},	// gaspipe-dl
	{1,0,188,188,IF_OBSTACLE|IF_TALL|IF_PUSHABLE|IF_LIGHTOK},	// gaspipe-ur
	{1,0,192,192,IF_OBSTACLE|IF_TALL|IF_PUSHABLE|IF_LIGHTOK},	// gaspipe-ul
	{1,128,207,214,0},	// gasspark
	{1,0,215,215,IF_OBSTACLE|IF_TALL|IF_SHADOW|IF_DESTROY},	// grave
	{1,0,216,216,IF_OBSTACLE|IF_TALL|IF_SHADOW|IF_DESTROY},	// grave2
	{1,0,217,217,IF_OBSTACLE|IF_SHADOW},	// table2
	{1,64,218,220,IF_SHADOW|IF_NOBULLET},	// autodoor
	{1,0,218,218,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// autodoor (inactive)
	{0,48,210,214,IF_GET|IF_SHADOW},	// talisman
	{0,72,215,222,IF_GET|IF_GLOW},	// triplefire
	{1,0,225,225,IF_OBSTACLE|IF_TALL|IF_SHADOW|IF_LIGHTOK},	// buststand
	{0,128,223,238,IF_GET|IF_SHADOW},	// bust
	{1,0,226,226,IF_OBSTACLE|IF_TALL|IF_SHADOW|IF_LIGHTOK},	// bustonstand
	{1,0,225,225,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// batstand
	{0,128,239,254,IF_GET|IF_SHADOW},	// bat
	{1,0,227,227,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// batonstand
	{0,0,255,255,IF_GET|IF_SHADOW},	// batdoll
	{0,0,256,256,IF_GET|IF_SHADOW},	// skeldoll
	{0,0,257,257,IF_GET|IF_SHADOW},	// frogdoll
	{0,0,258,258,IF_GET|IF_SHADOW},	// ghostdoll
	{0,0,259,259,IF_GET|IF_SHADOW},	// mummydoll
	{0,0,260,260,IF_GET|IF_SHADOW},	// swampdoll
	{0,0,261,261,IF_GET|IF_SHADOW},	// vampdoll
	{0,0,262,262,IF_GET|IF_SHADOW},	// wolfdoll
	{1,0,228,228,IF_GLOW},	// portal
	{0,0,263,263,IF_GET|IF_SHADOW},	// badge
	{0,0,264,264,IF_GET|IF_SHADOW},	// cat

	//ADDED FOR RANDOMIZER
	{2, 128, 0, 7, IF_GET | IF_SHADOW},// | IF_GLOW},	//zombie gem
	{2, 0, 8, 8, IF_GET | IF_SHADOW},	//mud boots
	{2, 0, 9, 9, IF_GET | IF_SHADOW},	//fertiziler
	{2, 0, 10, 10, IF_GET | IF_SHADOW | IF_GLOW},	//ghostpotion
	{2, 0, 11, 11, IF_GET | IF_SHADOW | IF_GLOW},	//lantern 
	{2, 0, 12, 12, IF_GET | IF_SHADOW | IF_GLOW},	//reflect gem
	{2, 0, 13, 13, IF_GET | IF_SHADOW},	//silversling
	{2, 0, 14, 14, IF_GET | IF_SHADOW}	//stick 
};

sprite_set_t *itmSpr[3];
static	int itemAnim;

void InitItems(void)
{
	itmSpr[0]=new sprite_set_t("graphics/pickups.jsp");
	itmSpr[1]=new sprite_set_t("graphics/items.jsp");
	itmSpr[2] = new sprite_set_t("graphics/randomizer.jsp");
	itemAnim=0;
}

void ExitItems(void)
{
	delete itmSpr[0];
	delete itmSpr[1];
}

char *WeaponName(byte w)
{
	static char c[7][32]={
		"Bombs",
		"Lightning Rod",
		"Ice Wand",
		"Potted Cactus",
		"Boomerang",
		"Whoopee Cushion",
		"Hot Pants"
	};

	return c[w];
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

	if(type==0 || type>=MAX_ITMS)
		return;

	if(itemInfo[type].rate>0)
		frm=(itemAnim*itemInfo[type].rate/256)%(itemInfo[type].spr2-itemInfo[type].spr+1);
	else
		frm=0;

	if(type>=ITM_DOOR && type<ITM_DOOR2)
		frm+=info/2;
	if(type==ITM_LAKECAVE)
		frm+=info;
	if(type==ITM_SWITCH1ON && info>0)
	{
		frm=info/2;
	}
	if(type==ITM_SWITCH1OFF && info>0)
	{
		frm=-info/2;
	}
	if(type==ITM_SWITCH2 && info>0)
	{
		frm=info/2;
	}
	if(type==ITM_MAGICPLANT && info>0)
	{
		frm=info;
	}

	if(type==ITM_AUTODOOR && info>0)
	{
		frm=info;
	}

	if(type==ITM_PORTAL)
	{
		if(info<16)
			frm=info/4;
		else
			frm=info-16+3;
	}

	if(type>=ITM_GASPIPELR && type<=ITM_GASPIPEUL)
	{
		frm+=info/2;
		if(info>0)
		{
			if(info<4)
				z+=Random(info*2);
			else
				z+=Random((8-info)*2);
		}
	}

	frm+=itemInfo[type].spr;

	if(itemInfo[type].flags&IF_GLOW)
		SprDraw(x,y,z,255,bright,itmSpr[itemInfo[type].sprset]->GetSprite(frm),DISPLAY_DRAWME|DISPLAY_GLOW);
	else
		SprDraw(x,y,z,255,bright,itmSpr[itemInfo[type].sprset]->GetSprite(frm),DISPLAY_DRAWME);
	if(itemInfo[type].flags&IF_SHADOW)
		SprDraw(x,y,0,255,bright,itmSpr[itemInfo[type].sprset]->GetSprite(frm),DISPLAY_DRAWME|DISPLAY_SHADOW);
}


void RenderSumHeal(int x,int y,int z,char bright)
{
	int frm;

	if(itemInfo[ITM_TINYHEART].rate>0)
		frm=(itemAnim*itemInfo[ITM_TINYHEART].rate/256)%(itemInfo[ITM_TINYHEART].spr2-itemInfo[ITM_TINYHEART].spr+1);
	else
		frm=0;

	frm+=itemInfo[ITM_TINYHEART].spr;

	OffSprDraw(x,y,z,4,1,bright,itmSpr[itemInfo[ITM_TINYHEART].sprset]->GetSprite(frm),DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
	SprDraw(x,y,0,255,bright,itmSpr[itemInfo[ITM_TINYHEART].sprset]->GetSprite(frm),DISPLAY_DRAWME|DISPLAY_SHADOW);
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

	frm=(anim*256/itemInfo[type].rate)%(itemInfo[type].spr2-itemInfo[type].spr+1);
	frm+=itemInfo[type].spr;

	itmSpr[itemInfo[type].sprset]->GetSprite(frm)->DrawBright(x,y,mgl,bright);
}

dword ItemFlags(int type)
{
	return itemInfo[type].flags;
}

byte GasCanEnter(byte face,int x,int y,Map *map)
{
	mapTile_t *m;

	if(x<0 || x>=map->width || y<0 || y>=map->height)
		return 0;

	m=&map->map[x+y*map->width];

	if(m->wall)
		return 0;

	if(ItemFlags(m->item)&IF_TALL)
	{
		if(m->item>=ITM_GASPIPELR && m->item<=ITM_GASPIPEUL)
		{
			switch(m->item)
			{
				case ITM_GASPIPELR:
					return (face==0 || face==4);
				case ITM_GASPIPEUD:
					return (face==2 || face==6);
				case ITM_GASPIPEUR:
					return (face==2 || face==4);
				case ITM_GASPIPEUL:
					return (face==2 || face==0);
				case ITM_GASPIPEDR:
					return (face==4 || face==6);
				case ITM_GASPIPEDL:
					return (face==0 || face==6);
			}
		}
		else
			return 0;
	}

	return 1;
}
