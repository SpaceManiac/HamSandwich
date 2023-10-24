#ifndef WORLD_H
#define WORLD_H

#include "map.h"
#include "tile.h"

constexpr int MAX_MAPS = 64;

// terrain flags
enum : dword
{
	TF_SOLID      = 1 << 0,
	TF_ICE        = 1 << 1,
	TF_MUD        = 1 << 2,
	TF_WATER      = 1 << 3,
	TF_LAVA       = 1 << 4,
	TF_PUSHY      = 1 << 5,   // if this is the roof of a wall, the wall is pushable
	TF_PUSHON     = 1 << 6,   // only PUSHON terrain can have things pushed over it
	TF_ANIM       = 1 << 7,   // changes every 1/4 second or so
	TF_STEP       = 1 << 8,   // changes when stepped on
	TF_DESTRUCT   = 1 << 9,   // changes when hit
	TF_TRANS      = 1 << 10,  // transparent roof
	TF_MINECART   = 1 << 11,  // minecarts go on it
	TF_BUNNY      = 1 << 12,  // bunnies travel on it
	TF_NOGHOST    = 1 << 13,  // ghosts can't cross it
	TF_NOENEMY    = 1 << 14,  // enemies can't cross it
	TF_RUBBER     = 1 << 15,  // bouncy!
	// Extended flags that encroach on the "next" field when saved/loaded.
	TF_SHADOWLESS = 1 << 16,  // shadowless wall
	// Adding flag 1<<17 would leave room for 16384 tiles.
	// Adding flag 1<<18 would leave room for 8192 tiles.
	// Adding flag 1<<19 would leave room for 4096 tiles.
	// Adding flag 1<<20 would leave room for 2048 tiles.
	// Adding flag 1<<21 would leave room for 1024 tiles.
	// Flag 1<<22 wouldn't leave room for 1000 tiles, so cannot be added.
};

struct io_terrain_t
{
	word flags;
	word next;
};

struct terrain_t
{
	dword flags;
	word next;
};

struct world_t
{
	byte numMaps;
	int  totalPoints;
	Map	 *map[MAX_MAPS];
	word numTiles;
	terrain_t terrain[NUMTILES];
	char author[32];
};

extern byte keyChainInLevel[MAX_MAPS];

byte NewWorld(world_t *world,MGLDraw *mgl);
byte LoadWorld(world_t *world,const char *fname);
byte SaveWorld(world_t *world,const char *fname);
void FreeWorld(world_t *world);

void InitWorld(world_t *world);
bool GetWorldName(const char *fname,char *buffer,char *authbuffer);

void RepairTileToTile(world_t *w);

void LocateKeychains(world_t *w);

byte BeginAppendWorld(world_t *world,const char *fname);
terrain_t *GetTerrain(world_t *w,word tile);

bool MustBeHamSandwichWorld(const world_t *world);

#endif
