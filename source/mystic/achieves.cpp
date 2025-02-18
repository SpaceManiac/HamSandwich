#include "achieves.h"
#include "display.h"
#include "control.h"
#include "clock.h"
#include "sound.h"
#include "appdata.h"
#include "pause.h"

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
static byte oldc;
static byte cursor;
static dword oldBtn;
byte* backgd;
int backX, backY;

void InitAchieveMenu(void)
{
	oldc=255;
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
	char c;
	byte c2;
	dword btn,j;
	int i;

	c=mgl->LastKeyPressed();
	c2=GetControls()|GetArrows();

	if(c==27)
		return 1;

	if((c2&CONTROL_UP) && (!(oldc&CONTROL_UP)))
	{
		cursor--;
		if(cursor>5)
			cursor=5;
	}
	if((c2&CONTROL_DN) && (!(oldc&CONTROL_DN)))
	{
		cursor++;
		if(cursor>5)
			cursor=0;
	}
	if((c2&(CONTROL_B1|CONTROL_B2)) && (!(oldc&(CONTROL_B1|CONTROL_B2))))
	{
		switch(cursor)
		{
			case 0:
				MakeNormalSound(SND_GOATSHOOT);
				break;
			case 1:
				
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				return 1;
				break;
		}
	}

	oldc=c2;

	return 0;
}

void RenderAchieveMenu(MGLDraw *mgl)
{
	int x, y;

	x = backX;
	if (x > 0) x -= 256;
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
			memcpy(&(mgl->GetScreen()[x + (j+y) * SCRWID]), &backgd[j * 256], x2 - x);
		x += 256;
		if (x >= SCRWID)
		{
			x = backX;
			if (x > 0) x -= 256;
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
		RenderSkillBox(x, y, x + 34, y + 34, 20, 4+27*(cursor==i));
		RenderSkillBox(x+2, y+2, x + 32, y + 32, 16, 4);
		x += 60;
		if ((i%6)==5)
		{
			x = SCRWID/2-3*60+20;
			y += 60;
		}
	}
}

TASK(void) AchieveMenu(MGLDraw *mgl)
{
	byte b=0;
	int lastTime=1;


	mgl->LastKeyPressed();
	oldc=CONTROL_B1|CONTROL_B2;

	StartClock();
	if(CurrentSong()!=SONG_SHOP && CurrentSong()!=SONG_INTRO)
		PlaySong(SONG_SHOP);
	InitAchieveMenu();

	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdateAchieveMenu(mgl);
		RenderAchieveMenu(mgl);
		AWAIT mgl->Flip();
		if(!mgl->Process())
		{
			ExitAchieveMenu();
			CO_RETURN;
		}

		EndClock();
	}
	ExitAchieveMenu();
}
