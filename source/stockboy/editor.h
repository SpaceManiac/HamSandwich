#ifndef EDITOR_H
#define EDITOR_H

#include "winpch.h"
#include "world.h"
#include "map.h"
#include "game.h"

// the different plop modes
#define PLOP_FLOOR  0
#define PLOP_WALL	1
#define PLOP_ITEM	2
#define PLOP_BADGUY 3
#define PLOP_LIGHT	4
#define PLOP_DARK	5
#define PLOP_SMOOTH 6
#define PLOP_TORCH	7
#define PLOP_SPECIAL 8
#define PLOP_RANDOM 9
#define MAXPLOP		10

// editor modes
// not doing anything
#define EDITMODE_EDIT		0
// moving the tool menu around
#define EDITMODE_DRAGTOOL	1
// moving the main menu around
#define EDITMODE_DRAGMENU	2
// plopping stuff down (so don't even check for clicks on menus
#define EDITMODE_DRAGPLOP	3
// asking a yes/no question
#define EDITMODE_YESNO		4
// not really a mode... exit the editor now!
#define EDITMODE_EXIT		5

typedef struct editopt_t
{
	byte displayFlags;
	byte plopMode;
	byte brushSize;
	int curWall;
	int curWallFloor;
	int curFloor;
	int curRandom;
	byte curItem;
	byte curBadguy;
	byte curTag;
	int  copyX,copyY,copyWidth,copyHeight;
} editopt_t;

extern byte editing;
extern sprite_set_t *editspr;
extern byte editMode;
extern byte testingLevel;

TASK(byte) LunaticEditor(MGLDraw *mgl);

void EditorNewWorld(void);
void EditorLoadWorld(char *fname);
void EditorSaveWorld(char *fname);
void EditorSelectMap(byte w);
void EditorLoadTiles(char *fname,byte wall);
void EditorMergeWorld(char *fname);
world_t *EditorGetWorld(void);
Map *EditorGetMap(void);
byte EditorGetMapNum(void);
void HideMenus(void);
byte MenusHidden(void);

#endif
