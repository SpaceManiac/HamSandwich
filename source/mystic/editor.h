#ifndef EDITOR_H
#define EDITOR_H

#include "winpch.h"
#include "world.h"
#include "map.h"
#include "game.h"
#include "filedialog.h"
#include "mapdialog.h"
#include "spcldialog.h"
#include "tiledialog.h"

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
#define MAXPLOP		9

typedef struct editopt_t
{
	byte displayFlags;
	byte plopMode;
	byte brushSize;
	byte curWall;
	byte curWallFloor;
	byte curFloor;
	byte curItem;
	byte curBadguy;
} editopt_t;

byte LunaticEditor(MGLDraw *mgl);

void EditorNewWorld(void);
void EditorLoadWorld(char *fname);
void EditorSaveWorld(char *fname);
void EditorSelectMap(byte w);
void EditorLoadTiles(char *fname);

#endif
