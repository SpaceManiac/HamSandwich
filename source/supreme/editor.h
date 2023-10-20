#ifndef EDITOR_H
#define EDITOR_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "filedialog.h"
#include "mapdialog.h"

// editor modes
enum : byte
{
	EDITMODE_EDIT,
	EDITMODE_PICKFLR,
	EDITMODE_PICKWALL,
	EDITMODE_PICKWALLFLR,
	EDITMODE_PICKNEXT,
	EDITMODE_PICKENEMY,
	EDITMODE_TERRAIN,
	EDITMODE_FILE,
	EDITMODE_MAPMENU,
	EDITMODE_SPECIAL,
	EDITMODE_TILELOAD,
	EDITMODE_ITEM,
	EDITMODE_SOUND,
	EDITMODE_HELP,
	EDITMODE_PICKSPOT,
	EDITMODE_PICKRSPOT,
	EDITMODE_PICKRSPOT2,
	EDITMODE_PICKSPOT2,
	EDITMODE_EXIT,
	EDITMODE_EXITYES,
	EDITMODE_LEVELMENU,
	EDITMODE_EXPORT,
	EDITMODE_PICKBULLET,
};

extern byte editing;

TASK(byte) LunaticEditor(MGLDraw *mgl);

void EditorSaveWorld(const char *fname);
void EditorSelectMap(byte w);
world_t *EditorGetWorld(void);
int EditorGetLastPick(void);

void SetEditMode(byte m);
void PickedTile(int t);

void EditorGetTileXY(int *x,int *y);
void SetEditPickerRect(int wid,int hei);
Map *EditorGetMap(void);
byte EditorShowMonsItems(void);
byte EditorGetSelectMode(void);
void EditorSetSelectMode(byte mode);
byte EditorGetMapNum(void);
word GetDisplayFlags(void);
void ToggleDisplayFlag(word f);
void ViewMenuOn(void);
void ViewMenuOff(void);

#endif
