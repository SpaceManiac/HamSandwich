#ifndef MAP_H
#define MAP_H

#include "winpch.h"
#include "tile.h"
#include "items.h"

const int MAX_LIGHT = 16;
const int MIN_LIGHT = -32;

// the maximum # of monsters you can put on the map
const int MAX_MAPMONS = 128;
const int MAX_SPECIAL = 32;

// flags for calling map render
enum {
	MAP_SHOWLIGHTS = 1,
	MAP_SHOWWALLS = 2,
	MAP_SHOWITEMS = 4,
	MAP_SHOWBADGUYS = 8,
	MAP_SHOWSPECIALS = 16
};

// special trigger flags
enum {
	TRG_STEP = 1,
	TRG_ENEMYSTEP = 2,
	TRG_NEAR = 4,
	TRG_PASSLEVELS = 8,
	TRG_KEYCHAINS = 16,
	TRG_KILLALL = 32,
	TRG_HAVEBRAINS = 64,
	TRG_SHOOT = 128,
	TRG_REPEATABLE = 256,		// works more than once
	TRG_MESSAGE = 512,		// displays a message
	TRG_CHAIN = 1024,	// goes off if any special in an adjacent square goes off
	TRG_TIMED = 2048,	// occurs every N seconds
	TRG_RANDOM = 4096,	// occurs randomly, with an N% chance each second
	TRG_HAVELOONY = 8192,	// has loony key for this world
	TRG_KILLONE = 16384,	// kill just one of the chosen monster type
	TRG_FLOORHERE = 32768	// the floor/roof here is N
};

// special effect choices
enum {
	SPC_NONE = 0,
	SPC_SUMMON,
	SPC_ZAPWALL,
	SPC_RAISEWALL,
	SPC_TOGGLEWALL,
	SPC_TELEPORT,
	SPC_LIGHT,
	SPC_GOTOMAP,
	SPC_EXIT,
	SPC_PICTURE,
	SPC_PLAYSONG,
	SPC_PLAYSOUND,
	SPC_DROPITEM,
	SPC_TEMPLIGHT,
	SPC_SWAPMAP,
	SPC_CHGTILE,
	SPC_PLAYSOUND2,
	SPC_WINANDGO,
	SPC_COPYMAP,
	SPC_KILLMONS,
	SPC_CHGMONS,
	SPC_RMVSPCL,
	SPC_TOGGLEITEM,

	SPC_MAXEFFECTS
};

// Map flags
enum {
	MAP_SNOWING = 1,
	MAP_MANYITEMS = 2,
	MAP_SECRET = 4,
	MAP_TORCHLIT = 8,
	MAP_STARS = 16
};

// map updating modes
enum {
	UPDATE_GAME = 0,
	UPDATE_EDIT,
	UPDATE_FADE,
	UPDATE_FADEIN
};

typedef struct special_t
{
	word trigger;
	byte trigValue;
	byte effect;
	byte x, y;
	byte effectX, effectY;
	int value;
	char msg[32];
} special_t;

typedef struct mapTile_t
{
	byte floor;
	byte wall;
	byte item;
	char light;
	char templight;
	byte opaque;
} mapTile_t;

typedef struct mapBadguy_t
{
	byte x, y;
	byte type;
} mapBadguy_t;

struct world_t;

class Map
{
public:
	Map(byte size, const char *name);
	Map(Map *m);
	Map(FILE *f);
	~Map(void);

	byte Save(FILE *f);
	void Init(world_t *wrld);
	void Render(world_t *world, int camX, int camY, byte flags);
	byte DropItem(int x, int y, byte itm);
	void PermaTorch(int x, int y, char brt);
	void TempTorch(int x, int y, char brt);
	void BrightTorch(int x, int y, char brt, byte size);
	void LightSpecial(int x, int y, char brt, byte size);
	void Update(byte mode, world_t *world);
	byte LOS(int x, int y, int radius, int value, byte(*DoIt)(int, int, int, int, int, Map *));
	void Swap(int sx, int sy, int blkwidth, int blkheight, int dx, int dy);
	void Copy(int sx, int sy, int blkwidth, int blkheight, int dx, int dy);
	void SmoothLight(int x, int y);
	void SmoothLights(void);

	int width, height;
	mapTile_t *map;
	char name[32];
	byte song;
	byte flags;
	mapBadguy_t badguy[MAX_MAPMONS];
	special_t special[MAX_SPECIAL];

	// Gourad stuff
	char smoothLight[9];
	char LightOf(int x, int y);
	bool Wall(int x, int y);
	byte MakeSmoothShadow(int x, int y);
	char* MakeSmoothLighting(bool beZero, int x, int y);

private:
	void LOSPoints(int x, int y, int curx, int cury, int *p1x, int *p1y, int *p2x, int *p2y);
};

byte PlaceItemCallback(int x, int y, int cx, int cy, int value, Map *map);
byte TorchCallback(int x, int y, int cx, int cy, int value, Map *map);
byte TempTorchCallback(int x, int y, int cx, int cy, int value, Map *map);
byte TempBrightCallback(int x, int y, int cx, int cy, int value, Map *map);

class Guy;
void SpecialStepCheck(Map *map, int x, int y, Guy *me);
void SpecialTakeEffect(byte num, Map *map, special_t *spcl, Guy *victim);

void ZapWall(Map *map, int x, int y, byte newFloor);
void SpecialShootCheck(Map *map, int x, int y);
void SpecialAnytimeCheck(Map *map);
void SpecialKillCheck(Map *map, byte type);
void RenderSpecialXes(MGLDraw *mgl, Map *map, byte world);
int TotalBrains(void);

void OpenMapFile(void);
void CloseMapFile(void);

#endif
