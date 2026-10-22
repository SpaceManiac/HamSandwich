#ifndef OPTIONS_H
#define OPTIONS_H

#include "jamultypes.h"
#include "mgldraw.h"
#include "highscore.h"

typedef struct options_t
{
	byte control[2][6];	// key scancodes
	byte joyCtrl[2];	// joystick 'codes' for the buttons
	byte sound,music;
	
	byte blowoutDiff;	// difficulty in Blowout mode
	byte clearanceDiff;	// difficulty in Inventory Clearance mode
	byte pestDiff;		// pest control difficulty

	byte curProfile;
	// best scores
	highScore_t score[NUM_GAMES][MAX_SCORES][3];	// high scores in different modes
} options_t;

extern options_t opt;

void LoadOptions(void);
void SaveOptions(void);

void OptionsMenu(MGLDraw *mgl);

#endif