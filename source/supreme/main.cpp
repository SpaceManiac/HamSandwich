/* DR. LUNATIC SUPREME WITH CHEESE

   A HamumuSoft Production.

   v 6.2

   Copyright 2003, Mike Hommel
*/

#include "winpch.h"
#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"
#include "extern.h"

#include "moron.h"
#include "game.h"
#include "editor.h"
#include "tile.h"
#include "sound.h"
#include "monster.h"
#include "title.h"
#include "profmenu.h"
#include "worldselect.h"
#include "playlist.h"
#include "hiscore.h"
#include "arcade.h"
#include "moron.h"
#include "config.h"
#include "log.h"
#include "netmenu.h"
#include "internet.h"
#include "appdata.h"
#include "steam.h"

#ifdef _WIN32
#include <shellapi.h>
#endif

extern const HamSandwichMetadata* GetHamSandwichMetadata();

TASK(int) main(int argc, char* argv[])
{
	g_HamExtern.ChooseNextSong = ChooseNextSong;
	g_HamExtern.SoundLoadOverride = SoundLoadOverride;

	bool windowedGame=false;

	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	AppdataInit(GetHamSandwichMetadata());
	LoadConfig();
	SetHamMusicEnabled(config.music);
	SetJamulSoundEnabled(config.sound, config.numSounds);
	SteamManager::Init();
	MGLDraw *mainmgl=new MGLDraw("Supreme With Cheese", SCRWID, SCRHEI, windowedGame);
	if(!mainmgl)
		CO_RETURN 0;

	LunaticInit(mainmgl);

	//CryptoTest();
#ifdef ARCADETOWN
	AWAIT SplashScreen(mainmgl,"graphics/at_presents.bmp",32,0);
#endif
	AWAIT SplashScreen(mainmgl,"graphics/hamumu.bmp",32,2);

	//NewComputerSpriteFix("graphics/items.jsp");
	//NewComputerSpriteFix("graphics/intface.jsp");
	shopping=0;

	bool running = true;
	while(running)
	{
		switch(AWAIT MainMenu(mainmgl))
		{
			case 255:	// quit
				running = false;
				break;
			case 0:	// new game
				shopping=0;
				AWAIT WorldSelectMenu(mainmgl);
				break;
			case 1:	// continue
				shopping=0;
				AWAIT ProfMenu(mainmgl);
				break;
			case 2:	// tutorial
				shopping=0;
				if(AWAIT PlayWorld(mainmgl,"tutorial.dlw"))
					AWAIT WorldSelectMenu(mainmgl);
				break;
			case 3:	// instructions
				AWAIT HelpScreens(mainmgl);
				break;
			case 6:	// shopping
				shopping=1;
				ResetMoron();
				AWAIT PlayWorld(mainmgl,"spismall.zlw");
				break;
			case 5:	// internet
				shopping=0;
				AWAIT NetMenu(mainmgl);

				if(DoWebPage()==1)
				{
#ifdef _WIN32
					ShellExecuteA(NULL,"open","http://hamumu.com/scores.php",NULL,NULL,SW_SHOWNORMAL);
#endif
					running = false;
				}
				else if(DoWebPage()==2)
				{

#ifdef _WIN32
					ShellExecuteA(NULL,"open","http://hamumu.com/addon.php",NULL,NULL,SW_SHOWNORMAL);
#endif
					running = false;
				}
				break;
			case 7:	// editor
				shopping=0;
				AWAIT LunaticEditor(mainmgl);
				break;
			case 8:	// insta-play the last world you were on
				shopping=0;
				if(profile.lastWorld[0]!='\0')
				{
					if(AWAIT PlayWorld(mainmgl,profile.lastWorld))
						AWAIT WorldSelectMenu(mainmgl);
				}
				break;
		}
	}

	StopSong();
	LunaticExit();
	delete mainmgl;
	SteamManager::Quit();
	JamulSoundExit();
	return 0;
}
