#include "achieves.h"
#include "display.h"
#include "control.h"
#include "clock.h"
#include "sound.h"
#include "appdata.h"
#include "pause.h"
#include "skills.h"
#include "game.h"
#include "options.h"

AchieveDef achieveDef[] = {
	{
		"Old Schoolkid",0,
		"Prove you are a true classic.",
		"Completed Classic Mode.",
		Progress::ONE_SHOT
	},
	{
		"Modern Gamer",1,
		"Hello, fellow children! I too am a gamer!",
		"Completed Modern Mode.",
		Progress::ONE_SHOT
	},
	{
		"Git Gud",2,
		"Brutalize Bobby Khan.",
		"Completed Brutal Mode (either Classic or Modern).",
		Progress::ONE_SHOT
	},
	{
		"A little BIT kRaZy",3,
		"Let's see if you're truly loco.",
		"Completed Madcap Mode (either Classic or Modern).",
		Progress::ONE_SHOT
	},
	{
		"Graduation Cap",4,
		"Earn the angriest hat of them all.",
		"Collected 99 of every type of Madcap Stone. Wow, what a grind.",
		Progress::ONE_SHOT
	},
	{
		"Kid Mystic's Pro Caster",5,
		"Master the challenges of this world.",
		"Beat Bpbby Khan in Challenge Mode (either Classic or Modern).",
		Progress::ONE_SHOT
	},
	{
		"Starkid",6,
		"Just like Luigi, you gotta collect the stars.",
		"Earned every star possible in Challenge Mode (either Classic or Modern).",
		Progress::ONE_SHOT
	},
	{
		"Rune Master",7,
		"Build a rock collection!",
		"Fully upgraded every Rune, just for fun. And this achievement.",
		Progress::ONE_SHOT
	},
	{
		"Caught 'Em All",8,
		"You know who we want you to catch.",
		"Caught every fairy!",
		Progress::ONE_SHOT
	},
	{
		"Forged In Air",9,
		"This would be easier if a watery tart just threw it at you.",
		"Completed the Armageddon Sword!",
		Progress::ONE_SHOT
	},
	{
		"Take The L",10,
		"This achievement was made by the makers of Spooky Castle's vault.",
		"Reached level 50!",
		Progress::ONE_SHOT
	},
	{
		"Energetic Boss Rage",11,	// NOT IMPLEMENTED BECAUSE HMM
		"Hint",
		"Defeat a boss with nothing but Energy Barrage.",
		Progress::ONE_SHOT
	},
	{
		"Amazing Blaze",12,
		"Hedges and ONLY hedges.",
		"Burn down every tree in Among The Hedges.",
		Progress::ONE_SHOT
	},
	{
		"Side Effects May Include Death",13,
		"Seek without seeking.",
		"Got 20 Seeker Bolt kills without casting a single Bolt.",
		Progress::ONE_SHOT
	},
	{
		"Kid Medusa",14,
		"Revenge is a dish best served cold.",
		"Froze 20 enemies at once.",
		Progress::ONE_SHOT
	},
	{
		"Mass Casualty Event",15,
		"An infernal catastrophe.",
		"Killed 20 enemies in one cast of Inferno.",
		Progress::ONE_SHOT
	},
	{
		"Take a Break",16,
		"Let somebody else save Tulipton.",
		"Your summons beat a level alone, with at least 20 kills.",
		Progress::ONE_SHOT
	},
	{
		"Rubber v. Glue",17,
		"Why are they hitting themselves?",
		"Won a level with at least 20 kills, using only Armor spells.",
		Progress::ONE_SHOT
	},
	{
		"Blurred Cirque",18,
		"Never stop never stopping!",
		"Remained Berserk for 1 minute straight.",
		Progress::ONE_SHOT
	},
	{
		"Born Again",19,
		"'Tis better to die a thousand deaths.",
		"Died twice in one level, and lived to tell the tale.",
		Progress::ONE_SHOT
	},
	{
		"Iron Chef",20,
		"Don't be an idiot sandwich.",
		"Cooked Swamp Stew to utter perfection.",
		Progress::ONE_SHOT
	},
	{
		"Skillbor",21,
		"Kid Mystic is a game of skill.",
		"Maxed out every single skill. What a clever build.",
		Progress::ONE_SHOT
	},
	{
		"Whacking Day",22,
		"Prove your carnival skills.",
		"Got over 75 whacks in Whack-a-Zoid.",
		Progress::ONE_SHOT
	},
	{
		"Delivery Boy",23,
		"Make it hot like Papa John.",
		"Fed all the Spooky Castle employees hot stew.",
		Progress::ONE_SHOT
	},
};

//-----------------------------------------------
static byte cursor;
byte* backgd;
int backX, backY;
sprite_set_t* chalSpr;

void InitAchieveMenu(void)
{
	cursor=0;

	chalSpr = new sprite_set_t("graphics/chal.jsp");

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
	delete chalSpr;
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

	if(ButtonTapped(CONTROL_ESCAPE))
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
	if(ButtonTapped(CONTROL_B1))
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
		byte col = 32 * 3;
		if (opt.achieve[i] == ACHIEVE_GOT)
			col = 32 * 5;

		if (cursor == i)
		{
			RenderSkillBox(x, y, x + 40, y + 40, 16, col+31);
			RenderSkillBox(x + 2, y + 2, x + 38, y + 38, 16, col+5);
			BlitIcon(achieveDef[i].icon, x + 4, y + 4, col, 5);

			RenderSkillBox(HALFWID - 250, SCRHEI - 160, HALFWID + 250, SCRHEI - 90, col+20, col + 4);
			CenterPrintGlow(SCRHEI - 160, achieveDef[i].name, 2);
			if (opt.achieve[i] == ACHIEVE_GOT)
			{
				chalSpr->GetSprite(2)->Draw(HALFWID - 250+20, SCRHEI - 160+10, mgl);
				chalSpr->GetSprite(2)->Draw(HALFWID + 250-20, SCRHEI - 160+10, mgl);
				CenterPrintItalics(HALFWID, SCRHEI - 135, achieveDef[i].hint, 7, 1);
				CenterPrint(HALFWID, SCRHEI - 105, achieveDef[i].desc, 0, 1);
			}
			else
			{
				CenterPrintItalics(HALFWID, SCRHEI - 115, achieveDef[i].hint, 15, 1);
			}
			
		}
		else
		{
			RenderSkillBox(x, y, x + 40, y + 40, col+16, col+4);
			RenderSkillBox(x + 2, y + 2, x + 38, y + 38, col+16, col+4);
			BlitIcon(achieveDef[i].icon, x + 4, y + 4, 3, 0);
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
			UpdateSounds();

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

//------------------------------------------------------- END OF MENU
std::vector<Achievement> achieveQueue;
Achievement achDisplayed;
int achX;
byte achState;

#define ACH_WIDTH (280)

void InitAchieveSystem(void)
{
	achieveQueue.clear();
	achDisplayed = Achievement::NONE;
}

void EarnAchieve(Achievement n)
{
	if (opt.achieve[(int)n] == ACHIEVE_GOT)
		return;

	opt.achieve[(int)n] = ACHIEVE_GOT;
	achieveQueue.push_back(n);
}

void UpdateAchieves(void)
{
	if (achDisplayed == Achievement::NONE)
	{
		if (achieveQueue.size() > 0)
		{
			achDisplayed = achieveQueue[0];
			achieveQueue.erase(achieveQueue.begin());
			achX = SCRWID;
			achState = 0;	// coming out
			MakeNormalSound(SND_TRIVIA);
		}
		else
			return;
	}

	if (achState == 0)	// popping out
	{
		achX -= 16;
		if (achX < SCRWID - ACH_WIDTH)
		{
			achX = SCRWID - ACH_WIDTH;
			achState = 1;
		}
	}
	else if (achState < 30 * 5)	// displaying
	{
		achState++;
		if (achState == 150)
			achState = 255;
	}
	else // going away
	{
		achX += 8;
		if (achX > SCRWID)
		{
			achDisplayed = Achievement::NONE;
		}
	}
}

void RenderAchieves(void)
{
	if (achDisplayed != Achievement::NONE)
	{
		RenderSkillBox(achX+2, SCRHEI - 62+2, achX + ACH_WIDTH - 3, SCRHEI - 3, 0,0);
		RenderSkillBox(achX, SCRHEI - 62, achX + ACH_WIDTH-5, SCRHEI - 5, 32 * 5 + 16, 32 * 5 + 5);
		BlitIcon(achieveDef[(int)achDisplayed].icon, achX + ACH_WIDTH-5 - 2 - 32, SCRHEI - 5 - 2 - 32, 5, 0);
		RightPrintGlow(achX + ACH_WIDTH-5, SCRHEI - 62, "Achievement Achieved!", 2);
		RightPrintGlow(achX + ACH_WIDTH - 5 - 2 - 32 - 5, SCRHEI - 5 - 15, achieveDef[(int)achDisplayed].name, 1);
	}
}
