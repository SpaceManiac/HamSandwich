#ifndef PAUSE_H
#define PAUSE_H

#include "winpch.h"
#include "display.h"

struct player_t;

void RenderPauseMenu(void);
void RenderSlotPickMenu(void);
byte UpdatePauseMenu(MGLDraw *mgl);
void InitPauseMenu(void);
float CalcTotalPercent(player_t *p);
void SetGiveUpText(byte gu);
void SetSubCursor(byte s);

#endif
