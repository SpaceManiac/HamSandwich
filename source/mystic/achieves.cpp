#include "achieves.h"
#include "display.h"
#include "control.h"
#include "clock.h"
#include "sound.h"
#include "appdata.h"
#include "pause.h"
#include "skills.h"
#include "game.h"

AchieveDef achieveDef[] = {
	{
		"Old Schoolkid",
		"Prove you are a true classic.",
		"Complete Classic Mode.",
		Progress::ONE_SHOT
	},
	{
		"Modern Gamer",
		"Hello, fellow children! I too am a gamer!",
		"Complete Modern Mode.",
		Progress::ONE_SHOT
	},
	{
		"Git Gud",
		"Brutalize Bobby Khan.",
		"Complete Brutal Mode (either Classic or Modern).",
		Progress::ONE_SHOT
	},
	{
		"A little BIT kRaZy",
		"Let's see if you're truly loco.",
		"Complete Madcap Mode (either Classic or Modern).",
		Progress::ONE_SHOT
	},
	{
		"Graduation Cap",
		"Earn the angriest hat of them all.",
		"Collect every single Stone in Madcap Mode. Wow, what a grind.",
		Progress::ONE_SHOT
	},
	{
		"Kid Mystic's Pro Caster",
		"Master the challenges of this world.",
		"Complete Challenge Mode (either Classic or Modern).",
		Progress::ONE_SHOT
	},
	{
		"Starkid",
		"Just like Luigi, you gotta collect the stars.",
		"Earn every star possible in Challenge Mode (either Classic or Modern).",
		Progress::ONE_SHOT
	},
};

//-----------------------------------------------
static byte cursor;
byte* backgd;
int backX, backY;

void InitAchieveMenu(void)
{
	cursor=0;

	backX = 0;
	backY = 0;
	GetDisplayMGL()->LoadBMP("graphics/rapid.bmp");
	backgd = (byte*)malloc(256*256);
	if (!backgd)
		FatalError("Out of memory!!");
	for (int i = 0; i < 256; i++)
	{
		memcpy(&backgd[i * 256], &(GetDisplayMGL()->GetScreen()[i * SCRWID]), 256);
		for (int j = 0; j < 256; j++)
		{
			byte b = (backgd[i * 256 + j]&31);
			if (b > 8)
				b -= 8;
			else b = 0;
			b+= 32 * 3;
			backgd[i * 256 + j] = b;
		}
	}
}

void ExitAchieveMenu(void)
{
	free(backgd);
}

byte UpdateAchieveMenu(MGLDraw *mgl)
{
	dword btn,j;
	int i;

	backY-=2;
	if (backY < 0)
		backY += 256;
	backX--;
	if (backX < 0)
		backX += 256;

	if(ButtonTapped(CONTROL_ESCAPE,true))
		return 1;

	if(AutoRepeatTapped(CONTROL_UP))
	{
		if (cursor == 100)
			cursor = (int)Achievement::NUM_ACHIEVES - 1;
		else if (cursor > 5)
			cursor -= 6;
		else cursor = 100;
	}
	if(AutoRepeatTapped(CONTROL_DN))
	{
		if (cursor < (int)Achievement::NUM_ACHIEVES)
		{
			cursor += 6;
			if (cursor > (int)Achievement::NUM_ACHIEVES - 1)
				cursor = 100;
		}
		else cursor = 0;
	}
	if (AutoRepeatTapped(CONTROL_LF))
	{
		if (cursor != 100)
		{
			if (cursor % 6)
				cursor--;
			else
				cursor += 5;
		}
	}
	if (AutoRepeatTapped(CONTROL_RT))
	{
		if (cursor != 100)
		{
			if ((cursor % 6)<5)
				cursor++;
			else
				cursor -= 5;
		}
	}
	if(ButtonTapped(CONTROL_B1,true))
	{
		if (cursor == 100)
			return 1;
	}

	return 0;
}

void RenderAchieveMenu(MGLDraw *mgl)
{
	int x,y;

	x = 0;
	y = backY;
	if (y > 0) y -= 256;
	while (1)
	{
		int x2, y2;
		x2 = x + 256;
		y2 = y + 256;
		if (x2 > SCRWID)
			x2 = SCRWID;
		if (y2 > SCRHEI)
			y2 = SCRHEI;
		
		for (int j = 0; j < (y2 - y); j++)
		{
			if(j+y>=0 && j+y<SCRHEI)
				memcpy(&(mgl->GetScreen()[x + (j + y) * SCRWID]), &backgd[j * 256], x2 - x);
		}
		x += 256;
		if (x >= SCRWID)
		{
			x = 0;
			y += 256;
			if (y >= SCRHEI)
				break;
		}
	}
	
	CenterPrintGlow(2,"Achievements",0);
	
	x = SCRWID / 2 - 3 * 60+20;
	y = 60;
	for (int i = 0; i < (int)Achievement::NUM_ACHIEVES; i++)
	{
		if (cursor == i)
		{
			RenderSkillBox(x, y, x + 40, y + 40, 16, 31);
			RenderSkillBox(x + 2, y + 2, x + 38, y + 38, 16, 5);
			BlitIcon(0, x + 4, y + 4, 0, 5);
		}
		else
		{
			RenderSkillBox(x, y, x + 40, y + 40, 3*32+16, 3*32+4);
			RenderSkillBox(x + 2, y + 2, x + 38, y + 38, 3*32+16, 3*32+4);
			BlitIcon(0, x + 4, y + 4, 3, 0);
		}
		
		x += 60;
		if ((i%6)==5)
		{
			x = SCRWID/2-3*60+20;
			y += 60;
		}
	}

	if (cursor == 100)
	{
		RenderSkillBox(SCRWID / 2 - 40, SCRHEI - 40-4, SCRWID / 2 + 40, SCRHEI - 40 + 20, 16, 31);
		RenderSkillBox(SCRWID / 2 - 40+2, SCRHEI - 40-4+2, SCRWID / 2 + 40-2, SCRHEI - 40 + 20-2, 16, 5);
	}
	else
	{
		RenderSkillBox(SCRWID / 2 - 40, SCRHEI - 40-4, SCRWID / 2 + 40, SCRHEI - 40 + 20, 3*32+16, 3*32+4);
		RenderSkillBox(SCRWID / 2 - 40 + 2, SCRHEI - 40 -4 + 2, SCRWID / 2 + 40 - 2, SCRHEI - 40 + 20 - 2, 3*32+16, 3*32+4);
	}
	int len = GetStrLength("Exit", 2);
	PrintBrightGlow(SCRWID/2 - len/2, SCRHEI - 40-3, "Exit", (cursor == 100) * 31, 2);
}

TASK(void) AchieveMenu(MGLDraw *mgl)
{
	byte b=0;
	int lastTime=1;

	mgl->LastKeyPressed();

	StartClock();
	if(CurrentSong()!=SONG_SHOP && CurrentSong()!=SONG_INTRO)
		PlaySong(SONG_SHOP);
	InitAchieveMenu();

	while (b == 0)
	{
		lastTime += TimeLength();
		StartClock();
		while (lastTime >= TIME_PER_FRAME)
		{
			UpdateControls();
			JamulSoundUpdate();

			b = UpdateAchieveMenu(mgl);
			lastTime -= TIME_PER_FRAME;
		}
		RenderAchieveMenu(mgl);
		AWAIT mgl->Flip();
		if (!mgl->Process())
		{
			ExitAchieveMenu();
			CO_RETURN;
		}
		EndClock();
	}
	ExitAchieveMenu();
}
