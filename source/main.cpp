/* DR. LUNATIC SUPREME WITH CHEESE

   A HamumuSoft Production.

   v 6.2

   Copyright 2003, Mike Hommel
*/

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
#include "profmenu.h"
#include "playlist.h"
#include "config.h"
#include "log.h"
#include "netmenu.h"
#include "internet.h"
#ifdef DEMO
#include "nag.h"
#endif

bool windowedGame=FALSE;
MGLDraw *mainmgl;

void ParseCmdLine(char *cmdLine)
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

	ParseCmdLine(cmdLine);

	InitLog();

	LoadConfig();
	mainmgl=new MGLDraw(hInstance,"Sleepless Hollow",640,480,8,windowedGame);
	if(!mainmgl)
		return 0;

	LunaticInit(mainmgl);

	SplashScreen(mainmgl,"graphics\\hamumu.bmp",32,2);

	//NewComputerSpriteFix("graphics\\items.jsp");
	//NewComputerSpriteFix("graphics\\intface.jsp");
	shopping=0;
	while(1)
	{
		switch(MainMenu(mainmgl))
		{
			case 3:
			case 255:	// quit
#ifdef DEMO
				if(Nag(mainmgl,1))
				{
					LunaticExit();
					delete mainmgl;
					ExitLog();
					FatalErrorQuit();
					ShellExecute(NULL,"open","http://hamumu.com/game.php?game=HOLLOW",NULL,NULL,SW_SHOWNORMAL);
				}
				else
#endif
				{
					LunaticExit();
					delete mainmgl;
					ExitLog();
					FatalErrorQuit();
				}
				return 0;
				break;
			case 0:	// play
				shopping=0;
#ifdef DEMO
				PlayWorld(mainmgl,"demo.shw");
#else
				PlayWorld(mainmgl,"hollow.shw");
#endif
				break;
			case 1:	// profile
				ProfMenu(mainmgl);
				break;
			case 2:	// internet
#ifndef DEMO
				shopping=0;
				NetMenu(mainmgl);

				if(DoWebPage()==1)
				{
					LunaticExit();
					delete mainmgl;
					ExitLog();
					FatalErrorQuit();
					ShellExecute(NULL,"open","http://hamumu.com/scores.php",NULL,NULL,SW_SHOWNORMAL);
					return 0;
				}
				else if(DoWebPage()==2)
				{
					LunaticExit();
					delete mainmgl;
					ExitLog();
					FatalErrorQuit();

					ShellExecute(NULL,"open","http://hamumu.com/addon.php",NULL,NULL,SW_SHOWNORMAL);
					return 0;
				}
#else
				LunaticExit();
				delete mainmgl;
				ExitLog();
				FatalErrorQuit();

				ShellExecute(NULL,"open","http://hamumu.com/game.php?game=HOLLOW",NULL,NULL,SW_SHOWNORMAL);
				return 0;
#endif
				break;
			case 5:	// editor
#ifdef _DEBUG
				shopping=0;
				LunaticEditor(mainmgl);
#endif
				break;
		}
	}
}
