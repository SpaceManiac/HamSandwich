#include "winpch.h"
#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"
#include <shellapi.h>
#include <crtdbg.h>

#include "game.h"
#include "editor.h"
#include "tile.h"
#include "sound.h"
#include "monster.h"
#include "title.h"
#include "options.h"
#include "badge.h"

bool windowedGame=FALSE;
MGLDraw *mainmgl;

void parseCmdLine(char *cmdLine)
{
	char *token;

	token=strtok(cmdLine," ");
	while(token!=NULL)
	{
		if(!strcmp(token,"window"))
			windowedGame=TRUE;
		token=strtok(NULL," ");
	}
}

int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR cmdLine,int nCmdShow)
{

#ifndef NDEBUG

int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag

flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit

_CrtSetDbgFlag(flag); // Set flag to the new value

#endif

	DBG("a");
	parseCmdLine(cmdLine);
	DBG("b");
	mainmgl=new MGLDraw("Loonyland",SCRWID,SCRHEI,8,windowedGame,hInstance);
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

	SplashScreen(mainmgl,"graphics\\hamumu.bmp",128,2);
	while(1)
	{
		DBG("Mainmenu");
		switch(MainMenu(mainmgl)-1)
		{
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
