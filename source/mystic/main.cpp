/* Kid Mystic

   A HamumuSoft Production.

   Copyright 1999, Mike Hommel
*/

#include "winpch.h"
#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"
#include "extern.h"

#include "game.h"
#include "editor.h"
#include "tile.h"
#include "sound.h"
#include "monster.h"
#include "title.h"
#include "challenge.h"
#include "options.h"
#include "nag.h"
#include "appdata.h"
#include "achieves.h"
#include "skills.h"
#include "steam.h"

extern const HamSandwichMetadata* GetHamSandwichMetadata();

TASK(int) main(int argc, char* argv[])
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

	AppdataInit(GetHamSandwichMetadata());
	SteamManager::Init("3531250");
	InitOptions();
	MGLDraw *mainmgl=new MGLDraw("Kid Mystic", SCRWID, SCRHEI, windowedGame);
	if(!mainmgl)
		CO_RETURN 0;

	LunaticInit(mainmgl);
	InitIcons();
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
				ExitIcons();
				LunaticExit();
				delete mainmgl;
				SteamManager::Quit();
#ifdef DEMO
				if(n==0)	// chose to buy
					ShellExecute(NULL,"open","http://hamumu.com/store.php?game=MYSTIC&src=demoexit","","",SW_SHOWNORMAL);
#endif
				CO_RETURN 0;
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
						ExitIcons();
						LunaticExit();
						delete mainmgl;
						ShellExecute(NULL,"open","http://hamumu.com/store.php?game=MYSTIC&src=demowin","","",SW_SHOWNORMAL);
						CO_RETURN 0;
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
						ExitIcons();
						LunaticExit();
						delete mainmgl;
						ShellExecute(NULL,"open","http://hamumu.com/store.php?game=MYSTIC&src=demowin","","",SW_SHOWNORMAL);
						CO_RETURN 0;
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
			case 5:
				AWAIT AchieveMenu(mainmgl);
				break;
			case 6:	// editor
#ifndef DEMO
				AWAIT LunaticEditor(mainmgl);
#endif
				break;
		}
	}
}
