#ifndef LEGACYLOAD_H
#define LEGACYLOAD_H

#include "mgldraw.h"

#define OLD_MAX_MAPMONS 128
#define OLD_MAX_SPECIAL 32

#define OLD_MAX_MAPS	24

typedef struct old_special_t
{
	word trigger;
	byte trigValue;
	byte effect;
	byte x,y;
	byte effectX,effectY;
	int value;
	char msg[32];
} old_special_t;

typedef struct old_mapTile_t
{
	byte floor;
	byte wall;
	byte item;
	char light;
	char templight;
	byte opaque;
} old_mapTile_t;

typedef struct old_mapBadguy_t
{
	byte x,y;
	byte type;
} old_mapBadguy_t;

typedef struct old_map_t
{
	int width,height;
	old_mapTile_t *map;
	char name[32];
	byte song;
	byte flags;
	old_mapBadguy_t badguy[OLD_MAX_MAPMONS];
	old_special_t   special[OLD_MAX_SPECIAL];
} old_map_t;


typedef struct old_terrain_t
{
	word flags;
	byte next;
} old_terrain_t;

typedef struct old_world_t
{
	byte numMaps;
	int  totalPoints;
	old_map_t *map[OLD_MAX_MAPS];
	old_terrain_t terrain[200];
} old_world_t;

struct world_t;

byte Legacy_LoadWorld(world_t *world,const char *fname);
byte Legacy_GetWorldName(const char *fname,char *buf);
const char *GetOriginalSongName(byte song);

#endif
