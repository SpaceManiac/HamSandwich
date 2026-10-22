#ifndef MAP_H
#define MAP_H

#include "winpch.h"
#include "tile.h"
#include "items.h"
#pragma pack(4)

#define MAX_LIGHT 16
#define MIN_LIGHT -32

// the maximum # of monsters you can put on the map
#define MAX_MAPMONS 256
#define MAX_SPECIAL 256

// flags for calling map render
#define MAP_SHOWLIGHTS 1
#define MAP_SHOWWALLS  2
#define MAP_SHOWITEMS  4
#define MAP_SHOWBADGUYS 8
#define MAP_SHOWSPECIALS 16

//special trigger flags
#define TRG_STEP		1
#define TRG_ENEMYSTEP	2
#define TRG_NEAR		4
#define	TRG_KILLALL		8
#define TRG_SHOOT		16
#define TRG_REPEATABLE	32		// works more than once
#define TRG_MESSAGE		64		// displays a message
#define TRG_CHAIN		128		// goes off if any special in an adjacent square goes off
#define TRG_TIMED		256		// occurs every N seconds
#define TRG_RANDOM		512		// occurs randomly, with an N% chance each second
#define TRG_VARON		1024	// triggers if var N=N2
#define TRG_VAROFF		2048	// triggers if var N!=N2
#define TRG_GETITEM		4096	// triggers if the item on the chosen space is taken
#define TRG_DELAYCHAIN	8192	// same as chain, but goes off N ticks later
#define TRG_ITEMHERE	16384	// triggers if item type N is on this space

//special effect choices
#define SPC_NONE		0
#define SPC_SUMMON		1
#define SPC_ZAPWALL		2
#define SPC_RAISEWALL	3
#define SPC_TOGGLEWALL	4
#define SPC_TELEPORT	5
#define SPC_LIGHT		6
#define SPC_GOTOMAP		7
#define SPC_SETVAR		8
#define SPC_PICTURE		9
#define SPC_COPYMAP		10
#define SPC_PLAYSOUND	11
#define SPC_DROPITEM	12
#define SPC_TEMPLIGHT	13
#define SPC_SWAPMAP		14
#define SPC_CHGTILE		15
#define SPC_CHAT		16
#define SPC_LIGHTBALL	17
#define SPC_MAXEFFECTS  18

// Map flags
#define MAP_SNOWING		1
#define MAP_LITDUNGEON	2
#define MAP_SECRET		4
#define MAP_TORCHLIT	8
#define MAP_STARS		16

// map updating modes
#define UPDATE_GAME		0
#define UPDATE_EDIT		1
#define UPDATE_FADE		2
#define UPDATE_FADEIN	3

typedef struct special_t
{
	dword trigger;
	int trigValue,trigValue2;
	int	trigX,trigY;
	byte effect;
	int x,y;
	byte effectTag;
	int effectX,effectY;
	int value;
	char msg[32];
} special_t;

typedef struct mapTile_t
{
	word floor;
	word wall;
	byte item;
	byte itemInfo;
	byte tag;
	char light;
	char templight;
	byte opaque;
} mapTile_t;

typedef struct mapBadguy_t
{
	byte x,y;
	byte type;
} mapBadguy_t;

struct world_t;

class Map
{
	public:
		Map(int wid,int hei,const char *name);
		Map(Map *m);
		Map(FILE *f);
		~Map(void);

		byte Save(FILE *f);
		void Init(world_t *wrld);
		void Render(world_t *world,int camX,int camY,byte flags);
		void ShowTags(int camX,int camY,int copyX,int copyY,int copyWid,int copyHei);
		byte DropItem(int x,int y,byte itm);
		void PermaTorch(int x,int y,char brt);
		void TempTorch(int x,int y,char brt);
		void BrightTorch(int x,int y,char brt,byte size);
		void WeakTorch(int x,int y);
		void DarkTorch(int x,int y,char brt,byte size);
		void LightSpecial(int x,int y,char brt,byte size);
		byte CheckLOS(int x,int y,int radius,int x2,int y2);
		void Update(byte mode,world_t *world);
		byte LOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *));
		void Swap(int sx,int sy,int blkwidth,int blkheight,int dx,int dy);
		void Clone(int sx,int sy,int blkwidth,int blkheight,int dx,int dy);
		void SmoothLight(int x,int y);
		void SmoothLights(void);
		void Resize(int wid,int hei);
		void CopyChunk(int cx,int cy,int cwidth,int cheight,int dx,int dy);
		void SaveProgress(FILE *f);
		void LoadFromProgress(FILE *f);
		void ChopTree(int x,int y,byte fx);

		int width,height;
		mapTile_t *map;
		char name[32];
		byte song;
		byte flags;
		mapBadguy_t badguy[MAX_MAPMONS];
		special_t   special[MAX_SPECIAL];

	private:
		void LOSPoints(int x,int y,int curx,int cury,int *p1x,int *p1y,int *p2x,int *p2y);
};

byte PlaceItemCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TorchCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TempTorchCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TempBrightCallback(int x,int y,int cx,int cy,int value,Map *map);
byte MapCheckCallback(int x,int y,int cx,int cy,int value,Map *map);

class Guy;
void SpecialStepCheck(Map *map,int x,int y,Guy *me);
void SpecialTakeEffect(byte num,Map *map,special_t *spcl,Guy *victim);
void SpecialItemGetCheck(Map *map,int x,int y);

void ZapWall(Map *map,int x,int y,word newFloor);
void ChgTile(Map *map,int x,int y,word newTile);
void SpecialShootCheck(Map *map,int x,int y);
void SpecialAnytimeCheck(Map *map);

void ChgTileTag(Map *map,byte tag,word newTile);
void ZapWallTag(Map *map,byte tag,word newFloor);
void RaiseWallTag(Map *map,byte tag,word newWall);
void ToggleWallTag(Map *map,byte tag,word newWall);
void DropItemTag(Map *map,byte type,byte tag,byte fire);

void SpecialPushCheck(Map *map,int x,int y);
void CheckForChopTree(Map *map,int x,int y);
byte RopeStump(Map *map,int x,int y,byte fx);
byte TerrainIs(int x,int y,word type);
void PrisonBreak(Map *map,byte situation);
void SmashFrostGate(Map *map,byte fx);
byte DrawStones(Map *map,Guy *me,word dmg);
void ExplodeStones(Map *map);

void DisableExits(Map *map);

#endif
