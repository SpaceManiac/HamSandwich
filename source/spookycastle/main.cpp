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

void ChooseNextSong() {}
bool ConfigMusicEnabled() { return true; }
bool ConfigSoundEnabled() { return true; }
int ConfigNumSounds() { return 16; }
SDL_RWops* SoundLoadOverride(int) { return nullptr; }
void SoundSystemExists() {}
void SetGameIdle(bool) {}

TASK(int) main(int argc, char* argv[])
{
	HAM_EXTERN_FULFILL
	AppdataInit();

	bool windowedGame=false;

	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	mainmgl=new MGLDraw("Spooky Castle",640,480,windowedGame);
	if(!mainmgl)
		return 0;

	LunaticInit(mainmgl);
	SplashScreen(mainmgl,"graphics\\hamumu.bmp",128,SND_HAMUMU,0);
	while(1)
	{
		switch(MainMenu(mainmgl))
		{
			case 255:	// quit
				mainmgl->ClearScreen();
				mainmgl->Flip();
				mainmgl->ClearScreen();
				LunaticExit();
				delete mainmgl;
				return 0;
				break;
			case 0:	// new game
				LunaticGame(mainmgl,0);
				break;
			case 1:	// continue
				LunaticGame(mainmgl,1);
				break;
			case 3:	// editor
				LunaticEditor(mainmgl);
				break;
		}
	}
}
