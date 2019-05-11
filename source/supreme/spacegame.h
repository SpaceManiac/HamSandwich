#ifndef SPACEGAME_H
#define SPACEGAME_H

#include "mgldraw.h"
#include "game.h"

void SpaceGame(MGLDraw *mgl);

// prototypes for internal whatnots
void PutOnEdge(int i);
void SpaceGoodguyHit(void);
void SpaceAddBadguys(void);
void LaunchBadguy(int x,int y,int dx);

#endif
