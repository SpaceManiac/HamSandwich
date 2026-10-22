#ifndef PAUSE_H
#define PAUSE_H

#include "winpch.h"
#include "display.h"

enum class PauseMenuResult : byte
{
	Paused,
	Continue,
	Quit,
	GiveUp,  // not used
	WarpToLooniton,
};

void RenderPauseMenu(MGLDraw *mgl);
void RenderSlotPickMenu(void);
PauseMenuResult UpdatePauseMenu(MGLDraw *mgl);
void InitPauseMenu(void);
void BumpSaveGem(void);
void LoadGame(int i);
void DeleteSave(int i);
void SetNoSaving(bool on);

#endif
