#ifndef TITLE_H
#define TITLE_H

#include "display.h"

#define VERSION_NO	"Version 2.0d"

struct title_t;

byte MainMenu(MGLDraw *mgl);
void Credits(MGLDraw *mgl);
void SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound);
void VictoryText(MGLDraw *mgl);
void HelpScreens(MGLDraw *mgl);
void DemoSplashScreens(MGLDraw *mgl);

#endif
