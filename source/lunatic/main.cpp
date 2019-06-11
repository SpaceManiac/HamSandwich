/* DR. LUNATIC (working title)

   A HamumuSoft Production.

   v 0.04

   Copyright 1998, Mike Hommel
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
#include "options.h"

#ifdef _WIN32
#include <shellapi.h>
#endif

int main(int argc, char* argv[])
{
	bool windowedGame=false;

	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	LoadOptions();
	MGLDraw *mainmgl = new MGLDraw("Dr. Lunatic", 640, 480, windowedGame);
	if (!mainmgl)
		return 0;

	LunaticInit(mainmgl);
	SplashScreen(mainmgl, "graphics/hamumu.bmp", 128, 2);
	ChooseNextSong();

	while (1)
	{
		switch (MainMenu(mainmgl)) {
			case 255: // quit
				LunaticExit();
				delete mainmgl;
				return 0;
				break;
			case 0: // new game
				LunaticGame(mainmgl, 0);
				break;
			case 1: // continue
				LunaticGame(mainmgl, 1);
				break;
			case 3: // editor
				LunaticEditor(mainmgl);
				break;
			case 4: // ordering
				LunaticExit();
				delete mainmgl;
#ifdef _WIN32
				ShellExecuteA(NULL, "open", "docs\\order.html", "", "", SW_SHOWNORMAL);
#endif
				return 0;
				break;
		}
	}
}
