#ifndef TITLE_H
#define TITLE_H

#include "jamultypes.h"

// returns:
// 0=Play Game
// 1=Options
// 2=Exit
byte TitleMenu(void);

// returns whether the player chose to continue or not
byte Continue(void);

void Victory(void);

#endif