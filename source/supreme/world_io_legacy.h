#ifndef WORLD_IO_LEGACY_H
#define WORLD_IO_LEGACY_H

#include "jamultypes.h"
#include "string_extras.h"

struct world_t;

bool Legacy_GetWorldName(SDL_RWops *f, StringDestination name);
bool Legacy_LoadWorld(world_t *world, const char *fname);

const char *GetOriginalSongName(byte song);

#endif // WORLD_IO_LEGACY_H
