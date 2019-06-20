#ifndef OPTIONS_H
#define OPTIONS_H

#include "jamultypes.h"
#include "mgldraw.h"

enum {
	MUSIC_OFF = 0,
	MUSIC_ON,
	MUSIC_RAND
};

enum {
	PLAYAS_BOUAPHA = 0,
	PLAYAS_LUNATIC,
	PLAYAS_HAPPY
};

struct options_t
{
	byte control[2][6]; // key scancodes
	byte joyCtrl[2]; // joystick 'codes' for the buttons
	byte sound, music;
	byte playAs; // if you wish to play as a different character after winning
	byte wonGame;
	byte gotAllSecrets;
	byte youSuck;
	// new options
	byte discoMode;
	byte smoothLight;
};

extern options_t opt;
extern byte oldPlayAs;

void LoadOptions(void);
void SaveOptions(void);

TASK(void) OptionsMenu(MGLDraw *mgl);

#endif
