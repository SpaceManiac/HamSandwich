#include "options.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "appdata.h"

static byte cursor;
static byte oldc;
static dword oldBtn;
static byte controlX, controlY;
static byte optMode;

options_t opt;
byte oldPlayAs;

void InitOptionsMenu(void)
{
	oldc = 255;
	controlX = 10;
	cursor = 0;
	optMode = 0;
}

void ExitOptionsMenu(void)
{
	SaveOptions();
}

void ApplyControlSettings()
{
	SetKeyboardBindings(0, 6, opt.control[0]);
	SetKeyboardBindings(1, 6, opt.control[1]);
	SetJoystickBindings(2, opt.joyCtrl);
}

byte UpdateOptionsMenu(MGLDraw *mgl)
{
	char c;
	byte c2 = 0; // warning prevention
	dword btn, j;
	int i;

	switch (optMode) {
		case 0: // just going through options
			c = mgl->LastKeyPressed();
			c2 = GetControls() | GetArrows();

			if (c == 27)
			{
				return 1;
			}
			else if (c == 'u')
			{
				if (opt.gotAllSecrets)
				{
					opt.gotAllSecrets = opt.wonGame = 0;
				}
				else
				{
					opt.gotAllSecrets = opt.wonGame = 1;
				}
			}

			if ((c2 & CONTROL_UP) && (!(oldc & CONTROL_UP)))
			{
				cursor--;
				if (cursor > 7)
					cursor = 7;
			}
			if ((c2 & CONTROL_DN) && (!(oldc & CONTROL_DN)))
			{
				cursor++;
				if (cursor > 7)
					cursor = 0;
			}
			if ((c2 & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3)) && (!(oldc & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3))))
			{
				switch (cursor) {
					case 0:
						opt.sound = 1 - opt.sound;
						break;
					case 1:
						opt.music++;
						if (opt.music > MUSIC_RAND)
							opt.music = MUSIC_OFF;
						PlayerSetMusicSettings(opt.music);
						CDStop();
						if (opt.music == MUSIC_ON)
						{
							i = GetCurSong();
							if (i == 3)
								i = 2;
							CDPlay(i);
						}
						else
						{
							ChooseNextSong();
						}
						break;
					case 2:
						if (opt.wonGame)
						{
							if (opt.gotAllSecrets)
							{
								opt.playAs++;
								if (opt.playAs > PLAYAS_HAPPY)
									opt.playAs = PLAYAS_BOUAPHA;
							}
							else
							{
								opt.playAs++;
								if (opt.playAs > PLAYAS_LUNATIC)
									opt.playAs = PLAYAS_BOUAPHA;
							}
						}
						break;
					case 3:
						if (opt.wonGame)
						{
							opt.discoMode = 1 - opt.discoMode;
						}
						break;
					case 4:
						opt.smoothLight = 1 - opt.smoothLight;
						break;
					case 5:
						opt.youSuck = 1 - opt.youSuck;
						break;
					case 6:
						optMode = 1;
						controlX = 0;
						controlY = 0;
						break;
					case 7:
						return 1;
						break;
				}
			}
			break;
		case 1: // selecting keys to configure
			c = mgl->LastKeyPressed();
			c2 = GetControls() | GetArrows();

			if (c == 27)
			{
				optMode = 0;
				controlX = 10;
				ApplyControlSettings();
				return 0;
			}
			if (c == 13)
				c2 |= CONTROL_B1;

			if ((c2 & CONTROL_UP) && (!(oldc & CONTROL_UP)))
			{
				controlY--;
				if (controlY > 5)
					controlY = 5;
			}
			if ((c2 & CONTROL_DN) && (!(oldc & CONTROL_DN)))
			{
				controlY++;
				if (controlY > 5)
					controlY = 0;
			}
			if ((c2 & CONTROL_LF) && (!(oldc & CONTROL_LF)))
			{
				controlX--;
				if (controlX > 2)
					controlX = 2;
			}
			if ((c2 & CONTROL_RT) && (!(oldc & CONTROL_RT)))
			{
				controlX++;
				if (controlX > 2)
					controlX = 0;
			}
			if ((c2 & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3)) && (!(oldc & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3))))
			{
				if (controlX < 2)
				{
					// keyboard
					optMode = 2;
					LastScanCode();
				}
				else if (controlY > 3)
				{
					btn = 0;
					oldBtn = ~0;
					optMode = 3;
				}
			}
			break;
		case 2: // entering a specific key
			c2 = LastScanCode();
			if (c2 == 1) // ESC key
			{
				optMode = 1;
				c2 = 255;
				oldc = 255;
				mgl->LastKeyPressed();
				return 0;
			}
			if (c2 != 0 && c2 != 67) // 67 = enter
			{
				opt.control[controlX][controlY] = c2;
				optMode = 1;
				mgl->LastKeyPressed();
			}
			c2 = 255;
			break;
		case 3: // pressing a joystick button
			c = mgl->LastKeyPressed();
			if (c == 27)
			{
				optMode = 1;
				c2 = 255;
				oldc = 255;
				return 0;
			}
			btn = GetJoyButtons();

			j = 1;
			for (i = 0; i < 16; i++)
			{
				if ((btn & j) && !(oldBtn & j))
				{
					opt.joyCtrl[controlY - 4] = i;
					optMode = 1;
					c2 = 255;
				}
				j *= 2;
			}
			oldBtn = btn;
			break;
	}

	oldc = c2;

	return 0;
}

void RenderControls(int x, int y)
{
	char dirName[6][12] = {"Up", "Down", "Left", "Right", "Hammer", "Weapon"};
	char btnTxt[64];
	int i;

	DrawFillBox(x, y - 2, x + 398, y + 20, 16);
	CenterPrint(x + 50, y, "Control", 0, 1);
	CenterPrint(x + 150, y, "Keyboard1", 0, 1);
	CenterPrint(x + 250, y, "Keyboard2", 0, 1);
	CenterPrint(x + 350, y, "Joystick", 0, 1);
	DrawBox(x + 98, y - 2, x + 198, y + 200, 16);
	DrawBox(x + 198, y - 2, x + 298, y + 200, 16);
	DrawBox(x + 298, y - 2, x + 398, y + 200, 16);

	for (i = 0; i < 6; i++)
	{
		if (controlY == i && controlX < 3)
		{
			if (optMode == 1)
				DrawFillBox(x + 99 + 100 * controlX, y + 20 + 1 + i * 30, x + 198 + 100 * controlX, y + 20 + 29 + i * 30, 20);
			else
			{
				DrawFillBox(x + 99 + 100 * controlX, y + 20 + 1 + i * 30, x + 198 + 100 * controlX, y + 20 + 29 + i * 30, 31);
				CenterPrint(x + 150 + controlX * 100, y + 27 + i * 30, "???", 0, 1);
			}
		}
		DrawFillBox(x, y + 20 + 1 + i * 30, x + 98, y + 20 + 29 + i * 30, 10);
		DrawBox(x, y + 20 + i * 30, x + 398, y + 20 + 30 + i * 30, 16);

		CenterPrint(x + 50, y + 27 + i * 30, dirName[i], 0, 1);
		if (optMode == 1 || controlX != 0 || controlY != i)
			CenterPrint(x + 150, y + 27 + i * 30, ScanCodeText(opt.control[0][i]), 0, 1);
		if (optMode == 1 || controlX != 1 || controlY != i)
			CenterPrint(x + 250, y + 27 + i * 30, ScanCodeText(opt.control[1][i]), 0, 1);

		if (i > 3)
		{
			if (optMode == 1 || controlX != 2 || controlY != i)
			{
				sprintf(btnTxt, "Button %d", opt.joyCtrl[i - 4] + 1);
				CenterPrint(x + 350, y + 27 + i * 30, btnTxt, 0, 1);
			}
		}
		else
		{
			CenterPrint(x + 350, y + 27 + i * 30, dirName[i], 16, 1);
		}
	}
	if (optMode == 0)
	{
		CenterPrint(x + 200, 210 + 232, "Move with arrow keys, ENTER to select", 0, 1);
		CenterPrint(x + 200, 210 + 252, "ESC to return to main menu", 0, 1);
	}
	else if (optMode == 1)
	{
		CenterPrint(x + 200, 210 + 232, "Select with arrow keys, ENTER to set new control", 0, 1);
		CenterPrint(x + 200, 210 + 252, "ESC to return to options", 0, 1);
	}
	else if (optMode == 2)
	{
		sprintf(btnTxt, "Press a key for %s", dirName[controlY]);
		CenterPrint(x + 200, 210 + 232, btnTxt, 0, 1);
		CenterPrint(x + 200, 210 + 252, "ESC to cancel", 0, 1);
	}
	else if (optMode == 3)
	{
		sprintf(btnTxt, "Press a joystick button for %s", dirName[controlY]);
		CenterPrint(x + 200, 210 + 232, btnTxt, 0, 1);
		CenterPrint(x + 200, 210 + 252, "ESC to cancel", 0, 1);
	}
}

void RenderOptionsMenu(MGLDraw *mgl)
{
	char onoff[3][8] = {"Off", "On", "Random"};
	char playAs[3][18] = {"Bouapha", "Dr. Lunatic", "Happy Stick Man"};
	char youSuck[2][18] = {"Sanitized", "Classic"};

	mgl->ClearScreen();
	CenterPrint(320, 2, "Game Options", 0, 0);

	int dy = 18, y = 80 - dy;

	DrawFillBox(250, 80 - 1 + dy*cursor, 390, 80 + 12 + dy*cursor, 10);

	CenterPrint(320, y += dy, "Sound", 0, 1);
	Print(392, y, onoff[opt.sound], 0, 1);
	CenterPrint(320, y += dy, "Music", 0, 1);
	Print(392, y, onoff[opt.music], 0, 1);
	if (!opt.wonGame)
	{
		CenterPrint(320, y += dy, "?????", 0, 1);
		CenterPrint(320, y += dy, "?????", 0, 1);
	}
	else
	{
		CenterPrint(320, y += dy, "Play As", 0, 1);
		Print(392, y, playAs[opt.playAs], 0, 1);
		CenterPrint(320, y += dy, "Disco Mode", 0, 1);
		Print(392, y, onoff[opt.discoMode], 0, 1);
	}
	CenterPrint(320, y += dy, "Smooth Lighting", 0, 1);
	Print(392, y, onoff[opt.smoothLight], 0, 1);
	CenterPrint(320, y += dy, "Game Over Msg.", 0, 1);
	Print(392, y, youSuck[opt.youSuck], 0, 1);

	CenterPrint(320, y += dy, "Configure Controls", 0, 1);
	CenterPrint(320, y += dy, "Exit To Main Menu", 0, 1);

	RenderControls(120, 230);
}

// ----------------

void LoadOptions(void)
{
	FILE *f;

	f = AppdataOpen("lunatic.cfg", "rb");
	if (!f)
	{
		opt.sound = 1;
		opt.music = MUSIC_ON;
		opt.playAs = PLAYAS_BOUAPHA;
		opt.wonGame = 0;
		opt.gotAllSecrets = 1;
		opt.smoothLight = 1; // new
		opt.discoMode = 0; // new

		opt.control[0][0] = SDL_SCANCODE_UP; // up
		opt.control[0][1] = SDL_SCANCODE_DOWN; // down
		opt.control[0][2] = SDL_SCANCODE_LEFT; // left
		opt.control[0][3] = SDL_SCANCODE_RIGHT; // right
		opt.control[0][4] = SDL_SCANCODE_LCTRL; // hammer: CTRL
		opt.control[0][5] = SDL_SCANCODE_LSHIFT; // special: shift
		opt.control[1][0] = 0; // up
		opt.control[1][1] = 0; // down
		opt.control[1][2] = 0; // left
		opt.control[1][3] = 0; // right
		opt.control[1][4] = SDL_SCANCODE_Z; // hammer: Z
		opt.control[1][5] = SDL_SCANCODE_X; // special: X

		opt.joyCtrl[0] = 0;
		opt.joyCtrl[1] = 1;
	}
	else
	{
		fread(&opt, sizeof (options_t), 1, f);
		fclose(f);
	}
	ApplyControlSettings();
}

void SaveOptions(void)
{
	FILE *f;

	f = AppdataOpen("lunatic.cfg", "wb");
	fwrite(&opt, sizeof (options_t), 1, f);
	fclose(f);
}

void OptionsMenu(MGLDraw *mgl)
{
	byte done = 0;

	InitOptionsMenu();

	while (!done)
	{
		done = UpdateOptionsMenu(mgl);
		RenderOptionsMenu(mgl);
		mgl->Flip();

		if (!mgl->Process())
			done = 1;

	}

	ExitOptionsMenu();
}
