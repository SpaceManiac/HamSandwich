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
	AWAIT ShowVictoryAnim(11);

	while(1)
	{
		switch(AWAIT MainMenu(mainmgl))
		{
			case 255:	// quit
				mainmgl->ClearScreen();
				AWAIT mainmgl->Flip();
				mainmgl->ClearScreen();
				ExitIcons();
				LunaticExit();
				delete mainmgl;
				SteamManager::Quit();
				CO_RETURN 0;
				break;
			case 0:	// new game
				AWAIT LunaticGame(mainmgl,0);
				break;
			case 1:	// continue
				AWAIT LunaticGame(mainmgl,1);
				break;
			case 2:	// challenge
				AWAIT ChallengeMenu(mainmgl);
				break;
			case 3:	// options
				AWAIT OptionsMenu(mainmgl);
				break;
			case 5:
				AWAIT AchieveMenu(mainmgl);
				break;
			case 6:	// editor
				AWAIT LunaticEditor(mainmgl);
				break;
		}
	}
}
