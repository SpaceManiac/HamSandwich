#include "items.h"
#include "display.h"

#define ITMOFSX (-1)
#define ITMOFSY (-2)

item_t itemInfo[MAX_ITMS]={
	{0,0,0,0,0,0},
	// pushables
	{0,0,1,1,2,IF_PUSHABLE|IF_TALL|IF_OBSTACLE|IF_SHADOW},	// barrel
	{0,0,0,0,6,IF_PUSHABLE|IF_TALL|IF_OBSTACLE|IF_SHADOW},	// ball
	{0,0,2,2,4,IF_TALL|IF_OBSTACLE|IF_SHADOW},	// crate
	// objects
	{0,0,9,9,4,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// switch
	{0,0,10,10,4,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// switch2
	{0,0,11,11,4,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// crank
	{0,0,5,5,4,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// door
	{0,0,5,5,4,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// opendoor
	{0,0,9,9,0,0},	// teleport
	{0,0,10,10,0,0},	// summon
	// monsters
	{0,0,2,2,0,IF_INVIS},	// mons1
	{0,0,12,12,0,IF_INVIS},	// mons2
	{0,0,13,13,0,IF_INVIS},	// mons3
	{0,0,14,14,0,IF_INVIS},	// mons4
	{0,0,15,15,0,IF_INVIS},	// start
	{0,0,16,16,0,IF_INVIS},	// exit
	{0,0,2,2,0,IF_INVIS},	// player
	{0,0,0,2,0,IF_INVIS},	// movingball
	{0,0,0,2,0,IF_INVIS},	// activebomb
	// pickups
	{0,64,16,31,0,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// bomb
	{0,64,64,79,4,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// detonate
	{0,64,96,111,0,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// shield
	{0,64,80,95,1,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// acid
	{0,64,48,63,0,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// laser
	{0,64,32,47,0,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// key
	{0,0,112,112,0,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// iceberg
	// goodie
	{0,0,122,122,0,IF_OBSTACLE|IF_TALL},	// goodie
	// pencil pickup
	{0,64,126,141,5,IF_OBSTACLE|IF_TALL|IF_SHADOW},	// pencil
};

sprite_set_t *itmSpr[2];
static	int itemAnim;

void InitItems(void)
{
	itmSpr[0]=new sprite_set_t("graphics\\items.jsp");
	itmSpr[1]=NULL;
	itemAnim=0;
}

void ExitItems(void)
{
	delete itmSpr[0];
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

	if(itemInfo[type].flags&IF_INVIS)
		return;

	if(itemInfo[type].rate>0)
		frm=(itemAnim*itemInfo[type].rate/256)%(itemInfo[type].spr2-itemInfo[type].spr+1);
	else
		frm=0;

	if(type==ITM_OPENDOOR || type==ITM_DOOR)
	{
		frm=(info/8)/2;
	}

	if(type==ITM_CRANK)
	{
		frm=(info/8)%4;
	}
	if(type==ITM_GOODIE)
		frm=info/8;

	if(type==ITM_LASER)
		z=8;
	if(type==ITM_ACID || type==ITM_DETONATE || type==ITM_SHIELD || type==ITM_KEY)
		z=4;

	if(type==ITM_ACID || type==ITM_SHIELD)	// animate these backwards, since I made them wrong
		frm=(itemInfo[type].spr2-itemInfo[type].spr)-frm;

	frm+=itemInfo[type].spr;

	OffSprDraw(x+ITMOFSX,y+ITMOFSY,z,itemInfo[type].colReplace,(info%8),bright,itmSpr[itemInfo[type].sprset]->GetSprite(frm),DISPLAY_DRAWME|DISPLAY_OFFCOLOR|DISPLAY_COLORBLIND);

	if(itemInfo[type].flags&IF_SHADOW)
		SprDraw(x+ITMOFSX,y+ITMOFSY,0,255,bright,itmSpr[itemInfo[type].sprset]->GetSprite(frm),DISPLAY_DRAWME|DISPLAY_SHADOW);
}

void InstaRenderItem(int x,int y,byte type,byte color,char bright,MGLDraw *mgl)
{
	int frm;

	if(type==0 || type>=MAX_ITMS)
		return;

	if(itemInfo[type].rate>0)
		frm=(itemAnim*itemInfo[type].rate/256)%(itemInfo[type].spr2-itemInfo[type].spr+1);
	else
		frm=0;

	if(type==ITM_OPENDOOR)
		frm+=3;

	if(type==ITM_GOODIE)
	{
		frm+=color/8;
		color=0;
	}
	if(type==ITM_ACID || type==ITM_SHIELD)	// animate these backwards, since I made them wrong
		frm=(itemInfo[type].spr2-itemInfo[type].spr)-frm;

	frm+=itemInfo[type].spr;

	itmSpr[itemInfo[type].sprset]->GetSprite(frm)->DrawOffColor(x+ITMOFSX,y+ITMOFSY,mgl,itemInfo[type].colReplace,color,bright);
}

void RenderItmSprite(int x,int y,byte spr,MGLDraw *mgl)
{
	itmSpr[0]->GetSprite(spr)->Draw(x+ITMOFSX,y+ITMOFSY,mgl);
}

sprite_t *GetItmSprite(int spr)
{
	return itmSpr[0]->GetSprite(spr);
}


dword ItemFlags(int type)
{
	return itemInfo[type].flags;
}
