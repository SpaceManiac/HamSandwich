#ifndef OPTIONS_H
#define OPTIONS_H

#include "jamultypes.h"
#include "mgldraw.h"

#define MUSIC_OFF  0
#define MUSIC_ON   1
#define MUSIC_RAND 2

#define PLAYAS_BOUAPHA 0
#define PLAYAS_LUNATIC 1
#define PLAYAS_HAPPY   2

typedef struct options_t
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
} options_t;

extern options_t opt;
extern byte oldPlayAs;

void LoadOptions(void);
void SaveOptions(void);

void OptionsMenu(MGLDraw *mgl);

#endif
