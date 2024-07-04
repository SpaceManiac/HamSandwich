#include "winpch.h"
#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"
#include "extern.h"

#include "game.h"
#include "editor.h"
#include "tile.h"
#include "sound.h"
#include "monster.h"
#include "title.h"
#include "options.h"
#include "badge.h"
#include "randomizer.h"
#include "log.h"
#include "appdata.h"
#include "steam.h"

extern const HamSandwichMetadata* GetHamSandwichMetadata();

TASK(int) main(int argc, char *argv[])
{
	bool windowedGame=false;
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	AppdataInit(GetHamSandwichMetadata());
	SteamManager::Init("2876900");

	MGLDraw *mainmgl=new MGLDraw("Loonyland", SCRWID, SCRHEI, windowedGame);
	if(!mainmgl)
		CO_RETURN 0;

	LunaticInit(mainmgl);

	LoopingSound(SND_HAMUMU);
	SetSongRestart(0);
	AWAIT SplashScreen(mainmgl,"graphics/hamumu.bmp",128,2);

	bool running = true;
	while (running)
	{
		DBG("Mainmenu");
		switch(AWAIT MainMenu(mainmgl))
		{
			default:
			case MainMenuResult::Exit:
				running = false;
				break;
			case MainMenuResult::NewGame:	// new game
				AWAIT LunaticGame(mainmgl,0,WORLD_NORMAL);
				SetSongRestart(1);
				break;
			case MainMenuResult::Remix:	// new game
				AWAIT LunaticGame(mainmgl,0,WORLD_REMIX);
				SetSongRestart(1);
				break;
			case MainMenuResult::LoadGame:	// continue
				AWAIT LunaticGame(mainmgl,WhichGameToLoad()+1,WORLD_NORMAL);
				SetSongRestart(1);
				break;
			case MainMenuResult::Editor:	// editor
				AWAIT LunaticEditor(mainmgl);
				SetSongRestart(1);
				break;
			case MainMenuResult::Survival:	// survival
				AWAIT LunaticGame(mainmgl,0,WORLD_SURVIVAL);
				SetSongRestart(1);
				break;
			case MainMenuResult::Loonyball:	// Loonyball
				AWAIT LunaticGame(mainmgl,0,WORLD_LOONYBALL);
				SetSongRestart(1);
				break;
			case MainMenuResult::Bowling:
				AWAIT LunaticGame(mainmgl,0,WORLD_BOWLING);
				SetSongRestart(1);
				break;
			case MainMenuResult::BossBash:	// boss bash
				AWAIT LunaticGame(mainmgl,0,WORLD_BOSSBASH);
				break;
			case MainMenuResult::HiScores:
				AWAIT HighScore(mainmgl);
				SetSongRestart(0);
				break;
			case MainMenuResult::Options:	// options
				AWAIT OptionsMenu(mainmgl);
				SetSongRestart(0);
				break;
			case MainMenuResult::Badges:	// badge menu
				AWAIT BadgeMenu(mainmgl);
				SetSongRestart(0);
				break;
			case MainMenuResult::Randomizer: //Randomizer settings
				AWAIT RandomizerMenu(mainmgl);
				SetSongRestart(0);
				break;

		}
	}

	LunaticExit();
	KillSong();
	delete mainmgl;
	SteamManager::Quit();
	CO_RETURN 0;
}
