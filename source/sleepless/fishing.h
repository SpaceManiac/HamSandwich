#ifndef FISHING_H
#define FISHING_H

#include "mgldraw.h"
#include "map.h"
#include "world.h"

char *FishName(byte n);
byte ShouldBob(void);
void CatchFish(int mapx,int mapy,Map *map,world_t *world);

#endif
