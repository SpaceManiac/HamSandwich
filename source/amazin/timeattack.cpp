#include "timeattack.h"
#include "options.h"
#include "control.h"
#include "display.h"
#include "hammusic.h"
#include "jamultypes.h"
#include "loop.h"
#include "register.h"
#include "title.h"
#include "game.h"
#include <stdint.h>

byte g_TimeAttackBright;
int8_t g_TimeAttackVelocity;
byte g_TimeAttackOldHeld;
byte g_TimeAttackCursor;
byte g_TimeAttackTheme;

bool TimeAttackUpdate();
void TimeAttackRender();

void TimeAttackInit()
{
	SetUpdate(TimeAttackUpdate);
	SetRender(TimeAttackRender);
	g_TimeAttackOldHeld = 0xff;
	g_TimeAttackBright = 0x1f;
	g_TimeAttackVelocity = -1;
	if (g_TimeAttackCursor == 0x15)
	{
		g_TimeAttackCursor = 0;
	}
	if (!mgl->LoadBMP("graphics/backgd.bmp"))
	{
		FatalError("Missing file: graphics/backgd.bmp");
		exit(1);
	}
	memcpy(g_Background, mgl->GetScreen(), 0x4b000);
	StopSong();
}

void TimeAttackExit()
{
}

bool TimeAttackUpdate()
{
	char k;
	byte mapNum;
	bool bVar1;

	g_TimeAttackBright = g_TimeAttackBright + g_TimeAttackVelocity;
	if (g_TimeAttackBright < 10)
	{
		g_TimeAttackVelocity = 1;
	}
	else if (g_TimeAttackBright == 31)
	{
		g_TimeAttackVelocity = -1;
	}
	k = mgl->LastKeyPressed();
	byte held = GetPlayerControls(0) | GetPlayerControls(1) | GetArrows();
	if (k == '\x1b')
	{
		TimeAttackExit();
		TitleInit();
		held = g_TimeAttackOldHeld;
	}
	else
	{
		if (((k == '\r') || (k == ' ')) ||
		    (((held & CONTROL_B1) != 0 && ((g_TimeAttackOldHeld & CONTROL_B1) == 0))))
		{
			if (g_TimeAttackCursor != 0)
			{
				if (g_TimeAttackCursor == 0x15)
				{
					TimeAttackExit();
					TitleInit();
					return true;
				}
				if ((g_TimeAttackCursor & 1) == 0)
				{
					mapNum = (g_TimeAttackCursor >> 1) + 10;
				}
				else
				{
					mapNum = (g_TimeAttackCursor >> 1) + 1;
				}
				if (g_TimeAttackTheme == 0)
				{
					GameInit(GameMode::HappyFields, true, mapNum, 1);
					return true;
				}
				GameInit(GameMode::DankDungeons, true, mapNum, 1);
				return true;
			}
			g_TimeAttackTheme = 1 - g_TimeAttackTheme;
		}
		if (RegistrationCurrentlyValid())
		{
			if (((held & CONTROL_RT) != 0) && ((g_TimeAttackOldHeld & CONTROL_RT) == 0))
			{
				if (g_TimeAttackCursor == 0)
				{
					g_TimeAttackCursor = 2;
				}
				else if (g_TimeAttackCursor == 0x15)
				{
					g_TimeAttackCursor = 0x14;
				}
				else if ((g_TimeAttackCursor & 1) != 0)
				{
					g_TimeAttackCursor = g_TimeAttackCursor + 1;
				}
			}
			if (((held & CONTROL_LF) != 0) && ((g_TimeAttackOldHeld & CONTROL_LF) == 0))
			{
				if (g_TimeAttackCursor == 0)
				{
					g_TimeAttackCursor = 1;
				}
				else if (g_TimeAttackCursor == 0x15)
				{
					g_TimeAttackCursor = 0x13;
				}
				else if ((g_TimeAttackCursor & 1) == 0)
				{
					g_TimeAttackCursor = g_TimeAttackCursor - 1;
				}
			}
		}
		if (((held & CONTROL_DN) != 0) && ((g_TimeAttackOldHeld & CONTROL_DN) == 0))
		{
			if (g_TimeAttackCursor == 0)
			{
				g_TimeAttackCursor = 1;
			}
			else if (g_TimeAttackCursor == 0x15)
			{
				g_TimeAttackCursor = 0;
			}
			else if ((g_TimeAttackCursor < 0x15) && (g_TimeAttackCursor = g_TimeAttackCursor + 2, g_TimeAttackCursor == 0x16))
			{
				g_TimeAttackCursor = 0x15;
			}
			if ((!RegistrationCurrentlyValid() && (9 < g_TimeAttackCursor)) && (g_TimeAttackCursor < 0x15))
			{
				g_TimeAttackCursor = 0x15;
			}
		}
		if (((held & CONTROL_UP) != 0) && ((g_TimeAttackOldHeld & CONTROL_UP) == 0))
		{
			if (g_TimeAttackCursor == 0x15)
			{
				g_TimeAttackCursor = 0x14;
			}
			else if (g_TimeAttackCursor == 0)
			{
				g_TimeAttackCursor = 0x15;
			}
			else
			{
				g_TimeAttackCursor = g_TimeAttackCursor - 2;
				if (0x15 < g_TimeAttackCursor)
				{
					g_TimeAttackCursor = 0;
				}
			}
			if ((!RegistrationCurrentlyValid() && (9 < g_TimeAttackCursor)) && (g_TimeAttackCursor < 0x15))
			{
				g_TimeAttackCursor = 9;
			}
		}
	}
	g_TimeAttackOldHeld = held;
	return true;
}

void TimeAttackRender()
{
	int iVar2;
	int i;
	char buf[64];

	memcpy(mgl->GetScreen(), g_Background, 0x4b000);
	CenterPrint(0x140, 2, "Time Attack");
	if (g_TimeAttackTheme == 0)
	{
		CenterPrintMultiline(0x140, 0x2a, "Happy Fields Levels");
	}
	else
	{
		CenterPrintMultiline(0x140, 0x2a, "Dank Dungeons Levels");
	}
	CenterPrintMultiline(0x140, 0x174, "Exit");
	if (RegistrationCurrentlyValid())
	{
		for (i = 0; i < 10; i = i + 1)
		{
			sprintf(buf, "L%02d:", i + 1);
			PrintMultiline(0x16, i * 0x1e + 0x48, buf);
			sprintf(buf, "%s", g_Config.times[g_TimeAttackTheme][i].name);
			PrintMultiline(0x48, i * 0x1e + 0x48, buf);
			sprintf(buf, "%02d:%02d", g_Config.times[g_TimeAttackTheme][i].minutes, g_Config.times[g_TimeAttackTheme][i].seconds);
			PrintMultiline(0xf2, i * 0x1e + 0x48, buf);
			sprintf(buf, "L%02d:", i + 11);
			PrintMultiline(0x14a, i * 0x1e + 0x48, buf);
			sprintf(buf, "%s", g_Config.times[g_TimeAttackTheme][i + 10].name);
			PrintMultiline(0x17c, i * 0x1e + 0x48, buf);
			sprintf(buf, "%02d:%02d", g_Config.times[g_TimeAttackTheme][i + 10].minutes, g_Config.times[g_TimeAttackTheme][i + 10].seconds);
			PrintMultiline(0x226, i * 0x1e + 0x48, buf);
		}
	}
	else
	{
		for (i = 0; i < 5; i = i + 1)
		{
			sprintf(buf, "L%02d:", i + 1);
			PrintMultiline(0x16, i * 0x1e + 0x48, buf);
			sprintf(buf, "%s", g_Config.times[g_TimeAttackTheme][i].name);
			PrintMultiline(0x48, i * 0x1e + 0x48, buf);
			sprintf(buf, "%02d:%02d", g_Config.times[g_TimeAttackTheme][i].minutes, g_Config.times[g_TimeAttackTheme][i].seconds);
			PrintMultiline(0xf2, i * 0x1e + 0x48, buf);
		}
	}
	if (g_TimeAttackCursor == 0)
	{
		mgl->Box(0x8c, 0x2a, 500, 0x45, g_TimeAttackBright);
		PrintMultiline(0x1e, 0x1c2, "Press Enter to switch level type");
	}
	else if (g_TimeAttackCursor == 0x15)
	{
		mgl->Box(0x118, 0x174, 0x168, 399, g_TimeAttackBright);
		PrintMultiline(0x1e, 0x1c2, "Press Enter to exit");
	}
	else
	{
		iVar2 = (int)(g_TimeAttackCursor - 1) / 2;
		if ((g_TimeAttackCursor & 1) == 0)
		{
			mgl->Box(0x148, iVar2 * 0x1e + 0x48, 0x26a, iVar2 * 0x1e + 99, g_TimeAttackBright);
		}
		else
		{
			mgl->Box(0x14, iVar2 * 0x1e + 0x48, 0x136, iVar2 * 0x1e + 99, g_TimeAttackBright);
		}
		PrintMultiline(0x1e, 0x1c2, "Press Enter to play this level");
	}
}
