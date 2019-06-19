/* Kid Mystic

   A HamumuSoft Production.

   Copyright 1999, Mike Hommel
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
#include "challenge.h"
#include "options.h"
#include "nag.h"

const char* AppdataFolderName()
{
	return "KidMystic";
}

TASK(void) main_task(MGLDraw *mainmgl);

int main(int argc, char* argv[])
{
#ifdef DEMO
	byte n;
#endif
	bool windowedGame=false;
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	InitOptions();
	MGLDraw *mainmgl=new MGLDraw("Kid Mystic", SCRWID, SCRHEI, windowedGame);
	if(!mainmgl)
		return 0;

	LunaticInit(mainmgl);
	coro::launch(std::bind(main_task, mainmgl));
	return 0;
}

TASK(void) main_task(MGLDraw *mainmgl)
{
	AWAIT SplashScreen(mainmgl,"graphics/intro1.bmp",128,SND_INTRO1,0);

#ifdef DEMO
#elif VALUE
#else
	AWAIT ShowVictoryAnim(11);
#endif

	//NewComputerSpriteFix("graphics\\items.jsp");
	while(1)
	{
		switch(AWAIT MainMenu(mainmgl))
		{
			case 255:	// quit
#ifdef DEMO
				n=Nag(mainmgl);
#endif
				mainmgl->ClearScreen();
				AWAIT mainmgl->Flip();
				mainmgl->ClearScreen();
				LunaticExit();
				delete mainmgl;
#ifdef DEMO
				if(n==0)	// chose to buy
					ShellExecute(NULL,"open","http://hamumu.com/store.php?game=MYSTIC&src=demoexit","","",SW_SHOWNORMAL);
#endif
				CO_RETURN;
				break;
			case 0:	// new game
#ifdef DEMO
				if(LunaticGame(mainmgl,0))
				{
					n=Nag(mainmgl);
					if(n==0)	// chose to buy
					{
						mainmgl->ClearScreen();
						mainmgl->Flip();
						mainmgl->ClearScreen();
						LunaticExit();
						delete mainmgl;
						ShellExecute(NULL,"open","http://hamumu.com/store.php?game=MYSTIC&src=demowin","","",SW_SHOWNORMAL);
						CO_RETURN;
					}
				}
#else
				AWAIT LunaticGame(mainmgl,0);
#endif
				break;
			case 1:	// continue
#ifdef DEMO
				if(LunaticGame(mainmgl,1))
				{
					n=Nag(mainmgl);
					if(n==0)	// chose to buy
					{
						mainmgl->ClearScreen();
						mainmgl->Flip();
						mainmgl->ClearScreen();
						LunaticExit();
						delete mainmgl;
						ShellExecute(NULL,"open","http://hamumu.com/store.php?game=MYSTIC&src=demowin","","",SW_SHOWNORMAL);
						CO_RETURN;
					}
				}
#else
				AWAIT LunaticGame(mainmgl,1);
#endif
				break;
			case 2:	// challenge
#ifndef DEMO
				AWAIT ChallengeMenu(mainmgl);
#endif
				break;
			case 3:	// options
				AWAIT OptionsMenu(mainmgl);
				break;
			case 5:	// editor
#ifndef DEMO
				AWAIT LunaticEditor(mainmgl);
#endif
				break;
		}
	}
}
