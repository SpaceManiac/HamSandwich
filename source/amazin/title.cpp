#include "title.h"
#include "display.h"
#include "jamultypes.h"
#include "loop.h"

byte g_SplashMode;
byte g_SplashWhich;
byte g_SplashTimer;
RGB g_SplashPalette[256];
RGB g_HamumuBmpPalette[256];

void TitleInit()
{
	// TODO
	g_MGL->Quit();
}

bool SplashUpdate();
void SplashRender();

void SplashInit(byte which)
{
	int i;

	g_SplashMode = 0xff;
	g_SplashWhich = which;
	for (i = 0; i < 0x100; i = i + 1)
	{
		g_SplashPalette[i].r = 0;
		g_SplashPalette[i].g = 0;
		g_SplashPalette[i].b = 0;
	}
	g_MGL->SetPalette(g_SplashPalette);
	SetUpdate(SplashUpdate);
	SetRender(SplashRender);
	g_MGL->LastKeyPressed();
}

void SplashExit()
{
}

bool SplashUpdate()
{
	char keych;
	int i;
	int j;
	int done_colors;

	keych = g_MGL->LastKeyPressed();
	if (keych != '\0')
	{
		g_SplashMode = 2;
	}
	switch (g_SplashMode)
	{
	case 0:
		for (i = 0; i < 8; i = i + 1)
		{
			done_colors = 0;
			for (j = 0; j < 0x100; j = j + 1)
			{
				if (g_SplashPalette[j].r < g_HamumuBmpPalette[j].r)
				{
					g_SplashPalette[j].r = g_SplashPalette[j].r + 1;
				}
				else
				{
					done_colors = done_colors + 1;
				}
				if (g_SplashPalette[j].g < g_HamumuBmpPalette[j].g)
				{
					g_SplashPalette[j].g = g_SplashPalette[j].g + 1;
				}
				else
				{
					done_colors = done_colors + 1;
				}
				if (g_SplashPalette[j].b < g_HamumuBmpPalette[j].b)
				{
					g_SplashPalette[j].b = g_SplashPalette[j].b + 1;
				}
				else
				{
					done_colors = done_colors + 1;
				}
			}
			if (done_colors == 0x300)
			{
				i = 8;
				g_SplashMode = 1;
				g_SplashTimer = 0;
			}
		}
		g_MGL->SetSecondaryPalette(g_SplashPalette);
		break;
	case 1:
		g_SplashTimer = g_SplashTimer + 1;
		if (g_SplashTimer == (uint)g_SplashWhich * 0xf0 + 0x3c)
		{
			g_SplashMode = 2;
		}
		break;
	case 2:
		for (i = 0; i < 8; i = i + 1)
		{
			done_colors = 0;
			for (j = 0; j < 0x100; j = j + 1)
			{
				if (g_SplashPalette[j].r == 0)
				{
					done_colors = done_colors + 1;
				}
				else
				{
					g_SplashPalette[j].r = g_SplashPalette[j].r - 1;
				}
				if (g_SplashPalette[j].g == 0)
				{
					done_colors = done_colors + 1;
				}
				else
				{
					g_SplashPalette[j].g = g_SplashPalette[j].g - 1;
				}
				if (g_SplashPalette[j].b == 0)
				{
					done_colors = done_colors + 1;
				}
				else
				{
					g_SplashPalette[j].b = g_SplashPalette[j].b - 1;
				}
			}
			if (done_colors == 0x300)
			{
				g_MGL->SetSecondaryPalette(g_SplashPalette);
				g_SplashMode = 3;
				return true;
			}
		}
		g_MGL->SetSecondaryPalette(g_SplashPalette);
		break;
	case 4:
		SplashExit();
		if (g_SplashWhich != 0)
		{
			return false;
		}
		TitleInit();
	}
	return true;
}

void SplashRender(void)
{
	if (g_SplashMode == 0xff)
	{
		if ((g_SplashWhich == 0) &&
		    !g_MGL->LoadBMP("graphics/hamumu.bmp", g_HamumuBmpPalette))
		{
			TitleInit();
			return;
		}
		g_SplashMode = 0;
	}
	if (g_SplashMode == 3)
	{
		g_MGL->RealizePalette();
		g_SplashMode = 4;
	}
	return;
}
