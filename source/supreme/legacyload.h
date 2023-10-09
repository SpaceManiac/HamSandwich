#ifndef LEGACYLOAD_H
#define LEGACYLOAD_H

#include "mgldraw.h"

struct world_t;
struct old_map_t;

byte Legacy_LoadWorld(world_t *world,const char *fname);
bool Legacy_GetWorldName(const char *fname,char *buf);
const char *GetOriginalSongName(byte song);

#endif
