#ifndef TITLE_H
#define TITLE_H

#include "winpch.h"
#include "display.h"

#define MENU_ADVENTURE	0
#define MENU_LOADGAME	1
#define MENU_BOWLING	2
#define MENU_SURVIVAL	3
#define MENU_BOSSATTACK 4
#define MENU_LOONYBALL	5
#define MENU_REMIX		6
#define MENU_BADGES		7
#define MENU_HISCORE	8
#define MENU_OPTIONS	9
#define MENU_EDITOR		10
#define MENU_EXIT		11
#define MENU_NOTHING2	12

#define MENU_CHOICES	13

//#define MENU_EDITOR	   254	// special since you can't normally do it

byte LunaticTitle(MGLDraw *mgl);
byte MainMenu(MGLDraw *mgl);
void Credits(MGLDraw *mgl,byte init);
void CheatText(MGLDraw *mgl,byte init);
void SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound);
void VictoryText(MGLDraw *mgl);
void HelpScreens(MGLDraw *mgl);
void DemoSplashScreens(MGLDraw *mgl);

byte WhichGameToLoad(void);
void SetSongRestart(byte b);

#endif
