#ifndef TITLE_H
#define TITLE_H

#include "winpch.h"
#include "display.h"

enum class MainMenuResult
{
	None,
	NewGame,
	LoadGame,
	Bowling,
	Survival,
	BossBash,
	Loonyball,
	Remix,
	Badges,
	HiScores,
	Options,
	Editor,
	Randomizer,
	Exit,
	Extras,
	BackToMain,
};

TASK(MainMenuResult) MainMenu(MGLDraw *mgl);
TASK(void) Credits(MGLDraw *mgl,byte init);
TASK(void) CheatText(MGLDraw *mgl,byte init);
TASK(void) SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound);
TASK(void) VictoryText(MGLDraw *mgl);

byte WhichGameToLoad(void);
void SetSongRestart(byte b);

#endif
