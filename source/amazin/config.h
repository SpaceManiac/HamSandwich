#ifndef CONFIG_H
#define CONFIG_H

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
	byte keys[8];
	char scoreNames[5][11];
	int32_t scores[5];
	hitime times[40];
};

static_assert(sizeof(amazinConfig) == 0x2ec);
static_assert(offsetof(amazinConfig, scores) == 0x58);

extern amazinConfig g_Config;

void LoadConfig();
void SaveConfig();

#endif  // CONFIG_H
