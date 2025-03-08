#include "options.h"
#include "display.h"
#include "control.h"
#include "clock.h"
#include "sound.h"
#include "appdata.h"

option_t opt;

void ApplyControlSettings()
{
	for (int kbd = 0; kbd < 3; ++kbd)
	{
		byte keys[NUM_CONTROLS];
		for (int k = 0; k < NUM_CONTROLS; ++k)
			keys[k] = opt.key[k][kbd];
		SetKeyboardBindings(kbd, NUM_CONTROLS, keys);
	}

	SetJoystickBindings(NUM_CONTROLS, opt.joyCtrl);
}

void DefaultOptions(void)
{
	// default values
	for (int i = 0; i < NUM_CONTROLS; i++)
	{
		for (int j = 0; j < 3; j++)
			opt.key[i][j] = 0;
		opt.joyCtrl[i] = 255;
	}
	opt.challenge = 0;
	// these are the arrow keys,  including the unchangable ones
	opt.key[CTL_ID_UP][0] = opt.key[CTL_ID_UP][1] = opt.key[CTL_ID_UP][2] = SDL_SCANCODE_UP;
	opt.key[CTL_ID_DN][0] = opt.key[CTL_ID_DN][1] = opt.key[CTL_ID_DN][2] = SDL_SCANCODE_DOWN;
	opt.key[CTL_ID_LF][0] = opt.key[CTL_ID_LF][1] = opt.key[CTL_ID_LF][2] = SDL_SCANCODE_LEFT;
	opt.key[CTL_ID_RT][0] = opt.key[CTL_ID_RT][1] = opt.key[CTL_ID_RT][2] = SDL_SCANCODE_RIGHT;
	// fire key
	opt.key[CTL_ID_B1][0] = SDL_SCANCODE_LCTRL;	// CTRL
	opt.key[CTL_ID_B1][1] = SDL_SCANCODE_Z;	// Z
	opt.key[CTL_ID_B1][2] = SDL_SCANCODE_RETURN;	// enter
	// spell key
	opt.key[CTL_ID_B2][0] = SDL_SCANCODE_LSHIFT;	// leftshift
	opt.key[CTL_ID_B2][1] = SDL_SCANCODE_X;	// X
	opt.key[CTL_ID_B2][2] = 0;	// no unchangable key
	// previous
	opt.key[CTL_ID_B3][0] = SDL_SCANCODE_A;	// A
	opt.key[CTL_ID_B3][1] = SDL_SCANCODE_F1;	// F1
	opt.key[CTL_ID_B3][2] = 0;	// no unchangable key
	// next
	opt.key[CTL_ID_B4][0] = SDL_SCANCODE_S;	// S
	opt.key[CTL_ID_B4][1] = SDL_SCANCODE_F2;	// F2
	opt.key[CTL_ID_B4][2] = 0;	// no unchangable
	opt.key[CTL_ID_ESCAPE][0] = SDL_SCANCODE_ESCAPE;
	opt.key[CTL_ID_ESCAPE][1] = SDL_SCANCODE_ESCAPE;
	opt.key[CTL_ID_ESCAPE][2] = SDL_SCANCODE_ESCAPE;
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

	opt.soundVol = 3;
	opt.musicVol = 2;
	opt.waterFX = 1;
	opt.lightFX = 1;
	for (int i = 0; i < (int)Achievement::NUM_ACHIEVES; i++)
		opt.achieve[i] = 0;
	for (int i = 0; i < (int)256; i++)
		opt.expansionSpace[i] = 0;
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
	optMode=0;
}

void ExitOptionsMenu(void)
{
}

byte UpdateOptionsMenu(MGLDraw *mgl)
{
	char c;
	dword btn,j;
	int i;

	c = mgl->LastKeyPressed();
	
	switch (optMode)
	{
	case 0:	// just going through options
		if (ButtonTapped(CONTROL_ESCAPE,true))
			return 1;
		
		if (AutoRepeatTapped(CONTROL_UP))
		{
			cursor--;
			if (cursor > 5)
				cursor = 5;
		}
		if (AutoRepeatTapped(CONTROL_DN))
		{
			cursor++;
			if (cursor > 5)
				cursor = 0;
		}
		if (ButtonTapped(CONTROL_B2,true))
		{
			return 1;
		}
		if (ButtonTapped(CONTROL_B1,true))
		{
			switch (cursor)
			{
			case 0:
				opt.soundVol--;
				if (opt.soundVol > 5)
					opt.soundVol = 5;
				VolumeSound(opt.soundVol);
				MakeNormalSound(SND_GOATSHOOT);
				break;
			case 1:
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
			case 2:
				opt.waterFX = 1 - opt.waterFX;
				break;
			case 3:
				opt.lightFX = 1 - opt.lightFX;
				break;
			case 4:
				optMode = 1;
				controlX = 0;
				controlY = 0;
				break;
			case 5:
				return 1;
				break;
			}
		}
		break;
	case 1:	// selecting keys to configure
		if (ButtonTapped(CONTROL_B2|CONTROL_ESCAPE,true))
		{
			optMode = 0;
			controlX = 10;
			ApplyControlSettings();
			return 0;
		}
		
		if (AutoRepeatTapped(CONTROL_UP))
		{
			controlY--;
			if (controlY > 7)
				controlY = 7;
		}
		if (AutoRepeatTapped(CONTROL_DN))
		{
			controlY++;
			if (controlY > 7)
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
		if (ButtonTapped(CONTROL_B1,true))
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
		if (ButtonTapped(CONTROL_ESCAPE,true))	// ESC key
		{
			optMode = 1;
			mgl->LastKeyPressed();
			return 0;
		}
		c = LastScanCode();

		if (c != 0)
		{
			opt.key[controlY][controlX] = c;
			optMode = 1;
			mgl->LastKeyPressed();
		}
		break;
	case 3: // pressing a joystick button
		if (ButtonTapped(CONTROL_ESCAPE,true))
		{
			optMode=1;
			return 0;
		}
		/*btn = GetJoyButtons();

		j=1;
		for(i=0;i<16;i++)
		{
			if((btn&j) && !(oldBtn&j))
			{
				opt.joyCtrl[controlY-4]=i;
				optMode=1;
				c2=255;
			}
			j*=2;
		}
		oldBtn=btn;
		*/
		break;
	}

	return 0;
}

void RenderGamepadButton(int x, int y, byte btn)
{

}

void RenderControls(int x,int y,MGLDraw *mgl)
{
	static const char dirName[8][12]={"Up","Down","Left","Right","Fire","Spell","Prev Spl","Next Spl"};
	char btnTxt[64];
	int i;

	mgl->FillBox(x,y-2,x+398,y+20,16);
	CenterPrint(x+50,y,"Control",0,1);
	CenterPrint(x+150,y,"Keyboard1",0,1);
	CenterPrint(x+250,y,"Keyboard2",0,1);
	CenterPrint(x+350,y,"Gamepad",0,1);
	mgl->Box(x+98,y-2,x+198,y+260,16);
	mgl->Box(x+198,y-2,x+298,y+260,16);
	mgl->Box(x+298,y-2,x+398,y+260,16);

	for(i=0;i<8;i++)
	{
		if(controlY==i && controlX<3)
		{
			if(optMode==1)
				mgl->FillBox(x+99+100*controlX,y+20+1+i*30,x+198+100*controlX,y+20+29+i*30,20);
			else
			{
				mgl->FillBox(x+99+100*controlX,y+20+1+i*30,x+198+100*controlX,y+20+29+i*30,31);
				CenterPrint(x+150+controlX*100,y+27+i*30,"???",0,1);
			}
		}
		mgl->FillBox(x,y+20+1+i*30,x+98,y+20+29+i*30,10);
		mgl->Box(x,y+20+i*30,x+398,y+20+30+i*30,16);

		CenterPrint(x+50,y+27+i*30,dirName[i],0,1);
		if(optMode==1 || controlX!=0 || controlY!=i)
			CenterPrint(x+150,y+27+i*30,ScanCodeText(opt.key[i][0]),0,1);
		if(optMode==1 || controlX!=1 || controlY!=i)
			CenterPrint(x+250,y+27+i*30,ScanCodeText(opt.key[i][1]),0,1);

		if(i>3)
		{
			if(optMode==1 || controlX!=2 || controlY!=i)
			{
				sprintf(btnTxt,"Button %d",opt.joyCtrl[i-4]+1);
				CenterPrint(x+350,y+27+i*30,btnTxt,0,1);
			}
		}
		else
		{
			sprintf(btnTxt,"D-Pad %s",dirName[i]);
			CenterPrint(x+350,y+27+i*30,btnTxt,0,1);
		}
	}
	if(optMode==0)
	{
		CenterPrint(x+200,y+292,"Move with arrow keys, ENTER to select",0,1);
		CenterPrint(x+200,y+312,"ESC to return to main menu",0,1);
	}
	else if(optMode==1)
	{
		CenterPrint(x+200,y+292,"Select with arrow keys, ENTER to set new control",0,1);
		CenterPrint(x+200,y+312,"ESC to return to options",0,1);
	}
	else if(optMode==2)
	{
		sprintf(btnTxt,"Press a key for %s",dirName[controlY]);
		CenterPrint(x+200,y+292,btnTxt,0,1);
		CenterPrint(x+200,y+312,"ESC to cancel",0,1);
	}
	else if(optMode==3)
	{
		sprintf(btnTxt,"Press a gamepad button for %s",dirName[controlY]);
		CenterPrint(x+200,y+292,btnTxt,0,1);
		CenterPrint(x+200,y+312,"ESC or START to cancel",0,1);
	}
}

void RenderOptionsMenu(MGLDraw *mgl)
{
	char onoff[6][8]={"Off","I","II","III","IV","V"};
	char fxonoff[2][8]={"Off","On"};

	mgl->ClearScreen();
	CenterPrint(HALFWID,2,"Game Options",0,0);

	DrawFillBox(250,40-1+20*cursor,390,40+17+20*cursor,10);

	CenterPrint(HALFWID,40,"Sound",0,1);
	Print(392,40,onoff[opt.soundVol],0,1);
	CenterPrint(HALFWID,60,"Music",0,1);
	Print(392,60,onoff[opt.musicVol],0,1);
	CenterPrint(HALFWID,80,"Fancy Water:",0,1);
	Print(392,80,fxonoff[opt.waterFX],0,1);

	CenterPrint(HALFWID,100,"Fancy Lighting:",0,1);
	Print(392,100,fxonoff[opt.lightFX],0,1);

	//Print(500,120,"(You can turn these settings off",0,1);
	//Print(500,135," if the game is slow or jerky, to",0,1);
	//Print(500,150," get better performance)",0,1);

	CenterPrint(HALFWID,120,"Configure Controls",0,1);
	CenterPrint(HALFWID,140,"Exit To Main Menu",0,1);

	RenderControls(120,170,mgl);
}

TASK(void) OptionsMenu(MGLDraw *mgl)
{
	byte b=0;
	int lastTime=1;

	mgl->LastKeyPressed();
	
	StartClock();
	if(CurrentSong()!=SONG_SHOP && CurrentSong()!=SONG_INTRO)
		PlaySong(SONG_SHOP);
	InitOptionsMenu();

	while(b==0)
	{
		UpdateControls();
		JamulSoundUpdate();

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
