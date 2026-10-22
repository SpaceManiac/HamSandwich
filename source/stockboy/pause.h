#ifndef PAUSE_H
#define PAUSE_H

#include "winpch.h"
#include "display.h"

// exitcodes for the pause menu update function
#define PAUSE_RETURN	0
#define PAUSE_EXITGAME	1
#define PAUSE_RETRY		2
#define PAUSE_PAUSING	3	// just keep on pausing

struct player_t;

void RenderPauseMenu(MGLDraw *mgl);
byte UpdatePauseMenu(MGLDraw *mgl);
void InitPauseMenu(void);
void ExitPauseMenu(void);

#endif