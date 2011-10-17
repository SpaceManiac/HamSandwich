#ifndef MAP_H
#define MAP_H

#include "winpch.h"
#include "tile.h"
#include "items.h"

#define MAX_LIGHT 16
#define MIN_LIGHT -32

// the maximum # of monsters you can put on the map
#define MAX_MAPMONS 128
#define MAX_SPECIAL 32

// flags for calling map render
#define MAP_SHOWLIGHTS 1
#define MAP_SHOWWALLS  2
#define MAP_SHOWITEMS  4
#define MAP_SHOWBADGUYS 8
#define MAP_SHOWSPECIALS 16

// special trigger flags
#define TRG_STEP		1
#define TRG_ENEMYSTEP	2
#define TRG_NEAR		4
#define TRG_PASSLEVELS	8
#define TRG_KEYCHAINS	16
#define	TRG_KILLALL		32
#define TRG_HAVEBRAINS	64
#define TRG_SHOOT		128
#define TRG_REPEATABLE	256		// works more than once
#define TRG_MESSAGE		512		// displays a message
#define TRG_CHAIN		1024	// goes off if any special in an adjacent square goes off
#define TRG_TIMED		2048	// occurs every N seconds
#define TRG_RANDOM		4096	// occurs randomly, with an N% chance each second
#define TRG_HAVELOONY	8192	// has loony key for this world
#define TRG_KILLONE		16384	// kill just one of the chosen monster type
#define TRG_FLOORHERE	32768	// the floor/roof here is N

// special effect choices
#define SPC_NONE		0
#define SPC_SUMMON		1
#define SPC_ZAPWALL		2
#define SPC_RAISEWALL	3
#define SPC_TOGGLEWALL	4
#define SPC_TELEPORT	5
#define SPC_LIGHT		6
#define SPC_GOTOMAP		7
#define SPC_EXIT		8
#define SPC_PICTURE		9
#define SPC_PLAYSONG	10
#define SPC_PLAYSOUND	11
#define SPC_DROPITEM	12
#define SPC_TEMPLIGHT	13
#define SPC_SWAPMAP		14
#define SPC_CHGTILE		15
#define SPC_PLAYSOUND2	16
#define SPC_WINANDGO	17
#define SPC_COPYMAP		18
#define SPC_KILLMONS	19
#define SPC_CHGMONS		20
#define SPC_RMVSPCL		21
#define SPC_TOGGLEITEM	22

#define SPC_MAXEFFECTS  23

// Map flags
#define MAP_SNOWING		1
#define MAP_MANYITEMS	2
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
