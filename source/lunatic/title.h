#ifndef TITLE_H
#define TITLE_H

#include "winpch.h"
#include "display.h"

struct title_t;

TASK(byte) LunaticTitle(MGLDraw *mgl);
TASK(byte) WorldPicker(MGLDraw *mgl);
TASK(byte) MainMenu(MGLDraw *mgl);
TASK(void) Credits(MGLDraw *mgl);
TASK(void) SplashScreen(MGLDraw *mgl, const char *fname, int delay, byte sound);
TASK(void) VictoryText(MGLDraw *mgl);
TASK(void) HelpScreens(MGLDraw *mgl);
TASK(void) DemoSplashScreens(MGLDraw *mgl);

void ScanWorldNames(void);
void ReScanWorldNames(void);

TASK(byte) GameSlotPicker(MGLDraw *mgl, title_t *title);

#endif
