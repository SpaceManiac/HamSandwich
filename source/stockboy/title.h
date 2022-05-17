#ifndef TITLE_H
#define TITLE_H

#include "winpch.h"
#include "display.h"

#define MENU_TRAINING	 0
#define MENU_STOCKROOM	 1
#define MENU_PARALLEL	 2
#define MENU_BLOWOUT	 3
#define MENU_CLEARANCE	 4
#define MENU_PESTCONTROL 5
#define MENU_EDITOR		 6
#define MENU_ADDON		 7
#define MENU_PROFILE	 8
#define MENU_GIFTSHOP	 9
#define MENU_COMPOSER	 10
#define MENU_EXIT		 11

#define MENU_CHOICES	 12

byte MainMenu(byte pageUp,MGLDraw *mgl);
void Credits(MGLDraw *mgl,byte init);
void SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound);
void VictoryText(MGLDraw *mgl);
void HelpScreens(MGLDraw *mgl);
void DemoSplashScreens(MGLDraw *mgl);

#endif
