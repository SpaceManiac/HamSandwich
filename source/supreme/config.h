#ifndef CONFIG_H
#define CONFIG_H

#include "mgldraw.h"

struct config_t
{
	byte sound,music;
	byte joystick;
	byte numSounds;
	byte hiscores;
	byte camera;
	byte shading;
	int numGuys;
	int numBullets;
	int numParticles;
};

extern config_t config;

void LoadConfig(void);

#endif
