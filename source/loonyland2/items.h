#ifndef ITEMS_H
#define ITEMS_H

#include "winpch.h"
#include "jamulspr.h"

enum itemList {
	IT_NONE,
	IT_TREE1,
	IT_TREE2,
	IT_TREE3,
	IT_CABIN1,
	IT_CABIN2,
	IT_POST1,
	IT_POST2,
	IT_POST3,
	IT_AXE,
	IT_MAGICAXE,
	IT_GOLDENAXE,
	IT_AMULET,
	IT_MAGICAMULET,
	IT_GOLDENAMULET,
	IT_PARKA,
	IT_MAGICPARKA,
	IT_GOLDENPARKA,
	IT_COIN,
	IT_SCROLL,
	IT_FESTIVUSPOLE,
	IT_BARREL,
	IT_CAULDRON,
	IT_BURIEDASB,
	IT_BURIEDSTONE,	// sharpening stone buried
	IT_CAVE,
	IT_ROCK,
	IT_ROCK2,
	IT_ROCK3,
	IT_ROCK4,
	IT_ICESPIKE,
	IT_DEADPLANT,
	IT_DEADPLANT2,
	IT_DEADPLANT3,
	IT_ALLUSIONARY,
	IT_SIGNAXE,
	IT_SIGNBIZ,
	IT_BED,
	IT_BED2,
	IT_BOOKCASE,
	IT_TABLE,
	IT_TABLE2,
	IT_CHAIR,
	IT_CHAIR2,
	IT_CHAIR3,
	IT_CHAIR4,
	IT_STOOL,
	IT_DRESSER,
	IT_TENCOIN,
	IT_LOGPILE,
	IT_STUMP,
	IT_CHOPTREE,
	IT_TREEXING,	// chopped down tree for crossing
	IT_INVISOWALK,	// invisible can be walked on
	IT_EXIT,		// the glowy thing that marks exits
	IT_INVISO,		// invisible obstacle for fleshing out large objects
	IT_OVEN,
	IT_PINESTUMP,
	IT_SIGN,
	IT_WATERROCK,
	IT_WATERROCK2,
	IT_WATERROCK3,
	IT_WATERROCK4,
	IT_CHEST1,
	IT_CHEST2,
	IT_CHEST3,
	IT_CHEST4,
	IT_CHEST5,
	IT_DOOR1,
	IT_DOOR2,
	IT_DOOR3,
	IT_DOOR4,
	IT_DOOR5,
	IT_DOOROPEN1,
	IT_DOOROPEN2,
	IT_DOOROPEN3,
	IT_DOOROPEN4,
	IT_DOOROPEN5,
	IT_SMLCAVEROCK,
	IT_SMLCAVEROCK2,
	IT_CAVEROCK,
	IT_CAVEROCK2,
	IT_CAVEROCK3,
	IT_CLOCK,
	IT_CRACK,
	IT_MTNROCK,
	IT_MTNROCK2,
	IT_MTNROCK3,
	IT_MTNROCKSML,
	IT_NONEVILSIGN,
	IT_ONIONBANNER,
	IT_BUNNYBANNER,
	IT_ROPEDSTUMP,
	IT_SUPPLIES,
	IT_SUPPLIES2,
	IT_SUPCRACK,
	IT_GEYSER,
	IT_PURPLEROCK,
	IT_PURPLEROCK2,
	IT_PURPLEROCKSML,
	IT_PURPLEROCKSML2,
	IT_SHROOM,
	IT_SHROOM2,
	IT_SHROOMBIG,
	IT_SHROOMBIG2,
	IT_SHROOMPATCH,
	IT_SHROOMPATCH2,
	IT_UPGEYSER,
	IT_PURPLEDARK,
	IT_PURPLEDARK2,
	IT_CRYSTAL,
	IT_CRYSTAL2,
	IT_CRYSTAL3,
	IT_CRYSTAL4,
	IT_CRACKUNDER,
	IT_SOUNDINGTREE,
	IT_VINEWALL,
	IT_VINEWALL2,
	IT_VINEWALL3,
	//inventory items (aside from equipment, which is way up there, which is fine since it goes first)
	IT_ROPE,
	// potions
	IT_POT1,
	IT_POT2,
	IT_POT3,
	IT_POT4,
	IT_POT5,
	IT_POT6,
	IT_POT7,
	IT_POT8,
	IT_POT9,
	IT_POT10,
	IT_POT11,
	IT_POT12,
	IT_POT13,
	IT_POT14,
	IT_POT15,
	IT_POT16,
	IT_POT17,
	IT_POT18,
	IT_POT19,
	IT_POT20,
	// herbs
	IT_HERBA,
	IT_HERBB,
	IT_HERBC,
	IT_HERBD,
	IT_HERBE,
	IT_HERBF,
	// junk items
	IT_LOG,		// from puppet
	IT_GEAR,	// from mouse
	IT_CAN,		// from tin soldier
	IT_STUFFING,	// from bear
	IT_SOCK,		// from sock monkey
	IT_ICECUBE,	// from ice titan/hound
	IT_SEED,	// from plant monsters
	IT_STONE,	// from rock enemies
	IT_ECTO,	// from ghost enemies
	IT_CLAW,	// from claw monsters
	// mining
	IT_SILVER,
	IT_GOLD,
	IT_RUBY,
	IT_EMERALD,
	IT_DIAMOND,
	// lenses
	IT_LENSA,
	IT_LENSB,
	IT_LENSC,
	IT_LENSD,
	IT_LENSE,
	IT_LENSF,
	IT_LENSG,
	IT_LENSH,
	IT_LENSI,
	IT_LENSJ,
	IT_LENSK,
	IT_LENSL,
	IT_LENSM,
	// unlosable quest items
	IT_KEY1,
	IT_KEY2,
	IT_KEY3,
	IT_KEY4,
	IT_KEY5,
	IT_SHARPSTONE,
	IT_BCRYSTAL,		// binding crystal
	IT_SHOVEL,
	IT_ICESKATE,
	IT_WINGBOOT,
	IT_PRESENT,
	IT_UNDIES,
	IT_TOOLBOX,
	IT_LETTER,
	IT_SOUNDING,	// sounding crystal
	IT_LAMEGLASSES,	// glasses you have initially
	IT_GLASSES,		// fancy magic glasses
	IT_SWORD,		// souvenir sword, collect in arena
	IT_PACKAGE,		// Curaga's package
	IT_CANDY,		// from a baby
	IT_STOLEN,		// stolen goods
	IT_BOOK,		// borrowed book
	IT_THING,		// thing of majig
	IT_THING2,		// thing of mabob
	IT_WATCH,		// watch of makalit

	// no longer gettable stuff
	IT_ENERGYBARRIER,
	IT_ICEBLOCK,
	IT_ICEMELT,
	IT_DEADVINE,
	IT_DEADVINE2,
	IT_DEADVINE3,

	IT_TOYCRYSTAL,	// Control Crystal for the toys
	IT_CASTLESWITCH,
	// these 3 are just used for visuals
	IT_ARTAXE,
	IT_ARTAMMY,
	IT_ARTPARKA,
	IT_PICTURE,
	IT_ARTBIT,
};

#define MAX_ITMS	212

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
#define IF_INVIS		1024	// don't draw it at all, except in the editor
#define IF_BOTTOM		2048	// draw under everything else, even shadows
#define IF_STEAM		4096	// it hurts you if you have no asbestos.  If you have asbestos, it is an exit to a specific place

#pragma pack(push, 1)
typedef struct item_t
{
	char name[32];
	int sprset,rate,spr,spr2;
	dword flags;
} item_t;
#pragma pack(pop)

class Map;
extern item_t itemInfo[MAX_ITMS];

void InitItems(void);
void ExitItems(void);
void UpdateItems(void);
dword ItemFlags(int type);
void RenderItem(int x,int y,int z,byte type,byte info,char bright);
void InstaRenderItem(int x,int y,byte type,char bright,MGLDraw *mgl);
void InstaRenderItem2(int x,int y,byte type,int anim,char bright,MGLDraw *mgl);
char *ItemName(byte itm);
void ChestPop(byte type,int mapx,int mapy);
void RenderClock(int x,int y,MGLDraw *mgl);

#endif
