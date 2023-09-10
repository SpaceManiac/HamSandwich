#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"
#include "control.h"
#include "extern.h"
#include "appdata.h"

MGLDraw *mainmgl;

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

	mainmgl = new MGLDraw("Amazin' SPISPOPD", 640, 480, windowedGame);
	if (!mainmgl)
		CO_RETURN 1;

	/*
	DisplayInit();
	MapInitGfx();
	LoadConfig();
	GuysInitGfx();
	SplashInit(0);
	StartClockAndLoopMain();
	GuysExitGfx();
	SaveConfig();
	MapExitGfx();
	UnloadFonts();
	MglDestroy();
	if (g_QuitAction == 1) {
		ShellExecuteA((HWND)0x0,&s_open_1,s_http://www.hamumu.com_00477eec,&DAT_0048db60,&DAT_0048db5c,1);
	}
	else if (g_QuitAction == 2) {
		ShellExecuteA((HWND)0x0,&s_open_2,s_http://www.hamumu.com/game_amazi_00477f0c,&DAT_0048db68,&DAT_0048db64,1);
	}
	else if (g_QuitAction == 3) {
		ShellExecuteA((HWND)0x0,&s_open_3,s_register.txt_00477f3c,&DAT_0048db70,&DAT_0048db6c,1);
	}
	*/

	CO_RETURN 0;
}
