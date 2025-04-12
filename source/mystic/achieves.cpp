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
#include "steam.h"

AchieveDef achieveDef[] = {
	{
		"Old Schoolkid",37,
		"Prove you are a true classic.",
		"Completed Classic Mode or 1999 Mode.",
	},
	{
		"Modern Gamer",38,
		"Hello, fellow children! I too am a gamer!",
		"Completed Modern Mode.",
	},
	{
		"Git Gud",39,
		"Brutalize Bobby Khan.",
		"Completed Brutal Mode (either Classic or Modern).",
	},
	{
		"A little BIT kRaZy",44,
		"Let's see if you're truly loco.",
		"Completed Madcap Mode (either Classic or Modern).",
	},
	{
		"Graduation Cap",45,
		"Earn the angriest hat of them all.",
		"Collected 99 of every type of Madcap Stone. Wow, what a grind.",
	},
	{
		"Kid Mystic's Pro Caster",46,
		"Master the challenges of this world.",
		"Beat Bobby Khan in Challenge Mode (either Classic or Modern).",
	},
	{
		"Starkid",47,
		"Just like Luigi, you gotta collect the stars.",
		"Earned every star possible in Challenge Mode (either Classic or Modern).",
	},
	{
		"Rune Master",48,
		"Build a rock collection!",
		"Fully upgraded every Rune, just for fun. And this achievement.",
	},
	{
		"Caught 'Em All",49,
		"You know who we want you to catch.",
		"Caught every fairy!",
	},
	{
		"Forged In Air",50,
		"This would be easier if a watery tart just threw it at you.",
		"Completed the Armageddon Sword!",
	},
	{
		"Take The L",51,
		"This achievement was made by the makers of Spooky Castle's vault.",
		"Reached level 50!",
	},
	{
		"Energetic Boss Rage",6,
		"Tickle it to death.",
		"Defeat a boss with nothing but Energy Barrage.",
	},
	{
		"Amazing Blaze",2,
		"Hedges and ONLY hedges.",
		"Burn down every tree in Among The Hedges.",
	},
	{
		"Side Effects May Include Death",12,
		"Seek without seeking.",
		"Got 20 Seeker Bolt kills without casting a single Bolt.",
	},
	{
		"Kid Medusa",15,
		"Revenge is a dish best served cold.",
		"Froze 20 enemies at once.",
	},
	{
		"Mass Casualty Event",18,
		"An infernal catastrophe.",
		"Killed 20 enemies in one cast of Inferno.",
	},
	{
		"Take a Break",21,
		"Let somebody else save Tulipton.",
		"Your summons beat a level alone, with at least 20 kills.",
	},
	{
		"Rubber v. Glue",26,
		"Why are they hitting themselves?",
		"Won a level with at least 20 kills, using only Armor spells.",
	},
	{
		"Blurred Cirque",27,
		"Never stop never stopping!",
		"Remained Berserk for 1 minute straight.",
	},
	{
		"Born Again",32,
		"'Tis better to die a thousand deaths.",
		"Died twice in one level, and lived to tell the tale.",
	},
	{
		"Iron Chef",54,
		"Don't be an idiot sandwich.",
		"Cooked Swamp Stew to utter perfection.",
	},
	{
		"Skillbor",25,
		"What a clever build.",
		"Maxed out every single skill.",
	},
	{
		"Whacking Day",53,
		"Prove your carnival skills.",
		"Got over 75 whacks in Whack-a-Zoid.",
	},
	{
		"Delivery Boy",52,
		"Make it hot like Papa John.",
		"Fed all the Spooky Castle employees hot stew.",
	},
};

CheatStoneDef cheatStones[6] = {
	{
		"Happy Stick!",
		"The overworld is overrun with Happy Sticks.",
	},
	{
		"Stamana",
		"Regenerate Mana like it's stamina. Fast.",
	},
	{
		"Potent Potables",
		"Every enemy drops a potion.",
	},
	{
		"Monster Party",
		"Overworld enemies won't attack you!",
	},
	{
		"Fire Up!",
		"Start every level with 2 Energy Orbs and Speed Gems.",
	},
	{
		"Cosmic Rebirth",
		"Nothing can stop Kid Mystic! Nothing!",
	},
};

//-----------------------------------------------
static byte cursor;
byte* backgd;
int backX, backY;
sprite_set_t* chalSpr;
byte cheatNotice;

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
	cheatNotice = 0;
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

	if (cheatNotice)
	{
		cheatNotice--;
		backY -= 4;
		if (backY < 0)
			backY += 256;
	}

	backY--;
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
			cursor = (int)Achievement::NUM_ACHIEVES +6- 1;
		else if (cursor > 5)
			cursor -= 6;
		else cursor = 100;
	}
	if(AutoRepeatTapped(CONTROL_DN))
	{
		if (cursor < (int)Achievement::NUM_ACHIEVES+6)
		{
			cursor += 6;
			if (cursor > (int)Achievement::NUM_ACHIEVES + 6 - 1)
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
		else if (cursor >= (int)Achievement::NUM_ACHIEVES)
		{
			int n = cursor - (int)Achievement::NUM_ACHIEVES;
			if (opt.cheatStone[n] == CHEATSTONE_OFF)
			{
				MakeNormalSound(SND_CHEATWIN);
				opt.cheatStone[n] = CHEATSTONE_ON;
			}
			else if (opt.cheatStone[n] == CHEATSTONE_ON)
			{
				MakeNormalSound(SND_DOOROPEN);
				opt.cheatStone[n] = CHEATSTONE_OFF;
			}
			else
				MakeNormalSound(SND_UNAVAILABLE);
		}
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
			RenderSkillBox(x-2, y-2, x + 42, y + 42, col+31, col + 16);
			RenderSkillBox(x, y, x + 40, y + 40, 16, col+31);
			RenderSkillBox(x + 2, y + 2, x + 38, y + 38, 16, col+5);
			BlitIconGlow(achieveDef[i].icon, x + 4, y + 4, 0);

			RenderSkillBox(HALFWID - 250, SCRHEI - 130, HALFWID + 250, SCRHEI - 60, col+20, col + 4);
			CenterPrintGlow(SCRHEI - 130, achieveDef[i].name, 2);
			if (opt.achieve[i] == ACHIEVE_GOT)
			{
				chalSpr->GetSprite(2)->Draw(HALFWID - 250+20, SCRHEI - 130+10, mgl);
				chalSpr->GetSprite(2)->Draw(HALFWID + 250-20, SCRHEI - 130+10, mgl);
				CenterPrintItalics(HALFWID, SCRHEI - 105, achieveDef[i].hint, 7, 1);
				CenterPrint(HALFWID, SCRHEI - 75, achieveDef[i].desc, 0, 1);
			}
			else
			{
				CenterPrintItalics(HALFWID, SCRHEI - 85, achieveDef[i].hint, 15, 1);
			}
		}
		else
		{
			RenderSkillBox(x, y, x + 40, y + 40, col+16, col+4);
			RenderSkillBox(x + 2, y + 2, x + 38, y + 38, col+16, col+4);
			BlitIconGlow(achieveDef[i].icon, x + 4, y + 4, -12);
		}

		x += 60;
		if ((i%6)==5)
		{
			x = SCRWID/2-3*60+20;
			y += 60;
		}
	}

	for (int i = 0; i < 6; i++)
	{
		byte col = 32 * 3;
		
		if (cursor == i+(int)Achievement::NUM_ACHIEVES)
		{
			BlitIcon(60+opt.cheatStone[i], x + 4 + 2, y + 2, 5, -31);
			BlitIcon(60+opt.cheatStone[i], x + 4, y, 5, 0);
			if (opt.cheatStone[i] != CHEATSTONE_LOCKED)
				BlitIcon(60+3+i, x + 4, y, 5, 0);

			RenderSkillBox(HALFWID - 250, SCRHEI - 130, HALFWID + 250, SCRHEI - 60, col + 20, col + 4);
			char s[64];
			sprintf(s, "Cheatstone: %s", cheatStones[i].name);
			CenterPrintGlow(SCRHEI - 130, s, 2);
			if (opt.cheatStone[i]>CHEATSTONE_LOCKED)
			{
				CenterPrint(HALFWID, SCRHEI - 75, cheatStones[i].desc, 0, 1);
			}
			else
			{
				CenterPrintItalics(HALFWID, SCRHEI - 85, "Earn the 4 achievements above to unlock.", 15, 1);
			}
		}
		else
		{
			BlitIcon(60 + opt.cheatStone[i], x + 4 + 2, y + 2, 3, -31);
			BlitIcon(60 + opt.cheatStone[i], x + 4, y, 3, 0);
			if(opt.cheatStone[i]!=CHEATSTONE_LOCKED)
				BlitIcon(60 + 3 + i, x + 4, y, 3, 0);
		}

		x += 60;
		if ((i % 6) == 5)
		{
			x = SCRWID / 2 - 3 * 60 + 20;
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
	if (cheatNotice > 0)
	{
		x = SCRWID / 2 + (Cosine((cheatNotice * 8)&255)*25)/FIXAMT;
		y = SCRHEI / 2 - 20 + (Sine((cheatNotice * 12)&255) * 25) / FIXAMT;

		CenterPrint(x+2,y+2, "NEW CHEATSTONE!", -31, 0);
		CenterPrint(x+2, y-2, "NEW CHEATSTONE!", -31, 0);
		CenterPrint(x-2, y+2, "NEW CHEATSTONE!", -31, 0);
		CenterPrint(x-2, y-2, "NEW CHEATSTONE!", -31, 0);
		CenterPrint(x, y, "NEW CHEATSTONE!", 0, 0);
	}
}

TASK(void) AchieveMenu(MGLDraw *mgl)
{
	byte b=0;
	int lastTime=1;

	StartClock();
	if(CurrentSong()!=SONG_SHOP && CurrentSong()!=SONG_INTRO)
		PlaySong(SONG_SHOP);
	InitAchieveMenu();
	if (CheatStoneCheck())
	{
		MakeNormalSound(SND_MUSHMAD);
		cheatNotice = 120;
	}
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

bool CheatStoneCheck(void)
{
	byte yay = false;

	for (int j = 0; j < 6; j++)
	{
		byte count = 0;
		for (int i = 0; i < 4; i++)
		{
			if (opt.achieve[i * 6 + j] == ACHIEVE_GOT)
				count++;
		}
		if (count == 4 && opt.cheatStone[j] == CHEATSTONE_LOCKED)
		{
			opt.cheatStone[j] = CHEATSTONE_OFF;
			yay = true;
		}
	}
	return yay;
}

void EarnAchieve(Achievement n)
{
	Steam()->CompleteGoal((int)n);

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
