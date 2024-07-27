#ifndef WORLD_H
#define WORLD_H

#include "winpch.h"
#include "map.h"
#include "tile.h"

// the only version number we accept for world files
#define WORLD_VERSION	1

#define MAX_MAPS	20

// terrain flags
#define TF_SOLID	1		// nothing can pass
#define TF_NOPUSH	2		// can walk on it, but can't push stuff over it
#define TF_UPONLY	4		// can only push stuff UP on it
#define TF_RTONLY	8		// can only push stuff to the RIGHT on it
#define TF_DNONLY	16		// can only push stuff DOWN on it
#define TF_LFONLY	32		// can only push stuff LEFT on it
#define TF_GREEN	64		// can only push GREEN on it
#define TF_BROWN	128		// can only push BROWN on it
#define TF_BLUE		256		// can only push BLUE on it
#define TF_RED		512		// can only push RED on it
#define TF_YELLOW	1024	// can only push YELLOW on it
#define TF_PURPLE	2048	// can only push PURPLE on it
#define TF_AQUA		4096	// can only push AQUA on it
#define TF_SPIKES	8192	// deadly to player, can't push on it

#define WORLD_DESC_LINELEN	32
#define WORLD_DESC_LINES	4

typedef struct terrain_t
{
	int flags;
} terrain_t;

typedef struct world_t
{
	byte version;
	char tileName[64];
	byte numMaps;
	char setname[WORLD_DESC_LINELEN];
	char desc[WORLD_DESC_LINELEN*WORLD_DESC_LINES];
	Map	 *map[MAX_MAPS];

	// not used anymore, but loaded and saved
	terrain_t terrain[200];
} world_t;

void WorldLoadBMP(const char *name,byte *dst);

byte NewWorld(world_t *world,MGLDraw *mgl);
byte LoadWorld(world_t *world,const char *fname,MGLDraw *mgl);
byte SaveWorld(world_t *world,const char *fname);
void FreeWorld(world_t *world);

void InitWorld(world_t *world,byte worldNum);
void GetWorldName(const char *fname,char *buf,char *auth);
byte GetWorldTiles(world_t *world,MGLDraw *mgl);

void MoveLevel(world_t *world,byte num,char d);
byte WorldNewLevel(world_t *world,Map *copyMe);
void WorldDeleteLevel(world_t *world,byte num);

#endif
