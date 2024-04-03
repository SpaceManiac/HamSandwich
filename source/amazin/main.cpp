#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"
#include "control.h"
#include "extern.h"
#include "appdata.h"
#include "display.h"
#include "map.h"
#include "options.h"
#include "guy.h"
#include "title.h"
#include "loop.h"
#include "openurl.h"

MGLDraw *mgl;

extern const HamSandwichMetadata* GetHamSandwichMetadata();

TASK(int) main(int argc, char* argv[])
{
	// HamSandwich stuff
	AppdataInit(GetHamSandwichMetadata());
	SetJamulSoundEnabled(true, 16);

	byte joyBinds[2] = { 0, 1 };
	SetJoystickBindings(2, joyBinds);

	// ParseCommandLine
	bool windowedGame = false;
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	mgl = new MGLDraw("Amazin' SPISPOPD", 640, 480, windowedGame);
	if (!mgl)
		CO_RETURN 1;

	DisplayInit();
	MapInit();
	LoadConfig();
	GuysInitGfx();
	SplashInit(0);
	AWAIT LoopMain();
	GuysExitGfx();
	SaveConfig();
	MapExit();
	DisplayExit();
	delete mgl;
	if (g_QuitAction == QuitAction::HamumuWebsite)
	{
		SDL_OpenURL("https://hamumu.com");  // was "http://www.hamumu.com"
	}
	else if (g_QuitAction == QuitAction::BuyOnline)
	{
		SDL_OpenURL("https://hamumu.itch.io/amazin-spispopd");  // was "http://www.hamumu.com/game_amazin.html"
	}
	else if (g_QuitAction == QuitAction::RegisterTxt)
	{
		// SDL_OpenURL("register.txt");
	}
	CO_RETURN 0;
}
