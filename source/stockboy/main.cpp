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
#include "profmenu.h"
#include "giftshop.h"
#include "composer.h"
#include "empmonth.h"
#include "addon.h"
#include "internet.h"
#include "extern.h"
#include "appdata.h"

#ifdef _WIN32
#include <crtdbg.h>
#endif

MGLDraw *mainmgl;

void ChooseNextSong() {}
bool ConfigMusicEnabled() { return true; }
bool ConfigSoundEnabled() { return true; }
int ConfigNumSounds() { return 16; }
SDL_RWops* SoundLoadOverride(int) { return nullptr; }
void SetGameIdle(bool) {}

TASK(int) main(int argc, char* argv[])
{
#if defined(WIN32) && !defined(NDEBUG)
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag
	flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
	_CrtSetDbgFlag(flag); // Set flag to the new value
#endif

	HAM_EXTERN_FULFILL
	AppdataInit();

	bool windowedGame=false;
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	mainmgl=new MGLDraw("Stockboy",SCRWID,SCRHEI,windowedGame);
	if(!mainmgl)
		return 0;

	LunaticInit(mainmgl);

	SplashScreen(mainmgl,"graphics\\hamumu.bmp",128,2);

	/*
	if(Web_Init()==IE_OK)
	{
		int i;
		Web_RequestData("www.hamumu.com","/goods/stock_addon.txt",&i);
		Web_Exit();
	}
	*/

	while(1)
	{
		switch(MainMenu(0,mainmgl)-1)
		{
			case 255:	// quit
			case MENU_EXIT:
				LunaticExit();
				delete mainmgl;
				return 0;
				break;
			case MENU_STOCKROOM:	// stockroom mode
				LunaticGame(mainmgl,0,GAME_STOCKROOM);
				break;
			case MENU_PARALLEL:	// parallel universe
				LunaticGame(mainmgl,0,GAME_PARALLEL);
				break;
			case MENU_PESTCONTROL:	// pest control
				LunaticGame(mainmgl,0,GAME_PESTCONTROL);
				break;
			case MENU_TRAINING:	// training
				LunaticGame(mainmgl,0,GAME_TRAINING);
				break;
			case MENU_BLOWOUT:	// blowout
				LunaticGame(mainmgl,0,GAME_BLOWOUT);
				break;
			case MENU_CLEARANCE:	// clearance
				LunaticGame(mainmgl,0,GAME_CLEARANCE);
				break;
			case MENU_EDITOR:	// editor
				LunaticEditor(mainmgl);
				break;
			case MENU_PROFILE:	// hi scores
				ProfileMenu(mainmgl);
				break;
			case MENU_GIFTSHOP:	// gift shop
				GiftShop(mainmgl);
				break;
			case MENU_COMPOSER:	// options
				Composer(mainmgl);
				break;
			case MENU_ADDON:	// Add-On levels
				LunaticGame(mainmgl,0,GAME_ADDON);
				break;
		}
	}
}
