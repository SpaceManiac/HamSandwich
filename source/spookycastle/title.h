#ifndef TITLE_H
#define TITLE_H

#include "winpch.h"
#include "display.h"

struct title_t;

TASK(byte) MainMenu(MGLDraw *mgl);
TASK(void) Credits(MGLDraw *mgl);
TASK(void) SplashScreen(MGLDraw *dispmgl,const char *fname,int delay,byte sound,byte specialdeal);
TASK(void) VictoryText(MGLDraw *mgl);

TASK(byte) GameSlotPicker(MGLDraw *mgl,title_t *title);

#endif
