#include "options.h"
#include "appdata.h"
#include "control.h"
#include "display.h"
#include "guy.h"
#include "hammusic.h"
#include "jamulspr.h"
#include "loop.h"
#include "title.h"
#include "register.h"

byte g_KeyConfigPlayer;
AmazinConfig g_Config;
byte g_KeyConfigFlip;
byte g_KeyConfigSelection;
byte g_OptionsSelection;
byte g_OptionsOldHeld;
byte g_KeyConfigInProgress[4];

void ApplyKeyConfigs();

void LoadConfig()
{
	auto f = AssetOpen_SDL_Owned("amazin.cfg");
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
			g_Config.times[0][i].minutes = 99;
			g_Config.times[0][i].seconds = 0x3b;
			g_Config.times[0][i].frames = 0x2bf02;
			strcpy(g_Config.times[1][i].name, "Nobody");
			g_Config.times[1][i].minutes = 99;
			g_Config.times[1][i].seconds = 0x3b;
			g_Config.times[1][i].frames = 0x2bf02;
		}
	}
	else
	{
		SDL_RWread(f, &g_Config, 1, 0x2ec);
		f.reset();
	}
	ApplyKeyConfigs();
	ControlSetUseJoystick(0, g_Config.useJoystick[0]);
	ControlSetUseJoystick(1, g_Config.useJoystick[1]);
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

void ApplyKeyConfigs()
{
	SetKeyboardBindings(0, 4, g_Config.keys[0]);
	SetKeyboardBindings(1, 4, g_Config.keys[1]);
}

bool ConfigGetSoundEnabled()
{
	return g_Config.sound;
}

bool ConfigGetMusicEnabled()
{
	return g_Config.music;
}

byte ConfigGetLives()
{
	return g_Config.lives;
}

bool ConfigGetBouaphettaMode()
{
	return g_Config.bouaphettaMode;
}

bool ConfigGetRivalryPumpkins()
{
	return g_Config.rivalryPumpkins;
}

byte ConfigGetRivalryLevel()
{
	return g_Config.rivalryLevel;
}

byte ConfigGetRivalryMapSize()
{
	return g_Config.rivalryMapMode;
}

byte ConfigGetRivalryMode()
{
	return g_Config.rivalryMapMode;
}

byte ConfigGetRivalryPowerups()
{
	return g_Config.rivalryPowerups;
}

byte ConfigGetRivalryMatches()
{
	return g_Config.rivalryMatches;
}

bool OptionsUpdate();
void OptionsRender();

void OptionsInit()
{
	SetUpdate(OptionsUpdate);
	SetRender(OptionsRender);
	g_OptionsSelection = 0;
	g_OptionsOldHeld = 0xff;
	if (!mgl->LoadBMP("graphics/backgd.bmp"))
	{
		FatalError("Missing file: graphics/backgd.bmp");
		exit(1);
	}
	memcpy(g_Background, mgl->GetScreen(), 0x4b000);
}

void OptionsExit()
{
	ControlSetUseJoystick(0, g_Config.useJoystick[0]);
	ControlSetUseJoystick(1, g_Config.useJoystick[1]);
}

void KeyConfigInit();
void RivalryOptionsInit();

bool OptionsUpdate()
{
	byte bVar1;

	char k = mgl->LastKeyPressed();
	byte held = GetPlayerControls(0) | GetPlayerControls(1) | GetArrows();
	if (k != '\x1b')
	{
		if ((((k == '\r') || (k == ' ')) ||
		     (((held & CONTROL_B1) != 0 && ((g_OptionsOldHeld & CONTROL_B1) == 0)))) ||
		    (((held & CONTROL_RT) != 0 && ((g_OptionsOldHeld & CONTROL_RT) == 0))))
		{
			switch (g_OptionsSelection)
			{
			case 0:
				g_Config.lives = g_Config.lives + 1;
				if (9 < g_Config.lives)
				{
					g_Config.lives = 1;
				}
				break;
			case 1:
				g_Config.bouaphettaMode = !g_Config.bouaphettaMode;
				break;
			case 2:
				bVar1 = g_Config.useJoystick[0] + 1;
				if (2 < (byte)(g_Config.useJoystick[0] + 1))
				{
					bVar1 = g_Config.useJoystick[0] - 2;
				}
				g_Config.useJoystick[0] = bVar1;
				if (g_Config.useJoystick[0] == g_Config.useJoystick[1])
				{
					g_Config.useJoystick[0] = g_Config.useJoystick[0] + 1;
				}
				if (2 < g_Config.useJoystick[0])
				{
					g_Config.useJoystick[0] = g_Config.useJoystick[0] - 3;
				}
				break;
			case 3:
				bVar1 = g_Config.useJoystick[1] + 1;
				if (2 < (byte)(g_Config.useJoystick[1] + 1))
				{
					bVar1 = g_Config.useJoystick[1] - 2;
				}
				g_Config.useJoystick[1] = bVar1;
				if (g_Config.useJoystick[1] == g_Config.useJoystick[0])
				{
					g_Config.useJoystick[1] = g_Config.useJoystick[1] + 1;
				}
				if (2 < g_Config.useJoystick[1])
				{
					g_Config.useJoystick[1] = g_Config.useJoystick[1] - 3;
				}
				break;
			case 4:
				OptionsExit();
				KeyConfigInit();
				return true;
			case 5:
				OptionsExit();
				KeyConfigInit();
				return true;
			case 6:
				OptionsExit();
				RivalryOptionsInit();
				return true;
			case 7:
				g_Config.sound = !g_Config.sound;
				break;
			case 8:
				g_Config.music = !g_Config.music;
				break;
			case 9:
				OptionsExit();
				TitleInit();
				return true;
			}
		}
		if (((held & CONTROL_LF) != 0) && ((g_OptionsOldHeld & CONTROL_LF) == 0))
		{
			switch (g_OptionsSelection)
			{
			case 0:
				g_Config.lives = g_Config.lives - 1;
				if (g_Config.lives == 0)
				{
					g_Config.lives = 9;
				}
				break;
			case 1:
				g_Config.bouaphettaMode = !g_Config.bouaphettaMode;
				break;
			case 2:
				bVar1 = g_Config.useJoystick[0] - 1;
				if (2 < (byte)(g_Config.useJoystick[0] - 1))
				{
					bVar1 = g_Config.useJoystick[0] + 2;
				}
				g_Config.useJoystick[0] = bVar1;
				if (g_Config.useJoystick[0] == g_Config.useJoystick[1])
				{
					g_Config.useJoystick[0] = g_Config.useJoystick[0] - 1;
				}
				if (2 < g_Config.useJoystick[0])
				{
					g_Config.useJoystick[0] = g_Config.useJoystick[0] + 3;
				}
				break;
			case 3:
				bVar1 = g_Config.useJoystick[1] - 1;
				if (2 < (byte)(g_Config.useJoystick[1] - 1))
				{
					bVar1 = g_Config.useJoystick[1] + 2;
				}
				g_Config.useJoystick[1] = bVar1;
				if (g_Config.useJoystick[1] == g_Config.useJoystick[0])
				{
					g_Config.useJoystick[1] = g_Config.useJoystick[1] - 1;
				}
				if (2 < g_Config.useJoystick[1])
				{
					g_Config.useJoystick[1] = g_Config.useJoystick[1] + 3;
				}
				break;
			case 4:
				OptionsExit();
				KeyConfigInit();
				return true;
			case 5:
				OptionsExit();
				KeyConfigInit();
				return true;
			case 6:
				OptionsExit();
				RivalryOptionsInit();
				return true;
			case 7:
				g_Config.sound = !g_Config.sound;
				break;
			case 8:
				g_Config.music = !g_Config.music;
				break;
			case 9:
				OptionsExit();
				TitleInit();
				return true;
			}
		}
		if ((((held & CONTROL_DN) != 0) && ((g_OptionsOldHeld & CONTROL_DN) == 0)) &&
		    (g_OptionsSelection = g_OptionsSelection + 1, 9 < g_OptionsSelection))
		{
			g_OptionsSelection = 0;
		}
		if ((((held & CONTROL_UP) != 0) && ((g_OptionsOldHeld & CONTROL_UP) == 0)) &&
		    (g_OptionsSelection = g_OptionsSelection - 1, 9 < g_OptionsSelection))
		{
			g_OptionsSelection = 9;
		}
		g_OptionsOldHeld = held;
		return true;
	}
	OptionsExit();
	TitleInit();
	return true;
}

void OptionsRender()
{
	static const char inputNames[3][16] = {
		"Keyboard1",
		"Keyboard2",
		"Joystick",
	};
	static const char optionDescriptions[10][48] = {
		"Number of chances to complete the game",
		"Select character controlled by each player",
		"Select control method for player 1",
		"Select control method for player 2",
		"Define the keys for Keyboard1 control",
		"Define the keys for Keyboard2 control",
		"Set up the Sibling Rivalry game",
		"Toggle sound effects",
		"Toggle background music",
		"Return to the title screen",
	};

	char buf[32];

	memcpy(mgl->GetScreen(), g_Background, 0x4b000);
	CenterPrint(0x140, 2, "Game Options");
	sprintf(buf, "Lives: %d", (unsigned int)g_Config.lives);
	PrintMultiline(0x52, 0x2a, buf);
	if (g_Config.bouaphettaMode == false)
	{
		PrintMultiline(0x52, 0x48, "Players: P1/Bouapha  P2/Bouaphetta");
	}
	else
	{
		PrintMultiline(0x52, 0x48, "Players: P1/Bouaphetta  P2/Bouapha");
	}
	sprintf(buf, "P1 Control: %s", inputNames[g_Config.useJoystick[0]]);
	PrintMultiline(0x52, 0x66, buf);
	sprintf(buf, "P2 Control: %s", inputNames[g_Config.useJoystick[1]]);
	PrintMultiline(0x52, 0x84, buf);
	PrintMultiline(0x52, 0xa2, "Setup Keyboard 1");
	PrintMultiline(0x52, 0xc0, "Setup Keyboard 2");
	PrintMultiline(0x52, 0xde, "Set Rivalry Options");
	if (g_Config.sound == false)
	{
		PrintMultiline(0x52, 0xfc, "Sound: Off");
	}
	else
	{
		PrintMultiline(0x52, 0xfc, "Sound: On");
	}
	if (g_Config.music == false)
	{
		PrintMultiline(0x52, 0x11a, "Music: Off");
	}
	else
	{
		PrintMultiline(0x52, 0x11a, "Music: On");
	}
	PrintMultiline(0x52, 0x138, "Exit");
	GetGuySprite(6, 13)->Draw(0x43, (unsigned int)g_OptionsSelection * 0x1e + 0x3a, mgl);
	CenterPrintMultiline(0x140, 0x1a4, optionDescriptions[g_OptionsSelection]);
	CenterPrintMultiline(0x140, 0x1c2, "Up & Down select. Left, Right, & Enter adjust.");
}

bool KeyConfigUpdate();
void KeyConfigRender();

void KeyConfigInit()
{
	SetUpdate(KeyConfigUpdate);
	SetRender(KeyConfigRender);
	g_KeyConfigPlayer = g_OptionsSelection != 4;
	g_KeyConfigSelection = 0;
	LastScanCode();
}

void KeyConfigExit()
{
	ApplyKeyConfigs();
	mgl->LastKeyPressed();
}

bool KeyConfigUpdate()
{
	byte scan;

	g_KeyConfigFlip = 1 - g_KeyConfigFlip;
	scan = LastScanCode();
	if (scan != 0)
	{
		if (scan == SDL_SCANCODE_ESCAPE)
		{
			KeyConfigExit();
			OptionsInit();
		}
		else
		{
			g_KeyConfigInProgress[g_KeyConfigSelection] = scan;
			g_KeyConfigSelection = g_KeyConfigSelection + 1;
			if (g_KeyConfigSelection == 4)
			{
				if (g_KeyConfigPlayer == 0)
				{
					g_Config.keys[0][0] = g_KeyConfigInProgress[0];
					g_Config.keys[0][1] = g_KeyConfigInProgress[1];
					g_Config.keys[0][2] = g_KeyConfigInProgress[2];
					g_Config.keys[0][3] = g_KeyConfigInProgress[3];
				}
				else
				{
					g_Config.keys[1][0] = g_KeyConfigInProgress[0];
					g_Config.keys[1][1] = g_KeyConfigInProgress[1];
					g_Config.keys[1][2] = g_KeyConfigInProgress[2];
					g_Config.keys[1][3] = g_KeyConfigInProgress[3];
				}
				KeyConfigExit();
				OptionsInit();
			}
		}
	}
	return true;
}

void KeyConfigRender()
{
	byte(*activeKeys)[4];
	char buf[32];

	memcpy(mgl->GetScreen(), g_Background, 0x4b000);
	sprintf(buf, "Setup Keyboard%d", g_KeyConfigPlayer + 1);
	CenterPrint(0x140, 2, buf);
	if (g_KeyConfigPlayer == 0)
	{
		activeKeys = &g_Config.keys[0];
	}
	else
	{
		activeKeys = &g_Config.keys[1];
	}
	PrintMultiline(0x2a, 0x2a, "Old Up Key:");
	PrintMultiline(0xfc, 0x2a, ScanCodeText((*activeKeys)[0]));
	PrintMultiline(0x2a, 0x48, "Old Down Key:");
	PrintMultiline(0xfc, 0x48, ScanCodeText((*activeKeys)[1]));
	PrintMultiline(0x2a, 0x66, "Old Left Key:");
	PrintMultiline(0xfc, 0x66, ScanCodeText((*activeKeys)[2]));
	PrintMultiline(0x2a, 0x84, "Old Right Key:");
	PrintMultiline(0xfc, 0x84, ScanCodeText((*activeKeys)[3]));
	PrintMultiline(0x2a, 0xb6, "New Up Key:");
	if (g_KeyConfigSelection != 0)
	{
		PrintMultiline(0x2a, 0xd4, "New Down Key:");
	}
	if (1 < g_KeyConfigSelection)
	{
		PrintMultiline(0x2a, 0xf2, "New Left Key:");
	}
	if (2 < g_KeyConfigSelection)
	{
		PrintMultiline(0x2a, 0x110, "New Right Key:");
	}
	PrintMultiline(0x2a, 0x156, "Press ESC to cancel changes");
	if (g_KeyConfigSelection == 0)
	{
		if (g_KeyConfigFlip != 0)
		{
			PrintMultiline(0xfc, 0xb6, "???");
		}
	}
	else
	{
		PrintMultiline(0xfc, 0xb6, ScanCodeText(g_KeyConfigInProgress[0]));
	}
	if (g_KeyConfigSelection == 1)
	{
		if (g_KeyConfigFlip != 0)
		{
			PrintMultiline(0xfc, 0xd4, "???");
		}
	}
	else if (1 < g_KeyConfigSelection)
	{
		PrintMultiline(0xfc, 0xd4, ScanCodeText(g_KeyConfigInProgress[1]));
	}
	if (g_KeyConfigSelection == 2)
	{
		if (g_KeyConfigFlip != 0)
		{
			PrintMultiline(0xfc, 0xf2, "???");
		}
	}
	else if (2 < g_KeyConfigSelection)
	{
		PrintMultiline(0xfc, 0xf2, ScanCodeText(g_KeyConfigInProgress[2]));
	}
	if (g_KeyConfigSelection == 3)
	{
		if (g_KeyConfigFlip != 0)
		{
			PrintMultiline(0xfc, 0x110, "???");
		}
	}
	else if (3 < g_KeyConfigSelection)
	{
		PrintMultiline(0xfc, 0x110, ScanCodeText(g_KeyConfigInProgress[3]));
	}
	return;
}

bool RivalryOptionsUpdate();
void RivalryOptionsRender();

void RivalryOptionsInit()
{
	SetUpdate(RivalryOptionsUpdate);
	SetRender(RivalryOptionsRender);
	g_OptionsSelection = 0;
	g_OptionsOldHeld = 0xff;
}

void RivalryOptionsExit()
{
}

bool RivalryOptionsUpdate()
{
	char k = mgl->LastKeyPressed();
	byte held = GetPlayerControls(0) | GetPlayerControls(1) | GetArrows();
	if (k == '\x1b')
	{
		RivalryOptionsExit();
		OptionsInit();
		held = g_OptionsOldHeld;
	}
	else
	{
		if ((((k == '\r') || (k == ' ')) ||
		     (((held & CONTROL_B1) != 0 && ((g_OptionsOldHeld & CONTROL_B1) == 0)))) ||
		    (((held & CONTROL_RT) != 0 && ((g_OptionsOldHeld & CONTROL_RT) == 0))))
		{
			switch (g_OptionsSelection)
			{
			case 0:
				if (RegistrationCurrentlyValid())
				{
					g_Config.rivalryPumpkins = (bool)('\x01' - g_Config.rivalryPumpkins);
				}
				break;
			case 1:
				if (RegistrationCurrentlyValid() &&
				    (g_Config.rivalryMatches = g_Config.rivalryMatches + 1, 5 < g_Config.rivalryMatches))
				{
					g_Config.rivalryMatches = 1;
				}
				break;
			case 2:
				if (RegistrationCurrentlyValid() &&
				    (g_Config.rivalryPowerups = g_Config.rivalryPowerups + 1, 10 < g_Config.rivalryPowerups))
				{
					g_Config.rivalryPowerups = 0;
				}
				break;
			case 3:
				if (RegistrationCurrentlyValid())
				{
					g_Config.rivalryMapMode = 3 - g_Config.rivalryMapMode;
				}
				break;
			case 4:
				if (RegistrationCurrentlyValid() &&
				    (g_Config.rivalryMapSize = g_Config.rivalryMapSize + 1, 5 < g_Config.rivalryMapSize))
				{
					g_Config.rivalryMapSize = 1;
				}
				break;
			case 5:
				if (RegistrationCurrentlyValid() &&
				    (g_Config.rivalryLevel = g_Config.rivalryLevel + 1, 0x14 < g_Config.rivalryLevel))
				{
					g_Config.rivalryLevel = 1;
				}
				break;
			case 6:
				RivalryOptionsExit();
				OptionsInit();
				return true;
			}
		}
		if (((held & CONTROL_LF) != 0) && ((g_OptionsOldHeld & CONTROL_LF) == 0))
		{
			switch (g_OptionsSelection)
			{
			case 0:
				if (RegistrationCurrentlyValid())
				{
					g_Config.rivalryPumpkins = (bool)('\x01' - g_Config.rivalryPumpkins);
				}
				break;
			case 1:
				if (RegistrationCurrentlyValid() &&
				    (g_Config.rivalryMatches = g_Config.rivalryMatches - 1, g_Config.rivalryMatches == 0))
				{
					g_Config.rivalryMatches = 5;
				}
				break;
			case 2:
				if (RegistrationCurrentlyValid() &&
				    (g_Config.rivalryPowerups = g_Config.rivalryPowerups - 1, 10 < g_Config.rivalryPowerups))
				{
					g_Config.rivalryPowerups = 10;
				}
				break;
			case 3:
				if (RegistrationCurrentlyValid())
				{
					g_Config.rivalryMapMode = 3 - g_Config.rivalryMapMode;
				}
				break;
			case 4:
				if (RegistrationCurrentlyValid() &&
				    (g_Config.rivalryMapSize = g_Config.rivalryMapSize - 1, g_Config.rivalryMapSize == 0))
				{
					g_Config.rivalryMapSize = 5;
				}
				break;
			case 5:
				if (RegistrationCurrentlyValid() &&
				    (g_Config.rivalryLevel = g_Config.rivalryLevel - 1, g_Config.rivalryLevel == 0))
				{
					g_Config.rivalryLevel = 0x14;
				}
				break;
			case 6:
				OptionsExit();
				TitleInit();
				return true;
			}
		}
		if (((held & CONTROL_DN) != 0) && ((g_OptionsOldHeld & CONTROL_DN) == 0))
		{
			g_OptionsSelection = g_OptionsSelection + 1;
			if (6 < g_OptionsSelection)
			{
				g_OptionsSelection = 0;
			}
			if ((g_OptionsSelection == 4) && (g_Config.rivalryMapMode == 1))
			{
				g_OptionsSelection = 5;
			}
		}
		if (((held & CONTROL_UP) != 0) && ((g_OptionsOldHeld & CONTROL_UP) == 0))
		{
			g_OptionsSelection = g_OptionsSelection - 1;
			if (6 < g_OptionsSelection)
			{
				g_OptionsSelection = 6;
			}
			if ((g_OptionsSelection == 4) && (g_Config.rivalryMapMode == 1))
			{
				g_OptionsSelection = 3;
			}
		}
	}
	g_OptionsOldHeld = held;
	return true;
}

void RivalryOptionsRender()
{
	static const char sizes[5][12] = {
		"Tiny",
		"Small",
		"Medium",
		"Large",
		"Huge",
	};
	static const char help[7][48] = {
		"Include pumpkins in the battle or not",
		"Number of matches to determine a winner",
		"How often powerups appear (0 means never)",
		"Play on game maps, or random ones",
		"Specify the size of the random maps",
		"How dangerous pumpkins are",
		"",
	};
	char buf[32];

	memcpy(mgl->GetScreen(), g_Background, 0x4b000);
	CenterPrint(0x140, 2, "Sibling Rivalry Options");
	if (g_Config.rivalryPumpkins == false)
	{
		PrintMultiline(0x52, 0x2a, "Rivalry Pumpkins: Off");
	}
	else
	{
		PrintMultiline(0x52, 0x2a, "Rivalry Pumpkins: On");
	}
	sprintf(buf, "Rivalry Matches: Best %d of %d", (unsigned int)g_Config.rivalryMatches, (unsigned int)g_Config.rivalryMatches * 2 + -1);
	PrintMultiline(0x52, 0x48, buf);
	sprintf(buf, "Rivalry Powerups: %d", (unsigned int)g_Config.rivalryPowerups);
	PrintMultiline(0x52, 0x66, buf);
	if (g_Config.rivalryMapMode == 1)
	{
		PrintMultiline(0x52, 0x84, "Rivalry Map: Preset");
	}
	else
	{
		PrintMultiline(0x52, 0x84, "Rivalry Map: Random");
	}
	if (g_Config.rivalryMapMode == 2)
	{
		sprintf(buf, "Rivalry Map Size: %s", sizes[g_Config.rivalryMapSize - 1]);
		PrintMultiline(0x52, 0xa2, buf);
	}
	sprintf(buf, "Rivalry Level: %d", (unsigned int)g_Config.rivalryLevel);
	PrintMultiline(0x52, 0xc0, buf);
	PrintMultiline(0x52, 0xde, "Exit");
	GetGuySprite(6, 13)->Draw(0x43, (unsigned int)g_OptionsSelection * 0x1e + 0x3a, mgl);
	if (!RegistrationCurrentlyValid())
	{
		CenterPrintMultiline(0x140, 0x186, "*** NOT AVAILABLE IN DEMO! ***");
	}
	CenterPrintMultiline(0x140, 0x1a4, (char *)help[g_OptionsSelection]);
	CenterPrintMultiline(0x140, 0x1c2, "Up & Down select. Left, Right, & Enter adjust.");
}
