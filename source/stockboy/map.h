#ifndef MAP_H
#define MAP_H

#include "winpch.h"
#include "tile.h"
#include "items.h"
#include "goal.h"

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

// map updating modes
#define UPDATE_GAME		0
#define UPDATE_EDIT		1
#define UPDATE_FADE		2
#define UPDATE_FADEIN	3

// maps are all 20x20
#define MAP_SIZE	20

// goodie types
#define GOODIE_BOOM		0
#define GOODIE_STEP		1
#define GOODIE_PUSH		2
#define GOODIE_MONSSTEP 3

/////////////////////////

typedef struct mapTile_t
{
	word floor;
	byte item;
	byte itemInfo;
	char light;
	char templight;
	byte opaque;
} mapTile_t;

struct world_t;

class Map
{
	public:
		Map(int wid,int hei,const char *name);
		Map(Map *m);
		Map(SDL_RWops *f);
		~Map(void);

		byte Save(SDL_RWops *f);
		void Init(world_t *wrld);
		void Render(world_t *world,int camX,int camY,byte flags);
		void RenderTiny(world_t *world,int x,int y);
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
		void SmoothLight(int x,int y);
		void SmoothLights(void);
		void ClearStarts(void);

		void Parallelize(void);

		byte TurnArrows(void);
		byte ToggleDoors(byte color);
		void PressurePlateCheck(int x,int y);
		byte SelectTeleporter(int srcx,int srcy,int *dstx,int *dsty);
		byte GoodieCheck(int x,int y,byte type);
		void Shift(void);

		int width,height;
		mapTile_t map[MAP_SIZE*MAP_SIZE];
		char name[32];
		goal_t goal;
		goal_t antigoal;	// do this and you LOSE
		word timer;			// number of seconds you have
		word genTime;		// time between generations of monstermakers
	private:
		void LOSPoints(int x,int y,int curx,int cury,int *p1x,int *p1y,int *p2x,int *p2y);
};

byte PlaceItemCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TorchCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TempTorchCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TempBrightCallback(int x,int y,int cx,int cy,int value,Map *map);
byte MapCheckCallback(int x,int y,int cx,int cy,int value,Map *map);

#endif
