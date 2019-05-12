#ifndef CH_GIRL_H
#define CH_GIRL_H

#include "player.h"

// her different bombs
#define BMB_NONE	0
#define BMB_KABOOM	1
#define BMB_SHOCK	2
#define BMB_ICE		3
#define BMB_HEALING 4
#define BMB_BUZZSAW	5
#define BMB_PLAGUE	6
#define BMB_SPEED	7

void GirlControlMe(Guy *me,mapTile_t *mapTile,world_t *world);

char *BombName(byte spl);
byte BombCost(byte spl);

void RenderKnifeUps(MGLDraw *mgl);

#endif
