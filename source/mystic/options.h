#ifndef OPTIONS_H
#define OPTIONS_H

#include "mgldraw.h"

typedef struct option_t
{
	byte challenge;
	byte key[8][3];
	byte joyCtrl[4];
	byte soundVol,musicVol;
	byte waterFX,lightFX;
} option_t;

extern option_t opt;

void InitOptions(void);
void ExitOptions(void);
TASK(void) OptionsMenu(MGLDraw *mgl);

#endif
