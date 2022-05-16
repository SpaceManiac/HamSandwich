#ifndef TITLE_H
#define TITLE_H

#include "winpch.h"
#include "display.h"

struct title_t;

byte MainMenu(MGLDraw *mgl);
void Credits(MGLDraw *mgl);
void SplashScreen(MGLDraw *dispmgl,const char *fname,int delay,byte sound,byte specialdeal);
void VictoryText(MGLDraw *mgl);

byte GameSlotPicker(MGLDraw *mgl,title_t *title);

#endif
