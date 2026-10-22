#ifndef OPTIONS_H
#define OPTIONS_H

#include "jamultypes.h"
#include "stddef.h"

struct HighTime
{
	char name[8];
	byte minutes;
	byte seconds;
	dword frames;
};

static_assert(sizeof(HighTime) == 0x10);
static_assert(offsetof(HighTime, frames) == 0xc);

struct AmazinConfig
{
	char registrationCode[9];
	byte registrationIdx;
	bool sound;
	bool music;
	byte lives;
	bool bouaphettaMode;
	byte useJoystick[2];
	bool rivalryPumpkins;
	byte rivalryMatches;
	byte rivalryPowerups;
	byte rivalryMapSize;
	byte rivalryMapMode;
	byte rivalryLevel;
	byte keys[2][4];
	char scoreNames[5][11];
	int32_t scores[5];
	HighTime times[2][20];
};

static_assert(sizeof(AmazinConfig) == 0x2ec);
static_assert(offsetof(AmazinConfig, scores) == 0x58);

extern AmazinConfig g_Config;

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

void OptionsInit();

#endif  // OPTIONS_H
