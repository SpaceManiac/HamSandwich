#ifndef TITLE_H
#define TITLE_H

#include "display.h"

#define VERSION_NO	"Version 9.0"

struct title_t;

TASK(byte) MainMenu(MGLDraw *mgl);
TASK(void) Credits(MGLDraw *mgl);
TASK(void) SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound);
TASK(void) VictoryText(MGLDraw *mgl);
TASK(void) HelpScreens(MGLDraw *mgl);
TASK(void) DemoSplashScreens(MGLDraw *mgl);

#endif
