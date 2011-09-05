/* DR. LUNATIC (working title)

   A HamumuSoft Production.

   v 0.04

   Copyright 1998, Mike Hommel
*/

#include "winpch.h"
#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"
#include <shellapi.h>
//#include <crtdbg.h>

#include "game.h"
#include "editor.h"
#include "tile.h"
#include "sound.h"
#include "monster.h"
#include "title.h"

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
/*#ifndef NDEBUG

int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag

flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit

_CrtSetDbgFlag(flag); // Set flag to the new value

#endif*/

	parseCmdLine(cmdLine);
	mainmgl=new MGLDraw("Dr. Lunatic",640,480,8,windowedGame,hInstance);
	if(!mainmgl)
		return 0;

	LunaticInit(mainmgl);
	SplashScreen(mainmgl,"graphics\\hamumu.bmp",128,2);

	while(1)
	{
		switch(MainMenu(mainmgl))
		{
			case 255:	// quit
#ifdef DEMO
				DemoSplashScreens(mainmgl);
#endif
				LunaticExit();
				delete mainmgl;
				return 0;
				break;
			case 0:	// new game
				LunaticGame(mainmgl,0);
				break;
			case 1:	// continue
				LunaticGame(mainmgl,1);
				break;
			case 3:	// editor
				LunaticEditor(mainmgl);
				break;
			case 4:	// ordering
				LunaticExit();
				delete mainmgl;
				ShellExecute(NULL,"open","docs\\order.html","","",SW_SHOWNORMAL);
				return 0;
				break;
		}
	}
}
