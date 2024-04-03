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

extern const HamSandwichMetadata* GetHamSandwichMetadata();

TASK(int) main(int argc, char* argv[])
{
#if defined(WIN32) && !defined(NDEBUG)
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag
	flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
	_CrtSetDbgFlag(flag); // Set flag to the new value
#endif

	AppdataInit(GetHamSandwichMetadata());
	SetJamulSoundEnabled(true, 16);

	bool windowedGame=false;
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	mainmgl=new MGLDraw("Stockboy",SCRWID,SCRHEI,windowedGame);
	if(!mainmgl)
		CO_RETURN 0;

	LunaticInit(mainmgl);

	AWAIT SplashScreen(mainmgl,"graphics/hamumu.bmp",128,2);

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
		switch((AWAIT MainMenu(0,mainmgl))-1)
		{
			case 255:	// quit
			case MENU_EXIT:
				LunaticExit();
				delete mainmgl;
				CO_RETURN 0;
				break;
			case MENU_STOCKROOM:	// stockroom mode
				AWAIT LunaticGame(mainmgl,0,GAME_STOCKROOM);
				break;
			case MENU_PARALLEL:	// parallel universe
				AWAIT LunaticGame(mainmgl,0,GAME_PARALLEL);
				break;
			case MENU_PESTCONTROL:	// pest control
				AWAIT LunaticGame(mainmgl,0,GAME_PESTCONTROL);
				break;
			case MENU_TRAINING:	// training
				AWAIT LunaticGame(mainmgl,0,GAME_TRAINING);
				break;
			case MENU_BLOWOUT:	// blowout
				AWAIT LunaticGame(mainmgl,0,GAME_BLOWOUT);
				break;
			case MENU_CLEARANCE:	// clearance
				AWAIT LunaticGame(mainmgl,0,GAME_CLEARANCE);
				break;
			case MENU_EDITOR:	// editor
				AWAIT LunaticEditor(mainmgl);
				break;
			case MENU_PROFILE:	// hi scores
				AWAIT ProfileMenu(mainmgl);
				break;
			case MENU_GIFTSHOP:	// gift shop
				AWAIT GiftShop(mainmgl);
				break;
			case MENU_COMPOSER:	// options
				AWAIT Composer(mainmgl);
				break;
			case MENU_ADDON:	// Add-On levels
				AWAIT LunaticGame(mainmgl,0,GAME_ADDON);
				break;
		}
	}
}
