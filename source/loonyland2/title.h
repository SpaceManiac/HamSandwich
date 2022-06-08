#ifndef TITLE_H
#define TITLE_H

#include "winpch.h"
#include "display.h"

#define MAX_CHARS	(32)

#ifdef DIRECTORS
#define TITLEBMP	"graphics/title_coll.bmp"
#else
#define TITLEBMP	"graphics/title.bmp"
#endif
#define MENU_NEWCHAR	1	// make a new character
#define MENU_PLAY		2	// play with the selected char
#define MENU_OPTIONS	3	// go to options menu
#define MENU_EXIT		4	// quit the game

#ifdef DIRECTORS
#define MM_PLAY			0
#define MM_ERASE		1
#define MM_ACHIEVE		2
#define MM_GALLERY		3
#define MM_EDITOR		4
#define MM_OPTIONS		5
#define MM_QUIT			6
#define MENU_CHOICES	7
#else
#define MM_PLAY			0
#define MM_ERASE		1
#define MM_ACHIEVE		2
#define MM_OPTIONS		3
#define MM_QUIT			4
#define MENU_CHOICES	5
#endif

#define MENU_EDITOR	   254	// special since you can't normally do it

#define MENU_MODE_NORMAL	0
#define MENU_MODE_ERASE		1
#define MENU_MODE_NEW		2

#define SC_NONE			0

#define SC_YES			0
#define SC_NO			1

#define SC_PLAYER_NAME	0
#define SC_MODIFIER_1	1
#define SC_MODIFIER_2	2
#define SC_MODIFIER_3	3
#define SC_START_GAME	4

extern byte gameToLoad;

TASK(byte) MainMenu(MGLDraw *mgl);
char *GetSavedName(void);
byte GetSavedMod(byte n);
void DeleteCharacter(void);
char *FetchAddOnName(void);

#endif
