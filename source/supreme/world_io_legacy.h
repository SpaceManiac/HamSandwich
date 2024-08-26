#ifndef WORLD_IO_LEGACY_H
#define WORLD_IO_LEGACY_H

#include "mgldraw.h"

struct world_t;

byte Legacy_LoadWorld(world_t *world, const char *fname);
bool Legacy_GetWorldName(const char *fname, char *buf);
const char *GetOriginalSongName(byte song);

#endif // WORLD_IO_LEGACY_H
