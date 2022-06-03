#ifndef WORLD_H
#define WORLD_H

#include "map.h"
#include "tile.h"

#define MAX_MAPS	72

// terrain flags
#define TF_SOLID	1
#define TF_ICE		2
#define TF_MUD		4
#define TF_WATER	8
#define TF_LAVA		16
#define TF_PUSHY	32		// if this is the roof of a wall, the wall is pushable
#define TF_PUSHON	64		// only PUSHON terrain can have things pushed over it
#define TF_ANIM		128		// changes every 1/4 second or so
#define TF_STEP		256		// changes when stepped on
#define TF_DESTRUCT 512		// changes when hit
#define TF_TRANS    1024	// transparent roof
#define TF_MINECART 2048	// minecarts go on it
#define TF_BUNNY	4096	// bunnies travel on it
#define TF_NOGHOST  8192	// ghosts can't cross it
#define TF_NOENEMY	16384	// enemies can't cross it
#define TF_RUBBER	32768	// bouncy!
#define TF_NOPLAYER	65536	// player can't cross it
#define TF_SPACE	131072	// space tiles - make the player hover and move around with little to no traction
#define TF_QUICKS	262144	// quicksand - damages anything not moving
#define TF_PROPULSE	524288	// propulsive tiling - speeds anything touching it up like WHEEEEEEEEEEEEW!!!

typedef struct terrain_t
{
	dword flags;
	word next;
} terrain_t;

typedef struct swc_terrain_t
{
	word flags;
	word next;
} swc_terrain_t;

typedef struct world_t
{
	byte numMaps;
	int  totalPoints;
	Map	 *map[MAX_MAPS];
	word numTiles;
	terrain_t terrain[NUMTILES];
	char author[32];
} world_t;

//maybe needed
typedef struct swc_world_t
{
	byte numMaps;
	int  totalPoints;
	Map	 *map[MAX_MAPS];
	word numTiles;
	swc_terrain_t terrain[NUMTILES];
	char author[32];
} swc_world_t;

extern byte keyChainInLevel[MAX_MAPS];

void WorldLoadBMP(const char *name,byte *dst);

byte NewWorld(world_t *world,MGLDraw *mgl);
byte LoadWorld(world_t *world,const char *fname);
byte SaveWorld(world_t *world,const char *fname);
void FreeWorld(world_t *world);

void InitWorld(world_t *world);
byte GetWorldName(const char *fname,char *buffer,char *authbuffer);

void RepairTileToTile(world_t *w);

void LocateKeychains(world_t *w);
void LogRequirements(world_t *w);

byte BeginAppendWorld(world_t *world,const char *fname);
terrain_t *GetTerrain(world_t *w,word tile);

#endif
