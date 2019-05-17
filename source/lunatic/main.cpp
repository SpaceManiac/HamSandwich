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

// Appdata shenanigans
#ifdef _WIN32
#include <io.h>
#include <shellapi.h>
#include <shlobj.h> // for SHGetFolderPath
#ifdef _MSC_VER
#include <direct.h>
#endif

FILE* AppdataOpen(const char* file, const char* mode)
{
	char buffer[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, buffer);
	sprintf(buffer + strlen(buffer), "\\Hamumu");
	mkdir(buffer);
	sprintf(buffer + strlen(buffer), "\\DrLunatic");
	mkdir(buffer);
	sprintf(buffer + strlen(buffer), "\\%s", file);
	return fopen(buffer, mode);
}
#else
FILE* AppdataOpen(const char* file, const char* mode)
{
    return fopen(file, mode);
}
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
