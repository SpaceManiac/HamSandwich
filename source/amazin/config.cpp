#include "config.h"
#include "appdata.h"
#include "control.h"
#include "hammusic.h"

amazinConfig g_Config;

void ApplyKeyConfigs()
{
	SetKeyboardBindings(0, 4, g_Config.keys[0]);
	SetKeyboardBindings(1, 4, g_Config.keys[1]);
}

void LoadConfig()
{
	auto f = AssetOpen_SDL_Owned("amazin.config");
	if (!f)
	{
		g_Config.sound = true;
		g_Config.music = true;
		g_Config.lives = 3;
		g_Config.bouaphettaMode = false;
		g_Config.useJoystick[0] = false;
		g_Config.useJoystick[1] = true;
		g_Config.rivalryPumpkins = true;
		g_Config.rivalryMatches = 2;
		g_Config.rivalryPowerups = 3;
		g_Config.rivalryMapMode = 1;
		g_Config.rivalryLevel = 1;
		g_Config.rivalryMapSize = 3;
		g_Config.keys[0][0] = SDL_SCANCODE_UP;
		g_Config.keys[0][1] = SDL_SCANCODE_DOWN;
		g_Config.keys[0][2] = SDL_SCANCODE_LEFT;
		g_Config.keys[0][3] = SDL_SCANCODE_RIGHT;
		g_Config.keys[1][0] = SDL_SCANCODE_W;
		g_Config.keys[1][1] = SDL_SCANCODE_S;
		g_Config.keys[1][2] = SDL_SCANCODE_A;
		g_Config.keys[1][3] = SDL_SCANCODE_D;
		strcpy(g_Config.scoreNames[0], "Bouapha");
		strcpy(g_Config.scoreNames[1], "Bouaphetta");
		strcpy(g_Config.scoreNames[2], "Jamul");
		strcpy(g_Config.scoreNames[3], "Tricia");
		strcpy(g_Config.scoreNames[4], "Yerfdog");
		g_Config.scores[0] = 10000;
		g_Config.scores[1] = 5000;
		g_Config.scores[2] = 1000;
		g_Config.scores[3] = 500;
		g_Config.scores[4] = 250;
		for (int i = 0; i < 0x14; i = i + 1)
		{
			strcpy(g_Config.times[0][i].name, "Nobody");
			g_Config.times[0][i].seconds = 99;
			g_Config.times[0][i].minutes = 0x3b;
			g_Config.times[0][i].frames = 0x2bf02;
			strcpy(g_Config.times[1][i].name, "Nobody");
			g_Config.times[1][i].seconds = 99;
			g_Config.times[1][i].minutes = 0x3b;
			g_Config.times[1][i].frames = 0x2bf02;
		}
	}
	else
	{
		SDL_RWread(f, &g_Config, 1, 0x2ec);
		f.reset();
	}
	ApplyKeyConfigs();
	// TODO ControlSetUseJoystick(0, g_Config.useJoystick[0]);
	// TODO ControlSetUseJoystick(1, g_Config.useJoystick[1]);
	StopSong();
	// ReadRegistrationCodeFromRegistry();
	// CheckRegistrationValid();
	return;
}

void SaveConfig()
{
	strcpy(g_Config.registrationCode, "00000000");
	g_Config.registrationIdx = 0;
	auto f = AppdataOpen_Write_SDL("amazin.cfg");
	SDL_RWwrite(f, &g_Config, 1, 0x2ec);
}
