#ifndef WORLD_IO_HAM1_H
#define WORLD_IO_HAM1_H

#include "jamultypes.h"

struct world_t;

bool Ham_GetWorldName(const char *fname, char *buffer, char *authbuffer);
byte Ham_LoadWorld(world_t *world, const char *fname);
byte Ham_SaveWorld(const world_t *world, const char *fname);

#endif
