#include "randomizer.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "title.h"
#include "plasma.h"
#include "appdata.h"

static byte cursor;
static byte oldc;
static dword oldBtn;
static byte optMode;
static std::string seed;

#define MAX_SEED_LENGTH 32;

void InitRandomizerMenu(void)
{
	oldc = 255;
	cursor = 0;
	InitPlasma(7);
	seed = "";
}

void ExitRandomizerMenu(void)
{
	ExitPlasma();
}

byte UpdateRandomizerMenu(int *lastTime, MGLDraw *mgl)
{
	char c;
	byte c2;
	dword btn, j;
	int i;

	if (*lastTime > TIME_PER_FRAME * 30)
		*lastTime = TIME_PER_FRAME * 30;
	while (*lastTime >= TIME_PER_FRAME)
	{
		switch (optMode)
		{
		case 0: // just going through options
			c = mgl->LastKeyPressed();
			c2 = GetControls() | GetArrows();

			if (c == 27)
			{
				return 1;
			}

			if ((c2 & CONTROL_UP) && (!(oldc & CONTROL_UP)))
			{
				cursor--;
				if (cursor > 1)
					cursor = 1;
				MakeNormalSound(SND_MENUCLICK);
			}
			if ((c2 & CONTROL_DN) && (!(oldc & CONTROL_DN)))
			{
				cursor++;
				if (cursor > 1)
					cursor = 0;
				MakeNormalSound(SND_MENUCLICK);
			}
			if ((c2 & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3)) && (!(oldc & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3))))
			{
				switch (cursor)
				{
				case 0:
					MakeNormalSound(SND_MENUSELECT);
					LastScanCode();
					optMode=1;
					break;
				case 1:
					MakeNormalSound(SND_MENUSELECT);
					return 1;
					break;
				}
			}
			break;
		case 1: // Typing in something
			c = mgl->LastKeyPressed();
			if(c==SDLK_ESCAPE)	// ESC key
			{
				optMode=0;
				c2=255;
				oldc=255;
				mgl->LastKeyPressed();
				MakeNormalSound(SND_MENUCANCEL);
				return 0;
			}
			else if (c==SDLK_BACKSPACE && seed.length() > 0)
			{
				seed.pop_back();
				MakeNormalSound(SND_MENUSELECT);
			}
			else if(c!=0)
			{
				if (seed.length() >= 32){
					MakeNormalSound(SND_MENUCANCEL);
				}
				else
				{
					if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'))
					{
						seed += c;
						MakeNormalSound(SND_MENUSELECT);
					}
				}
			}
			c2=255;
			break;
		}

		oldc = c2;

		UpdatePlasma();

		*lastTime -= TIME_PER_FRAME;
	}

	return 0;
}

void RenderRandomizerMenu(MGLDraw *mgl)
{

	int wid;
	byte *pos;
	int i;

	wid = mgl->GetWidth();
	pos = mgl->GetScreen() + 40 * wid;
	for (i = 40; i < 480 - 40; i++)
	{
		memset(pos, 7 * 32 + 2, 640);
		pos += wid;
	}

	RenderPlasma(mgl, 1);
	RenderPlasma(mgl, 0);

	CenterPrintGlow(320, 2, "Game Randomizer", 0, 2);

	PrintColor(240, 80, "Seed", 7, -10, 0);
	PrintColor(300,80, seed.c_str() ,7,-10,0);
	if (cursor == 0)
	{
		PrintColor(239, 79, "Seed", 0, 0, 0);
		PrintColor(299 ,79, seed.c_str() ,0,0,0);
	}

	PrintColor(240, 170, "Exit To Main Menu", 7, -10, 0);
	if (cursor == 1)
		PrintColor(239, 169, "Exit To Main Menu", 0, 0, 0);
}

//----------------

TASK(void)
RandomizerMenu(MGLDraw *mgl)
{
	byte done = 0;
	int lastTime;

	InitRandomizerMenu();
	lastTime = 1;

	while (!done)
	{
		lastTime += TimeLength();
		StartClock();
		done = UpdateRandomizerMenu(&lastTime, mgl);
		RenderRandomizerMenu(mgl);
		AWAIT mgl->Flip();
		EndClock();

		if (!mgl->Process())
			done = 1;
	}

	ExitRandomizerMenu();
}
