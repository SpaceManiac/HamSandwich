#ifndef ITEMS_H
#define ITEMS_H

#include "jamulspr.h"
#include <stdio.h>

#define ITM_NONE	 0
#define ITM_HAMMERUP 1
#define ITM_PANTS	 2
#define ITM_REVERSE  3
#define ITM_REFLECT  4
#define ITM_MISSILES 5
#define ITM_AK8087	 6
#define ITM_TAKEOUT  7
#define ITM_SHIELD   8
#define ITM_BOMBS	 9
#define ITM_FLAME	 10
#define ITM_BRAIN	 11
#define ITM_KEYCH1	 12
#define ITM_KEYCH2	 13
#define ITM_KEYCH3	 14
#define ITM_KEYCH4	 15
#define ITM_KEY		 16
#define ITM_KEYR	 17
#define ITM_KEYG	 18
#define ITM_KEYB	 19
#define ITM_LOONYKEY 20
#define ITM_BIGAXE	 21
#define ITM_PWRARMOR 22
#define ITM_LIGHTNING 23
#define ITM_SPEAR	 24
#define ITM_MACHETE	 25
#define ITM_MINES	 26
#define ITM_GARLIC	 27
#define ITM_ORBITER  28
#define ITM_ACCEL	 29

#define ITM_SMLROCKS 30
#define ITM_HOLETREE 31
#define ITM_IGLOO	 32
#define ITM_WEB		 33
#define ITM_WEB2	 34
#define ITM_WEB3	 35
#define ITM_WEB4	 36
#define ITM_GRASS	 37
#define ITM_GRASS2	 38
#define ITM_VILLAGE  39

#define ITM_COIN	 40
#define ITM_CANDLE	 41
#define ITM_ORBITER2 42
#define ITM_PIZZA	 43
#define ITM_TORCH	 44
#define ITM_CONE	 45
#define ITM_FALLENCONE	 46
#define ITM_BUBBLES	 47
#define ITM_SCUBA	 48
#define ITM_MAILBOX  49

#define ITM_BOX		 50
#define ITM_STUMP	 51
#define ITM_BUSH	 52
#define ITM_BIGROCKS 53
#define ITM_POST	 54
#define ITM_CHAIR1	 55
#define ITM_CHAIR2   56
#define ITM_WALLGRASS 57
#define ITM_BARREL	 58
#define ITM_BARREL2	 59
#define ITM_BARREL3	 60
#define ITM_TRASHCAN 61
#define ITM_TRASHCAN2 62
#define ITM_CRATE	 63
#define ITM_CRATE2	 64
#define ITM_BUSH2	 65
#define ITM_BUSH3	 66
#define ITM_BRWNROCK 67

#define ITM_MAILBOX2 68

#define ITM_PALM	 69
#define ITM_TREE	 70
#define ITM_DOOR1	 71
#define ITM_DOOR1R	 72
#define ITM_DOOR1G	 73
#define ITM_DOOR1B	 74
#define ITM_DOOR2	 75
#define ITM_DOOR2R	 76
#define ITM_DOOR2G	 77
#define ITM_DOOR2B	 78
#define ITM_SIGN	 79
#define ITM_PINE	 80
#define ITM_DEADTREE 81
#define ITM_DEADTREE2 82
#define ITM_FATPALM	 83
#define ITM_TREE2	 84
#define ITM_MINEBLOCK 85
#define ITM_SIGN2	 86
#define ITM_SIGN3	 87
#define ITM_SIGN4	 88
#define ITM_SIGN5	 89
#define ITM_SIGN6	 90
#define ITM_BIGROCK	 91

#define ITM_TURRETWPN 92
#define ITM_MINDCONTROL 93
#define ITM_REFLECTOR 94
#define ITM_INVIS	  95
#define ITM_JETPACK	  96
#define ITM_UNHAMMER  97
#define ITM_UNPANTS	  98
#define ITM_SWAPGUN	  99
#define ITM_BADCHINESE 100
#define ITM_MAILBOX3	101
#define ITM_SCANNER		102
#define ITM_MINISUB		103
#define ITM_HYDRANT		104
#define ITM_TRAFFICLT1	105
#define ITM_TRAFFICLT2	106
#define ITM_TRAFFICLT3	107
#define ITM_AMMOCRATE	108
#define ITM_AMMOPAK		109
#define ITM_FLATROCK	110
#define ITM_FLATROCK2	111
#define ITM_SEAWEED		112
#define ITM_SEAWEED2	113
#define ITM_SEAWEED3	114
#define ITM_FREEZERAY	115
#define ITM_ROADSIGN1	116
#define ITM_ROADSIGN2	117
#define ITM_ROADSIGN3	118
#define ITM_ROADSIGN4	119
#define ITM_ROADSIGN5	120
#define ITM_ROADSIGN6	121
#define ITM_ROADSIGN7	122
#define ITM_ROADSIGN8	123
#define ITM_ROADSIGN9	124
#define ITM_ROADSIGN10	125
#define ITM_ROADSIGN11	126
#define ITM_ROADSIGN12	127
#define ITM_ROADSIGN13	128
#define ITM_ROADSIGN14	129
#define ITM_ROADSIGN15	130
#define ITM_STOPWATCH	131
#define ITM_STRTLIGHT	132
#define ITM_SUPREME		133
// NEW items
#define ITM_KEYW		134
#define ITM_KEYBL		135
#define ITM_DOORW	 	136
#define ITM_DOORBL	 	137
#define ITM_HAY			138
#define ITM_BARREL4		139
// unused items
#define ITM_BOOMERANG	140
#define ITM_CACTUS		141
#define ITM_ROCKET		142
#define ITM_WATERGUN	143
#define ITM_MEGAPHONE	144
#define ITM_PUMPKING	145
#define ITM_DOUBLEGUN	146
#define ITM_WHOOPIE		147
#define ITM_BLACKHOLE	148
#define ITM_MEDICKIT	149

#define NUM_ORIGINAL_ITEMS	150
#define MAX_ITEMS	255

#define ITM_RANDOM	255		// the item number for random item drops

// item flags
#define IF_SHADOW      (1<<0)	// casts a shadow
#define IF_GLOW        (1<<1)	// is glowy rather than solid
#define IF_SOLID       (1<<2)	// can't walk through it
#define IF_BULLETPROOF (1<<3)	// can't shoot through it
#define IF_PICKUP      (1<<4)	// player can pick it up for some effect
#define IF_LOONYCOLOR  (1<<5)	// using the loony color thing that loonykeys do
#define IF_TILE        (1<<6)	// render a tile instead of a sprite
#define IF_USERJSP     (1<<7)	// use custom item jsp
#define IF_BUBBLES     (1<<8)	// spout bubbles
#define IF_BIGMSG      (1<<9)	// display a big message

// themes
#define IT_PICKUP	(1<<0)	// items you can collect
#define IT_DECOR	(1<<1)	// not obstacles or bulletproof
#define IT_OBSTACLE (1<<2)	// obstacles
#define IT_BULLETPROOF (1<<3)	// obstacles you can't shoot through
#define IT_DOOR		(1<<4)	// doors
#define IT_TREE		(1<<5)	// vegetation
#define IT_ROCK		(1<<6)	// rocks
#define IT_CRATE	(1<<7)	// crates & barrels & trashcans
#define IT_SIGN		(1<<8)	// signposts, etc
#define IT_WEAPON	(1<<9)	// player pickup: weapons
#define IT_POWERUP	(1<<10)	// player pickup: powerups
#define IT_KEY		(1<<11)	// player pickup: keys
#define IT_COLLECT	(1<<12)	// player pickup: collectibles
#define IT_FOOD		(1<<13)	// player pickup: food
#define IT_ENTRANCE	(1<<14)	// entrance type items, like hollow trees
#define IT_CHAIR	(1<<15)	// chairs
#define IT_CUSTOM	(1<<16)	// the custom theme, "My Theme"

// triggers
#define ITR_NONE	0		// can't be triggered
#define ITR_GET		1		// triggered on pickup
#define ITR_SHOOT	2		// triggers when shot (must be shootable)
#define ITR_PLAYERBUMP	4	// triggers when stepped on/bumped into by player
#define ITR_ENEMYBUMP 8		// triggers when bumped by an enemy
#define ITR_FRIENDBUMP 16	// step/bump by friendly monster
#define ITR_CHOP	32		// triggers when hit by machete or other chopper
#define ITR_MINECART 64		// triggers when crashed into by a minecart
#define ITR_ALWAYS	128		// triggers repeatedly, as fast as tiles animate
#define ITR_BOMBED 256		// triggers when explodinated
#define ITR_BURNT 512		// triggers when burnt by fire!
#define ITR_FROZEN 1024		// triggers when burnt by fire!

// effects
// what occurs when you pick up the item if it's a pickup,
// or bump it if it's not
#define IE_NONE		0	// none
#define IE_DESTROY	1	// vanishes, with particles of color N (0-7, or -1 for no particles)
#define IE_HEAL		2	// heals N (negative harms)
#define IE_WEAPON	3	// gives you weapon N
#define IE_HAMMER	4	// gives you N hammers (can be negative of course)
#define IE_PANTS	5	// gives you N pants
#define IE_KEY		6	// gives you N yellow keys
#define IE_RKEY		7	// gives you N red keys (N is here, so you can use -1 to remove it)
#define IE_GKEY		8	// green
#define IE_BKEY		9	// blue
#define IE_LOONYKEY	10	// gives you N loonykeys (again, -1 to remove it)
#define IE_COIN		11	// gives you N coins
#define IE_CANDLE	12	// gives you N candles
#define IE_OXYGEN	13	// gives you N oxygen
#define IE_RAGE		14	// gives you N rage
#define IE_WEIRD	15	// the radioactive chinese effect - variety of possibilities
#define IE_BECOME	16	// changes to item N
#define IE_SUMMON	17	// summons monster N
#define IE_POWERUP	18	// power the player up with power up N, use -N to remove power up
#define IE_HEALRANDOM 19	// the random healing you get from chinese food
#define IE_BRAIN	20	// gives you N brains
#define IE_KEYCHAIN 21	// gives you keychain N (1-4), use -N to remove the keychain
#define IE_DOOR		22	// door of color N (0=yellow, 123=RGB), disappears if you trigger it with
						// the right key in hand.  If not, it does nothing
#define IE_ORBITER	23	// summon N orbiters, -N will remove them
#define IE_ORBITER2 24	// summon N orbit bombers, -N will remove them
#define IE_PUSH		25	// get pushed
#define IE_INCVAR	26	// increment variable N
#define IE_DECVAR	27	// decrement variable N
#define IE_MOVE		28	// move up/down/left/right (only if it could move there as a pushable item)
#define IE_WKEY		29	// white key
#define IE_BLKEY		30	// black key
#define IE_ORBITER3 31	// summon N weather orbiters, -N will remove them
#define IE_ORBITER4 32	// summon N poison orbiters, -N will remove them

#define IE_MAX		33	// max # of effects

// power ups
#define PU_REVERSE	1	// reverse hammer
#define PU_REFLECT	2	// reflect hammer
#define PU_SHIELD	3	// energy shield
#define PU_GARLIC	4	// garlic
#define PU_SPEED	5	// particle accelerator effect
#define PU_INVISO	6	// invisibility
#define PU_AMMO		7	// infinite ammo
#define PU_AMMO2	8	// reload current weapon
#define PU_CHEESE	9	// supreme cheese
#define PU_POISON	10	// poison
#define PU_FROZEN	11	// frozen
#define PU_IGNITED	12	// ignited
#define PU_WEAKNESS	13	// weakness
#define PU_STRENGTH	14	// strength
#define PU_WATERWALK 15	// water walk
#define PU_CONFUSION 16	// confusion
#define MAX_POWERUP 17

typedef struct item_t
{
	char name[32];
	char xofs,yofs;	// draw it offset by some pixels
	word sprNum;	// which sprite
	byte fromColor,toColor;	// if you want it to use offcolors
	char bright;	// if you don't want it to use normal brightness
	byte rarity;	// if can be dropped by monsters, how often?
	word flags;		// what special flags it has
	dword theme;		// flags for which themes it goes in
	word trigger;	// what triggers it
	word effect;	// what it does when triggered
	int effectAmt;	// a modifier for the effect
	char msg[64];	// message when effect occurs
	word sound;		// sound when effect occurs
} item_t;

void InitItems(void);
void ExitItems(void);
void RenderItem(int x,int y,byte type,char bright,byte flags);
void InstaRenderItem(int x,int y,byte type,char bright,MGLDraw *mgl);
void DrawRedX(int x,int y,byte candle,MGLDraw *mgl);
item_t *GetItem(int type);
item_t *GetBaseItem(int type);
word NumItems(void);
int NewItem(void);
void UpdateItems(void);
byte GetRandomItem(void);
void SetupRandomItems(void);
int GetTotalRarity(void);
void SaveItems(FILE *f);
void LoadItems(FILE *f);
void DeleteItem(int itm);
int NumCustomSprites(void);
int NumItemSprites(void);
int BrainsGiven(int type);
int CandlesGiven(int type);
int FindItemByName(const char *name);

byte AppendItems(FILE *f);

struct mapTile_t;
void UpdateItem(mapTile_t *m,int width,int offset);

class Guy;
struct mapTile_t;

class Map;
struct world_t;

void MoveMovableItem(int x,int y,Map *map,world_t *world);
void SetCustomItemSprites(char* filename);
void DetectCustomItemSprites(world_t *world);

byte InteractWithItem(Guy *me,mapTile_t *m,int x,int y);
byte TriggerItem(Guy *me,mapTile_t *m,int x,int y);
byte BulletHitItem(byte bulType,mapTile_t *m,int x,int y);

void RepairItemToItem(int n);	// when item N is deleted, repair references to it and others in
								// all items
void RepairItemToSound(int n);	// when sound N is deleted, repair references to it and others in
								// all items
void RepairItemToTile(void);	// when tiles are messed with, repair the references in items

#endif
