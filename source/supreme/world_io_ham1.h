#ifndef WORLD_IO_HAM1_H
#define WORLD_IO_HAM1_H

#include "jamultypes.h"
#include "string_extras.h"

struct world_t;

bool Ham_GetWorldName(const char *fname, StringDestination name, StringDestination author);
byte Ham_LoadWorld(world_t *world, const char *fname);
byte Ham_SaveWorld(const world_t *world, const char *fname);

#endif
