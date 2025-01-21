#ifndef TITLE_H
#define TITLE_H

#include "display.h"

#define VERSION_NO         "Version 9.4"
#define COPYRIGHT_YEARS    "1998-2024"
#define COPYRIGHT_COMPANY  "Hamumu Games, Inc."

TASK(byte) MainMenu(MGLDraw *mgl);
TASK(void) Credits(MGLDraw *mgl);
TASK(void) SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound);
TASK(void) VictoryText(MGLDraw *mgl);
TASK(void) HelpScreens(MGLDraw *mgl);
TASK(void) DemoSplashScreens(MGLDraw *mgl);

#endif
