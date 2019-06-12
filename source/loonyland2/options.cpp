#include "options.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "title.h"
#include "pause.h"
#include "music.h"

static byte subcursor=0;
static char lastKey=0;
static byte subMode;
static byte darkness;
static int offX;
static byte oldc;

options_t opt;

void ExitOptionsMenu(void)
{
	SaveOptions();
}

void InitOptionsMenu(void)
{
	MakeNormalSound(SND_PAUSE);
	lastKey=0;
	subMode=SUBMODE_OPTIONS;
	subcursor=0;
	darkness=0;
	offX=400;
	oldc=255;
}

void HandleOptionKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

byte UpdateOptionsMenu(MGLDraw *mgl)
{
	byte c;
	static byte reptCounter=0;
	int i,j;

	if(darkness<30)
		darkness++;
	if(offX>-100)
	{
		offX-=32;
		if(offX<-100)
			offX=-100;
	}

	c=GetControls()|GetArrows();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if(subMode==SUBMODE_OPTIONS)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			subcursor--;
			if(subcursor==2)
				subcursor=1;
			if(subcursor>2)
				subcursor-=2;
			if(subcursor==5 || subcursor==8 || subcursor==11 || subcursor==14)	// invalid spots
				subcursor--;
			if(subcursor>23)
				subcursor=21;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			subcursor++;
			if(subcursor==2)
				subcursor++;
			if(subcursor>3)
				subcursor+=2;
			if(subcursor>23)
				subcursor=0;
		}
		if((c&CONTROL_LF) && (!reptCounter))
		{
			if(subcursor>2 && subcursor<15)	// in the arrow key area
			{
				if(subcursor%3)
					subcursor--;
				else
					subcursor++;
			}
			else if(subcursor>14)
			{
				if(!(subcursor%3))
					subcursor+=2;
				else
					subcursor--;
			}
		}
		if((c&CONTROL_RT) && (!reptCounter))
		{
			if(subcursor>2 && subcursor<15)	// in the arrow key area
			{
				if(subcursor%3)
					subcursor--;
				else
					subcursor++;
			}
			else if(subcursor>14)
			{
				if((subcursor%3)==2)
					subcursor-=2;
				else
					subcursor++;
			}
		}
		if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
		{
			switch(subcursor)
			{
				case 0:	// sound
					if(opt.sound>=127)
						opt.sound=0;
					else
					{
						opt.sound+=26;
						if(opt.sound>127)
							opt.sound=127;
					}
					JamulSoundVolume(opt.sound*2);
					MakeNormalSound(SND_MENUSELECT);
					break;
				case 1:	// music
					if(opt.music>=127)
						opt.music=0;
					else
					{
						opt.music+=26;
						if(opt.music>127)
							opt.music=127;
					}
					SetMusicVolume(opt.music*2);
					MakeNormalSound(SND_MENUSELECT);
					break;
				case 2:	// commentary
					break;
				default:	// key setting
					if((subcursor%3)==2)	// joystick
					{
						subMode=SUBMODE_SETBTN;
						mgl->ClearKeys();
						i=0;
						while(i<10000 && GetJoyButtons())
						{
							i++;
							SDL_Delay(1);
						}
						MakeNormalSound(SND_MENUSELECT);
					}
					else
					{
						subMode=SUBMODE_SETKEY;
						mgl->ClearKeys();
						mgl->LastKeyPressed();
						LastScanCode();
						lastKey=0;
						MakeNormalSound(SND_MENUSELECT);
					}
					break;
			}
			SaveOptions();
		}
		if((c&CONTROL_B2) && (!(oldc&CONTROL_B2)))
		{
			// exit the options menu
			return 1;
		}
	}
	else if(subMode==SUBMODE_SETKEY)
	{
		byte r;

		r=LastScanCode();
		if(r==SDL_SCANCODE_ESCAPE)
		{
			subMode=SUBMODE_OPTIONS;	// escape
			MakeNormalSound(SND_MENUCANCEL);
			mgl->LastKeyPressed();
		}
		else if(r!=0)
		{
			for(i=0;i<7;i++)
				for(j=0;j<2;j++)
					if(opt.control[j][i]==r)
						opt.control[j][i]=0;
			opt.control[(subcursor%3)][(subcursor-3)/3]=r;
			subMode=SUBMODE_OPTIONS;
			MakeNormalSound(SND_MENUSELECT);
			ApplyControlSettings();
			SaveOptions();
		}
	}
	else if(subMode==SUBMODE_SETBTN)
	{
		dword btn;

		btn=GetJoyButtons();

		j=1;
		for(i=0;i<32;i++)
		{
			if((btn&j))
			{
				opt.joyCtrl[(subcursor-17)/3]=i;
				subMode=SUBMODE_OPTIONS;
				MakeNormalSound(SND_MENUSELECT);
				ApplyControlSettings();
				SaveOptions();
			}
			j*=2;
		}
	}

	oldc=c;

	HandleOptionKeyPresses(mgl);
	if(lastKey==27)	// hit ESC to exit pause menu
	{
		if(subMode==SUBMODE_SETKEY || subMode==SUBMODE_SETBTN)
			subMode=SUBMODE_OPTIONS;
		else
			return 1;

		lastKey=0;
	}
	return 0;
}

static void OptionButton(int x,int y,int wid,const char *txt,byte spot)
{
	if(subcursor!=spot || subMode!=SUBMODE_OPTIONS)
		PauseBox(x,y,x+wid,y+28,240);
	else if(subMode==SUBMODE_OPTIONS)
		PauseBox(x,y,x+wid,y+28,32*5+16);
	CenterPrintDark(x+wid/2,y+2,txt,0);
}

static void RenderSetKey(int x,int y,byte t)
{
	char actName[7][16]={
		"Up",
		"Down",
		"Left",
		"Right",
		"Fire",
		"Jump",
		"Spell",
	};

	PauseBox(x+130,y+80,x+630,y+240,234);

	if(t==0)
		CenterPrintGlow(x+380,y+100,"Press a key to assign to",0,0);
	if(t==1)
		CenterPrintGlow(x+380,y+100,"Press a gamepad button to assign to",0,0);
	CenterPrintGlow(x+380,y+150,actName[((subcursor-3)/3)],0,0);

	CenterPrintGlow(x+380,y+204,"Or ESC to cancel",0,0);
}

void RenderOptionsMenu(int x,int y,byte *backScr)
{
	int i,j;
	char s[48];

	for(i=0;i<480;i++)
		memcpy(GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,&backScr[i*640],640);

	PauseBox(x+202,y+5,x+638,y+448,234);
	PrintGlow(x+251,y+8,"Game Options",0,0);

	x+=206;
	y+=34;

	OptionButton(x,y,180,"Sound",0);
	PauseBox(x+185,y+4,x+185+133,y+24,234);
	DrawFillBox(x+188,y+8,x+188+opt.sound,y+20,31);
	y+=31;
	OptionButton(x,y,180,"Music",1);
	PauseBox(x+185,y+4,x+185+133,y+24,234);
	DrawFillBox(x+188,y+8,x+188+opt.music,y+20,31);
	y+=31;
	//if(1)	// if commentary is allowed
		//OptionButton(x,y,180,"Commentary",2);
	//else
	CenterPrintDark(x+78+1*116+112/2,y+2,"Controls",0);
	y+=31;

	CenterPrintDark(x+78+0*116+112/2,y+2,"Key 1",0);
	CenterPrintDark(x+78+1*116+112/2,y+2,"Key 2",0);
	CenterPrintDark(x+78+2*116+112/2,y+2,"Gamepad",0);
	y+=31;

	PrintDark(x+2,y+0*31+2,"Up",0);
	PrintDark(x+2,y+1*31+2,"Down",0);
	PrintDark(x+2,y+2*31+2,"Left",0);
	PrintDark(x+2,y+3*31+2,"Right",0);
	PrintDark(x+2,y+4*31+2,"Fire",0);
	PrintDark(x+2,y+5*31+2,"Jump",0);
	PrintDark(x+2,y+6*31+2,"Spell",0);

	CenterPrintDark(x+78+2*116+112/2,y+0*31+2,"Up",0);
	CenterPrintDark(x+78+2*116+112/2,y+1*31+2,"Down",0);
	CenterPrintDark(x+78+2*116+112/2,y+2*31+2,"Left",0);
	CenterPrintDark(x+78+2*116+112/2,y+3*31+2,"Right",0);
	for(i=0;i<3;i++)
		for(j=0;j<7;j++)
		{
			if(i<2)
				OptionButton(x+78+i*116,y+j*31,112,ScanCodeText(opt.control[i][j]),3+i+j*3);
			else if(j>3)
			{
				sprintf(s,"Btn %d",opt.joyCtrl[j-4]);
				OptionButton(x+78+i*116,y+j*31,112,s,3+i+j*3);
			}
		}

	PrintDark(x+2,y+238,"Up, Down, Left and Right to select.  Fire to adjust an",1);
	PrintDark(x+2,y+238+12,"an option.  Jump to exit.  Changes made here affect new",1);
	PrintDark(x+2,y+238+24,"profiles and the title screen itself.",1);
	PrintDark(x+2,y+238+36,"Use the Pause menu to change settings in your profile.",1);

	if(subMode==SUBMODE_SETKEY)
		RenderSetKey(x,y,0);
	else if(subMode==SUBMODE_SETBTN)
		RenderSetKey(x,y,1);

}

//----------------

void ApplyControlSettings(void)
{
	SetKeyboardBindings(0, 8, opt.control[0]);
	SetKeyboardBindings(1, 8, opt.control[1]);
	SetJoystickBindings(4, opt.joyCtrl);
}

void ApplyControlSettingsMem(byte *data)
{
	SetKeyboardBindings(0, 8, &data[0*8]);
	SetKeyboardBindings(1, 8, &data[1*8]);
	SetJoystickBindings(4, &data[2*8]);
}

void LoadOptions(void)
{
	FILE *f;
	int i;

	f=fopen("loony.cfg","rb");
	if(!f)
	{
		opt.sound=127;
		opt.music=26*3;

		opt.control[0][0]=SDL_SCANCODE_UP;	// up
		opt.control[0][1]=SDL_SCANCODE_DOWN;	// down
		opt.control[0][2]=SDL_SCANCODE_LEFT;	// left
		opt.control[0][3]=SDL_SCANCODE_RIGHT;	// right
		opt.control[0][4]=SDL_SCANCODE_LCTRL;	// hammer: CTRL
		opt.control[0][5]=SDL_SCANCODE_LSHIFT;	// jump: shift
		opt.control[0][6]=SDL_SCANCODE_TAB;	// magic: TAB
		opt.control[0][7]=0;	// wpn switch: SPACE
		opt.control[1][0]=0;	// up
		opt.control[1][1]=0;	// down
		opt.control[1][2]=0;	// left
		opt.control[1][3]=0;	// right
		opt.control[1][4]=SDL_SCANCODE_Z;	// hammer: Z
		opt.control[1][5]=SDL_SCANCODE_X;	// jump: X
		opt.control[1][6]=SDL_SCANCODE_A;	// magic: A
		opt.control[1][7]=0;	// wpn switch: S

		opt.joyCtrl[0]=0;
		opt.joyCtrl[1]=1;
		opt.joyCtrl[2]=2;
		opt.joyCtrl[3]=3;

		for(i=0;i<30;i++)
			opt.modifier[i]=0;
		opt.modifier[0]=1;
		opt.modifier[1]=1;
		opt.modifier[2]=1;
		opt.modifier[3]=1;	// start with "None", and the first 3 modifiers

		for(i=0;i<100;i++)
			opt.achieve[i]=0;
		opt.lastProfile=0;
#ifdef DIRECTORS
		for(i=0;i<GALLERY_PIX;i++)
			opt.galPix[i]=0;
#endif
	}
	else
	{
		if(fread(&opt,1,sizeof(options_t),f)<sizeof(options_t))
		{
#ifdef DIRECTORS
		for(i=0;i<GALLERY_PIX;i++)
			opt.galPix[i]=0;
#endif
		}
		fclose(f);
	}
	ApplyControlSettings();
}

void SaveOptions(void)
{
	FILE *f;

	f=fopen("loony.cfg","wb");
	fwrite(&opt,sizeof(options_t),1,f);
	fclose(f);
}

void OptionsMenu(MGLDraw *mgl,byte *backScr)
{
	byte done=0;
	int lastTime;

	InitOptionsMenu();
	lastTime=1;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		if(lastTime>TIME_PER_FRAME*30)
			lastTime=TIME_PER_FRAME*30;

		while(lastTime>=TIME_PER_FRAME && !done)
		{
			done=UpdateOptionsMenu(mgl);
			lastTime-=TIME_PER_FRAME;
		}
		RenderOptionsMenu(offX,-offX-5-100,backScr);
		mgl->Flip();
		EndClock();

		if(!mgl->Process())
			done=1;
	}

	ExitOptionsMenu();
}
