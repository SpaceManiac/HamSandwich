/* DR. LUNATIC SUPREME WITH CHEESE

   A HamumuSoft Production.

   v 6.2

   Copyright 2003, Mike Hommel
*/

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
#include "profmenu.h"
#include "playlist.h"
#include "config.h"
#include "log.h"
#include "netmenu.h"
#include "internet.h"
#ifdef DEMO
#include "nag.h"
#endif

#ifdef _WIN32
#include <shellapi.h>
#endif

MGLDraw *mainmgl;

int main(int argc, char* argv[])
{
	bool windowedGame=false;

	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	LoadConfig();
	mainmgl=new MGLDraw("Sleepless Hollow", SCRWID, SCRHEI, windowedGame);
	if(!mainmgl)
		return 0;

	LunaticInit(mainmgl);

	SplashScreen(mainmgl,"graphics/hamumu.bmp",32,2);

	//NewComputerSpriteFix("graphics/items.jsp");
	//NewComputerSpriteFix("graphics/intface.jsp");
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
					ShellExecute(NULL,"open","http://hamumu.com/game.php?game=HOLLOW",NULL,NULL,SW_SHOWNORMAL);
				}
				else
#endif
				{
					LunaticExit();
					delete mainmgl;
				}
				return 0;
				break;
			case 0:	// play
				shopping=0;
#ifdef DEMO
				PlayWorld(mainmgl,"demo.shw");
#else
				PlayWorld(mainmgl,profile.lastWorld);
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
#ifdef _WIN32
					ShellExecuteA(NULL,"open","http://hamumu.com/scores.php",NULL,NULL,SW_SHOWNORMAL);
#endif
					return 0;
				}
				else if(DoWebPage()==2)
				{
					LunaticExit();
					delete mainmgl;
#ifdef _WIN32
					ShellExecuteA(NULL,"open","http://hamumu.com/addon.php",NULL,NULL,SW_SHOWNORMAL);
#endif
					return 0;
				}
#else
				LunaticExit();
				delete mainmgl;
				ExitLog();
#ifdef _WIN32
				ShellExecute(NULL,"open","http://hamumu.com/game.php?game=HOLLOW",NULL,NULL,SW_SHOWNORMAL);
#endif
				return 0;
#endif
				break;
			case 5:	// editor
				shopping=0;
				LunaticEditor(mainmgl);
				break;
		}
	}

	StopSong();
	LunaticExit();
	delete mainmgl;
	JamulSoundExit();
}
