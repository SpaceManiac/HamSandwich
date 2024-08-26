#ifndef MAP_H
#define MAP_H

#include "tile.h"
#include "items.h"
#include "special.h"

constexpr int MAX_LIGHT = 16;
constexpr int MIN_LIGHT = -32;

// The maximum # of monsters you can put on the map.
constexpr int MAX_MAPMONS = 128;

// Maximum width or height.
constexpr int MAX_MAPSIZE = 200;

// Flags for calling map render.
enum : word
{
	MAP_SHOWLIGHTS      = 1 << 0,
	MAP_SHOWWALLS       = 1 << 1,
	MAP_SHOWPICKUPS     = 1 << 2,
	MAP_SHOWBADGUYS     = 1 << 3,
	MAP_SHOWSPECIALS    = 1 << 4,
	MAP_SHOWSELECT      = 1 << 5,
	MAP_SHOWOTHERITEMS  = 1 << 6,
	MAP_TEMPTORCH       = 1 << 7,
	MAP_ZOOMOUT         = 1 << 8,
};

constexpr word MAP_SHOWITEMS = MAP_SHOWPICKUPS | MAP_SHOWOTHERITEMS;  // combine them in one

// Map flags. SERIALIZED.
enum : word
{
	MAP_SNOWING     = 1 << 0,
	MAP_RAIN        = 1 << 1,
	MAP_HUB         = 1 << 2,
	MAP_SECRET      = 1 << 3,
	MAP_TORCHLIT    = 1 << 4,
	MAP_WELLLIT     = 1 << 5,
	MAP_STARS       = 1 << 6,
	MAP_UNDERWATER  = 1 << 7,
	MAP_LAVA        = 1 << 8,
	MAP_STEALTH     = 1 << 9,
	MAP_WAVY        = 1 << 10,
	MAP_OXYGEN      = 1 << 11,
};
constexpr int NUM_LVL_FLAGS = 12;
const char* MapFlagName(int flagIndex);  // expects 0, 1, 2, 3, not the constants above

// map updating modes
enum
{
	UPDATE_GAME,
	UPDATE_EDIT,
	UPDATE_FADE,
	UPDATE_FADEIN,
};

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

// SERIALIZED.
typedef struct mapBadguy_t
{
	byte x,y;
	dword type;
	byte item;
} mapBadguy_t;

struct world_t;

class Map
{
	public:
		explicit Map(byte size, const char *name);
		explicit Map(byte width, byte height, const char *name);
		explicit Map(const Map *m);

		~Map(void);

		void Init(world_t *wrld);
		void Render(world_t *world,int camX,int camY,byte flags);
		void RenderSelect(world_t *world,int camX,int camY,byte flags);

		byte DropItem(int x,int y,byte itm);
		void PermaTorch(int x,int y,char brt);
		void TempTorch(int x,int y,char brt);
		void BrightTorch(int x,int y,char brt,byte size);
		void GlowCursor(int x,int y,char brt,byte size);
		void DimTorch(int x,int y,byte size);
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
		void RenderStars(int camX, int camY);
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

void InitStars();

#endif
