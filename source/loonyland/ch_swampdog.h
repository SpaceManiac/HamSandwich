#ifndef CH_SWAMPDOG_H
#define CH_SWAMPDOG_H

#include "player.h"

#define SD_LEVELUP	20

void SwampdogControlMe(Guy *me,mapTile_t *mapTile,world_t *world);
void FakeLevelUp(void);

#endif
