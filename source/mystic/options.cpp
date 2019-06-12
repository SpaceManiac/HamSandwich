#include "options.h"
#include "display.h"
#include "control.h"
#include "clock.h"
#include "sound.h"

option_t opt;

void ApplyControlSettings()
{
	for (int kbd = 0; kbd < 3; ++kbd) {
		byte keys[8];
		for (int k = 0; k < 8; ++k)
			keys[k] = opt.key[k][kbd];
		SetKeyboardBindings(kbd, 8, keys);
	}

	SetJoystickBindings(4, opt.joyCtrl);
}

void InitOptions(void)
{
	FILE *f;

	f=fopen("options.cfg","rb");
	if(!f)
	{
		opt.challenge=0;
		// these are the arrow keys,  including the unchangable ones
		opt.key[0][0]=opt.key[0][1]=opt.key[0][2]=SDL_SCANCODE_UP;
		opt.key[1][0]=opt.key[1][1]=opt.key[1][2]=SDL_SCANCODE_DOWN;
		opt.key[2][0]=opt.key[2][1]=opt.key[2][2]=SDL_SCANCODE_LEFT;
		opt.key[3][0]=opt.key[3][1]=opt.key[3][2]=SDL_SCANCODE_RIGHT;
		// fire key
		opt.key[4][0]=SDL_SCANCODE_LCTRL;	// CTRL
		opt.key[4][1]=SDL_SCANCODE_Z;	// Z
		opt.key[4][2]=SDL_SCANCODE_RETURN;	// enter
		// spell key
		opt.key[5][0]=SDL_SCANCODE_LSHIFT;	// leftshift
		opt.key[5][1]=SDL_SCANCODE_X;	// X
		opt.key[5][2]=0;	// no unchangable key
		// previous
		opt.key[6][0]=SDL_SCANCODE_A;	// A
		opt.key[6][1]=SDL_SCANCODE_F1;	// F1
		opt.key[6][2]=0;	// no unchangable key
		// next
		opt.key[7][0]=SDL_SCANCODE_S;	// S
		opt.key[7][1]=SDL_SCANCODE_F2;	// F2
		opt.key[7][2]=0;	// no unchangable

		// joystick buttons
		opt.joyCtrl[0]=0;
		opt.joyCtrl[1]=1;
		opt.joyCtrl[2]=2;
		opt.joyCtrl[3]=3;

		opt.soundVol=3;
		opt.musicVol=2;
		opt.waterFX=1;
		opt.lightFX=1;
	}
	else
	{
		fread(&opt,sizeof(option_t),1,f);
		fclose(f);
	}
	ApplyControlSettings();
}

void ExitOptions(void)
{
	FILE *f;
	f=fopen("options.cfg","wb");
	fwrite(&opt,sizeof(option_t),1,f);
	fclose(f);
}


//-----------------------------------------------
static byte oldc;
static byte cursor,optMode;
static dword oldBtn;
static byte controlX,controlY;

void InitOptionsMenu(void)
{
	oldc=255;
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
	byte c2;
	dword btn,j;
	int i;

	switch(optMode)
	{
		case 0:	// just going through options
			c=mgl->LastKeyPressed();
			c2=GetControls()|GetArrows();

			if(c==27)
			{
				return 1;
			}

			if((c2&CONTROL_UP) && (!(oldc&CONTROL_UP)))
			{
				cursor--;
				if(cursor>5)
					cursor=5;
			}
			if((c2&CONTROL_DN) && (!(oldc&CONTROL_DN)))
			{
				cursor++;
				if(cursor>5)
					cursor=0;
			}
			if((c2&(CONTROL_B1|CONTROL_B2)) && (!(oldc&(CONTROL_B1|CONTROL_B2))))
			{
				switch(cursor)
				{
					case 0:
						opt.soundVol++;
						if(opt.soundVol>3)
							opt.soundVol=0;
						VolumeSound(opt.soundVol);
						MakeNormalSound(SND_GOATSHOOT);
						break;
					case 1:
						opt.musicVol++;
						if(opt.musicVol>3)
							opt.musicVol=0;
						if(opt.musicVol==0)
							StopSong();
						else
							VolumeSong(opt.musicVol);
						if(opt.musicVol==1)
							ReplaySong();
						break;
					case 2:
						opt.waterFX=1-opt.waterFX;
						break;
					case 3:
						opt.lightFX=1-opt.lightFX;
						break;
					case 4:
						optMode=1;
						controlX=0;
						controlY=0;
						break;
					case 5:
						return 1;
						break;
				}
			}
			break;
		case 1:	// selecting keys to configure
			c=mgl->LastKeyPressed();
			c2=GetControls()|GetArrows();

			if(c==27)
			{
				optMode=0;
				controlX=10;
				ApplyControlSettings();
				return 0;
			}
			if(c==13)
				c2|=CONTROL_B1;

			if((c2&CONTROL_UP) && (!(oldc&CONTROL_UP)))
			{
				controlY--;
				if(controlY>7)
					controlY=7;
			}
			if((c2&CONTROL_DN) && (!(oldc&CONTROL_DN)))
			{
				controlY++;
				if(controlY>7)
					controlY=0;
			}
			if((c2&CONTROL_LF) && (!(oldc&CONTROL_LF)))
			{
				controlX--;
				if(controlX>2)
					controlX=2;
			}
			if((c2&CONTROL_RT) && (!(oldc&CONTROL_RT)))
			{
				controlX++;
				if(controlX>2)
					controlX=0;
			}
			if((c2&(CONTROL_B1|CONTROL_B2|CONTROL_B3)) && (!(oldc&(CONTROL_B1|CONTROL_B2|CONTROL_B3))))
			{
				if(controlX<2)
				{
					// keyboard
					optMode=2;
					LastScanCode();
				}
				else if(controlY>3)
				{
					btn=0;
					oldBtn=~0;
					optMode=3;
				}
			}
			break;
		case 2: // entering a specific key
			c2=LastScanCode();
			if(c2==1)	// ESC key
			{
				optMode=1;
				c2=255;
				oldc=255;
				mgl->LastKeyPressed();
				return 0;
			}
			if(c2!=0)
			{
				opt.key[controlY][controlX]=c2;
				optMode=1;
				mgl->LastKeyPressed();
			}
			c2=255;
			break;
		case 3: // pressing a joystick button
			c=mgl->LastKeyPressed();
			if(c==27)
			{
				optMode=1;
				c2=255;
				oldc=255;
				return 0;
			}
			btn=GetJoyButtons();

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
			break;
	}

	oldc=c2;

	return 0;
}

void RenderControls(int x,int y,MGLDraw *mgl)
{
	char dirName[8][12]={"Up","Down","Left","Right","Fire","Spell","Prev Spl","Next Spl"};
	char btnTxt[64];
	int i;

	mgl->FillBox(x,y-2,x+398,y+20,16);
	CenterPrint(x+50,y,"Control",0,1);
	CenterPrint(x+150,y,"Keyboard1",0,1);
	CenterPrint(x+250,y,"Keyboard2",0,1);
	CenterPrint(x+350,y,"Joystick",0,1);
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
			sprintf(btnTxt,"Joy %s",dirName[i]);
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
		sprintf(btnTxt,"Press a joystick button for %s",dirName[controlY]);
		CenterPrint(x+200,y+292,btnTxt,0,1);
		CenterPrint(x+200,y+312,"ESC to cancel",0,1);
	}
}

void RenderOptionsMenu(MGLDraw *mgl)
{
	char onoff[4][8]={"Off","I","II","III"};
	char fxonoff[2][8]={"Off","On"};

	mgl->ClearScreen();
	CenterPrint(320,2,"Game Options",0,0);

	DrawFillBox(250,40-1+20*cursor,390,40+17+20*cursor,10);

	CenterPrint(320,40,"Sound",0,1);
	Print(392,40,onoff[opt.soundVol],0,1);
	CenterPrint(320,60,"Music",0,1);
	Print(392,60,onoff[opt.musicVol],0,1);
	CenterPrint(320,80,"Fancy Water:",0,1);
	Print(392,80,fxonoff[opt.waterFX],0,1);

	CenterPrint(320,100,"Fancy Lighting:",0,1);
	Print(392,100,fxonoff[opt.lightFX],0,1);

	//Print(500,120,"(You can turn these settings off",0,1);
	//Print(500,135," if the game is slow or jerky, to",0,1);
	//Print(500,150," get better performance)",0,1);

	CenterPrint(320,120,"Configure Controls",0,1);
	CenterPrint(320,140,"Exit To Main Menu",0,1);

	RenderControls(120,170,mgl);
}

void OptionsMenu(MGLDraw *mgl)
{
	byte b=0;
	int lastTime=1;

	mgl->LastKeyPressed();
	oldc=CONTROL_B1|CONTROL_B2;

	StartClock();
	if(CurrentSong()!=SONG_SHOP && CurrentSong()!=SONG_INTRO)
		PlaySong(SONG_SHOP);
	InitOptionsMenu();

	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdateOptionsMenu(mgl);
		RenderOptionsMenu(mgl);
		mgl->Flip();
		if(!mgl->Process())
		{
			ExitOptionsMenu();
			return;
		}

		EndClock();
	}
	ExitOptionsMenu();
}
