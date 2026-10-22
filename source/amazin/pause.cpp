#include "pause.h"
#include "jamultypes.h"
#include "control.h"
#include "display.h"

byte g_PauseSelection;
byte g_PauseOldHeld;
bool g_IsPaused;

void PauseInit()
{
	g_IsPaused = true;
	g_PauseSelection = 0;
	g_PauseOldHeld = 0xff;
	mgl->LastKeyPressed();
}

bool IsPaused()
{
	return g_IsPaused;
}

bool PauseUpdate()
{
	char k = mgl->LastKeyPressed();
	byte held = GetPlayerControls(0) | GetPlayerControls(1) | GetArrows();
	if (((held & CONTROL_DN) != 0) && ((g_PauseOldHeld & CONTROL_DN) == 0))
	{
		g_PauseSelection = 1 - g_PauseSelection;
	}
	if (((held & CONTROL_UP) != 0) && ((g_PauseOldHeld & CONTROL_UP) == 0))
	{
		g_PauseSelection = 1 - g_PauseSelection;
	}
	if (k == '\x1b')
	{
		g_IsPaused = false;
		return true;
	}
	else if (((k == '\r') || (k == ' ')) || (((held & CONTROL_B1) != 0 && ((g_PauseOldHeld & CONTROL_B1) == 0))))
	{
		if (g_PauseSelection == 0)
		{
			g_IsPaused = false;
			return true;
		}
		else
		{
			g_IsPaused = false;
			return false;
		}
	}
	else
	{
		g_PauseOldHeld = held;
		return true;
	}
}

void PauseRenderBackground(int x1, int y1, int x2, int y2)
{
	mgl->FillBox(x1 + 1, y1, x2 + -1, y1, 0xd0);
	mgl->FillBox(x1 + 1, y2, x2 + -1, y2, 0xd0);
	mgl->FillBox(x1, y1 + 1, x1, y2 + -1, 0xd0);
	mgl->FillBox(x2, y1 + 1, x2, y2 + -1, 0xd0);
	mgl->Box(x1 + 1, y1 + 1, x2 + -1, y2 + -1, 0xca);
	mgl->FillBox(x1 + 2, y1 + 2, x2 + -2, y2 + -2, 0xc6);
}

void PauseRender(void)
{
	PauseRenderBackground(0xff, 0xcd, 0x181, 0x10e);
	CenterPrint(0x140, 0xd2, "Continue");
	CenterPrint(0x140, 0xf0, "Exit");
	mgl->Box(0x104, (unsigned int)g_PauseSelection * 0x1e + 0xd2, 0x17c, (unsigned int)g_PauseSelection * 0x1e + 0xeb, 0x1f);
}
