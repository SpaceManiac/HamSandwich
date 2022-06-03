#ifndef ARCADE_H
#define ARCADE_H

#include "mgldraw.h"
#include "game.h"

TASK(void) CaveGame(MGLDraw *mgl);
TASK(void) Arcade(MGLDraw *mgl, byte gm);

#endif
