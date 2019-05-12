#ifndef PAUSE_H
#define PAUSE_H

#include "winpch.h"
#include "display.h"

struct player_t;

void RenderPauseMenu(MGLDraw *mgl);
void RenderSlotPickMenu(void);
byte UpdatePauseMenu(MGLDraw *mgl);
void InitPauseMenu(void);
void SetSubCursor(byte s);
void BumpSaveGem(void);
void LoadGame(int i);
void DeleteSave(int i);
void SetNoSaving(byte on);

#endif
