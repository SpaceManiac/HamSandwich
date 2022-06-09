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

#define MENU_EDITOR	   254	// special since you can't normally do it

extern byte gameToLoad;

TASK(byte) MainMenu(MGLDraw *mgl);
char *GetSavedName(void);
byte GetSavedMod(byte n);
void DeleteCharacter(void);
char *FetchAddOnName(void);

#endif
