#include "title.h"
#include "game.h"
#include "jamulfmv.h"
#include "pause.h"
#include "options.h"
#include "plasma.h"
#include "palettes.h"
#include "appdata.h"
#include "steam.h"
#include "badge.h"

#define VERSION_NO         "Version 3.0"
#define COPYRIGHT_YEARS    "2001-2024"
#define COPYRIGHT_COMPANY  "Hamumu Games, Inc."

// special codes in the credits:
// @ = use GirlsRWeird font
// # = draw a major horizontal line
// % = draw a minor horizontal line
// $ = last line of the whole deal
static const char* credits[] = {
	"@LoonyLand:",
	"",
	"@Halloween Hill",
	"",
	"",
	("Copyright " COPYRIGHT_YEARS ", " COPYRIGHT_COMPANY),
	"#",
	"Original Concept",
	"Mike Hommel",
	"%",
	"Programming",
	"Mike Hommel",
	"Tad \"SpaceManiac\" Hardesty",
	"%",
	"HamSandwich Contributors",
#include "../credits.inl"
	"%",
	"Character Design",
	"Mike Hommel",
	"%",
	"Level Design",
	"Mike Hommel",
	"%",
	"3D Graphics",
	"Mike Hommel",
	"%",
	"2D Graphics",
	"Mark Stevens",
	"Mike Hommel",
	"%",
	"Sound Effects",
	"Solange Hunt",
	"Mike Hommel",
	"%",
	"\"Music\"",
	"Mike Hommel",
	"%",
	"Testing",
	"Eric Arevalo",
	"Solange Hunt",
	"BJ McRae",
	"Barry Pennington",
	"Mark Stevens",
	"Carol \"Baba\" VanderMolen",
	"%",
	"Muse",
	"Solange Hunt",
	"%",
	"This game goes out as",
	"a special thanks to the",
	"Dr. Lunatic fans!!",
	"Keep me fed, and I'll keep",
	"making these things...",
	"",
	"#",
	"Stop by www.hamumu.com!",
	"","","","","","","","","","","","","","","","","","","",
	"P.S.... watch for",
	"Loonyland: Insanity",
	"and",
	"Sol Hunt",
	"coming soon!!",
	"$"
	};

static const char* victoryTxt[] = {
	"",
	"",
	"&After a little time to rest and wipe off a whole",
	"",
	"&lot of mud, Loony got back to his feet and returned",
	"",
	"&to Luniton, a hero at last.",
	"",
	"",
	"",
	"#",
	"",
	"",
	"&All around the land of Halloween Hill, life was",
	"",
	"&returning to normal.  The frogs stopped licking",
	"",
	"&so viciously, the mummies settled down and returned",
	"",
	"&to their work in the toilet paper industry - Even",
	"",
	"&the werewolves went back to their den to take a nap.",
	"",
	"&Yes, everything was back to being as peaceful as it",
	"",
	"&gets in Halloween Hill.  Which, by the way, is not",
	"",
	"&terribly peaceful.",
	"",
	"",
	"",
	"",
	"",
	"#",
	"",
	"",
	"&There was just one thing wrong...",
	"",
	"&The trees in the south of Wicked Woods were still",
	"",
	"&hung with stick figures.  Almost as if the evil",
	"",
	"&presence there hadn't been the result of the",
	"",
	"&Evilizer at all... and was still lurking somewhere.",
	"$"
	};

static const char* cheatTxt[] = {
	"@Congratulations on getting",
	"",
	"",
	"@all 40 Merit Badges!",
	"",
	"",
	"#",
	"So since you've proven you don't need them,",
	"here are the cheat codes to the game!",
	"",
	"These ones are to be typed in while playing:",
	"",
	"%",
	"SUPERBOOM",
	"Causes tremendous explosions to wipe out enemies",
	"",
	"%",
	"GETALIFE",
	"Heals you to your full health",
	"",
	"%",
	"BANGZOOM",
	"Renders you invulnerable for a long time",
	"",
	"%",
	"GOGOGO",
	"Gives you double speed for a while",
	"",
	"",
	"#",
	"These cheats must be entered in the Badges screen,",
	"while holding down both fire buttons:",
	"",
	"%",
	"GIMME",
	"Gives you the merit badge you have highlighted",
	"If you already have it, it removes it instead",
	"",
	"%",
	"GIVEALL",
	"Gives you every merit badge",
	"",
	"%",
	"CLEAR",
	"Removes all merit badges, letting you make a fresh",
	"start if you want to earn them all again - also",
	"marks all bosses un-beaten, for Boss Bash mode",
	"",
	"%",
	"BOSSBASH",
	"Mark all bosses beaten for purposes of playing Boss",
	"Bash mode",
	"",
	"#",
	"",
	"Oh, and one other thing... start a new game as",
	"Loony or Werewolf Loony for a special surprise!",
	"",
	"#",
	"",
	"@Thank you for playing Loonyland!",
	"",
	"",
	"@Look for more good stuff coming soon at",
	"",
	"",
	"@WWW.HAMUMU.COM",
	"$",
};

static byte oldc=0;

static int saveOffset = 0;
static byte cursor;
static byte *backScr;
static int numRuns;
static byte songRestart;

enum class TitleSubmenu : byte
{
	MainMenu,
	LoadGame,
	ChooseDiff,  // difficulty chooser
};
static TitleSubmenu loadingGame;

struct MenuItem
{
	char txt[64];
	MainMenuResult action;

	bool known;
	char bright;
};

static constexpr int MENU_ITEMS_PER_PAGE = 12;
static constexpr int MENU_ITEMS_PER_ROW = 4;
static const MenuItem mainMenu[] = {
	{"New Game", MainMenuResult::NewGame},
	{"Load Game", MainMenuResult::LoadGame},
	{"Bowling", MainMenuResult::Bowling},
	{"Survival", MainMenuResult::Survival},
	{"Boss Bash", MainMenuResult::BossBash},
	{"Loony Ball", MainMenuResult::Loonyball},
	{"Remix", MainMenuResult::Remix},
	{"Badges", MainMenuResult::Badges},
	{"Hi Scores", MainMenuResult::HiScores},
	{"Options", MainMenuResult::Options},
	{"Extras...", MainMenuResult::Extras},
	{"Exit", MainMenuResult::Exit},
};
static const MenuItem extrasMenu[] = {
	{"Back...", MainMenuResult::BackToMain},
	{"Editor", MainMenuResult::Editor},
	{"Randomizer", MainMenuResult::Randomizer},
};
static std::vector<MenuItem> menu;

struct SaveMenuItem
{
	char txt[64];
	bool known;
	char bright;
};

static SaveMenuItem saves[5];
static MainMenuResult choosingDiffFor;

void SetSongRestart(byte b)
{
	songRestart=b;
}

byte WhichGameToLoad(void)
{
	return saveOffset + cursor;
}

void GetSavesForMenu(void)
{
	FILE *f;
	int i;
	char txt[64];
	float pct;
	player_t p;

	for(i=0;i<5;i++)
	{
		ham_sprintf(txt,"save%d.sav", saveOffset + i + 1);
		f=AppdataOpen(txt);
		if(!f)
		{
			sprintf(saves[i].txt, "%d: Unused", saveOffset + i + 1);
			saves[i].known = false;
		}
		else
		{
			fread(&p,sizeof(player_t),1,f);
			fclose(f);

			DescribeSave(saves[i].txt, &p);
			saves[i].known = true;
		}
		saves[i].bright=-32;
	}
}

void ShowMenuOption(int x,int y,const MenuItem &m,byte on)
{
	int i;

	if(m.known)
	{
		PrintGlow(x,y,m.txt,m.bright,2);
		if(on && m.bright>-20)
		{
			PrintGlow(x-4+Random(9),y-4+Random(9),m.txt,-20,2);
		}
	}
	else // if this option is unknown
	{
		// bouncy question marks!
		if(on)
		{
			for(i=0;i<8;i++)
			{
				PrintGlow(x+i*16-2+Random(5),y-2+Random(5),"?",m.bright,2);
			}
		}
		else
		{
			PrintGlow(x,y,"????????",m.bright,2);
		}
	}
}

void ShowSavedGame(int x,int y,const SaveMenuItem &m,byte on)
{
	PrintGlow(x,y,m.txt,m.bright,0);
	if(on && m.bright>-20)
	{
		PrintGlow(x-2+Random(5),y-2+Random(5),m.txt,-20,0);
	}
}

void MainMenuDisplay(MGLDraw *mgl)
{
	int i;
	int x,y;

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	// Workshop status
	const char* workshopStatus = Steam()->DescribeWorkshopStatus();
	Print(3, 3, workshopStatus, 1, 1);
	Print(3-1, 3-1, workshopStatus, 0, 1);
	// version #:
	Print(560,3,VERSION_NO,1,1);
	Print(559,2,VERSION_NO,0,1);
	// Copyright:
	RightPrintGlow(641, 467, "Copyright " COPYRIGHT_YEARS ", " COPYRIGHT_COMPANY, -10, 1);

	// menu options
	x=5;
	y=320;
	for(i=0;i<menu.size();i++)
	{
		ShowMenuOption(x,y-menu[i].bright/4,menu[i],(cursor==i));
		x+=160;
		if(x>640-150)
		{
			x=5;
			y+=45;
		}
	}
}

void DiffChooseDisplay(MGLDraw *mgl)
{
	int i;

	static const char diffDesc[][3][128]={
		// beginner
		"Enemies never do more than 1 Heart of damage, and move slower than normal.  You",
		"begin with 15 Hearts, and do extra damage.  Enemies drop more items than normal.",
		"",
		// normal
		"The standard difficulty.  Good for someone who has an idea of what they're doing!",
		"You begin with 10 Hearts.",
		"",
		// challenge
		"If you're looking for a bigger challenge, this is it!  Enemies do more damage, and",
		"you do less. You begin with 5 Hearts, and enemies drop fewer items.",
		"",
		// mad
		"A special difficulty to inject more skillful play into the late-game, although it makes",
		"the early going very tough.  Only for Loonyland aficionados.  You begin with 3 Hearts",
		"and enemies are much tougher to beat.",
		// Loony
		"For masochists.  Enemies do double damage, you do 1/4 damage, you begin with ONE",
		"Heart, poison is twice as deadly, enemies move faster, and enemies only drop items",
		"when Combo'd!",
		// Hard
		"Compared to normal, enemies deal more damage, and item drops are slightly reduced.",
		"Intended difficulty for the Randomizer.",
		"",
	};
	static_assert(SDL_arraysize(diffDesc) == NUM_DIFFICULTY);

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	// Workshop status
	const char* workshopStatus = Steam()->DescribeWorkshopStatus();
	Print(3, 3, workshopStatus, 1, 1);
	Print(3-1, 3-1, workshopStatus, 0, 1);
	// version #:
	Print(560,3,VERSION_NO,1,1);
	Print(559,2,VERSION_NO,0,1);

	PrintGlow(310,318,"Select with arrow keys and press Enter",0,1);
	if (cursor == 0)
	{
		PrintGlow(5,330,"Difficulty Level:",0,2);

		if(opt.difficulty != DIFF_BEGINNER)
			PrintGlow(280,330,"<<<",0,2);

		CenterPrintGlow(440,330,DifficultyName(opt.difficulty),0,2);
		CenterPrintGlow(440-4+Random(9),330-4+Random(9),DifficultyName(opt.difficulty),-20,2);

		if(opt.difficulty != DIFF_LOONY)
			PrintGlow(560,330,">>>",0,2);

		for(i=0;i<3;i++)
		{
			PrintGlow(5,390+i*15,diffDesc[opt.difficulty][i],0,1);
		}

		PrintGlow(5,450,"The chosen difficulty applies to all game modes.  You can change it at any time",0,1);
		PrintGlow(5,465,"in the Options menu on the title screen, but that doesn't affect saved games.",0,1);
	}
	else if (cursor == 1)
	{
		auto pc = GetCurrentPC();
		PrintGlow(5, 330, "Character:", 0, 2);

		if (pc > 0)
			PrintGlow(280,330,"<<<",0,2);

		CenterPrintGlow(440,330,PlayerCharacterName(pc),0,2);
		CenterPrintGlow(440-4+Random(9),330-4+Random(9),PlayerCharacterName(pc),-20,2);

		if (pc < PC_MAX - 1)
			PrintGlow(560,330,">>>",0,2);

		auto desc = GetCharacterDescription(pc);
		if (desc != nullptr)
		{
			char together[512];
			span<char> dst = together;
			for (i = 0; i < 8; i++)
			{
				dst = ham_sprintf(dst, "%s ", desc[i]);
			}

			PrintGlowRect(3, 388, SCRWID-3, SCRHEI, together, 15, 0, 1);
		}
		else
		{
			PrintGlow(5, 450, "The chosen character applies to all game modes.  You can change it at any time", 0, 1);
			PrintGlow(5, 465, "in the Badges menu on the title screen, but that doesn't affect saved games.", 0, 1);
		}
	}

	if (IsAnyCharacterUnlocked())
	{
		PrintGlow(5+35, 330-30+20, "^^^", 0, 0);
		PrintGlow(5+38, 330+60-28, "vvv", 0, 0);
	}
}

void LoadGameDisplay(MGLDraw *mgl)
{
	int i;
	int x,y;

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	// Workshop status
	const char* workshopStatus = Steam()->DescribeWorkshopStatus();
	Print(3, 3, workshopStatus, 1, 1);
	Print(3-1, 3-1, workshopStatus, 0, 1);
	// version #:
	Print(560,3,VERSION_NO,1,1);
	Print(559,2,VERSION_NO,0,1);

	PrintGlow(20-23/4, 117+320-135, "^", 0, 0);
	PrintGlow(20-23/4, 273+320-135-(30-26)*5, "v", 0, 0);

	const char* cancelText = ShowGamepadText() ? "Select a game to load or press Weapon to cancel" : "Select a game to load or press ESC to cancel";
	PrintGlow(5,467,cancelText,0,1);
	// menu options
	x=20;
	y=320;
	for(i=0;i<5;i++)
	{
		ShowSavedGame(x+saves[i].bright/4,y,saves[i],(cursor==i));
		y+=26;
	}
}

MainMenuResult MainMenuUpdate(int *lastTime,MGLDraw *mgl)
{
	byte c;
	static byte reptCounter=0;
	int i;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		for(i=0;i<menu.size();i++)
		{
			if(cursor==i)
			{
				if(menu[i].bright<0)
					menu[i].bright++;
			}
			else
			{
				if(menu[i].bright>-23)
					menu[i].bright--;
				else if(menu[i].bright<-23)
					menu[i].bright++;
			}
		}

		// now real updating
		c=GetControls()|GetArrows();

		if((c&CONTROL_UP) && !(oldc&CONTROL_UP))
		{
			if (cursor >= MENU_ITEMS_PER_ROW)  // up off the first row
				cursor -= MENU_ITEMS_PER_ROW;
			else
			{
				cursor += MENU_ITEMS_PER_PAGE;
				while (cursor >= menu.size())
					cursor -= MENU_ITEMS_PER_ROW;
			}

			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_DN) && !(oldc&CONTROL_DN))
		{
			cursor += MENU_ITEMS_PER_ROW;
			if (cursor >= menu.size())  // down off the last row (of this column)
			{
				cursor %= MENU_ITEMS_PER_ROW;
			}

			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_LF) && !(oldc&CONTROL_LF))
		{
			if ((cursor % MENU_ITEMS_PER_ROW) >= 1)
				cursor -= 1;
			else
			{
				cursor = std::min(cursor + MENU_ITEMS_PER_ROW - 1, (int)menu.size() - 1);
			}

			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_RT) && !(oldc&CONTROL_RT))
		{
			if (cursor == menu.size() - 1)  // right off the last item in the menu
				cursor -= cursor % MENU_ITEMS_PER_ROW;
			else if (cursor % MENU_ITEMS_PER_ROW == MENU_ITEMS_PER_ROW - 1)  // right off the last item in the row
				cursor -= cursor % MENU_ITEMS_PER_ROW;
			else
				cursor++;

			MakeNormalSound(SND_MENUCLICK);
		}

		if(c & ~oldc & CONTROL_B1)
		{
			if(menu[cursor].known)
			{
				MakeNormalSound(SND_MENUSELECT);
				return menu[cursor].action;
			}
			else
				MakeNormalSound(SND_MENUCANCEL);
		}

		if(c)	// hitting any key prevents credits
			numRuns=0;

		byte key=mgl->LastKeyPressed();
		if (menu[0].action != MainMenuResult::NewGame)
		{
			if (key == 27 || (c & ~oldc & CONTROL_B2))
				return MainMenuResult::BackToMain;
		}
		else
		{
			if(key==27)
				return MainMenuResult::Exit;
		}

#ifndef NDEBUG
		if(key=='e')
			return MainMenuResult::Editor;
#endif
		*lastTime-=TIME_PER_FRAME;
		numRuns++;
		oldc=c;
	}
	return MainMenuResult::None;
}

byte LoadGameUpdate(int *lastTime,MGLDraw *mgl)
{
	byte c;
	int i;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	byte scan = LastScanCode();
	if (scan == SDL_SCANCODE_PAGEUP)
	{
		saveOffset = (saveOffset + 250 - 5) % 250;
		GetSavesForMenu();
	}
	else if (scan == SDL_SCANCODE_PAGEDOWN)
	{
		saveOffset = (saveOffset + 5) % 250;
		GetSavesForMenu();
	}

	while(*lastTime>=TIME_PER_FRAME)
	{
		for(i=0;i<5;i++)
		{
			if(cursor==i)
			{
				if(saves[i].bright<0)
					saves[i].bright++;
			}
			else
			{
				if(saves[i].bright>-23)
					saves[i].bright--;
				else if(saves[i].bright<-23)
					saves[i].bright++;
			}
		}

		// now real updating
		c=GetControls()|GetArrows();
		byte taps = c & ~oldc;

		if(taps & CONTROL_UP)
		{
			cursor--;
			if(cursor>4)
			{
				cursor=4;
				saveOffset = (saveOffset + 250 - 5) % 250;
				GetSavesForMenu();
			}
			MakeNormalSound(SND_MENUCLICK);
		}
		if(taps & CONTROL_DN)
		{
			cursor++;
			if(cursor>4)
			{
				cursor=0;
				saveOffset = (saveOffset + 5) % 250;
				GetSavesForMenu();
			}
			MakeNormalSound(SND_MENUCLICK);
		}

		if(taps & CONTROL_B1)
		{
			if(saves[cursor].known)
			{
				MakeNormalSound(SND_MENUSELECT);
				return 2;
			}
			else
				MakeNormalSound(SND_MENUCANCEL);
		}

		// no credits while sitting here
		numRuns=0;

		oldc=c;

		c=mgl->LastKeyPressed();
		if(c==27 || (taps & CONTROL_B2))
		{
			MakeNormalSound(SND_MENUCANCEL);
			return 0;
		}

		*lastTime-=TIME_PER_FRAME;
		numRuns++;
	}
	return 1;
}

byte ChooseDiffUpdate(int *lastTime,MGLDraw *mgl)
{
	byte c;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		// now real updating
		c=GetControls()|GetArrows();
		byte taps = c & ~oldc;

		if (IsAnyCharacterUnlocked() && (taps & (CONTROL_UP | CONTROL_DN)))
		{
			cursor = !cursor;
		}

		if (cursor == 0)
		{
			if (taps & CONTROL_LF)
			{
				opt.difficulty = PrevDifficulty(opt.difficulty);
				MakeNormalSound(SND_MENUCLICK);
			}
			if(taps & CONTROL_RT)
			{
				opt.difficulty = NextDifficulty(opt.difficulty);
				MakeNormalSound(SND_MENUCLICK);
			}
		}
		else if (cursor == 1)
		{
			if (taps & CONTROL_LF)
			{
				PrevCharacter();
				MakeNormalSound(SND_MENUCLICK);
			}
			if (taps & CONTROL_RT)
			{
				NextCharacter();
				MakeNormalSound(SND_MENUCLICK);
			}
		}

		if(taps & CONTROL_B1)
		{
			MakeNormalSound(SND_MENUSELECT);
			SaveOptions();
			return 2;
		}

		// no credits while sitting here
		numRuns=0;

		oldc=c;

		c=mgl->LastKeyPressed();
		if(c==27 || (taps & CONTROL_B2))
		{
			MakeNormalSound(SND_MENUCANCEL);
			return 0;
		}

		*lastTime-=TIME_PER_FRAME;
		numRuns++;
	}
	return 1;
}

bool MenuItemKnown(MainMenuResult action)
{
	switch (action)
	{
		case MainMenuResult::Badges:
			return opt.modes[MODE_BADGES];
		case MainMenuResult::Bowling:
			return opt.modes[MODE_BOWLING];
		case MainMenuResult::Loonyball:
			return opt.modes[MODE_LOONYBALL];
		case MainMenuResult::BossBash:
			return opt.modes[MODE_BOSSBASH];
		case MainMenuResult::Survival:
			return opt.modes[MODE_SURVIVAL];
		case MainMenuResult::Remix:
			return opt.remixMode;
		default:
			return true;
	}
}

void SetMenu(span<const MenuItem> newMenu)
{
	menu.assign(newMenu.data(), newMenu.data() + newMenu.size());
	for (auto& item : menu)
	{
		item.known = MenuItemKnown(item.action);
		item.bright = -32;
	}
}

TASK(MainMenuResult) MainMenu(MGLDraw *mgl)
{
	MainMenuResult b = MainMenuResult::None;
	int i;
	int lastTime=1;

	if(songRestart)
	{
		JamulSoundPurge();
		LoopingSound(SND_HAMUMU);
	}

	mgl->LoadBMP("graphics/title.bmp");
	if(opt.cheats[CH_VINTAGE])
		GreyPalette(mgl);
	mgl->LastKeyPressed();
	oldc=255;//CONTROL_B1|CONTROL_B2;

	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],mgl->GetScreen()+mgl->GetWidth()*i,640);

	SetMenu(mainMenu);

	cursor=0;
	loadingGame=TitleSubmenu::MainMenu;
	GetTaps();
	while(b==MainMenuResult::None)
	{
		lastTime+=TimeLength();
		StartClock();

		if(loadingGame==TitleSubmenu::MainMenu)
		{
			b=MainMenuUpdate(&lastTime,mgl);
			MainMenuDisplay(mgl);
		}
		else if(loadingGame==TitleSubmenu::LoadGame)
		{
			byte b2=LoadGameUpdate(&lastTime,mgl);
			LoadGameDisplay(mgl);
			if(b2==0)
			{
				for (auto& item : menu)
					item.bright = -32;

				cursor=1;
				loadingGame=TitleSubmenu::MainMenu;
			}
			else if(b2==2)
			{
				free(backScr);
				CO_RETURN MainMenuResult::LoadGame;
			}
		}
		else if (loadingGame == TitleSubmenu::ChooseDiff)
		{
			byte b2=ChooseDiffUpdate(&lastTime,mgl);
			DiffChooseDisplay(mgl);
			if(b2==0)
			{
				cursor=0;
				loadingGame=TitleSubmenu::MainMenu;
			}
			else if(b2==2)
			{
				free(backScr);
				CO_RETURN choosingDiffFor;
			}
		}

		AWAIT mgl->Flip();
		if(!mgl->Process())
		{
			free(backScr);
			CO_RETURN MainMenuResult::Exit;
		}
		EndClock();
		if(loadingGame==TitleSubmenu::MainMenu && numRuns>=30*15)
		{
			GetTaps();
			oldc=255;
			AWAIT Credits(mgl,1);
			mgl->LoadBMP("graphics/title.bmp");
			numRuns=0;
		}
		else if(loadingGame==TitleSubmenu::MainMenu && b==MainMenuResult::LoadGame)
		{
			loadingGame=TitleSubmenu::LoadGame;
			GetSavesForMenu();
			cursor=0;
			b=MainMenuResult::None;
			oldc=255;
		}
		else if(loadingGame==TitleSubmenu::MainMenu && (b==MainMenuResult::NewGame || b == MainMenuResult::Remix))
		{
			loadingGame=TitleSubmenu::ChooseDiff;
			choosingDiffFor=b;
			cursor=0;
			b=MainMenuResult::None;
			oldc=255;
		}
		else if (b == MainMenuResult::Extras)
		{
			SetMenu(extrasMenu);
			b = MainMenuResult::None;
			cursor = 0;
			oldc = 255;
		}
		else if (b == MainMenuResult::BackToMain)
		{
			SetMenu(mainMenu);
			b = MainMenuResult::None;
			cursor = 0;
			oldc = 255;
		}
	}
	free(backScr);

	CO_RETURN b;
}

// true = continue, false = document is now offscreen
bool CreditsRender(int y, const char* const* document)
{
	int i,ypos;
	const char *s;
	char b;

	i=0;

	ypos=0;
	while(document[i][0]!='$')
	{
		s=document[i];
		if(ypos-y>-60)
		{
			if(ypos-y>240)
				b=-((ypos-y)-300)/10;
			else
				b=0;

			if(b>0)
				b=0;

			if(s[0]=='@')
			{
				CenterPrintColor(320,ypos-y,&s[1],4,b,2);
			}
			else if(s[0]=='#')
			{
				DrawFillBox(320-200,ypos-y+8,320+200,ypos-y+11,4*32+31+b);
			}
			else if(s[0]=='%')
			{
				DrawFillBox(320-70,ypos-y+8,320+70,ypos-y+9,4*32+31+b);
			}
			else if(s[0]=='&')
			{
				CenterPrintColor(320,ypos-y,&s[1],4,b,0);
			}
			else
				CenterPrintColor(320,ypos-y,s,4,b,1);
		}
		ypos+=20;
		i++;
		if(ypos-y>=480)
			return true;  // went off the bottom before document ended
	}

	// document ended, was it off the top?
	return ypos-y>-60;
}

static TASK(void) DoCredits(MGLDraw *mgl, const char* const* document)
{
	int y = -470;
	dword lastTime = 0;

	GetTaps();
	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");

	StartClock();
	while (true)
	{
		EndClock();
		lastTime += TimeLength();
		StartClock();

		lastTime = std::min(lastTime, TIME_PER_FRAME * 30u);
		while (lastTime >= TIME_PER_FRAME)
		{
			if (GetControls() & CONTROL_UP)
				y = std::max(-470, y - 3);
			else if (GetControls() & (CONTROL_DN | CONTROL_B1))
				y += 3;
			else
				y += 1;
			UpdatePlasma();
			lastTime -= TIME_PER_FRAME;
		}

		memset(mgl->GetScreen(), 4*32 + 2, mgl->GetWidth() * mgl->GetHeight());
		RenderPlasma2(mgl);
		bool stillGoing = CreditsRender(y, document);
		AWAIT mgl->Flip();

		if(!mgl->Process() || mgl->LastKeyPressed() == SDLK_ESCAPE || (GetTaps() & CONTROL_B2) || !stillGoing)
		{
			break;
		}
	}
	ExitPlasma();
}

TASK(void) Credits(MGLDraw *mgl, byte init)
{
	if(init)
		InitPlasma(4);

	int hangon=0;
	for(int i=0;i<40;i++)
		if(opt.meritBadge[i])
			hangon++;

	if(hangon==40)
		AWAIT CheatText(mgl,0);

	AWAIT DoCredits(mgl, credits);
}

TASK(void) CheatText(MGLDraw *mgl,byte init)
{
	if(init)
		InitPlasma(4);

	AWAIT DoCredits(mgl, cheatTxt);
}

TASK(void) VictoryText(MGLDraw *mgl)
{
	InitPlasma(4);
	AWAIT DoCredits(mgl, victoryTxt);
}

TASK(void) SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound)
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
		if(mgl->LastKeyPressed() || GetJoyButtons())
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
						//if(sound==2)
							//MakeNormalSound(SND_HAMUMU);
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
