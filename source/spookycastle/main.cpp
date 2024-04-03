/* DR. LUNATIC (working title)

   A HamumuSoft Production.

   v 0.04

   Copyright 1998, Mike Hommel
*/

#include "winpch.h"
#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"
#include "appdata.h"
#include "extern.h"

#include "game.h"
#include "editor.h"
#include "tile.h"
#include "sound.h"
#include "monster.h"
#include "title.h"

MGLDraw *mainmgl;

extern const HamSandwichMetadata* GetHamSandwichMetadata();

TASK(int) main(int argc, char* argv[])
{
	AppdataInit(GetHamSandwichMetadata());
	SetJamulSoundEnabled(true, 16);

	byte kb0[6] = { SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_LCTRL, SDL_SCANCODE_LSHIFT };
	byte kb1[6] = { SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_2, SDL_SCANCODE_KP_4, SDL_SCANCODE_KP_6, SDL_SCANCODE_Z, SDL_SCANCODE_X };
	byte kb2[6] = { 0, 0, 0, 0, SDL_SCANCODE_RETURN, 0 };
	byte joyBinds[2] = { 0, 1 };
	SetKeyboardBindings(0, 6, kb0);
	SetKeyboardBindings(1, 6, kb1);
	SetKeyboardBindings(2, 6, kb2);
	SetJoystickBindings(2, joyBinds);

	bool windowedGame=false;

	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	mainmgl=new MGLDraw("Spooky Castle",640,480,windowedGame);
	if(!mainmgl)
		CO_RETURN 0;

	LunaticInit(mainmgl);
	AWAIT SplashScreen(mainmgl,"graphics/hamumu.bmp",128,SND_HAMUMU,0);
	while(1)
	{
		switch(AWAIT MainMenu(mainmgl))
		{
			case 255:	// quit
				mainmgl->ClearScreen();
				AWAIT mainmgl->Flip();
				mainmgl->ClearScreen();
				LunaticExit();
				delete mainmgl;
				CO_RETURN 0;
				break;
			case 0:	// new game
				AWAIT LunaticGame(mainmgl,0);
				break;
			case 1:	// continue
				AWAIT LunaticGame(mainmgl,1);
				break;
			case 3:	// editor
				AWAIT LunaticEditor(mainmgl);
				break;
		}
	}
}
