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

	bool windowedGame=false;

	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	byte buy;

	mainmgl=new MGLDraw("Eddie Galaxy",640,480,windowedGame);
	if(!mainmgl)
		return 0;

	CentipedeInit(mainmgl);
	buy=CentipedeGame();
	CentipedeExit();
	delete mainmgl;
#ifdef DEMO
	if(buy)
		ShellExecute(NULL,"open","http://hamumu.com/store.php?game=EDDIE&src=demoexit",NULL,NULL,SW_SHOWNORMAL);
#endif
	return 0;
}
