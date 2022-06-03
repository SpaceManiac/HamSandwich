/* Eddie Galaxy: Intergalactic Exterminator

   A HamumuSoft Production.

   v 1.2

   Copyright 1998, Mike Hommel
*/

#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"
#include "game.h"
#include "extern.h"
#include "appdata.h"

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

	byte buy;

	mainmgl=new MGLDraw("Eddie Galaxy",640,480,windowedGame);
	if(!mainmgl)
		CO_RETURN 0;

	CentipedeInit(mainmgl);
	buy=AWAIT CentipedeGame();
	CentipedeExit();
	delete mainmgl;
#ifdef DEMO
	if(buy)
		ShellExecute(NULL,"open","http://hamumu.com/store.php?game=EDDIE&src=demoexit",NULL,NULL,SW_SHOWNORMAL);
#endif
	CO_RETURN 0;
}
