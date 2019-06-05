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
#pragma pack(1)

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

byte LunaticEditor(MGLDraw *mgl);

void EditorNewWorld(void);
void EditorLoadWorld(const char *fname);
void EditorSaveWorld(const char *fname);
void EditorSelectMap(byte w);
void EditorLoadTiles(const char *fname,byte wall);

word SnowCheck(word v);

#endif
