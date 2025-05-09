#include "options.h"
#include "display.h"
#include "control.h"
#include "clock.h"
#include "sound.h"
#include "appdata.h"
#include "steam.h"

option_t opt;
byte firstRightHandOption = (OPT_EXIT+1)/2;
byte maxConfig;
dword prevGamePad;
static const char dirName[8][12] = { "Up","Down","Left","Right","Fire","Cast","Prev Spl","Next Spl" };

void ApplyControlSettings()
{
	// force certain controls:
	opt.joyCtrl[CTL_ID_UP] = RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_UP;
	opt.joyCtrl[CTL_ID_DN] = RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_DN;
	opt.joyCtrl[CTL_ID_LF] = RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_LF;
	opt.joyCtrl[CTL_ID_RT] = RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_RT;
	opt.joyCtrl[CTL_ID_ESCAPE] = SDL_CONTROLLER_BUTTON_START;

	// menu controls
	opt.key[CTL_ID_LF][2] = SDL_SCANCODE_LEFT;
	opt.key[CTL_ID_RT][2] = SDL_SCANCODE_RIGHT;
	opt.key[CTL_ID_UP][2] = SDL_SCANCODE_UP;
	opt.key[CTL_ID_DN][2] = SDL_SCANCODE_DOWN;
	opt.key[CTL_ID_ESCAPE][2] = SDL_SCANCODE_ESCAPE;
	opt.key[CTL_ID_B1][2] = SDL_SCANCODE_RETURN;
	opt.key[CTL_ID_B2][2] = SDL_SCANCODE_SPACE;

	for (int kbd = 0; kbd < 3; ++kbd)
	{
		byte keys[NUM_CONTROLS];
		for (int k = 0; k < NUM_CONTROLS; ++k)
			keys[k] = opt.key[k][kbd];
		SetKeyboardBindings(kbd, NUM_CONTROLS, keys);
	}

	SetJoystickBindings(NUM_CONTROLS, opt.joyCtrl);
	SetDpadToMode(opt.dpadToMove);
}

void DefaultControls(void)
{
	for (int i = 0; i < NUM_CONTROLS; i++)
	{
		for (int j = 0; j < 3; j++)
			opt.key[i][j] = 0;
		opt.joyCtrl[i] = 255;
	}
	opt.dpadToMove = false;
	// these are the arrow keys,  including the unchangable ones
	opt.key[CTL_ID_UP][0] = opt.key[CTL_ID_UP][2] = SDL_SCANCODE_UP;
	opt.key[CTL_ID_DN][0] = opt.key[CTL_ID_DN][2] = SDL_SCANCODE_DOWN;
	opt.key[CTL_ID_LF][0] = opt.key[CTL_ID_LF][2] = SDL_SCANCODE_LEFT;
	opt.key[CTL_ID_RT][0] = opt.key[CTL_ID_RT][2] = SDL_SCANCODE_RIGHT;

	opt.key[CTL_ID_UP][1] = 0;
	opt.key[CTL_ID_DN][1] = 0;
	opt.key[CTL_ID_LF][1] = 0;
	opt.key[CTL_ID_RT][1] = 0;
	// fire key
	opt.key[CTL_ID_B1][0] = SDL_SCANCODE_LCTRL;	// CTRL
	opt.key[CTL_ID_B1][1] = SDL_SCANCODE_Z;	// Z
	// spell key
	opt.key[CTL_ID_B2][0] = SDL_SCANCODE_LSHIFT;	// leftshift
	opt.key[CTL_ID_B2][1] = SDL_SCANCODE_X;	// X
	// previous
	opt.key[CTL_ID_B3][0] = SDL_SCANCODE_A;	// A
	opt.key[CTL_ID_B3][1] = SDL_SCANCODE_F1;	// F1
	// next
	opt.key[CTL_ID_B4][0] = SDL_SCANCODE_S;	// S
	opt.key[CTL_ID_B4][1] = SDL_SCANCODE_F2;	// F2
	opt.key[CTL_ID_ESCAPE][0] = SDL_SCANCODE_ESCAPE;
	opt.key[CTL_ID_ESCAPE][1] = 0;
	for (int i = 0; i < 10; i++)
		opt.key[CTL_ID_QC_1 + i][0] = SDL_SCANCODE_1 + i;	// keys 1-0 on the keyboard are the default spell keys

	// joystick buttons
	opt.joyCtrl[CTL_ID_UP] = RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_UP;
	opt.joyCtrl[CTL_ID_DN] = RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_DN;
	opt.joyCtrl[CTL_ID_LF] = RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_LF;
	opt.joyCtrl[CTL_ID_RT] = RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_RT;
	opt.joyCtrl[CTL_ID_B1] = SDL_CONTROLLER_BUTTON_A;
	opt.joyCtrl[CTL_ID_B2] = SDL_CONTROLLER_BUTTON_B;
	opt.joyCtrl[CTL_ID_B3] = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	opt.joyCtrl[CTL_ID_B4] = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	opt.joyCtrl[CTL_ID_ESCAPE] = SDL_CONTROLLER_BUTTON_START;
	opt.joyCtrl[CTL_ID_QC_1] = RAWGAMEPADAXIS_BASE+RawGamepadAxis::LT;	// energy barrage
	opt.joyCtrl[CTL_ID_QC_2] = RAWGAMEPADAXIS_BASE + RawGamepadAxis::RT; // dragon's flame
	opt.joyCtrl[CTL_ID_QC_3] = SDL_CONTROLLER_BUTTON_X; // seeker bolt
	opt.joyCtrl[CTL_ID_QC_4] = SDL_CONTROLLER_BUTTON_Y; // ice
	opt.joyCtrl[CTL_ID_QC_5] = SDL_CONTROLLER_BUTTON_LEFTSTICK; // inferno
	opt.joyCtrl[CTL_ID_QC_6] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT; // summon
	opt.joyCtrl[CTL_ID_QC_7] = SDL_CONTROLLER_BUTTON_DPAD_LEFT; // stoneskin
	opt.joyCtrl[CTL_ID_QC_8] = SDL_CONTROLLER_BUTTON_DPAD_DOWN; // berserk
	opt.joyCtrl[CTL_ID_QC_9] = SDL_CONTROLLER_BUTTON_DPAD_UP; // heal
	opt.joyCtrl[CTL_ID_QC_0] = SDL_CONTROLLER_BUTTON_RIGHTSTICK; // armageddon
}

void DefaultOptions(void)
{
	// default values
	DefaultControls();

	opt.challenge = 0;
	opt.soundVol = 5;
	opt.musicVol = 3;
	opt.waterFX = 1;
	opt.lightFX = 1;
	for (int i = 0; i < (int)Achievement::NUM_ACHIEVES; i++)
		opt.achieve[i] = 0;
	for (int i = 0; i < (int)CheatStone::NUM_CHEATSTONES; i++)
		opt.cheatStone[i] = CHEATSTONE_LOCKED;

	for (int i = 0; i < (int)OPT_EXPANSION_SIZE; i++)
		opt.expansionSpace[i] = 0;
	opt.classicMusic = 0;
}

void InitOptions(void)
{
	auto f = AppdataOpen("options.cfg");

	DefaultOptions();

	char optCode[5];
	if(f)
	{
		SDL_RWread(f, optCode,sizeof(char), 4);
		if (optCode[0] == 'K' && optCode[1] == 'I' && optCode[2] == 'D')
		{
			byte b = (byte)optCode[3];
			if (b != OPT_VERSION)	// this is an older version
			{
				// do whatever we need to do to update
				// from version 1-2, nothing needed. It just won't be able to load the cheatstone data, but that's already been cleared by default options
			}
			SDL_RWread(f, &opt, sizeof(option_t), 1);
		}
		else // this is so old it doesn't even have the cool KID code, so we need to reset it
		{
			DefaultOptions();
		}

		f.reset();
	}
	ApplyControlSettings();
	opt.lightFX = 1;	// in case you saved it with them turned off
	Steam()->ProfileReady();
}

void ExitOptions(void)
{
	auto f = AppdataOpen_Write("options.cfg");
	char optCode[5]="KID0";
	optCode[3] = (char)OPT_VERSION;
	SDL_RWwrite(f, optCode, sizeof(char), 4);
	SDL_RWwrite(f,&opt,sizeof(option_t),1);
	f.reset();
	AppdataSync();
}


//-----------------------------------------------
static byte cursor,optMode;
static dword oldBtn;
static byte controlX,controlY;

void InitOptionsMenu(void)
{
	controlX=10;
	cursor=0;
	optMode=OPTMODE_IDLE;
}

void ExitOptionsMenu(void)
{
}

byte UpdateOptionsMenu(MGLDraw *mgl)
{
	char c;
	byte b;
	dword btn,j;
	int i;

	c = mgl->LastKeyPressed();

	switch (optMode)
	{
	case OPTMODE_IDLE:	// just going through options
		if (ButtonTapped(CONTROL_ESCAPE))
			return 1;

		if (AutoRepeatTapped(CONTROL_UP))
		{
			cursor--;
			if (cursor == firstRightHandOption - 1)
				cursor = OPT_EXIT;
			else if (cursor > OPT_EXIT)
				cursor = firstRightHandOption - 1;
		}
		if (AutoRepeatTapped(CONTROL_DN))
		{
			cursor++;
			if (cursor == firstRightHandOption)
				cursor = 0;
			else if (cursor == OPT_EXIT + 1)
				cursor = firstRightHandOption;
		}
		if (AutoRepeatTapped(CONTROL_LF | CONTROL_RT))
		{
			cursor += firstRightHandOption;
			if (cursor > OPT_EXIT)
				cursor -= firstRightHandOption * 2;
		}
		if (ButtonTapped(CONTROL_B2))
		{
			return 1;
		}
		if (ButtonTapped(CONTROL_B1))
		{
			switch (cursor)
			{
			case OPT_SOUNDVOL:
				opt.soundVol--;
				if (opt.soundVol > 5)
					opt.soundVol = 5;
				VolumeSound(opt.soundVol);
				MakeNormalSound(SND_GOATSHOOT);
				break;
			case OPT_MUSICVOL:
				opt.musicVol--;
				if (opt.musicVol > 5)
					opt.musicVol = 5;
				if (opt.musicVol == 0)
					StopSong();
				else
					VolumeSong(opt.musicVol);
				if (opt.musicVol == 5)
					ReplaySong();
				break;
			case OPT_FANCYWATER:
				opt.waterFX = 1 - opt.waterFX;
				break;
			case OPT_CLASSICMUSIC:
				StopSong();
				opt.classicMusic = 1 - opt.classicMusic;
				PlaySong(SONG_SHOP);
				break;
			case OPT_DPADTOMOVE:
				opt.dpadToMove = 1 - opt.dpadToMove;
				SetDpadToMode(opt.dpadToMove);
				break;
			case OPT_CONFIG:
				optMode = OPTMODE_KEYCONFIG;
				maxConfig = 8;
				controlX = 0;
				controlY = 0;
				break;
			case OPT_CONFIGCAST:
				optMode = OPTMODE_QC_CONFIG;
				maxConfig = 10;
				controlX = 0;
				controlY = 0;
				break;
			case OPT_DEFAULTS:
				optMode = OPTMODE_VERIFY_DEFAULTS;
				cursor = 1;
				break;
			case OPT_SPELL_QUICK:
				opt.quickCast++;
				if (opt.quickCast > QUICKCAST_CASTONLY)
					opt.quickCast = QUICKCAST_SELECTONLY;
				break;
			case OPT_EXIT:
				return 1;
				break;
			}
		}
		break;
	case OPTMODE_VERIFY_DEFAULTS:
		if (ButtonTapped(CONTROL_ESCAPE | CONTROL_B2))
		{
			optMode = OPTMODE_IDLE;
			cursor = OPT_DEFAULTS;
		}
		else if (AutoRepeatTapped(CONTROL_LF) || AutoRepeatTapped(CONTROL_RT))
			cursor=1-cursor;
		if (ButtonTapped(CONTROL_B1))
		{
			if (cursor == 0)
			{
				DefaultControls();
				MakeNormalSound(SND_BOBBYSPIN);
			}
			optMode = OPTMODE_IDLE;
			cursor = OPT_DEFAULTS;
		}
		break;
	case OPTMODE_KEYCONFIG:	// selecting keys to configure
		if (ButtonTapped(CONTROL_B2 | CONTROL_ESCAPE))
		{
			optMode = OPTMODE_IDLE;
			controlX = 10;
			ApplyControlSettings();
			return 0;
		}

		if (AutoRepeatTapped(CONTROL_UP))
		{
			controlY--;
			if (controlY >= maxConfig)
				controlY = maxConfig - 1;
		}
		if (AutoRepeatTapped(CONTROL_DN))
		{
			controlY++;
			if (controlY >= maxConfig)
				controlY = 0;
		}
		if (AutoRepeatTapped(CONTROL_LF))
		{
			controlX--;
			if (controlX > 2)
				controlX = 2;
		}
		if (AutoRepeatTapped(CONTROL_RT))
		{
			controlX++;
			if (controlX > 2)
				controlX = 0;
		}
		if (ButtonTapped(CONTROL_B1))
		{
			if (controlX < 2)
			{
				// keyboard
				optMode = OPTMODE_KEYBIND;
				SetInMenu(false);
				LastScanCode();
				UpdateControls();
			}
			else if (controlY > 3)
			{
				btn = 0;
				oldBtn = ~0;
				optMode = OPTMODE_GAMEPADBIND;
				SetInMenu(false);
				prevGamePad = GetRawGamepad();
				UpdateControls();
			}
		}
		break;
	case OPTMODE_QC_CONFIG:	// selecting keys to configure
		if (ButtonTapped(CONTROL_B2 | CONTROL_ESCAPE))
		{
			optMode = OPTMODE_IDLE;
			controlX = 10;
			ApplyControlSettings();
			return 0;
		}

		if (AutoRepeatTapped(CONTROL_UP))
		{
			controlY--;
			if (controlY >= maxConfig)
				controlY = maxConfig - 1;
		}
		if (AutoRepeatTapped(CONTROL_DN))
		{
			controlY++;
			if (controlY >= maxConfig)
				controlY = 0;
		}
		if (AutoRepeatTapped(CONTROL_LF))
		{
			controlX--;
			if (controlX > 2)
				controlX = 2;
		}
		if (AutoRepeatTapped(CONTROL_RT))
		{
			controlX++;
			if (controlX > 2)
				controlX = 0;
		}
		if (ButtonTapped(CONTROL_B1))
		{
			if (controlX < 2)
			{
				// keyboard
				optMode = OPTMODE_QC_KEYBIND;
				SetInMenu(false);
				LastScanCode();
				UpdateControls();
			}
			else
			{
				btn = 0;
				oldBtn = ~0;
				optMode = OPTMODE_QC_GAMEPADBIND;
				SetInMenu(false);
				prevGamePad = GetRawGamepad();
				UpdateControls();
			}
		}
		break;
	case OPTMODE_KEYBIND: // entering a specific key
	case OPTMODE_QC_KEYBIND:
		if (ButtonTapped(CONTROL_ESCAPE))	// ESC key
		{
			optMode--; // takes us to KEYCONFIG or QC_CONFIG, as appropriate
			UpdateControls();
			mgl->LastKeyPressed();
			SetInMenu(true);
			return 0;
		}
		b = LastScanCode();

		if (b != 0)
		{
			if (optMode == OPTMODE_QC_KEYBIND)
			{
				opt.key[controlY + CTL_ID_QC_1][controlX] = b;
				for (int j = 0; j < NUM_CONTROLS; j++)
				{
					for (int k = 0; k < 2; k++)	// we only mess with the 2 changeable keyboards
						if (j != CTL_ID_ESCAPE && j != (controlY+CTL_ID_QC_1) && opt.key[j][k] == b)
							opt.key[j][k] = 0;	// unset any other place that uses this key. You can put multiple copies on the same action though, if you want to override something else there
				}
			}
			else
			{
				opt.key[controlY][controlX] = b;
				for (int j = 0; j < NUM_CONTROLS; j++)
				{
					for (int k = 0; k < 2; k++)	// we only mess with the 2 changeable keyboards
						if (j != CTL_ID_ESCAPE && j != controlY && opt.key[j][k] == b)
							opt.key[j][k] = 0;	// unset any other place that uses this key. You can put multiple copies on the same action though, if you want to override something else there
				}
			}
			optMode--;
			mgl->LastKeyPressed();
			UpdateControls();
			SetInMenu(true);
			return 0;
		}
		break;
	case OPTMODE_GAMEPADBIND: // pressing a joystick button
	case OPTMODE_QC_GAMEPADBIND:
		if (ButtonTapped(CONTROL_ESCAPE))
		{
			optMode -= 2;	// takes us to keyconfig or qc_config as appropriate
			UpdateControls();
			SetInMenu(true);
			return 0;
		}
		else
		{
			dword g = GetRawGamepad();
			dword taps = (g & (~prevGamePad));
			prevGamePad = g;

			if (taps != 0)
			{
				for (int i = 0; i <= (int)(RAWGAMEPADAXIS_BASE + RawGamepadAxis::RT); i++)
				{
					if ((i == RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_DN) ||
						(i == RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_UP) ||
						(i == RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_LF) ||
						(i == RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_RT) ||
						(i==SDL_CONTROLLER_BUTTON_START))
						continue; // can't bind left stick movement, it's fixed to movement, or START which is fixed to pause/exit

					if (taps & (1 << i))
					{
						if (optMode == OPTMODE_QC_GAMEPADBIND)
						{
							opt.joyCtrl[controlY + CTL_ID_QC_1] = i;
							for (int j = 0; j < NUM_CONTROLS; j++)
							{
								if (j != CTL_ID_ESCAPE && j != controlY+CTL_ID_QC_1 && opt.joyCtrl[j] == i)
									opt.joyCtrl[j] = 255;	// unset any other controls using the same button
							}
						}
						else
						{
							opt.joyCtrl[controlY] = i;
							for (int j = 0; j < NUM_CONTROLS; j++)
							{
								if (j != CTL_ID_ESCAPE && j != controlY && opt.joyCtrl[j] == i)
									opt.joyCtrl[j] = 255;	// unset any other controls using the same button
							}
						}
						optMode -= 2;
						UpdateControls();
						SetInMenu(true);
						return 0;
					}
				}
			}
		}
		break;
	}

	return 0;
}

void RenderGamepadButton(int x, int y, byte rawBtn)
{
	byte imgNum[] = {
		8,	// SDL_CONTROLLER_BUTTON_A,
		9,	// SDL_CONTROLLER_BUTTON_B,
		10, // SDL_CONTROLLER_BUTTON_X,
		11, // SDL_CONTROLLER_BUTTON_Y,
		22, // SDL_CONTROLLER_BUTTON_BACK, (select button img)
		24, // SDL_CONTROLLER_BUTTON_GUIDE, (just a ?, because I THINK this is the big "X" button on an xbox controller)
		23, // SDL_CONTROLLER_BUTTON_START,
		14, // SDL_CONTROLLER_BUTTON_LEFTSTICK,
		15, // SDL_CONTROLLER_BUTTON_RIGHTSTICK,
		12, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
		13, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
		19, //SDL_CONTROLLER_BUTTON_DPAD_UP,
		17, // SDL_CONTROLLER_BUTTON_DPAD_DOWN,
		18, // SDL_CONTROLLER_BUTTON_DPAD_LEFT,
		16, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
		25, //SDL_CONTROLLER_BUTTON_MISC1,    /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */ - i just made up some weird symbol
		26, //SDL_CONTROLLER_BUTTON_PADDLE1,  /* Xbox Elite paddle P1 */
		27, //SDL_CONTROLLER_BUTTON_PADDLE2,  /* Xbox Elite paddle P3 */
		28, //SDL_CONTROLLER_BUTTON_PADDLE3,  /* Xbox Elite paddle P2 */
		29, // SDL_CONTROLLER_BUTTON_PADDLE4,  /* Xbox Elite paddle P4 */
		30, //SDL_CONTROLLER_BUTTON_TOUCHPAD, /* PS4/PS5 touchpad button */
		0,	// empty spot
		0,	//	LS_UP,
		1,	//	LS_DN,
		2,	//	LS_LF,
		3,	//	LS_RT,
		4,	// RS_UP,
		5, // RS_DN,
		6, // RS_LF,
		7, // RS_RT,
		20, // LT,
		21, //RT,
	};

	int sx, sy;
	if (rawBtn == 255)
	{
		CenterPrint(x+8, y, "- - -", 0, 1);
	}
	else
	{
		sx = 384 + (imgNum[rawBtn] % 10) * 16;
		sy = 416 + (imgNum[rawBtn] / 10) * 16;
		if(opt.dpadToMove && ((rawBtn == RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_DN) ||
				(rawBtn == RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_LF) ||
				(rawBtn == RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_UP) ||
				(rawBtn == RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_RT)))
			{
				BlitIconBit(sx, sy, sx + 15, sy + 15, x-10, y, 255, 0);
				rawBtn = SDL_CONTROLLER_BUTTON_DPAD_UP + rawBtn - (RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_UP);
				sx = 384 + (imgNum[rawBtn] % 10) * 16;
				sy = 416 + (imgNum[rawBtn] / 10) * 16;
				BlitIconBit(sx, sy, sx + 15, sy + 15, x + 10, y, 255, 0);
			}
		else
			BlitIconBit(sx, sy, sx + 15, sy + 15, x, y, 255, 0);
	}
}

void RenderControls(int x,int y,MGLDraw *mgl)
{
	char btnTxt[64];
	int i;
	int yy;
	int hgt = 23;

	mgl->FillBox(x,y-2,x+398,y+14,16);
	CenterPrint(x+50,y,"Control",0,1);
	CenterPrint(x+150,y,"Keyboard1",0,1);
	CenterPrint(x+250,y,"Keyboard2",0,1);
	CenterPrint(x+350,y,"Gamepad",0,1);

	mgl->Box(x+98,y-2,x+198,y+15+hgt*8,16);
	mgl->Box(x+198,y-2,x+298,y+15+hgt*8,16);
	mgl->Box(x+298,y-2,x+398,y+15+hgt*8,16);

	for(i=0;i<8;i++)
	{
		yy = y + 15 + i * hgt;
		if((optMode==OPTMODE_KEYCONFIG || optMode==OPTMODE_GAMEPADBIND) && i<4)	// can't change gamepad movement, make it red
			mgl->FillBox(x + 99 + 100 * 2, yy, x + 198 + 100 * 2, yy+hgt-1, 32 * 4 + 6);

		if(controlY==i && controlX<3)
		{
			if (optMode == OPTMODE_KEYCONFIG || optMode==OPTMODE_GAMEPADBIND)
			{
				if(controlX==2 && controlY<4)	// you can't change the controls for movement on gamepad
					mgl->FillBox(x + 99 + 100 * controlX, yy, x + 198 + 100 * controlX, yy+hgt-1, 32*4+10);
				else
					mgl->FillBox(x + 99 + 100 * controlX, yy, x + 198 + 100 * controlX, yy+hgt-1, 20);
			}
			else
			{
				mgl->FillBox(x+99+100*controlX,yy,x+198+100*controlX,yy+hgt-1,31);
				CenterPrint(x+150+controlX*100,yy+6,"???",0,1);
			}
		}
		mgl->FillBox(x,yy,x+98,yy+hgt-1,10);
		mgl->Box(x,yy,x+398,yy+hgt,16);

		CenterPrint(x+50,yy+6,dirName[i],0,1);
		if(optMode==OPTMODE_KEYCONFIG || controlX!=0 || controlY!=i)
			CenterPrint(x+150,yy+6,ScanCodeText(opt.key[i][0]),0,1);
		if(optMode==OPTMODE_KEYCONFIG || controlX!=1 || controlY!=i)
			CenterPrint(x+250,yy+6,ScanCodeText(opt.key[i][1]),0,1);
		if(optMode==OPTMODE_KEYCONFIG || controlX!=2 || controlY!=i)
			RenderGamepadButton(x+350-8,yy+6,opt.joyCtrl[i]);
	}
}

void RenderQuickCast(int x, int y, MGLDraw* mgl)
{
	char btnTxt[64];
	int i;
	int hgt = 23;
	int yy;

	mgl->FillBox(x, y - 2, x + 398, y + 14, 16);
	CenterPrint(x + 50, y, "Spell", 0, 1);
	CenterPrint(x + 150, y, "Keyboard1", 0, 1);
	CenterPrint(x + 250, y, "Keyboard2", 0, 1);
	CenterPrint(x + 350, y, "Gamepad", 0, 1);

	mgl->Box(x + 98, y - 2, x + 198, y + 15 + hgt * 10, 16);
	mgl->Box(x + 198, y - 2, x + 298, y + 15 + hgt * 10, 16);
	mgl->Box(x + 298, y - 2, x + 398, y + 15 + hgt * 10, 16);

	for (i = 0; i < 10; i++)
	{
		yy = y + 15 + i * hgt;

		if (controlY == i && controlX < 3)
		{
			if (optMode == OPTMODE_QC_CONFIG)
			{
				mgl->FillBox(x + 99 + 100 * controlX, yy, x + 198 + 100 * controlX, yy+hgt-1, 20);
			}
			else
			{
				mgl->FillBox(x + 99 + 100 * controlX, yy, x + 198 + 100 * controlX, yy+hgt-1, 31);
				CenterPrint(x + 150 + controlX * 100, yy+6, "???", 0, 1);
			}
		}
		mgl->FillBox(x, yy, x + 98, yy + hgt - 1, 10);
		mgl->Box(x, yy, x + 398, yy + hgt, 16);

		sprintf(btnTxt, "Spell %d", i + 1);
		CenterPrint(x + 50, yy+6, btnTxt, 0, 1);
		if (optMode == OPTMODE_QC_CONFIG || controlX != 0 || controlY != i)
			CenterPrint(x + 150, yy+6, (opt.key[i+CTL_ID_QC_1][0]==0)?"- - -":ScanCodeText(opt.key[i + CTL_ID_QC_1][0]), 0, 1);
		if (optMode == OPTMODE_QC_CONFIG || controlX != 1 || controlY != i)
			CenterPrint(x + 250, yy+6, (opt.key[i + CTL_ID_QC_1][1] == 0) ? "- - -" : ScanCodeText(opt.key[i+CTL_ID_QC_1][1]), 0, 1);
		if (optMode == OPTMODE_QC_CONFIG || controlX != 2 || controlY != i)
			RenderGamepadButton(x + 350 - 8, yy+6, opt.joyCtrl[i+CTL_ID_QC_1]);
	}
}

void RenderOptionsMenu(MGLDraw *mgl)
{
	char onoff[6][8]={"Off","I","II","III","IV","V"};
	char fxonoff[2][8]={"Off","On"};
	char quickCast[3][16] = { "Select Only","Cast & Select","Cast Only" };
	char optionsList[][32] = {
		"Sound:",
		"Music:",
		"Classic Music:",
		"Fancy Water:",
		"Quick Cast:",
		"D-Pad To Move:",
		"Configure Controls",
		"Configure Quick Cast",
		"Reset Controls",
		"Exit To Main Menu",
	};

	char s[64];
	bool dpadWarning=false;

	mgl->ClearScreen();
	CenterPrint(HALFWID,2,"Game Options",0,0);

	if (optMode == OPTMODE_VERIFY_DEFAULTS)
	{
		CenterPrint(HALFWID, 50, "Really reset all controls?", 0, 1);
		int x = HALFWID / 2 + 35;
		int y = 70;
		if (cursor == 0)
			DrawFillBox(x - 45, y - 2, x + 115, y + 14, 10);
		Print(x - 40, y, "Yes", 0, 1);
		x = HALFWID + 30 + 40;
		if (cursor == 1)
			DrawFillBox(x - 45, y - 2, x + 115, y + 14, 10);
		Print(x - 40, y, "No", 0, 1);
	}
	else
	{
		int x = HALFWID / 2;
		int y = 50;
		for (int i = 0; i <= OPT_EXIT; i++)
		{
			if (i == firstRightHandOption)
			{
				y = 50;
				x = HALFWID + 30 + 40+35;
			}
			if (cursor == i)
			{
				if(cursor==OPT_SPELL_QUICK)
					DrawFillBox(x - 45, y - 2, x + 185, y + 14, 10);
				else
					DrawFillBox(x - 45, y - 2, x + 115, y + 14, 10);
			}
			Print(x - 40, y, optionsList[i], 0, 1);

			switch (i)
			{
			case OPT_SOUNDVOL:
				sprintf(s, "%s", onoff[opt.soundVol]);
				break;
			case OPT_MUSICVOL:
				sprintf(s, "%s", onoff[opt.musicVol]);
				break;
			case OPT_CLASSICMUSIC:
				sprintf(s, "%s", fxonoff[opt.classicMusic]);
				break;
			case OPT_FANCYWATER:
				sprintf(s, "%s", fxonoff[opt.waterFX]);
				break;
			case OPT_DPADTOMOVE:
				sprintf(s, "%s", fxonoff[opt.dpadToMove]);
				break;
			case OPT_SPELL_QUICK:
				sprintf(s, "%s", quickCast[opt.quickCast]);
				break;
			default:
				s[0] = '\0';	// the rest have no bonus text
				break;
			}
			Print(x + 135 - 50, y, s, 0, 1);
			y += 20;
		}
	}

	if ((optMode == OPTMODE_IDLE && cursor == OPT_CONFIGCAST) ||
		optMode == OPTMODE_QC_CONFIG || optMode == OPTMODE_QC_GAMEPADBIND || optMode == OPTMODE_QC_KEYBIND)
		RenderQuickCast(120, 160, mgl);
	else
		RenderControls(120, 160, mgl);

	if (optMode == OPTMODE_IDLE)
	{
		switch (cursor)
		{
			case OPT_SOUNDVOL:
				CenterPrint(HALFWID, 460, "This is how loud the sound effects are. You know.", 0, 1);
				break;
			case OPT_MUSICVOL:
				CenterPrint(HALFWID,460, "Hey man, is that Mystic Rock? Well turn it up, man!", 0, 1);
				break;
			case OPT_FANCYWATER:
				CenterPrint(HALFWID, 460, "This is a holdover from the Pentium 386 days. Leave it on.", 0, 1);
				break;
			case OPT_CLASSICMUSIC:
				CenterPrint(HALFWID, 460, "Listen to the original Kid Mystic soundtrack, in all its lo-fi glory.", 0, 1);
				break;
			case OPT_SPELL_QUICK:
				if (opt.quickCast == QUICKCAST_CASTANDSELECT)
					CenterPrint(HALFWID, 440, "Pressing a specific Spell button both casts the spell and selects it.", 0, 1);
				else if (opt.quickCast == QUICKCAST_CASTONLY)
					CenterPrint(HALFWID, 440, "Pressing a specific Spell button casts the spell immediately, without selecting it.", 0, 1);
				else if (opt.quickCast == QUICKCAST_SELECTONLY)
					CenterPrint(HALFWID, 440, "Pressing a specific Spell button only selects the spell, without casting it.", 0, 1);

				CenterPrint(HALFWID, 460, "Press the Cast button to cast your selected spell.", 0, 1);
				break;
			case OPT_DPADTOMOVE:
				for (int i = CTL_ID_B1; i < NUM_CONTROLS; i++)
				{
					if (opt.joyCtrl[i] == SDL_CONTROLLER_BUTTON_DPAD_DOWN ||
						opt.joyCtrl[i] == SDL_CONTROLLER_BUTTON_DPAD_UP ||
						opt.joyCtrl[i] == SDL_CONTROLLER_BUTTON_DPAD_LEFT ||
						opt.joyCtrl[i] == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
						dpadWarning = true;
				}
				if (opt.dpadToMove && dpadWarning)
				{
					CenterPrint(HALFWID, 440, "*** WARNING: You have actions assigned to D-Pad directions! ***", 0, 1);
					CenterPrint(HALFWID, 460, "Pressing those will also move you if D-Pad to move is on.", 0, 1);
				}
				else
				{
					CenterPrint(HALFWID, 440, "When disabled, only the Left Stick moves Kid Mystic,", 0, 1);
					CenterPrint(HALFWID, 460, "so you can assign actions to the dpad directions.", 0, 1);
				}
				break;
			case OPT_DEFAULTS:
				CenterPrint(HALFWID, 440, "Resets all controls to their defaults in case you", 0, 1);
				CenterPrint(HALFWID, 460, "made a terrible mess of things.", 0, 1);
				break;
		}
	}
	else if (optMode == OPTMODE_QC_CONFIG)
	{
		CenterPrint(HALFWID, 460, "Select a key or button to change", 0, 1);
	}
	else if (optMode == OPTMODE_QC_KEYBIND)
	{
		sprintf(s, "Press a key for Spell %d", controlY + 1);
		CenterPrint(HALFWID, 440, s, 0, 1);
		CenterPrint(HALFWID, 460, "ESC to cancel", 0, 1);
	}
	else if (optMode == OPTMODE_QC_GAMEPADBIND)
	{
		sprintf(s, "Press a gamepad button for Spell %d", controlY + 1);
		CenterPrint(HALFWID, 440, s, 0, 1);
		CenterPrint(HALFWID, 460, "ESC or START to cancel", 0, 1);
	}
	else if (optMode == OPTMODE_KEYCONFIG)
	{
		CenterPrint(HALFWID, 460, "Select a key or button to change", 0, 1);
	}
	else if (optMode == OPTMODE_KEYBIND)
	{
		sprintf(s, "Press a key for %s", dirName[controlY]);
		CenterPrint(HALFWID, 440, s, 0, 1);
		CenterPrint(HALFWID, 460, "ESC to cancel", 0, 1);
	}
	else if (optMode == OPTMODE_GAMEPADBIND)
	{
		sprintf(s, "Press a gamepad button for %s", dirName[controlY]);
		CenterPrint(HALFWID, 440, s, 0, 1);
		CenterPrint(HALFWID, 460, "ESC or START to cancel", 0, 1);
	}
}

TASK(void) OptionsMenu(MGLDraw *mgl)
{
	byte b=0;
	int lastTime=1;

	mgl->LastKeyPressed();

	StartClock();
	if(CurrentSongAdjusted()!=SONG_SHOP && CurrentSongAdjusted()!=SONG_INTRO)
		PlaySong(SONG_SHOP);
	InitOptionsMenu();

	while(b==0)
	{
		UpdateControls();
		UpdateSounds();

		lastTime+=TimeLength();
		StartClock();
		b=UpdateOptionsMenu(mgl);
		RenderOptionsMenu(mgl);
		AWAIT mgl->Flip();
		if(!mgl->Process())
		{
			ExitOptionsMenu();
			CO_RETURN;
		}

		EndClock();
	}
	ExitOptionsMenu();
}

bool CheatStoneOn(CheatStone c)
{
	return opt.cheatStone[(int)c] == CHEATSTONE_ON;
}
