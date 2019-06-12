#ifndef OPTIONS_H
#define OPTIONS_H

#include "jamultypes.h"
#include "mgldraw.h"
#include "gallery.h"
#pragma pack(1)

typedef struct options_t
{
	byte control[2][8];	// key scancodes
	byte joyCtrl[4];	// joystick 'codes' for the buttons
	byte sound,music;
	byte modifier[30];	// have access to these mods
	byte achieve[100];
	byte lastProfile;
#ifdef DIRECTORS
	byte galPix[GALLERY_PIX];
#endif
} options_t;

extern options_t opt;

void LoadOptions(void);
void SaveOptions(void);

void OptionsMenu(MGLDraw *mgl,byte *backScr);

void ApplyControlSettings(void);
void ApplyControlSettingsMem(byte *data);

#endif
