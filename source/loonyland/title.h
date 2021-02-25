#ifndef TITLE_H
#define TITLE_H

#include "winpch.h"
#include "display.h"

#define MENU_ADVENTURE	0
#define MENU_LOADGAME	1
#define MENU_RANDOMIZER	2
#define MENU_BADGES		3
#define MENU_HISCORE	4
#define MENU_OPTIONS	5
#define MENU_EDITOR		6
#define MENU_EXIT		7
#define MENU_NOTHING2	8

#define MENU_CHOICES	9

#define MENU_BOWLING	8
#define MENU_SURVIVAL	9
#define MENU_BOSSATTACK 10
#define MENU_LOONYBALL	11
#define MENU_REMIX		12

//#define MENU_EDITOR	   254	// special since you can't normally do it

TASK(byte) LunaticTitle(MGLDraw *mgl);
TASK(byte) MainMenu(MGLDraw *mgl);
TASK(void) Credits(MGLDraw *mgl,byte init);
TASK(void) CheatText(MGLDraw *mgl,byte init);
TASK(void) SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound);
TASK(void) VictoryText(MGLDraw *mgl);
TASK(void) HelpScreens(MGLDraw *mgl);
TASK(void) DemoSplashScreens(MGLDraw *mgl);

byte WhichGameToLoad(void);
void SetSongRestart(byte b);

#endif
