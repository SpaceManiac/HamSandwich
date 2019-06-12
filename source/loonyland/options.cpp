#include "options.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "title.h"
#include "plasma.h"

static byte cursor;
static byte oldc;
static dword oldBtn;
static byte controlX,controlY;
static byte optMode;

options_t opt;

void InitOptionsMenu(void)
{
	oldc=255;
	controlX=10;
	cursor=0;
	optMode=0;
	InitPlasma(7);
}

void ExitOptionsMenu(void)
{
	SaveOptions();
	ExitPlasma();
}

void ApplyControlSettings(void)
{
	SetKeyboardBindings(0, 6, opt.control[0]);
	SetKeyboardBindings(1, 6, opt.control[1]);
	SetJoystickBindings(2, opt.joyCtrl);
}

byte UpdateOptionsMenu(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte c2;
	dword btn,j;
	int i;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;
	while(*lastTime>=TIME_PER_FRAME)
	{
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
					if(cursor>4)
						cursor=4;
					MakeNormalSound(SND_MENUCLICK);
				}
				if((c2&CONTROL_DN) && (!(oldc&CONTROL_DN)))
				{
					cursor++;
					if(cursor>4)
						cursor=0;
					MakeNormalSound(SND_MENUCLICK);
				}
				if((c2&CONTROL_LF) && (!(oldc&CONTROL_LF)))
				{
					switch(cursor)
					{
						case 0:
							opt.sound=1-opt.sound;
							if(opt.sound==0)
								JamulSoundPurge();
							else
								LoopingSound(SND_HAMUMU);
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 1:
							MakeNormalSound(SND_MENUSELECT);
							opt.helpOn=1-opt.helpOn;
							break;
						case 2:
							MakeNormalSound(SND_MENUSELECT);
							// diffy
							opt.difficulty--;
							if(opt.difficulty>4)
								opt.difficulty=0;
							break;
					}
				}
				if((c2&CONTROL_RT) && (!(oldc&CONTROL_RT)))
				{
					switch(cursor)
					{
						case 0:
							opt.sound=1-opt.sound;
							if(opt.sound==0)
								JamulSoundPurge();
							else
								LoopingSound(SND_HAMUMU);
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 1:
							MakeNormalSound(SND_MENUSELECT);
							opt.helpOn=1-opt.helpOn;
							break;
						case 2:
							MakeNormalSound(SND_MENUSELECT);
							// diffy
							opt.difficulty++;
							if(opt.difficulty>4)
								opt.difficulty=4;
							break;
					}
				}
				if((c2&(CONTROL_B1|CONTROL_B2|CONTROL_B3)) && (!(oldc&(CONTROL_B1|CONTROL_B2|CONTROL_B3))))
				{
					switch(cursor)
					{
						case 0:
							opt.sound=1-opt.sound;
							if(opt.sound==0)
								JamulSoundPurge();
							else
								LoopingSound(SND_HAMUMU);
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 1:
							MakeNormalSound(SND_MENUSELECT);
							opt.helpOn=1-opt.helpOn;
							break;
						case 2:
							MakeNormalSound(SND_MENUSELECT);
							// diffy
							opt.difficulty++;
							if(opt.difficulty==5)
								opt.difficulty=0;
							break;
						case 3:
							MakeNormalSound(SND_MENUSELECT);
							optMode=1;
							controlX=0;
							controlY=0;
							break;
						case 4:
							MakeNormalSound(SND_MENUSELECT);
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
					MakeNormalSound(SND_MENUCANCEL);
					return 0;
				}
				if(c==13)
					c2|=CONTROL_B1;

				if((c2&CONTROL_UP) && (!(oldc&CONTROL_UP)))
				{
					MakeNormalSound(SND_MENUCLICK);
					controlY--;
					if(controlY>5)
						controlY=5;
				}
				if((c2&CONTROL_DN) && (!(oldc&CONTROL_DN)))
				{
					MakeNormalSound(SND_MENUCLICK);
					controlY++;
					if(controlY>5)
						controlY=0;
				}
				if((c2&CONTROL_LF) && (!(oldc&CONTROL_LF)))
				{
					MakeNormalSound(SND_MENUCLICK);
					controlX--;
					if(controlX>2)
						controlX=2;
				}
				if((c2&CONTROL_RT) && (!(oldc&CONTROL_RT)))
				{
					MakeNormalSound(SND_MENUCLICK);
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
						MakeNormalSound(SND_MENUSELECT);
					}
					else if(controlY>3)
					{
						btn=0;
						oldBtn=~0;
						optMode=3;
						MakeNormalSound(SND_MENUSELECT);
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
					MakeNormalSound(SND_MENUCANCEL);
					return 0;
				}
				if(c2!=0)
				{
					opt.control[controlX][controlY]=c2;
					optMode=1;
					mgl->LastKeyPressed();
					MakeNormalSound(SND_MENUSELECT);
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
					MakeNormalSound(SND_MENUCANCEL);
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
						MakeNormalSound(SND_MENUSELECT);
					}
					j*=2;
				}
				oldBtn=btn;
				break;
		}

		oldc=c2;

		UpdatePlasma();

		*lastTime-=TIME_PER_FRAME;
	}

	return 0;
}

void RenderControls(int x,int y)
{
	char dirName[6][12]={"Up","Down","Left","Right","Fire","Weapon"};
	char btnTxt[64];
	int i;

	DrawFillBox(x,y-2,x+398,y+20,16);
	CenterPrint(x+50,y+2,"Control",0,1);
	CenterPrint(x+150,y+2,"Keyboard1",0,1);
	CenterPrint(x+250,y+2,"Keyboard2",0,1);
	CenterPrint(x+350,y+2,"Joystick",0,1);
	DrawBox(x+98,y-2,x+198,y+200,16);
	DrawBox(x+198,y-2,x+298,y+200,16);
	DrawBox(x+298,y-2,x+398,y+200,16);

	for(i=0;i<6;i++)
	{
		if(controlY==i && controlX<3)
		{
			if(optMode==1)
				DrawFillBox(x+99+100*controlX,y+20+1+i*30,x+198+100*controlX,y+20+29+i*30,20);
			else
			{
				DrawFillBox(x+99+100*controlX,y+20+1+i*30,x+198+100*controlX,y+20+29+i*30,31);
				CenterPrint(x+150+controlX*100,y+27+i*30,"???",0,1);
			}
		}
		DrawFillBox(x,y+20+1+i*30,x+98,y+20+29+i*30,10);
		DrawBox(x,y+20+i*30,x+398,y+20+30+i*30,16);

		CenterPrint(x+50,y+27+i*30,dirName[i],0,1);
		if(optMode==1 || controlX!=0 || controlY!=i)
			CenterPrint(x+150,y+27+i*30,ScanCodeText(opt.control[0][i]),0,1);
		if(optMode==1 || controlX!=1 || controlY!=i)
			CenterPrint(x+250,y+27+i*30,ScanCodeText(opt.control[1][i]),0,1);

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
			CenterPrint(x+350,y+27+i*30,dirName[i],16,1);
		}
	}
	if(optMode==0)
	{
		CenterPrintGlow(320,425,"Move with arrow keys, ENTER to select",0,0);
		CenterPrintGlow(320,450,"ESC to return to main menu",0,0);
	}
	else if(optMode==1)
	{
		CenterPrintGlow(320,425,"Select with arrow keys, ENTER to set new control",0,0);
		CenterPrintGlow(320,450,"ESC to return to options",0,0);
	}
	else if(optMode==2)
	{
		sprintf(btnTxt,"Press a key for %s",dirName[controlY]);
		CenterPrintGlow(320,425,btnTxt,0,0);
		CenterPrintGlow(320,450,"ESC to cancel",0,0);
	}
	else if(optMode==3)
	{
		sprintf(btnTxt,"Press a joystick button for %s",dirName[controlY]);
		CenterPrintGlow(320,425,btnTxt,0,0);
		CenterPrintGlow(320,450,"ESC to cancel",0,0);
	}
}

void RenderOptionsMenu(MGLDraw *mgl)
{
	char onoff[3][8]={"Off","On"};
	char diffy[5][18]={"Beginner","Normal","Challenge","Mad","Loony"};

	int wid;
	int pos;
	int i;

	wid=mgl->GetWidth();
	pos=(int)mgl->GetScreen()+40*wid;
	for(i=40;i<480-40;i++)
	{
		memset((byte *)pos,7*32+2,640);
		pos+=wid;
	}

	RenderPlasma(mgl,1);
	RenderPlasma(mgl,0);

	CenterPrintGlow(320,2,"Game Options",0,2);

	PrintColor(240,50,"Sound",7,-10,0);
	PrintColor(360,50,onoff[opt.sound],7,-10,0);
	if(cursor==0)
	{
		PrintColor(239,49,"Sound",0,0,0);
		PrintColor(359,49,onoff[opt.sound],0,0,0);
	}

	PrintColor(240,80,"Help Screen",7,-10,0);
	PrintColor(360,80,onoff[opt.helpOn],7,-10,0);
	if(cursor==1)
	{
		PrintColor(239,79,"Help Screen",0,0,0);
		PrintColor(359,79,onoff[opt.helpOn],0,0,0);
	}

	PrintColor(240,110,"Difficulty",7,-10,0);
	PrintColor(360,110,diffy[opt.difficulty],7,-10,0);
	if(cursor==2)
	{
		PrintColor(239,109,"Difficulty",0,0,0);
		PrintColor(359,109,diffy[opt.difficulty],0,0,0);
	}

	PrintColor(240,140,"Configure Controls",7,-10,0);
	if(cursor==3)
		PrintColor(239,139,"Configure Controls",0,0,0);

	PrintColor(240,170,"Exit To Main Menu",7,-10,0);
	if(cursor==4)
		PrintColor(239,169,"Exit To Main Menu",0,0,0);

	RenderControls(120,210);
}

//----------------

void LoadOptions(void)
{
	FILE *f;
	int i;

	f=fopen("loony.cfg","rb");
	if(!f)
	{
		opt.sound=1;

		opt.control[0][0]=SDL_SCANCODE_UP;	// up
		opt.control[0][1]=SDL_SCANCODE_DOWN;	// down
		opt.control[0][2]=SDL_SCANCODE_LEFT;	// left
		opt.control[0][3]=SDL_SCANCODE_RIGHT;	// right
		opt.control[0][4]=SDL_SCANCODE_LCTRL;	// hammer: CTRL
		opt.control[0][5]=SDL_SCANCODE_LSHIFT;	// special: shift
		opt.control[1][0]=0;	// up
		opt.control[1][1]=0;	// down
		opt.control[1][2]=0;	// left
		opt.control[1][3]=0;	// right
		opt.control[1][4]=SDL_SCANCODE_Z;	// hammer: Z
		opt.control[1][5]=SDL_SCANCODE_X;	// special: X

		opt.joyCtrl[0]=0;
		opt.joyCtrl[1]=1;

		opt.difficulty=0;		// default to Beginner
		opt.helpOn=1;

		for(i=0;i<40;i++)
			opt.meritBadge[i]=0;
		for(i=0;i<20;i++)
			opt.cheats[i]=0;
		for(i=0;i<10;i++)
			opt.bossDead[i]=0;
		for(i=0;i<9;i++)
			opt.expando[i]=0;

		ResetHighScores();
	}
	else
	{
		fread(&opt,sizeof(options_t),1,f);
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

void OptionsMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	InitOptionsMenu();
	lastTime=1;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateOptionsMenu(&lastTime,mgl);
		RenderOptionsMenu(mgl);
		mgl->Flip();
		EndClock();

		if(!mgl->Process())
			done=1;

	}

	ExitOptionsMenu();
}

void KilledBoss(byte boss)
{
	opt.bossDead[boss]=1;
	SaveOptions();
}
