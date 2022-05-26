#ifndef TITLE_H
#define TITLE_H

#include "jamultypes.h"
#include "coro.h"

// returns:
// 0=Play Game
// 1=Options
// 2=Exit
TASK(byte) TitleMenu(void);

// returns whether the player chose to continue or not
TASK(byte) Continue(void);

TASK(void) Victory(void);

#endif
