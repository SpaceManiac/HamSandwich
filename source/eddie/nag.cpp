#include "nag.h"
#include "game.h"
#include "sound.h"
#include "random.h"

static byte menuCursor=0;
static int titmsx,titmsy;
static int runCount;
static byte keys,oldKeys;
static word starx[64],stary[64];
static byte starcolor[64],victoryMode;
static int screenY;
static byte *nagScr;

byte NagUpdate(int *lastTime)
{
	int msx,msy;

	while(*lastTime>=TIME_PER_FRAME)
	{
		UpdateMouse();
		GetMouse(&msx,&msy);
		UpdateBackgd();
		JamulSoundUpdate();
		oldKeys=keys;
		keys=GetControls();
		
		if(keys&CONTROL_UP)
			msy=-10;
		if(keys&CONTROL_DN)
			msy=10;
		if(keys&CONTROL_LF)
			msx=-10;
		if(keys&CONTROL_RT)
			msx=10;

		titmsx+=msx;
		titmsy+=msy;

		if(titmsx<20)
			titmsx=20;
		if(titmsx>620)
			titmsx=620;
		if(titmsy>460)
			titmsy=460;
		if(titmsy<400)
			titmsy=400;

		if(MouseClick() || ((keys&CONTROL_B1)&&(!(oldKeys&CONTROL_B1))))
		{
			if(titmsx<320)
			{
				MakeSound(SND_CONTYES,1200);
				menuCursor=1;
			}
			else
			{
				MakeSound(SND_CONTNO,1200);
				menuCursor=0;
			}

			return 0;
		}
		(*lastTime)-=TIME_PER_FRAME;
		runCount++;
	}
	if(LastKeyPressed()==27)
	{
		menuCursor=0;
		MakeSound(SND_CONTNO,1200);
		return 0;
	}

	return 1;
}

void NagRender(MGLDraw *mgl)
{
	int i;

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&nagScr[i*640],640);	

	if(titmsx<320)
	{
		PrintBright(40-1+GetRand(3),415-1+GetRand(3),"Buy Now",10,2);
		Print(500,415,"Later",2);
	}
	else
	{
		Print(40,415,"Buy Now",2);
		PrintBright(500-1+GetRand(3),415-1+GetRand(3),"Later",10,2);
	}
	
	ShowSprite(SPR_ROCKET,0,titmsx,titmsy);

	FlipScreen();
}

byte Nag(MGLDraw *mgl)
{
	byte quit;
	int lastTime,i;
	dword clock,endclock;

	quit=0;
	clock=0;
	endclock=0;
	lastTime=TIME_PER_FRAME;

	LastKeyPressed();
	runCount=0;
	titmsx=310;
	titmsy=430;
	keys=0xFF;

	mgl->LoadBMP("graphics\\nagscreen.bmp");
	nagScr=(byte *)malloc(640*480);
	for(i=0;i<480;i++)
		memcpy(&nagScr[i*640],mgl->GetScreen()+mgl->GetWidth()*i,640);

	while(!quit)
	{
		lastTime+=endclock-clock;
		clock=GetTime();

		if(NagUpdate(&lastTime)==0)
			quit=1;

		NagRender(mgl);
		
		endclock=GetTime();
	}
	free(nagScr);
	return menuCursor;
}