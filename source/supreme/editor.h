#ifndef EDITOR_H
#define EDITOR_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "filedialog.h"
#include "mapdialog.h"

// editor modes
#define EDITMODE_EDIT		0
#define EDITMODE_PICKFLR	1
#define EDITMODE_PICKWALL	2
#define EDITMODE_PICKWALLFLR 3
#define EDITMODE_PICKNEXT	4
#define EDITMODE_PICKENEMY  5
#define EDITMODE_TERRAIN	6
#define EDITMODE_FILE		7
#define EDITMODE_MAPMENU	8
#define EDITMODE_SPECIAL	9
#define EDITMODE_TILELOAD	10
#define EDITMODE_ITEM		11
#define EDITMODE_SOUND		12
#define EDITMODE_HELP		13
#define EDITMODE_PICKSPOT	14
#define EDITMODE_PICKRSPOT	15
#define EDITMODE_PICKRSPOT2	16
#define EDITMODE_PICKSPOT2	17
#define EDITMODE_EXIT		18
#define EDITMODE_EXITYES	19
#define EDITMODE_LEVELMENU	20

extern byte editing;

byte LunaticEditor(MGLDraw *mgl);

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
byte GetDisplayFlags(void);
void ToggleDisplayFlag(byte f);
void ViewMenuOn(void);
void ViewMenuOff(void);

#endif
