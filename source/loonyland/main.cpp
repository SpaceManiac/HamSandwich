#include "winpch.h"
#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"

#include "game.h"
#include "editor.h"
#include "tile.h"
#include "sound.h"
#include "monster.h"
#include "title.h"
#include "options.h"
#include "badge.h"
#include "log.h"

const char* AppdataFolderName()
{
	return "Loonyland";
}

TASK(int) main(int argc, char *argv[])
{
	DBG("a");

	bool windowedGame=false;
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	DBG("b");
	MGLDraw *mainmgl=new MGLDraw("Loonyland", SCRWID, SCRHEI, windowedGame);
	DBG("c");
	if(!mainmgl)
		CO_RETURN 0;
	DBG("d");
	DBG("Init!");
	LunaticInit(mainmgl);

	DBG("Songstart");
	LoopingSound(SND_HAMUMU);
	SetSongRestart(0);
	DBG("Splash");
	AWAIT SplashScreen(mainmgl,"graphics/hamumu.bmp",128,2);

	while(1)
	{
		DBG("Mainmenu");
		switch((AWAIT MainMenu(mainmgl)) - 1)
		{
			default:
			case 255:	// quit
			case MENU_EXIT:
				LunaticExit();
				delete mainmgl;
				CO_RETURN 0;
				break;
			case MENU_ADVENTURE:	// new game
				AWAIT LunaticGame(mainmgl,0,WORLD_NORMAL);
				SetSongRestart(1);
				break;
			case MENU_REMIX:	// new game
				AWAIT LunaticGame(mainmgl,0,WORLD_REMIX);
				SetSongRestart(1);
				break;
			case MENU_LOADGAME:	// continue
				AWAIT LunaticGame(mainmgl,WhichGameToLoad()+1,WORLD_NORMAL);
				SetSongRestart(1);
				break;
			case MENU_EDITOR:	// editor
				AWAIT LunaticEditor(mainmgl);
				SetSongRestart(1);
				break;
			case MENU_SURVIVAL:	// survival
				AWAIT LunaticGame(mainmgl,0,WORLD_SURVIVAL);
				SetSongRestart(1);
				break;
			case MENU_LOONYBALL:	// Loonyball
				AWAIT LunaticGame(mainmgl,0,WORLD_LOONYBALL);
				SetSongRestart(1);
				break;
			case MENU_BOWLING:
				AWAIT LunaticGame(mainmgl,0,WORLD_BOWLING);
				SetSongRestart(1);
				break;
			case MENU_BOSSATTACK:	// boss bash
				AWAIT LunaticGame(mainmgl,0,WORLD_BOSSBASH);
				break;
			case MENU_HISCORE:
				AWAIT HighScore(mainmgl);
				SetSongRestart(0);
				break;
			case MENU_OPTIONS:	// options
				AWAIT OptionsMenu(mainmgl);
				SetSongRestart(0);
				break;
			case MENU_BADGES:	// badge menu
				AWAIT BadgeMenu(mainmgl);
				SetSongRestart(0);
				break;

		}
	}
}
