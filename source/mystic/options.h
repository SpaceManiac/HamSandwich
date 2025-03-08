#ifndef OPTIONS_H
#define OPTIONS_H

#include "mgldraw.h"
#include "achieves.h"
#include "control.h"

#define OPT_VERSION (1)
#define EXPANSION_SIZE (256)

typedef struct option_t
{

	byte challenge;
	byte key[NUM_CONTROLS][3];
	byte joyCtrl[NUM_CONTROLS];
	byte soundVol,musicVol;
	byte waterFX,lightFX;
	byte achieve[(int)Achievement::NUM_ACHIEVES];
	byte expansionSpace[EXPANSION_SIZE];
} option_t;

extern option_t opt;

void InitOptions(void);
void ExitOptions(void);
TASK(void) OptionsMenu(MGLDraw *mgl);

#endif
