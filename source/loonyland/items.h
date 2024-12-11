#ifndef ITEMS_H
#define ITEMS_H

#include "winpch.h"
#include "jamulspr.h"

#define ITM_NONE		0
// pickup items
#define ITM_TINYHEART	1
#define ITM_BIGHEART	2
#define ITM_SUPERHEART	3
#define ITM_LITTLEGEM	4
#define ITM_BIGGEM		5
#define ITM_SUPERGEM	6
#define ITM_FIREPOWERUP 7
#define ITM_RANGEUP		8
#define ITM_FIRERATEUP  9
#define ITM_KEY			10
#define ITM_KEY2		11	// skull key
#define ITM_KEY3		12	// bat key
#define ITM_KEY4		13	// pumpkin key
#define ITM_WBOMB		14
#define ITM_WLIGHTNING	15
#define ITM_WICE		16
#define ITM_WCACTUS		17
#define ITM_WBOOMERANG	18
#define ITM_WWHOOPEE	19
#define ITM_WHOTPANTS	20
#define ITM_POTION		21
// doors
#define ITM_DOOR		22
#define ITM_SKULLDOOR	23
#define ITM_BATDOOR		24
#define ITM_PUMPKINDOOR 25
// open doors
#define ITM_DOOR2		26
#define ITM_SKULLDOOR2	27
#define ITM_BATDOOR2	28
#define ITM_PUMPKINDOOR2 29
// trees
#define ITM_TREE1		30
#define ITM_TREE2		31	// the evil tree pacified
// stone objects
#define ITM_LAKECAVE	32
// chairs & tables
#define ITM_TABLE		33
#define ITM_STOOL		34
#define ITM_CHAIR		35
#define ITM_CHAIR2		36
#define ITM_CHAIR3		37
#define ITM_CHAIR4		38
#define ITM_CHAIR5		39
#define ITM_BIGCHAIR	40
#define ITM_BIGCHAIR2	41
#define ITM_BIGCHAIR3	42
#define ITM_BIGCHAIR4	43
// mystery orb
#define ITM_MYSORB		44
#define ITM_ORBSTAND	45
#define ITM_ORBSTAND2	46
// save gem
#define ITM_SAVEGEM		47
// bottles n' stuff, pure scenery
#define ITM_BOTTLE		48
#define ITM_BOTTLE2		49
#define ITM_BOTTLE3		50
#define ITM_BOTTLE4		51
#define ITM_BOTTLE5		52
#define ITM_BOTTLE6		53
#define ITM_BOTTLE7		54
#define ITM_BOTTLE8		55
#define ITM_BOTTLE9		56
#define ITM_BOTTLE10	57
#define ITM_BOTTLE11	58
// signs
#define ITM_SIGN		59
#define ITM_SIGN2		60
#define ITM_SIGN3		61
// mansion gear
#define ITM_KNIGHT		62
#define ITM_KNIGHT2		63
#define ITM_CLOCK		64
#define ITM_CANDLE		65
#define ITM_BOOKCASE	66
#define ITM_BED			67
// logs
#define ITM_LOG			68
#define ITM_LOG2		69
#define ITM_LOG3		70
// ground stuff
#define ITM_TWIG		71
#define ITM_ROCK		72
#define ITM_ROCKPILE	73
// barrels
#define ITM_BARREL		74
#define ITM_BARREL2		75
// trees
#define ITM_TREE3		76
#define ITM_TREE4		77	// witch tree
#define ITM_TREE5		78	// cat tree!!
#define ITM_TREE6		79
#define ITM_TREE7		80
// pickup items
#define ITM_SHROOM		81
#define ITM_DAISY		82
#define ITM_SILVER		83
// magic plants
#define ITM_MAGICPLANT	84
#define ITM_WALKPLANT	85	// the full grown magic plant, you can walk on it
// switches
#define ITM_SWITCH1ON	86
#define ITM_SWITCH1OFF	87
#define ITM_SWITCH2		88
// grass
#define ITM_GRASS		89
#define ITM_GRASS2		90
#define ITM_GRASS3		91
#define ITM_CATTAIL		92
// rocks
#define ITM_LILROCK		93
#define ITM_BIGROCK		94
#define ITM_BOOMROCK	95  // can be blown up by bombs
// torch
#define ITM_TORCH		96	// a standing torch
// gas-pipe related
#define ITM_GASHOLE		97
#define ITM_GASPIPELR	98
#define ITM_GASPIPEUD	99
#define ITM_GASPIPEDR	100
#define ITM_GASPIPEDL	101
#define ITM_GASPIPEUR	102
#define ITM_GASPIPEUL	103
#define ITM_GASSPARK	104
// graves
#define ITM_GRAVE		105
#define ITM_GRAVE2		106
// table
#define ITM_TABLE2		107
// autodoor
#define ITM_AUTODOOR	108	// autodoor
#define ITM_AUTODOOR2	109	// dead autodoor
// other
#define ITM_TALISMAN	110	// happy stick talisman
#define ITM_TRIPLEFIRE	111
#define ITM_BUSTSTAND	112	// stand for vamp bust
#define ITM_BUST		113
#define ITM_BUSTONSTAND 114
#define ITM_BATSTAND	115
#define ITM_BAT			116
#define ITM_BATONSTAND	117
// dolls
#define ITM_BATDOLL		118
#define ITM_SKELDOLL	119
#define ITM_FROGDOLL	120
#define ITM_GHOSTDOLL	121
#define ITM_MUMMYDOLL	122
#define ITM_SWAMPDOLL	123
#define ITM_VAMPDOLL	124
#define ITM_WOLFDOLL	125
// other
#define ITM_PORTAL		126
#define ITM_BADGE		127	// the badge you can find in the game
#define ITM_CAT			128	// fake item, just used to be rendered in Kick The Cat

#define ITM_ZOMBGEM		129 //100 gems
#define ITM_BOOTS		130 //mud boots
#define ITM_FERTILIZER	131
#define ITM_GHOSTPOTION	132
#define ITM_LANTERN		133
#define ITM_REFLECTGEM	134
#define ITM_SILVERSLING	135
#define ITM_STICK		136
#define ITM_ARCHIPELAGO 137

#define MAX_ITMS		138

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

class Map;

void InitItems(void);
void ExitItems(void);
void UpdateItems(void);
dword ItemFlags(int type);
void RenderItem(int x,int y,int z,byte type,byte info,char bright);
void InstaRenderItem(int x,int y,byte type,char bright,MGLDraw *mgl);
void InstaRenderItem2(int x,int y,byte type,int anim,char bright,MGLDraw *mgl);
char *WeaponName(byte w);
byte GasCanEnter(byte face,int x,int y,Map *map);
void RenderSumHeal(int x,int y,int z,char bright);

struct ItemRenderExtents
{
	int left, right, up, down;
};
ItemRenderExtents GetItemRenderExtents();

#endif
