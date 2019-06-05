#ifndef TITLE_H
#define TITLE_H

#include "winpch.h"
#include "display.h"

struct title_t;

#define VERSION_NO "Version 3.7"

byte MainMenu(MGLDraw *mgl);
void Credits(MGLDraw *mgl,byte mode);
void SplashScreen(MGLDraw *dispmgl,const char *fname,int delay,byte sound,byte specialdeal);

byte GameSlotPicker(MGLDraw *mgl,title_t *title);
void VictoryText(MGLDraw *mgl,byte victoryType);

#endif
