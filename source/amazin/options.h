#ifndef OPTIONS_H
#define OPTIONS_H

#include "jamultypes.h"
#include "stddef.h"

struct hitime
{
	char name[8];
	byte seconds;
	byte minutes;
	dword frames;
};

static_assert(sizeof(hitime) == 0x10);
static_assert(offsetof(hitime, frames) == 0xc);

struct amazinConfig
{
	char registrationCode[9];
	byte registrationIdx;
	bool sound;
	bool music;
	byte lives;
	bool bouaphettaMode;
	bool useJoystick[2];
	bool rivalryPumpkins;
	byte rivalryMatches;
	byte rivalryPowerups;
	byte rivalryMapSize;
	byte rivalryMapMode;
	byte rivalryLevel;
	byte keys[2][4];
	char scoreNames[5][11];
	int32_t scores[5];
	hitime times[2][20];
};

static_assert(sizeof(amazinConfig) == 0x2ec);
static_assert(offsetof(amazinConfig, scores) == 0x58);

extern amazinConfig g_Config;

void LoadConfig();
void SaveConfig();

bool ConfigGetSoundEnabled();
bool ConfigGetMusicEnabled();
byte ConfigGetLives();
bool ConfigGetBouaphettaMode();
bool ConfigGetRivalryPumpkins();
byte ConfigGetRivalryLevel();
byte ConfigGetRivalryMapSize();
byte ConfigGetRivalryMode();
byte ConfigGetRivalryPowerups();
byte ConfigGetRivalryMatches();

#endif  // OPTIONS_H
