#ifndef WORLD_IO_SUPREME_H
#define WORLD_IO_SUPREME_H

#include "jamultypes.h"
#include "string_extras.h"

struct SDL_IOStream;
struct world_t;
class Map;

bool Supreme_GetWorldName(SDL_IOStream *f, StringDestination name, StringDestination author);
bool Supreme_LoadWorld(world_t *world, const char* fname, SDL_IOStream *f);
bool Supreme_SaveWorld(const world_t *world, SDL_IOStream *f);
bool Supreme_CanSaveWorld(const world_t *world);

bool BeginAppendWorld(world_t *world, const char *fname);

#endif
