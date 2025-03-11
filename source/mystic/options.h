#ifndef OPTIONS_H
#define OPTIONS_H

#include "mgldraw.h"
#include "achieves.h"
#include "control.h"

#define OPT_VERSION (1)
#define OPT_EXPANSION_SIZE (243)

#define OPT_SOUNDVOL (0)
#define OPT_MUSICVOL (1)
#define OPT_FANCYWATER (2)
#define OPT_FANCYLIGHTING (3)
#define OPT_DPADTOMOVE (4)
#define OPT_CONFIG (5)
#define OPT_CONFIGCAST (6)
#define OPT_EXIT (7)

#define OPTMODE_IDLE (0)
#define OPTMODE_KEYCONFIG (1)
#define OPTMODE_KEYBIND (2)	// actually entering a button/key on the config
#define OPTMODE_GAMEPADBIND (3) // same but gamepad
#define OPTMODE_QC_CONFIG (4)	// choosing a quickcast key to config
#define OPTMODE_QC_KEYBIND (5)
#define OPTMODE_QC_GAMEPADBIND (6)

typedef struct option_t
{

	byte challenge;
	byte key[NUM_CONTROLS][3];
	byte joyCtrl[NUM_CONTROLS];
	byte soundVol,musicVol;
	byte waterFX,lightFX;
	byte dpadToMove;
	byte expansionSpace[OPT_EXPANSION_SIZE];
	byte achieve[(int)Achievement::NUM_ACHIEVES];
	
} option_t;

extern option_t opt;

void InitOptions(void);
void ExitOptions(void);
TASK(void) OptionsMenu(MGLDraw *mgl);

#endif
