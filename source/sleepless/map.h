#ifndef MAP_H
#define MAP_H

#include "tile.h"
#include "items.h"
#include "special.h"

#define MAX_LIGHT 16
#define MIN_LIGHT -32

// the maximum # of monsters you can put on the map
#define MAX_MAPMONS 128

// maximum width or height
#define MAX_MAPSIZE	200

// flags for calling map render
#define MAP_SHOWLIGHTS 1
#define MAP_SHOWWALLS  2
#define MAP_SHOWPICKUPS  4
#define MAP_SHOWBADGUYS 8
#define MAP_SHOWSPECIALS 16
#define MAP_SHOWSELECT	32
#define MAP_SHOWOTHERITEMS 64
#define MAP_TEMPTORCH	128

#define MAP_SHOWITEMS (MAP_SHOWPICKUPS|MAP_SHOWOTHERITEMS)	// combine them in one

// Map flags
#define MAP_SNOWING		1
#define MAP_RAIN		2
#define MAP_HUB			4
#define MAP_SECRET		8
#define MAP_TORCHLIT	16
#define MAP_WELLLIT		32
#define MAP_STARS		64
#define MAP_UNDERWATER	128
#define MAP_LAVA		256
#define MAP_STEALTH		512
#define MAP_WAVY		1024

#define NUM_LVL_FLAGS	11

// map updating modes
#define UPDATE_GAME		0
#define UPDATE_EDIT		1
#define UPDATE_FADE		2
#define UPDATE_FADEIN	3

typedef struct mapTile_t
{
	word floor;
	word wall;
	byte item;
	char light;
	char templight;
	byte opaque;
	byte select;
} mapTile_t;

typedef struct saveTile_t
{
	word floor;
	word wall;
	byte item;
	char light;
} saveTile_t;

typedef struct mapBadguy_t
{
	byte x,y;
	byte type;
	byte item;
} mapBadguy_t;

struct world_t;

class Map
{
	public:
		Map(byte size,const char *name);
		Map(Map *m);
		Map(SDL_RWops *f);

		~Map(void);

		byte Save(SDL_RWops *f);
		void SaveMapData(SDL_RWops *f);
		void LoadMapData(SDL_RWops *f);
		void Init(world_t *wrld);
		void Render(world_t *world,int camX,int camY,byte flags);
		void RenderSelect(world_t *world,int camX,int camY,byte flags);

		byte DropItem(int x,int y,byte itm);
		void PermaTorch(int x,int y,char brt);
		void TempTorch(int x,int y,char brt);
		void BrightTorch(int x,int y,char brt,byte size);
		void GlowCursor(int x,int y,char brt,byte size);
		void DimTorch(int x,int y,byte size);
		void InstaTorch(int x,int y,char brt);
		void LightSpecial(int x,int y,char brt,byte size);
		void LightRect(int x,int y,int x2,int y2,char brt,byte perm);
		void Update(byte mode,world_t *world);

		byte LOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *));
		byte TightLOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *));
		byte TightestLOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *));

		int  ItemCount(byte itm);
		int  ItemCountInRect(byte itm,int x,int y,int x2,int y2);

		void Swap(int sx,int sy,int blkwidth,int blkheight,int dx,int dy);
		void Copy(int sx,int sy,int blkwidth,int blkheight,int dx,int dy);
		void TileChange(int x,int y,int floor,int wall,byte fx);
		void ContiguousTileChange(int x,int y,int floor,int wall,byte fx);
		void AllTileChange(int x,int y,int floor,int wall,byte fx);

		byte ItemChange(int x,int y,byte item,byte fx);
		byte ContiguousItemChange(int x,int y,byte item,byte fx);
		byte AllItemChange(int x,int y,byte item,byte fx);

		byte FindGuy(int x,int y,byte size,Guy *g);

		byte Keychains(void);	// return bitflags for which keychains are in this level

		mapTile_t *GetTile(int x,int y);
		void FindNearBrain(int myx,int myy);
		void FindNearCandle(int myx,int myy);

		void SmoothLight(int x,int y);
		void SmoothLights(void);
		byte Resize(byte w,byte h);

		byte CompareRegions(int x,int y,int x2,int y2,int tx,int ty,byte checkMons);

		byte width,height;
		mapTile_t *map;
		char name[32];
		char song[32];
		word flags;
		word numBrains;
		word numCandles;
		word itemDrops;	// how often items drop, a fixshifted percent
		mapBadguy_t badguy[MAX_MAPMONS];
		special_t   special[MAX_SPECIAL];
	private:
		void LOSPoints(int x,int y,int curx,int cury,int *p1x,int *p1y,int *p2x,int *p2y);
};

byte PlaceItemCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TorchCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TempTorchCallback(int x,int y,int cx,int cy,int value,Map *map);
byte GlowCursorCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TempBrightCallback(int x,int y,int cx,int cy,int value,Map *map);
byte FindGuyCallback(int x,int y,int cx,int cy,int value,Map *map);

class Guy;
void SpecialStepCheck(Map *map,int x,int y,Guy *me);
void SpecialTakeEffect(byte num,Map *map,special_t *spcl,Guy *victim);

void ZapWall(Map *map,int x,int y,word newFloor);
void SpecialShootCheck(Map *map,int x,int y);
void SpecialAnytimeCheck(Map *map);
void SpecialKillCheck(Map *map,byte type);

byte GlassSmashed(Map *map);

#endif
