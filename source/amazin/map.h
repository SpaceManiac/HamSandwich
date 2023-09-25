#ifndef MAP_H
#define MAP_H

#include "jamultypes.h"

class sprite_set_t;

extern sprite_set_t* g_ItemsJsp;

void MapInit();
void MapExit();

void TileRender(byte tile, int x, int y);

#endif  // MAP_H
