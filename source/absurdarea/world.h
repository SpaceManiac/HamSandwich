#ifndef WORLD_H
#define WORLD_H

#include "winpch.h"
#include "map.h"
#include "tile.h"
#pragma pack(4)

#define MAX_MAPS	64

// terrain flags
#define TF_SOLID	(1<<0)
#define TF_ICE		(1<<1)
#define TF_SKYFX	(1<<2)
#define TF_WATER	(1<<3)
#define TF_LAVA		(1<<4)
#define TF_PUSHY	(1<<5)		// if this is the roof of a wall, the wall is pushable
#define TF_PUSHON	(1<<6)		// only PUSHON terrain can have things pushed over it
#define TF_ANIM		(1<<7)
#define TF_STEP		(1<<8)
#define TF_SKY		(1<<9)	// works the same as cliff, but no particles or sound
#define TF_TRANS    (1<<10)
#define TF_NOSHADOW	(1<<11)	// if this is a wall, don't cast a shadow
#define TF_CLIFF    (1<<12)	// slide down if walked on, with dirt particles flying
#define TF_FIRE		(1<<13)	// fire erupts from this tile (visual only)
#define TF_NOBULLET	(1<<14)	// bullets can't pass this
#define TF_WATERFX	(1<<15)

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

void GetWorldName(const char *fname,char *buf);
int  GetWorldPoints(const char *fname);

#endif
