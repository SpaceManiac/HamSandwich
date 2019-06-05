#ifndef WORLD_H
#define WORLD_H

#include "winpch.h"
#include "map.h"
#include "tile.h"

#define MAX_MAPS	64

// terrain flags
#define TF_SOLID	1
#define TF_ICE		2
#define TF_MUD		4
#define TF_WATER	8
#define TF_LAVA		16
#define TF_PUSHY	32		// if this is the roof of a wall, the wall is pushable
#define TF_PUSHON	64		// only PUSHON terrain can have things pushed over it
#define TF_ANIM		128
#define TF_STEP		256
#define TF_DESTRUCT 512
#define TF_TRANS    1024
#define TF_NOSHADOW	2048	// if this is a wall, don't cast a shadow
#define TF_NOVILLAGER 4096	// villagers can't walk here
#define TF_FIRE		8192	// fire erupts from this tile (visual only)
#define TF_NOBULLET	16384	// bullets can't pass this

typedef struct terrain_t
{
	int flags;
	word next;
} terrain_t;

typedef struct world_t
{
	byte numMaps;
	int  totalPoints;
	Map	 *map[MAX_MAPS];
	terrain_t terrain[NUMTILES];
} world_t;

void WorldLoadBMP(const char *name,byte *dst);

byte NewWorld(world_t *world,MGLDraw *mgl);
byte LoadWorld(world_t *world,const char *fname);
byte SaveWorld(world_t *world,const char *fname);
void FreeWorld(world_t *world);

void InitWorld(world_t *world,byte worldNum);
void GetWorldName(const char *fname,char *buf);
int  GetWorldPoints(const char *fname);

#endif
