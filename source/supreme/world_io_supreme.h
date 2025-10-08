#ifndef WORLD_IO_SUPREME_H
#define WORLD_IO_SUPREME_H

#include "jamultypes.h"
#include "string_extras.h"

struct SDL_RWops;
struct world_t;
class Map;

bool Supreme_GetWorldName(SDL_RWops *f, StringDestination name, StringDestination author);
bool Supreme_LoadWorld(world_t *world, const char* fname, SDL_RWops *f);
bool Supreme_SaveWorld(const world_t *world, SDL_RWops *f);
bool Supreme_CanSaveWorld(const world_t *world);

void LoadTerrain(world_t *world, const char *fname, SDL_RWops *f);
Map *LoadMap(SDL_RWops *f);

#endif
