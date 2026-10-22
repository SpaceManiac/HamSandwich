#ifndef PAUSE_H
#define PAUSE_H

#include "display.h"

enum PauseMenuResult : byte
{
	PAUSE_CONTINUE,
	PAUSE_WORLDSEL,
	PAUSE_GIVEUP,
	PAUSE_RETRY,
	PAUSE_PAUSED,
	PAUSE_EXIT,
	PAUSE_SHOP,
};

struct player_t;

void UpdateUnpaused(void);
void RenderUnpaused(void);

void RenderPauseMenu(void);
PauseMenuResult UpdatePauseMenu(MGLDraw *mgl);
void InitPauseMenu(void);
void ExitPauseMenu(void);

#endif
