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
#include "profmenu.h"
#include "giftshop.h"
#include "composer.h"
#include "empmonth.h"
#include "addon.h"
#include "internet.h"

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

	parseCmdLine(cmdLine);
	mainmgl=new MGLDraw("Stockboy",SCRWID,SCRHEI,8,windowedGame,hInstance);
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
