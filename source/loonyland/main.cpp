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

int main(int argc, char *argv[])
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
		return 0;
	DBG("d");
	DBG("Init!");
	LunaticInit(mainmgl);

	DBG("Songstart");
	LoopingSound(SND_HAMUMU);
	SetSongRestart(0);
	DBG("Splash");

	SplashScreen(mainmgl,"graphics/hamumu.bmp",128,2);
	while(1)
	{
		DBG("Mainmenu");
		switch(MainMenu(mainmgl)-1)
		{
			default:
			case 255:	// quit
			case MENU_EXIT:
				LunaticExit();
				delete mainmgl;
				return 0;
				break;
			case MENU_ADVENTURE:	// new game
				LunaticGame(mainmgl,0,WORLD_NORMAL);
				SetSongRestart(1);
				break;
			case MENU_REMIX:	// new game
				LunaticGame(mainmgl,0,WORLD_REMIX);
				SetSongRestart(1);
				break;
			case MENU_LOADGAME:	// continue
				LunaticGame(mainmgl,WhichGameToLoad()+1,WORLD_NORMAL);
				SetSongRestart(1);
				break;
			case MENU_EDITOR:	// editor
				LunaticEditor(mainmgl);
				SetSongRestart(1);
				break;
			case MENU_SURVIVAL:	// survival
				LunaticGame(mainmgl,0,WORLD_SURVIVAL);
				SetSongRestart(1);
				break;
			case MENU_LOONYBALL:	// Loonyball
				LunaticGame(mainmgl,0,WORLD_LOONYBALL);
				SetSongRestart(1);
				break;
			case MENU_BOWLING:
				LunaticGame(mainmgl,0,WORLD_BOWLING);
				SetSongRestart(1);
				break;
			case MENU_BOSSATTACK:	// boss bash
				LunaticGame(mainmgl,0,WORLD_BOSSBASH);
				break;
			case MENU_HISCORE:
				HighScore(mainmgl);
				SetSongRestart(0);
				break;
			case MENU_OPTIONS:	// options
				OptionsMenu(mainmgl);
				SetSongRestart(0);
				break;
			case MENU_BADGES:	// badge menu
				BadgeMenu(mainmgl);
				SetSongRestart(0);
				break;

		}
	}
}
