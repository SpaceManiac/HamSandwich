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
enum {
	PLOP_FLOOR = 0,
	PLOP_WALL,
	PLOP_ITEM,
	PLOP_BADGUY,
	PLOP_LIGHT,
	PLOP_DARK,
	PLOP_SMOOTH,
	PLOP_TORCH,
	PLOP_SPECIAL,
	MAXPLOP
};

struct editopt_t
{
	byte displayFlags;
	byte plopMode;
	byte brushSize;
	byte curWall;
	byte curWallFloor;
	byte curFloor;
	byte curItem;
	byte curBadguy;
};

extern byte editing;

TASK(byte) LunaticEditor(MGLDraw *mgl);

void EditorNewWorld(void);
void EditorLoadWorld(const char *fname);
void EditorSaveWorld(const char *fname);
void EditorSelectMap(byte w);
void EditorLoadTiles(const char *fname);

#endif
