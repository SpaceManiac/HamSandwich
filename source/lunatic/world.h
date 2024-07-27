#ifndef WORLD_H
#define WORLD_H

#include "winpch.h"
#include "map.h"
#include "tile.h"

const int MAX_MAPS = 24;

// terrain flags
enum {
	TF_SOLID = 1,
	TF_ICE = 2,
	TF_MUD = 4,
	TF_WATER = 8,
	TF_LAVA = 16,
	TF_PUSHY = 32,		// if this is the roof of a wall, the wall is pushable
	TF_PUSHON = 64,		// only PUSHON terrain can have things pushed over it
	TF_ANIM = 128,
	TF_STEP = 256,
	TF_DESTRUCT = 512,
	TF_TRANS = 1024,
	TF_MINECART = 2048,
	TF_BUNNY = 4096
};

struct terrain_t
{
	word flags;
	byte next;
};

struct world_t
{
	byte numMaps;
	int totalPoints;
	Map * map[MAX_MAPS];
	terrain_t terrain[200];
};

void WorldLoadBMP(char *name, byte *dst);

byte NewWorld(world_t *world, MGLDraw *mgl);
byte LoadWorld(world_t *world, const char *fname);
byte SaveWorld(world_t *world, const char *fname);
void FreeWorld(world_t *world);

void InitWorld(world_t *world, byte worldNum);
void GetWorldName(const char *fname, char *buf);
int GetWorldPoints(const char *fname);

#endif
