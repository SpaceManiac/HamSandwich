#ifndef PAUSE_H
#define PAUSE_H

#include "display.h"

#define PAUSE_CONTINUE	0
#define PAUSE_WORLDSEL	1
#define PAUSE_GIVEUP	2
#define PAUSE_RETRY		3
#define PAUSE_PAUSED	4
#define PAUSE_EXIT		5
#define PAUSE_SHOP		6

struct player_t;

void UpdateUnpaused(void);
void RenderUnpaused(void);

void RenderPauseMenu(void);
byte UpdatePauseMenu(MGLDraw *mgl);
void InitPauseMenu(void);
void ExitPauseMenu(void);

void ChangeWeapon(char dir);

#endif
