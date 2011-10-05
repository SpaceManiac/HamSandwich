#ifndef CONFIG_H
#define CONFIG_H

#include "mgldraw.h"

typedef struct config_t
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
} config_t;

extern config_t config;

void LoadConfig(void);

#endif
