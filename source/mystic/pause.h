#ifndef PAUSE_H
#define PAUSE_H

#include "winpch.h"
#include "display.h"

struct player_t;

enum class PauseResult : byte
{
	Resume = 0,
	Pause = 1,
	GiveUp = 2,
	Quit = 3,
};

void RenderPauseMenu(void);
void RenderSlotPickMenu(void);
void RenderSkillMenu(void);
void RenderWeirdMenu(void);
void RenderRuneMenu(void);
PauseResult UpdatePauseMenu(MGLDraw *mgl);
void UpdateUnPausedMenu();
void InitPauseMenu(void);
void SetGiveUpText(byte gu);
void SetSubCursor(byte s);
void ResetPauseMenu(void);
void RenderSkillBox(int x, int y, int x2, int y2, byte outC, byte inC);

#endif
