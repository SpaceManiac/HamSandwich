#ifndef WORLD_IO_SUPREME_H
#define WORLD_IO_SUPREME_H

#include "jamultypes.h"
#include "string_extras.h"

struct SDL_RWops;
struct world_t;

bool Supreme_GetWorldName(SDL_RWops *f, StringDestination name, StringDestination author);
byte Supreme_LoadWorld(world_t *world, SDL_RWops *f);
byte Supreme_SaveWorld(world_t *world, const char *fname);

#endif
