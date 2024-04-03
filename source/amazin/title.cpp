#include "title.h"
#include "control.h"
#include "display.h"
#include "editor.h"
#include "game.h"
#include "guy.h"
#include "help.h"
#include "highscore.h"
#include "jamulspr.h"
#include "jamultypes.h"
#include "loop.h"
#include "options.h"
#include "player.h"
#include "register.h"
#include "sound.h"
#include "timeattack.h"

int32_t g_TitleTimer;
sprite_set_t *g_ButtonsJsp;
int g_TitleTextBright;
byte g_SplashWhich;
int g_TitleMouseX, g_TitleMouseY;
byte g_SplashTimer;
RGB g_HamumuBmpPalette[256];
byte g_TitleOldHeld;
byte g_SplashMode;
int g_TitleSongTimer;
RGB g_SplashPalette[256];
int g_RegistrationTextX;
byte g_TitleButton;
int8_t g_TitleSelectionX;
int8_t g_TitleSelectionY;
QuitAction g_QuitAction = QuitAction::None;
bool g_TitleMouseMode;

bool TitleUpdate();
void TitleRender();

void TitleInit()
{
	g_TitleTimer = -0xf0;
	g_RegistrationTextX = 0;
	mgl->LoadBMP("graphics/backgd.bmp");
	SetUpdate(TitleUpdate);
	SetRender(TitleRender);
	g_TitleOldHeld = 0xff;
	TitleInitGuys();
	g_TitleTextBright = -150;
	StopSong();
	PlaySong(100);
	g_TitleSongTimer = 0;
	g_TitleMouseMode = false;
	mgl->GetMouse(&g_TitleMouseX, &g_TitleMouseY);
}

void TitleExit()
{
	StopSong();
}

bool TitleUpdate()
{
	TitleUpdateGuys();
	int oldMouseY = g_TitleMouseY;
	int oldMouseX = g_TitleMouseX;
	if (g_TitleTextBright < 0)
	{
		g_TitleTextBright = g_TitleTextBright + 1;
	}
	if (g_TitleTimer < 0)
	{
		g_TitleTimer = g_TitleTimer + 16;
	}
	g_RegistrationTextX = g_RegistrationTextX + -4;
	if (g_RegistrationTextX < -2248)
	{
		g_RegistrationTextX = 0;
	}
	mgl->GetMouse(&g_TitleMouseX, &g_TitleMouseY);
	if ((oldMouseX != g_TitleMouseX) || (oldMouseY != g_TitleMouseY))
	{
		if ((17 < g_TitleMouseX) &&
		    (((g_TitleMouseX < 624 && (259 < g_TitleMouseY)) && (g_TitleMouseY < 412))))
		{
			g_TitleSelectionX = (int8_t)((g_TitleMouseX + -18) / 202);
			g_TitleSelectionY = (int8_t)((g_TitleMouseY + -260) / 38);
			g_TitleButton = g_TitleSelectionX + g_TitleSelectionY * 3;
		}
		g_TitleMouseMode = true;
	}
	char k = mgl->LastKeyPressed();
	if (k == '\x1b')
	{
		TitleExit();
		if (RegistrationCurrentlyValid())
		{
			return false;
		}
		HelpInit(24);
		return true;
	}
#ifndef NDEBUG
	if (k == 'e')
	{
		TitleExit();
		EditorInit();
		return true;
	}
#endif
	g_TitleSongTimer = g_TitleSongTimer + 1;
	if (g_TitleSongTimer == 210)
	{
		StopSong();
	}
	byte held = GetPlayerControls(0) | GetPlayerControls(1) | GetArrows();
	if (mgl->MouseTap() && 17 < g_TitleMouseX && g_TitleMouseX < 624 && 259 < g_TitleMouseY && g_TitleMouseY < 412)
	{
		k = '\r';
	}
	if (((k == '\r') || (k == ' ')) ||
	    (((held & CONTROL_B1) != 0 && ((g_TitleOldHeld & CONTROL_B1) == 0))))
	{
		switch (g_TitleButton)
		{
		case 0:
			TitleExit();
			GameInit(GameMode::HappyFields, false, 1, 1);
			break;
		case 1:
			TitleExit();
			GameInit(GameMode::DankDungeons, false, 1, 1);
			break;
		case 2:
			TitleExit();
			TimeAttackInit();
			break;
		case 3:
			TitleExit();
			GameInit(GameMode::HappyFields, false, 1, 2);
			break;
		case 4:
			TitleExit();
			GameInit(GameMode::DankDungeons, false, 1, 2);
			break;
		case 5:
			TitleExit();
			GameInit(GameMode::SiblingRivalry, false, 1, 2);
			break;
		case 6:
			TitleExit();
			OptionsInit();
			break;
		case 7:
			TitleExit();
			HelpInit(0);
			break;
		case 8:
			g_QuitAction = QuitAction::HamumuWebsite;
			TitleExit();
			return false;
		case 9:
			TitleExit();
			HelpInit(22);
			break;
		case 10:
			TitleExit();
			HelpInit(20);
			break;
		case 11:
			TitleExit();
			if (RegistrationCurrentlyValid())
			{
				return false;
			}
			HelpInit(24);
			return true;
		}
	}
	if (((held & CONTROL_RT) != 0) && ((g_TitleOldHeld & CONTROL_RT) == 0))
	{
		g_TitleSelectionX = g_TitleSelectionX + 1;
		if (2 < g_TitleSelectionX)
		{
			g_TitleSelectionX = 0;
		}
		g_TitleButton = g_TitleSelectionX + g_TitleSelectionY * 3;
		if ((g_TitleButton == 10) && RegistrationCurrentlyValid())
		{
			g_TitleSelectionX = 2;
			g_TitleButton = 0xb;
		}
	}
	if (((held & CONTROL_LF) != 0) && ((g_TitleOldHeld & CONTROL_LF) == 0))
	{
		g_TitleSelectionX = g_TitleSelectionX + -1;
		if (g_TitleSelectionX < 0)
		{
			g_TitleSelectionX = 2;
		}
		g_TitleButton = g_TitleSelectionX + g_TitleSelectionY * 3;
		if ((g_TitleButton == 10) && RegistrationCurrentlyValid())
		{
			g_TitleSelectionX = 0;
			g_TitleButton = 9;
		}
	}
	if (((held & CONTROL_UP) != 0) && ((g_TitleOldHeld & CONTROL_UP) == 0))
	{
		g_TitleSelectionY = g_TitleSelectionY + -1;
		if (g_TitleSelectionY < 0)
		{
			g_TitleSelectionY = 3;
		}
		g_TitleButton = g_TitleSelectionX + g_TitleSelectionY * 3;
		if ((g_TitleButton == 10) && RegistrationCurrentlyValid())
		{
			g_TitleSelectionY = 2;
			g_TitleButton = 7;
		}
	}
	if (((held & CONTROL_DN) != 0) && ((g_TitleOldHeld & CONTROL_DN) == 0))
	{
		g_TitleSelectionY = g_TitleSelectionY + 1;
		if (3 < g_TitleSelectionY)
		{
			g_TitleSelectionY = 0;
		}
		g_TitleButton = g_TitleSelectionX + g_TitleSelectionY * 3;
		if ((g_TitleButton == 10) && RegistrationCurrentlyValid())
		{
			g_TitleSelectionY = 0;
			g_TitleButton = 1;
		}
	}
	g_TitleOldHeld = held;
	return true;
}

void TitleRender()
{
	static const char registrationDescription[128] = "This is the UNREGISTERED version of Amazin' SPISPOPD. Select \"How To Register\" to gain access to all the levels and options!";
	static const char buttonDescriptions[12][48] = {
		"Single player normal mazes",
		"Single player random mazes",
		"Try to get a record time!",
		"Two player normal mazes",
		"Two player random mazes",
		"Race for the most candles!",
		"Set up the game",
		"Playing Instructions",
		"Takes you to www.hamumu.com",
		"View current High Scores",
		"Find out how to get the full game!",
		"No more fun for you",
	};

	mgl->ClearScreen();
	for (int i = 0; i < 9; i = i + 1)
	{
		mgl->FillBox(0, (8 - i) * -8 + 471, 639, (8 - i) * -8 + 479, i + 96);
		mgl->FillBox(0, i * -2 + 16, 639, i * -2 + 17, i + 96);
	}
	g_ButtonsJsp->GetSprite(24)->Draw(440, 460, mgl);
	g_ButtonsJsp->GetSprite(25)->Draw(310, g_TitleTimer + 8, mgl);
	if (!RegistrationCurrentlyValid())
	{
		PrintMultiline(g_RegistrationTextX + 640, 236, registrationDescription);
	}

	GuysRender();
	DisplayListRender();

	g_ButtonsJsp->GetSprite(0 + (g_TitleButton == 0))->Draw(g_TitleTimer + 18, g_TitleTimer + 260, mgl);
	g_ButtonsJsp->GetSprite(4 + (g_TitleButton == 1))->Draw(220, g_TitleTimer * 2 + 260, mgl);
	g_ButtonsJsp->GetSprite(8 + (g_TitleButton == 2))->Draw(422 - g_TitleTimer, g_TitleTimer + 260, mgl);
	g_ButtonsJsp->GetSprite(2 + (g_TitleButton == 3))->Draw(g_TitleTimer + 18, g_TitleTimer / 2 + 298, mgl);
	g_ButtonsJsp->GetSprite(6 + (g_TitleButton == 4))->Draw(220, g_TitleTimer + 298, mgl);
	g_ButtonsJsp->GetSprite(10 + (g_TitleButton == 5))->Draw(422 - g_TitleTimer, g_TitleTimer / 2 + 298, mgl);
	g_ButtonsJsp->GetSprite(12 + (g_TitleButton == 6))->Draw(g_TitleTimer + 18, 336 - g_TitleTimer / 2, mgl);
	g_ButtonsJsp->GetSprite(20 + (g_TitleButton == 7))->Draw(220, 336 - g_TitleTimer, mgl);
	g_ButtonsJsp->GetSprite(18 + (g_TitleButton == 8))->Draw(422 - g_TitleTimer, 336 - g_TitleTimer / 2, mgl);
	g_ButtonsJsp->GetSprite(16 + (g_TitleButton == 9))->Draw(g_TitleTimer + 18, 374 - g_TitleTimer, mgl);
	if (!RegistrationCurrentlyValid())
	{
		g_ButtonsJsp->GetSprite(22 + (g_TitleButton == 10))->Draw(220, 374 - g_TitleTimer * 2, mgl);
	}
	g_ButtonsJsp->GetSprite(14 + (g_TitleButton == 11))->Draw(422 - g_TitleTimer, 374 - g_TitleTimer, mgl);

	CenterPrintMultiline(320, 430, buttonDescriptions[g_TitleButton]);
	if (-32 < g_TitleTextBright)
	{
		CenterPrintBright(120, 200, "Smoove", g_TitleTextBright);
		CenterPrintBright(260, 200, "Chuckles", g_TitleTextBright);
		CenterPrintBright(380, 200, "Helga", g_TitleTextBright);
		CenterPrintBright(520, 200, "Pete", g_TitleTextBright);
	}

	if (!RegistrationCurrentlyValid())
	{
		CenterPrintBright(70, 50, "*DEMO*", 0);
		CenterPrintBright(570, 50, "*DEMO*", 0);
	}

	if (g_TitleMouseMode != false)
	{
		int msx, msy;
		mgl->GetMouse(&msx, &msy);
		g_ButtonsJsp->GetSprite(26)->Draw(msx, msy, mgl);
	}
}

bool CongratsUpdate();
void CongratsRender();

void CongratsInit()
{
	mgl->LoadBMP("grahics/backgd.bmp");
	memcpy(g_Background, mgl->GetScreen(), 0x4b000);
	SetUpdate(CongratsUpdate);
	SetRender(CongratsRender);
	g_TitleTextBright = -0x20;
	g_TitleTimer = 0;
	mgl->LastKeyPressed();
	CongratsInitGuys();
	if (RegistrationCurrentlyValid())
	{
		g_Player[0].score = g_Player[0].score + 5000;
		g_Player[1].score = g_Player[1].score + 5000;
	}
}

void CongratsExit()
{
}

bool CongratsUpdate()
{
	char k;

	TitleUpdateGuys();
	if (g_TitleTextBright < 0)
	{
		g_TitleTextBright = g_TitleTextBright + 1;
	}
	k = mgl->LastKeyPressed();
	if (k == '\x1b')
	{
		CongratsExit();
		HighScoreInit();
	}
	else
	{
		g_TitleTimer = g_TitleTimer + 1;
		if (g_TitleTimer == 300)
		{
			CongratsExit();
			HighScoreInit();
		}
	}
	return true;
}

void CongratsRender()
{
	memcpy(mgl->GetScreen(), g_Background, 0x4b000);
	GuysRender();
	DisplayListRender();
	if (RegistrationCurrentlyValid())
	{
		CenterPrintBright(0x140, 100, "CONGRATULATIONS!", (int8_t)g_TitleTextBright);
		CenterPrintBright(0x140, 0x82, "You recovered all the candles", (int8_t)g_TitleTextBright);
		CenterPrintBright(0x140, 0xa0, "from the Happy Fields.", (int8_t)g_TitleTextBright);
		CenterPrintBright(0x140, 0xbe, "Pretty darn impressive.", (int8_t)g_TitleTextBright);
	}
	else if (g_GameMode == GameMode::HappyFields)
	{
		CenterPrintBright(0x140, 100, "That's as far as you can", (int8_t)g_TitleTextBright);
		CenterPrintBright(0x140, 0x82, "get in this demo version of", (int8_t)g_TitleTextBright);
		CenterPrintBright(0x140, 0xa0, "Amazin' SPISPOPD.  Order the", (int8_t)g_TitleTextBright);
		CenterPrintBright(0x140, 0xbe, "full version to get the real", (int8_t)g_TitleTextBright);
		CenterPrintBright(0x140, 0xdc, "good stuff!", (int8_t)g_TitleTextBright);
	}
	else
	{
		CenterPrintBright(0x140, 100, "That's as far as you can", (int8_t)g_TitleTextBright);
		CenterPrintBright(0x140, 0x82, "get in this demo version of", (int8_t)g_TitleTextBright);
		CenterPrintBright(0x140, 0xa0, "Amazin' SPISPOPD.  In the full", (int8_t)g_TitleTextBright);
		CenterPrintBright(0x140, 0xbe, "version, the Dungeons keep", (int8_t)g_TitleTextBright);
		CenterPrintBright(0x140, 0xdc, "getting bigger and scarier!", (int8_t)g_TitleTextBright);
	}
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
	mgl->SetPalette(g_SplashPalette);
	SetUpdate(SplashUpdate);
	SetRender(SplashRender);
	mgl->LastKeyPressed();
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

	keych = mgl->LastKeyPressed();
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
		mgl->SetSecondaryPalette(g_SplashPalette);
		break;
	case 1:
		g_SplashTimer = g_SplashTimer + 1;
		if (g_SplashTimer == (unsigned int)g_SplashWhich * 0xf0 + 0x3c)
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
				mgl->SetSecondaryPalette(g_SplashPalette);
				g_SplashMode = 3;
				return true;
			}
		}
		mgl->SetSecondaryPalette(g_SplashPalette);
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
		    !mgl->LoadBMP("graphics/hamumu.bmp", g_HamumuBmpPalette))
		{
			TitleInit();
			return;
		}
		g_SplashMode = 0;
	}
	if (g_SplashMode == 3)
	{
		mgl->RealizePalette();
		g_SplashMode = 4;
	}
	return;
}
