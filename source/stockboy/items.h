#ifndef ITEMS_H
#define ITEMS_H

#include "winpch.h"
#include "jamulspr.h"

#define ITM_NONE		0

// pushables
#define ITM_BARREL		1
#define ITM_BALL		2
#define ITM_CRATE		3

// objects (switches,etc)
#define ITM_SWITCH		4
#define ITM_SWITCH2		5
#define ITM_CRANK		6
#define ITM_DOOR		7
#define ITM_OPENDOOR	8
#define ITM_TELEPORT	9
#define ITM_SUMMON		10

// monsters
#define ITM_MONS1		11
#define ITM_MONS2		12
#define ITM_MONS3		13
#define ITM_MONS4		14
#define ITM_START		15
#define ITM_EXIT		16

// non-items... used to mark where moving things are, like the player, so other things
// won't step there
#define ITM_PLAYER		17	
#define ITM_MOVINGBALL	18	// a ball in motion
#define ITM_ACTIVEBOMB	19	// a bomb blowing up or counting down
// pick-ups
#define ITM_BOMB		20
#define ITM_DETONATE	21
#define ITM_SHIELD		22
#define ITM_ACID		23
#define ITM_LASER		24
#define ITM_KEY			25
#define ITM_ICEBERG		26	// equivalent to a crate

#define ITM_GOODIE		27	// special goodie you can trigger
#define ITM_PENCIL		28	// stars to collect
#define MAX_ITMS		29

// item flags
#define IF_GET			1	// can be picked up by player
#define IF_OBSTACLE		2	// prevents movement
#define IF_TALL			4	// prevents flyers and bullets from moving
#define IF_PUSHABLE		8	// can be pushed like a block
#define IF_GLOW			16	// drawn as glow
#define IF_SHADOW		32	// casts a shadow
#define IF_WALK			64	// can be walked on EVEN IF the terrain is otherwise unwalkable (water/lava/impassable)
#define IF_LIGHTOK		128	// if it's a tall item, light goes through it anyway
#define IF_NOBULLET		256	// bullets can't pass
#define IF_DESTROY		512	// can be destroyed
#define IF_INVIS		1024 // invisible

typedef struct item_t
{
	int sprset,rate,spr,spr2;
	byte colReplace;
	dword flags;
} item_t;

class Map;

void InitItems(void);
void ExitItems(void);
void UpdateItems(void);
dword ItemFlags(int type);
void RenderItem(int x,int y,int z,byte type,byte info,char bright);
void InstaRenderItem(int x,int y,byte type,byte color,char bright,MGLDraw *mgl);
char *WeaponName(byte w);
void RenderItmSprite(int x,int y,byte spr,MGLDraw *mgl);
sprite_t *GetItmSprite(int spr);

#endif