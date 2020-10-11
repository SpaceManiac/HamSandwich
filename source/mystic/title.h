#ifndef TITLE_H
#define TITLE_H

#include "winpch.h"
#include "display.h"

struct title_t;

#define VERSION_NO "Version 3.7"

TASK(byte) MainMenu(MGLDraw *mgl);
TASK(void) Credits(MGLDraw *mgl,byte mode);
TASK(void) SplashScreen(MGLDraw *dispmgl,const char *fname,int delay,byte sound,byte specialdeal);

TASK(byte) GameSlotPicker(MGLDraw *mgl,title_t *title);
TASK(void) VictoryText(MGLDraw *mgl,byte victoryType);

#endif
