#include "title.h"
#include "game.h"
#include "jamulfmv.h"
#include "pause.h"
#include "challenge.h"
#include "options.h"
#include "appdata.h"
#include "string_extras.h"

// special codes in the credits:
// @ = use GirlsRWeird font
// # = draw a major horizontal line
// % = draw a minor horizontal line
// $ = last line of the whole deal
static const char credits[][64]={
	"The Adventures Of",
	"@KID MYSTIC",
	"",
	"Enchanted Edition",
	"",
	"A Hamumu Software Production",
	"www.hamumu.com",
	"#",
	"Programming",
	"Mike Hommel",
	"Tad 'SpaceManiac' Hardesty",
	"%",
	"Art",
	"Mike Hommel",
	"Ben Rose",
	"Icons by Lorc from",
	"www.game-icons.net",
	"%",
	"Music",
	"Brent Christian",
	"Thomas Reed",
	"%",
	"Level Design",
	"Mike Hommel",
	"%",
	"Sound Effects",
	"Brent Christian",
	"Mike Hommel",
	"%",
	"HamSandwich Contributors",
#include "../credits.inl"
	"%",
	"Testers",
	"Ben Rose",
	"Megadog",
	"Corey 'Coolguy' Connolly",
	"Micah Green",
	"Bryan Harrington",
	"Dave 'Custom Title' Hettel",
	"Wesley Jakab",
	"Lisa Keck",
	"TD Miller",
	"Barry Pennington",
	"Betty Scherber",
	"Thousands of people over the years",
	"#",
	"Thanks for playing!",
	"Visit us at www.hamumu.com!",
	"","","","","","","","","","","","","","",
	"@THE END",
	"$"
	};

static const char victoryTxt[][64]={
	"With the shield restored,",
	"the people of Tulipton could",
	"return to their peaceful,",
	"monster-free lives.",
	"But not before holding a",
	"week-long party in Kid",
	"Mystic's honor and creating",
	"a giant statue of him in the",
	"village square.",
	"",
	"#",
	"",
	"Mr. Wilkins had used the evil",
	"warlord disguise so that he",
	"could steal the mystical orbs",
	"to protect Spooky Castle from",
	"monsters.  That way, he'd get",
	"many more tourists!  When Kid",
	"Mystic foiled his scheme, Mr.",
	"Wilkins was banished to the",
	"dungeon.  He spent his days",
	"plotting revenge, vowing one",
	"day to return and reclaim",
	"Spooky Castle as his own.",
	"",
	"$"
	};

static const char madcapWinTxt[][64]={
	"Wow!  You actually completed",
	"Madcap Mode!  That's almost",
	"disturbing.  Congratulations,",
	"and now enjoy the Happy Stick",
	"Dancers!",
	"",
	"$"
};

static const char demoWinTxt[][64]={
	"Having defeated the Optimum",
	"Octon, Kid Mystic is well",
	"on his way to the fearsome",
	"Spooky Castle.",
	"",
	"But to finish the journey,",
	"you'll have to purchase the",
	"full version of Kid Mystic.",
	"available at our website",
	"http://hamumu.com!",
	"$",
	};

// once the credits have scrolled to END_OF_CREDITS pixels, they end
#define END_OF_CREDITS 480*2+400
#define END_OF_VICTORY 480*2-400
#define END_OF_DEMOWIN 480-250
#define END_OF_MADCAPWIN 480-300

// the most custom worlds it will handle
#define MAX_CUSTOM 64

struct title_t
{
	int bouaphaX;
	int blueY;
	char titleBright;
	char titleDir;
	int moonY;
	int optionsX;
	byte cursor;
	byte savecursor;
	int saveOffset;
	byte saveChapter[5], saveHour[5], saveMin[5], saveLevel[5], saveNightmare[5],savePercent[5];
	Difficulty saveDiff[5];
};

sprite_set_t *planetSpr;
static int numRunsToMakeUp;
byte pickerpos;
char pickeroffset;
byte offsetdir;
byte curCustom;
byte challengePct[4];

mfont_t pickerFont;
char customName[MAX_CUSTOM][32];
char customFname[MAX_CUSTOM][32];
static byte creditsOrIntro;

static byte keyAnim=0;
static byte *backgd;

byte starColorTable[]={214,81,63,49,33,21,32,83,93};
static dword startTime;

void MainMenuDisplay(MGLDraw *mgl,title_t title,bool hideAchieves)
{
	int i;
	int orbY[]={237,288,339,384,439,450};

	memcpy(mgl->GetScreen(),backgd,SCRWID*SCRHEI);

	Print(SCRWID-285,468,"Copyright 2004-2025, Hamumu Software",0,1);
	Print(2,468,VERSION_NO,0,1);

	for(i=0;i<5;i++)
	{
		if(title.cursor==i)
			planetSpr->GetSprite(0)->DrawGlow(50,orbY[i],mgl,title.titleBright);
		if(i!=2 || opt.challenge)
		{
			if(title.cursor==i)
				planetSpr->GetSprite(i*2+1+(title.cursor!=i))->DrawGlow(80,210,mgl,title.titleBright);
			else
				planetSpr->GetSprite(i*2+1+(title.cursor!=i))->Draw(80,210,mgl);

		}
	}

	if (!hideAchieves)
	{
		if (title.cursor == 5)
			planetSpr->GetSprite(0)->DrawGlow(340, orbY[5], mgl, title.titleBright);
		if (title.cursor == 5)
			planetSpr->GetSprite(13)->DrawGlow(370, 420, mgl, title.titleBright);
		else
			planetSpr->GetSprite(12)->Draw(370, 420, mgl);
	}
	planetSpr->GetSprite(15)->DrawGlow(HALFWID + 60, 165, mgl, title.titleBright-8);
	planetSpr->GetSprite(14)->DrawBright(HALFWID+60, 165, mgl,-2+title.titleBright/2);

#ifdef CHEAT
	CenterPrint(HALFWID,80,"*CHEAT EDITION*",MGL_random(48)-24,0);
#endif
}

byte MainMenuUpdate(MGLDraw *mgl,title_t *title,int *lastTime)
{
	while(*lastTime>=TIME_PER_FRAME)
	{
		UpdateSounds();
		UpdateControls();

		// update graphics
		title->titleBright+=title->titleDir;
		if(title->titleBright>8)
		{
			title->titleDir=-1;
			title->titleBright=8;
		}
		if(title->titleBright<-8)
		{
			title->titleDir=1;
			title->titleBright=-8;
		}

		// now real updating

		if(AutoRepeatTapped(CONTROL_UP) && title->cursor!=5)
		{
			(title->cursor)--;
			if(title->cursor==255)
				title->cursor=4;
			if(!opt.challenge && title->cursor==2)
				title->cursor=1;
			MakeNormalSound(SND_MENUCLICK);
			startTime=timeGetTime();	// reset the clock if any key is pressed
		}
		if(AutoRepeatTapped(CONTROL_DN) && title->cursor!=5)
		{
			(title->cursor)++;
			if(title->cursor==5)
				title->cursor=0;
			if(!opt.challenge && title->cursor==2)
				title->cursor=3;
			MakeNormalSound(SND_MENUCLICK);
			startTime=timeGetTime();	// reset the clock if any key is pressed
		}
		if (AutoRepeatTapped(CONTROL_RT))
		{
			title->cursor = 5;
			MakeNormalSound(SND_MENUCLICK);
			startTime = timeGetTime();	// reset the clock if any key is pressed
		}
		if (AutoRepeatTapped(CONTROL_LF))
		{
			if(title->cursor==5)
				title->cursor = 4;
			MakeNormalSound(SND_MENUCLICK);
			startTime = timeGetTime();	// reset the clock if any key is pressed
		}
		if(ButtonTapped(CONTROL_B1))
		{
			MakeNormalSound(SND_MENUSELECT);
			startTime=timeGetTime();	// reset the clock if any key is pressed
			return 1;
		}

		/*if (ButtonTapped(CONTROL_ESCAPE))
		{
			MakeNormalSound(SND_MENUSELECT);
			startTime=timeGetTime();	// reset the clock if any key is pressed
			UpdateControls();
			return 3;
		}*/
#if defined(CHEAT) || !defined(NDEBUG)
		char c = mgl->LastKeyPressed();
		if(c=='e')
		{
			title->cursor=6;
			return 1;
		}
#endif
		*lastTime-=TIME_PER_FRAME;
	}

	return 0;
}

TASK(byte) MainMenu(MGLDraw *mgl)
{
	dword now;
	byte b=0;
	title_t title;
	int lastTime=1;

	creditsOrIntro=0;
	mgl->LoadBMP("graphics/title.bmp");
	backgd=(byte *)malloc(SCRWID*SCRHEI);
	if(!backgd)
		FatalError("Out of memory!!");
	memcpy(backgd,mgl->GetScreen(),SCRWID*SCRHEI);

	mgl->LastKeyPressed();
	mgl->ClearScreen();
	planetSpr=new sprite_set_t("graphics/title.jsp");

	title.bouaphaX=SCRWID;
	title.optionsX=-300;
	title.titleBright=-32;
	title.titleDir=4;
	title.cursor=0;
	title.blueY=1;
	title.moonY=SCRHEI;
	startTime=timeGetTime();
	StartClock();
	if(CurrentSongAdjusted()!=SONG_SHOP && CurrentSongAdjusted()!=SONG_INTRO)
		PlaySong(SONG_SHOP);
	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();
		b=MainMenuUpdate(mgl,&title,&lastTime);
		MainMenuDisplay(mgl,title,false);
		AWAIT mgl->Flip();
		if(!mgl->Process())
		{
			delete planetSpr;
			free(backgd);
			CO_RETURN 255;
		}
		if (b == 1 && title.cursor == 0)	// selected New Game
		{
			if (!AWAIT DifficultyPicker(mgl, &title))	// pressed ESC on the slot picker
				b = 0;

			startTime = timeGetTime();
		}
		if(b==1 && title.cursor==1)	// selected Continue
		{
			if(!AWAIT GameSlotPicker(mgl,&title))	// pressed ESC on the slot picker
				b=0;

			startTime = timeGetTime();
		}
		if (b == 1 && title.cursor == 2)	// selected Challenge
		{
			if (!AWAIT DifficultyPicker(mgl, &title))	// pressed ESC on the slot picker
				b = 0;

			startTime = timeGetTime();
		}

		now=timeGetTime();
		if(now-startTime>1000*10)
		{
			// alternate between showing credits and the intro movie if the user sits at the title
			if(creditsOrIntro)
				AWAIT ShowVictoryAnim(11);
			else
				AWAIT Credits(mgl,0);
			creditsOrIntro=1-creditsOrIntro;

			startTime=timeGetTime();
		}
		EndClock();
	}
	delete planetSpr;
	free(backgd);
	if(b==1)	// something was selected
	{
		if(title.cursor==4)	// exit
			CO_RETURN 255;
		else
			CO_RETURN title.cursor;
	}
	else
		CO_RETURN 255;	// ESC was pressed
}

static void GetSaves(title_t *title)
{
	for (int i = 0; i < 5; i++)
	{
		char s[32];
		sprintf(s, "mystic%d.sav", title->saveOffset + i + 1);
		auto f = AppdataOpen(s);
		if (!f)
		{
			title->saveLevel[i] = 0;
			title->saveChapter[i] = 0;
			title->saveHour[i] = 0;
			title->saveMin[i] = 0;
			title->saveNightmare[i] = 0;
			title->saveDiff[i] = Difficulty::UNUSED;	// unused
		}
		else
		{
			player_t p;
			SDL_RWread(f, &p, sizeof(player_t), 1);
			title->saveLevel[i] = p.level;
			title->saveChapter[i] = p.worldNum + 1;
			title->saveHour[i] = (byte)(p.gameClock / (30 * 60 * 60));
			title->saveMin[i] = (byte)((p.gameClock / (30 * 60)) % 60);
			title->saveNightmare[i] = p.nightmare;
			title->saveDiff[i] = p.difficulty;
			title->savePercent[i] = CalcGamePercent(&p);
			f.reset();
		}
	}
}

void GameSlotPickerDisplay(MGLDraw *mgl,title_t title)
{
	int i;
	char s[32];

	MainMenuDisplay(mgl,title,true);

	for(i=0;i<5;i++)
	{
		int x = 450;
		if (title.saveOffset == 245)
		{
			sprintf(s, "Bk%d", i + 1);
			if (i == 0)
			{
				CenterPrint(SCRWID - HALFWID / 2 + 1, 200 - 30 + 1, "Backups are saved automatically", -31, 1);
				CenterPrint(SCRWID - HALFWID / 2 + 1, 200 - 18 + 1, "whenever you exit the game!", -31, 1);
				CenterPrint(SCRWID - HALFWID / 2, 200 - 30, "Backups are saved automatically", 0, 1);
				CenterPrint(SCRWID - HALFWID / 2, 200 - 18, "whenever you exit the game!", 0, 1);
			}
		}
		else
		{
			sprintf(s, "%d", title.saveOffset + i + 1);
		}
		if (title.savecursor == i)
		{
			x += 10;
			GetItemSprite(7)->DrawGlow(x-5 - 20-GetStrLength(s,0), 200 + i * 50 + 22, mgl, title.titleBright);
			RightPrint(x-5+ 2, 200 + i * 50 + 2, s, -31, 0);
		}
		RightPrint(x-5, 200 + i * 50, s, -16 + (title.savecursor == i) * 16, 0);

		if(title.saveChapter[i]==0 || title.saveLevel[i]==0 || title.saveDiff[i]==Difficulty::UNUSED)
		{
			if(title.savecursor==i)
				PrintBright(x+2,200+i*50+2,"Empty Slot",-31,2);
			PrintBrightGlow(x, 200 + i * 50, "Empty Slot", -16 + (title.savecursor == i) * 16, 2);
		}
		else
		{
			auto dest = ham_sprintf(s, "Chapter %d", title.saveChapter[i]);
			if(title.saveNightmare[i])
				dest = ham_strcpy(dest, "!!!");
			dest = ham_strcpy(dest, DifficultySuffix(title.saveDiff[i]));
			if (title.savecursor == i)
				PrintBright(x + 2, 200 + i * 50 + 2, s, -31, 2);
			PrintBrightGlow(x, 200 + i * 50, s, -16 + (title.savecursor == i) * 16, 2);

			sprintf(s,"%02d:%02d L%02d %d%%",title.saveHour[i],title.saveMin[i],title.saveLevel[i],title.savePercent[i]);

			if(title.savecursor==i)
				PrintBright(x+2, 200 + i * 50 + 20+2, s, -31, 2);
			PrintBrightGlow(x,200+i*50+20,s,-16+(title.savecursor==i)*16,2);
		}
	}
}

byte GameSlotPickerUpdate(MGLDraw *mgl,title_t *title,int *lastTime)
{
	while(*lastTime>=TIME_PER_FRAME)
	{
		UpdateControls();
		UpdateSounds();

		// update graphics
		title->titleBright+=title->titleDir;
		if(title->titleBright>8)
		{
			title->titleDir=-1;
			title->titleBright=8;
		}
		if(title->titleBright<-8)
		{
			title->titleDir=1;
			title->titleBright=-8;
		}

		// now real updating
		byte scan = LastScanCode();
		if(AutoRepeatTapped(CONTROL_UP))
		{
			(title->savecursor)--;
			if(title->savecursor==255)
			{
				title->savecursor=4;
				title->saveOffset = (title->saveOffset + 250 - 5) % 250;
				GetSaves(title);
			}
			MakeNormalSound(SND_MENUCLICK);
		}
		if (AutoRepeatTapped(CONTROL_DN))
		{
			(title->savecursor)++;
			if(title->savecursor==5)
			{
				title->savecursor=0;
				title->saveOffset = (title->saveOffset + 5) % 250;
				GetSaves(title);
			}
			MakeNormalSound(SND_MENUCLICK);
		}
		if (AutoRepeatTapped(CONTROL_LF) || AutoRepeatTapped(CONTROL_B3))
		{
			title->saveOffset = (title->saveOffset + 250 - 5) % 250;
			GetSaves(title);

			MakeNormalSound(SND_MENUCLICK);
		}
		if (AutoRepeatTapped(CONTROL_RT) || AutoRepeatTapped(CONTROL_B4))
		{
			title->saveOffset = (title->saveOffset + 5) % 250;
			GetSaves(title);

			MakeNormalSound(SND_MENUCLICK);
		}

		if(ButtonTapped(CONTROL_B1))
		{
			if(title->saveChapter[title->savecursor]==0 || title->saveLevel[title->savecursor]==0)
				MakeNormalSound(SND_ACIDSPLAT);
			else
			{
				MakeNormalSound(SND_MENUSELECT);
				return 1;
			}
		}
		if (ButtonTapped(CONTROL_B2|CONTROL_ESCAPE))
		{
			MakeNormalSound(SND_MENUSELECT);
			UpdateControls();
			return 2;
		}

		*lastTime-=TIME_PER_FRAME;
	}

	return 0;
}

void InitGameSlotPicker(MGLDraw *mgl,title_t *title)
{
	GetSaves(title);
	mgl->LastKeyPressed();
}

TASK(byte) GameSlotPicker(MGLDraw *mgl,title_t *title)
{
	byte b=0;
	int lastTime=1;

	title->saveOffset=0;
	title->savecursor=0;
	InitGameSlotPicker(mgl,title);

	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();
		b=GameSlotPickerUpdate(mgl,title,&lastTime);
		GameSlotPickerDisplay(mgl,*title);
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN 0;
		EndClock();
	}
	if(b==1)	// something was selected
	{
		InitPlayer(INIT_GAME,0,0);
		PlayerLoadGame(title->saveOffset + title->savecursor);
		// make it remember which was picked so the pause menu will start on the same
		SetSubCursor(title->saveOffset + title->savecursor);
		CO_RETURN 1;
	}
	else
		CO_RETURN 0;
}

void DifficultyPickerDisplay(MGLDraw* mgl, title_t title)
{
	int i;
	static const char diffName[][16] = {
		"Classic",
		"Modern",
		"Brutal Classic",
		"Brutal Modern",
		"1999 Mode",
	};
	char s[32];

	MainMenuDisplay(mgl, title,true);
	RenderSkillBox(360, 144, 636, 460, 31, 3);
	PrintBrightGlow((360+636-GetStrLength("Difficulty", 0))/2, 150, "Difficulty", 0, 0);

	int x;
	for (i = 0; i < (title.cursor == 2 ? 4 : 5); i++)
	{
		x = 370;
		if (title.cursor == 2)	// challenge mode shows your % on that save
		{
			sprintf(s, "%s [%d%%]", diffName[i], challengePct[i]);
			if (title.savecursor == i)
			{
				x += 20;
				GetItemSprite(7)->DrawGlow(x - 16, 190 + i * 25 + 17, mgl, title.titleBright);
				PrintBright(x + 2, 190 + i * 25 + 2, s, -31, 2);
			}
			PrintBrightGlow(x, 190 + i * 25, s, -16 + (title.savecursor == i) * 16, 2);
		}
		else
		{
			if (title.savecursor == i)
			{
				x += 20;

				GetItemSprite(7)->DrawGlow(x - 16, 190 + i * 25 +17, mgl,title.titleBright);
				PrintBright(x + 2, 190 + i * 25 + 2, diffName[i], -31, 2);
			}
			PrintBrightGlow(x, 190 + i * 25, diffName[i], -16 + (title.savecursor == i) * 16, 2);
		}
	}
	switch (title.savecursor)
	{
		case 0:
			PrintBrightGlow(365, 200 + 4 * 30, "The homegrown remake of the", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14, "original game from 2004!", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14*2, "This added 4 levels to each chapter,", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14*3, "new enemies, and of course the", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 4, "world-renowned Fairy System.", 0, 1);
			break;
		case 1:
			PrintBrightGlow(365, 200 + 4 * 30, "It's 2025, can there be a game with", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14, "no skill tree? Let's not find out.", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 2, "This adds many tweaks to levels,", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 3, "spells and enemies, along with runes,", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 4, "some new levels, and a skill tree.", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 5, "It's probably easier, and definitely", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 6, "smoother.", 0, 1);
			break;
		case 2:
		case 3:
			PrintBrightGlow(365, 200 + 4 * 30, "Brutal mode enemies are five times", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14, "tougher and stronger. Luckily, you", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 2, "can buy Madcap Stones, and you", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 3, "start with the first upgrades for", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 4, "your Boots, Hat, and Staff.", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 5, "Available in both Classic and Modern", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 6, "flavors.", 0, 1);
			break;
		case 4:
			PrintBrightGlow(365, 200 + 4 * 30, "The truly original game, released in", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14, "1999 as 'Spooky Castle: The", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 2, "Adventures of Kid Mystic', which", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 3, "people rightfully confused with 'The", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 4, "Adventures of Bouapha: Spooky", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 5, "Castle', my other game published by", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 6, "the same company! I just wanted to", 0, 1);
			PrintBrightGlow(365, 200 + 4 * 30 + 14 * 7, "call it Kid Mystic, and now we do.", 0, 1);
			break;
	}
}

byte DifficultyPickerUpdate(MGLDraw* mgl, title_t* title, int* lastTime)
{
	while (*lastTime >= TIME_PER_FRAME)
	{
		UpdateControls();
		UpdateSounds();

		// update graphics
		title->titleBright += title->titleDir;
		if (title->titleBright > 8)
		{
			title->titleDir = -1;
			title->titleBright = 8;
		}
		if (title->titleBright < -8)
		{
			title->titleDir = 1;
			title->titleBright = -8;
		}

		// now real updating

		if (AutoRepeatTapped(CONTROL_UP))
		{
			(title->savecursor)--;
			if (title->savecursor == 255)
				title->savecursor = (title->cursor == 2 ? 4 : 5) - 1;
			MakeNormalSound(SND_MENUCLICK);
		}
		if (AutoRepeatTapped(CONTROL_DN))
		{
			(title->savecursor)++;
			if (title->savecursor >= (title->cursor == 2 ? 4 : 5))
				title->savecursor = 0;
			MakeNormalSound(SND_MENUCLICK);
		}
		if (ButtonTapped(CONTROL_B1))
		{
			player.difficulty = (Difficulty)title->savecursor;
			MakeNormalSound(SND_MENUSELECT);
			return 1;
		}
		if (ButtonTapped(CONTROL_ESCAPE|CONTROL_B2))
		{
			MakeNormalSound(SND_MENUSELECT);
			UpdateControls();
			return 2;
		}

		*lastTime -= TIME_PER_FRAME;
	}

	return 0;
}

void InitDifficultyPicker(MGLDraw* mgl, title_t* title)
{
	title->savecursor = 0;
	mgl->LastKeyPressed();
	LoadChallengePercents(challengePct);
}

TASK(byte) DifficultyPicker(MGLDraw* mgl, title_t* title)
{
	byte b = 0;
	int lastTime = 1;

	title->savecursor = 0;
	InitDifficultyPicker(mgl, title);

	while (b == 0)
	{
		lastTime += TimeLength();
		StartClock();
		b = DifficultyPickerUpdate(mgl, title, &lastTime);
		DifficultyPickerDisplay(mgl, *title);
		AWAIT mgl->Flip();
		if (!mgl->Process())
			CO_RETURN 0;
		EndClock();
	}
	if (b == 1)	// something was selected
	{
		InitPlayer(INIT_GAME, 0, 0);
		CO_RETURN 1;
	}
	else
		CO_RETURN 0;
}

void CreditsRender(int y,byte mode)
{
	int i,ypos;
	const char *s;

	i=0;

	ypos=0;
	while(credits[i][0]!='$')
	{
		s=credits[i];
		if(ypos-y>-60)
		{
			if(s[0]=='@')
			{
				if(s[1]!='T' || mode!=0)
					CenterPrint(HALFWID,ypos-y,&s[1],0,0);
			}
			else if(s[0]=='#')
			{
				DrawFillBox(HALFWID-200,ypos-y+8,HALFWID+200,ypos-y+11,255);
			}
			else if(s[0]=='%')
			{
				DrawFillBox(HALFWID-70,ypos-y+8,HALFWID+70,ypos-y+9,255);
			}
			else
				CenterPrint(HALFWID,ypos-y,s,0,2);
		}
		ypos+=20;
		i++;
		if(ypos-y>=SCRHEI)
			return;
	}
}

TASK(void) Credits(MGLDraw *mgl,byte mode)
{
	int y=-470;
	static byte flip=0;
	byte scroll=1;
	dword startTime,endTime;

	mgl->LastKeyPressed();

	mgl->LoadBMP("graphics/pal.bmp");
	while(1)
	{
		UpdateControls();
		UpdateSounds();

		startTime=timeGetTime();
		mgl->ClearScreen();
		CreditsRender(y,mode);

		if(scroll)
			y+=1;

		if(y==END_OF_CREDITS-HALFHEI*3/2 && mode==1)
			scroll=0;

		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN;
		if (ButtonTapped(CONTROL_B1 | CONTROL_B2 | CONTROL_ESCAPE | CONTROL_B3 | CONTROL_B4))
		{
			UpdateControls();
			CO_RETURN;
		}

		if(y==END_OF_CREDITS-HALFHEI*3/2 && mode==0)
			CO_RETURN;

		endTime=timeGetTime();
		while((endTime-startTime)<TIME_PER_FRAME)
			endTime=timeGetTime();
	}
}

void VictoryTextRender(int y,byte type)
{
	int i,ypos;
	const char *s;

	i=0;

	ypos=0;

	while(1)
	{
		switch(type)
		{
			case 0:
				s=victoryTxt[i];
				break;
			case 1:
				s=madcapWinTxt[i];
				break;
			case 2:
				s=demoWinTxt[i];
				break;
		}
		if(s[0]=='$')
			break;
		if(ypos-y>-60)
		{
			if(s[0]=='@')
			{
				CenterPrintGlow(ypos-y,&s[1],0);
			}
			else if(s[0]=='#')
			{
				DrawFillBox(HALFWID-200,ypos-y+8,HALFWID+200,ypos-y+11,255);
			}
			else if(s[0]=='%')
			{
				DrawFillBox(HALFWID-70,ypos-y+8,HALFWID+70,ypos-y+9,255);
			}
			else
				CenterPrintGlow(ypos-y,s,2);
		}

		ypos+=20;
		i++;
		if(ypos-y>=SCRHEI)
			return;
	}
}

TASK(void) VictoryText(MGLDraw *mgl,byte victoryType)
{
	int y=-470;
	int darkY,i,j,k;
	byte *scr;
	dword startTime,endTime;
	byte flip=0;
	int length=10;

	scr=(byte *)malloc(SCRWID*SCRHEI);
	if(!scr)
		CO_RETURN;

	switch(victoryType)
	{
		case 0:
			mgl->LoadBMP("graphics/zoobo.bmp");
			length=END_OF_VICTORY;
			break;
		case 1:
			mgl->LoadBMP("graphics/zoobo.bmp");
			length=END_OF_MADCAPWIN;
			break;
		case 2:
			mgl->LoadBMP("graphics/zoobo2.bmp");
			length=END_OF_DEMOWIN;
			break;
	}

	mgl->LastKeyPressed();
	memcpy(scr,mgl->GetScreen(),SCRWID*SCRHEI);
	darkY=0;
	while(1)
	{
		UpdateControls();
		UpdateSounds();

		startTime=timeGetTime();
		memcpy(mgl->GetScreen(),scr,SCRWID*SCRHEI);
		VictoryTextRender(y,victoryType);
		if((flip=1-flip) || victoryType==1 || victoryType==2)
			y++;
		AWAIT mgl->Flip();
		if(!mgl->Process())
			break;

		if (ButtonTapped(CONTROL_ESCAPE))
		{
			UpdateControls();
			break;
		}
		if(y==length)
			break;

		if(y>0)
		{
			for(k=1;k<20;k++)
			{
				j=darkY*SCRWID;
				for(i=0;i<SCRWID;i++)
				{
					if(scr[i+j]>0)
						scr[i+j]--;
				}
				darkY+=19;
				if(darkY>SCRHEI-1)
					darkY-=SCRHEI;
			}
		}

		endTime=timeGetTime();
		while((endTime-startTime)<TIME_PER_FRAME)
			endTime=timeGetTime();
	}
	free(scr);
}

TASK(void) SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound,byte specialdeal)
{
	int i,j,clock;
	RGB desiredpal[256],curpal[256];
	byte mode,done;
	dword tick;

	for(i=0;i<256;i++)
	{
		curpal[i].r=0;
		curpal[i].g=0;
		curpal[i].b=0;
	}
	mgl->SetPalette(curpal);
	mgl->RealizePalette();

	mgl->LastKeyPressed();

	if (!mgl->LoadBMP(fname, desiredpal))
		CO_RETURN;

	StartClock();
	mode=0;
	clock=0;
	done=0;
	tick=0;
	while(!done)
	{
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN;
		UpdateControls();
		UpdateSounds();

		if(ButtonTapped(CONTROL_ESCAPE|CONTROL_B1|CONTROL_B2))
			mode=2;

		EndClock();
		tick+=TimeLength();
		StartClock();
		if(tick>1000/30)
		{
			clock++;
			tick-=1000/30;
			switch(mode)
			{
				case 0:	// fading in
					for(j=0;j<8;j++)
						for(i=0;i<256;i++)
						{
							if(curpal[i].r<desiredpal[i].r)
								curpal[i].r++;
							if(curpal[i].g<desiredpal[i].g)
								curpal[i].g++;
							if(curpal[i].b<desiredpal[i].b)
								curpal[i].b++;
						}
					mgl->SetPalette(curpal);
					mgl->RealizePalette();
					if(clock==32)
					{
						if(sound)
							MakeNormalSound(sound);
					}
					if(clock>64)
					{
						mode=1;
						clock=0;
					}
					break;
				case 1:
					if(clock>delay)
					{
						mode=2;
						clock=0;
					}
					break;
				case 2:	// fading out
					clock=0;
					for(j=0;j<8;j++)
						for(i=0;i<256;i++)
						{
							if(curpal[i].r>0)
								curpal[i].r--;
							else
								clock++;
							if(curpal[i].g>0)
								curpal[i].g--;
							else
								clock++;
							if(curpal[i].b>0)
								curpal[i].b--;
							else
								clock++;
						}
					mgl->SetPalette(curpal);
					mgl->RealizePalette();
					if(clock==256*3*8)
						done=1;
					break;
			}
		}
	}
	mgl->ClearScreen();
	AWAIT mgl->Flip();
}
