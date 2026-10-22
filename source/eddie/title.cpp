#include "title.h"
#include "options.h"
#include "control.h"
#include "display.h"
#include "tile.h"
#include "backgd.h"
#include "status.h"
#include "game.h"
#include "random.h"
#include "sound.h"
#include "credits.h"

byte menuCursor=0;
static int titmsx,titmsy;
int titleY;
byte titleAnim;
int eddieX,eddieY,eddieDx,eddieDy;
int eddieAnim,eddieAnimDir;
static int runCount;

int menuY;
char extermBright,theEndBright;

static byte keys,oldKeys;

static word starx[64],stary[64];
static byte starcolor[64],victoryMode;
static int screenY;

static void Clamp(int *value,int amt)
{
	if(*value>amt)
		*value=amt;
	if(*value<-amt)
		*value=-amt;
}

byte TitleRun(int *lastTime)
{
	int msx,msy;
	int oldCur;

	while(*lastTime>=TIME_PER_FRAME)
	{
		UpdateMouse();
		GetMouse(&msx,&msy);
		UpdateBackgd();
		JamulSoundUpdate();

		// update the various objects
		titleAnim++;
		if(titleY<0)
			titleY+=8;
		if(menuY>276)
			menuY-=8;

		if(extermBright<0 && titleY==0)
			extermBright++;

		// update Eddie hisself
		eddieAnim+=eddieAnimDir;
		if(eddieAnim>4*3+3)
		{
			eddieAnim=4*3+3;
			eddieAnimDir=-1;
		}
		if(eddieAnim<0)
		{
			eddieAnim=0;
			eddieAnimDir=1;
		}
		if(eddieX<140*FIXAMT)
			eddieDx+=GetRand(FIXAMT/2);
		else
			eddieDx-=GetRand(FIXAMT/2);

		if(eddieY<320*FIXAMT)
			eddieDy+=GetRand(FIXAMT/2);
		else
			eddieDy-=GetRand(FIXAMT/2);
		Clamp(&eddieDx,FIXAMT*6);
		Clamp(&eddieDy,FIXAMT*6);

		eddieX+=eddieDx;
		eddieY+=eddieDy;

		titmsx+=msx;
		titmsy+=msy;

		if(titmsx<3)
			titmsx=3;
		if(titmsy<5)
			titmsy=5;
		if(titmsx>123)
			titmsx=123;
		if(titmsy>144)
			titmsy=144;

		oldCur=menuCursor;
		oldKeys=keys;
		keys=GetControls();

		if((keys&CONTROL_UP) && (!(oldKeys&CONTROL_UP)))
		{
			if(titmsy<50)
				titmsy=98+24;
			else if(titmsy<98)
				titmsy=24;
			else
				titmsy=50+24;
		}
		else if((keys&CONTROL_DN) && (!(oldKeys&CONTROL_DN)))
		{
			if(titmsy<50)
				titmsy=50+24;
			else if(titmsy<98)
				titmsy=98+24;
			else
				titmsy=24;
		}

		if(titmsy<50)
			menuCursor=0;
		else if(titmsy<98)
			menuCursor=1;
		else
			menuCursor=2;

		if(oldCur!=menuCursor)	// you moved onto a different one, make a noise
			MakeSound(SND_TYPING,1000);

		if(mainmgl->MouseTap() || ((keys&CONTROL_B1)&&(!(oldKeys&CONTROL_B1))))
		{
			MakeSound(SND_MENUSELECT,1100);
			return 0;
		}
		(*lastTime)-=TIME_PER_FRAME;
		runCount++;
	}
	if(mainmgl->LastKeyPressed()==27)
	{
		menuCursor=2;
		return 0;
	}

	return 1;
}

TASK(void) TitleRender(void)
{

	DrawBackgd();


	// render the title
	ShowSprite(SPR_TITLE,(titleAnim/8)&1,40,titleY);
#ifdef JUNIOR
	ShowSprite(SPR_TITLE,13,340,titleY+220);
#elif TRIAL
	ShowSprite(SPR_TITLE,14,350,titleY+220);
#elif DEMO
	ShowSprite(SPR_TITLE,15,350,titleY+220);
#endif

	// render the subtitle
	GlowSprite(SPR_TITLE,2,30,235,extermBright);

	// render the menu
	ShowSprite(SPR_TITLE,3,320,menuY);
	ShowSprite(SPR_TITLE,4+menuCursor,320,menuY);
	ShowSprite(SPR_STATUS,20,320+titmsx,menuY+40+titmsy);

	// render eddie behind everything
	ShowSprite(SPR_TITLE,(eddieAnim/4)+8,eddieX>>FIXSHIFT,eddieY>>FIXSHIFT);

	// render the copyright
	//ShowSprite(SPR_TITLE,7,6,472);
	Print(2,465,"Copyright 2004, Hamumu Software",0);

	RenderHiScoreDisplay();

	AWAIT FlipScreen();
}

void InitTitleMenu(void)
{
	// "EDDIE GALAXY"
	titleY=-256;
	titleAnim=0;

	// "Intergalactic Exterminator"
	extermBright=-32;

	// eddie himself floating around
	eddieX=(640)<<FIXSHIFT;
	eddieY=(480)<<FIXSHIFT;
	eddieDx=0;
	eddieDy=0;
	eddieAnim=0;
	eddieAnimDir=1;

	// the menu
	menuY=484;
	menuCursor=0;

	// the mouse cursor
	titmsx=6;
	titmsy=6;

	// run count, to count down until the credits should show
	runCount=0;

	SetCheat(0);	// every time you get back to this screen, you're not cheating anymore

	PlaySong(106);
	keys=0xFF;
}

TASK(byte) TitleMenu(void)
{
	byte quit;
	int lastTime;
	dword clock,endclock;

	InitTitleMenu();

	quit=0;
	clock=0;
	endclock=0;
	lastTime=TIME_PER_FRAME;

	LoadBackgd("graphics/space.bmp");
	SetBackScroll(1);

	mainmgl->LastKeyPressed();

	while(!quit)
	{
		lastTime+=endclock-clock;
		clock=GetTime();

		if(TitleRun(&lastTime)==0)
			quit=1;

		AWAIT TitleRender();

		endclock=GetTime();
		if(runCount>=30*27)	// approximately the length of the title song
		{
			runCount=0;
			AWAIT Credits();
			InitTitleMenu();
		}
	}
	CO_RETURN menuCursor;
}

byte ContinueRun(int *lastTime)
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
		if(titmsx>460)
			titmsx=460;
		if(titmsy>460)
			titmsy=460;
		if(titmsy<400)
			titmsy=400;

		if(mainmgl->MouseTap() || ((keys&CONTROL_B1)&&(!(oldKeys&CONTROL_B1))))
		{
			if(titmsx<240)
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
		if((runCount%30)==0)
			MakeSound(SND_MENUSELECT,1200);	// tick the clock
	}
	if(mainmgl->LastKeyPressed()==27)
	{
		menuCursor=0;
		MakeSound(SND_CONTNO,1200);
		return 0;
	}

	return 1;
}

TASK(void) ContinueRender(void)
{
	char s[16];

	DrawBackgd();

	// draw the text "Continue?" and so forth, and the sprite to move between
	CenterPrint(200,"Continue?",2,0);
	sprintf(s,"%d",9-(runCount/30));
	CenterPrint(280,s,2,15-(runCount%30)/2);
	CenterPrint(400,"YES         NO",2,0);
	ShowSprite(SPR_ROCKET,0,titmsx,titmsy);

	RenderHiScoreDisplay();

	AWAIT FlipScreen();
}

TASK(byte) Continue(void)
{
	byte quit;
	int lastTime;
	dword clock,endclock;

	quit=0;
	clock=0;
	endclock=0;
	lastTime=TIME_PER_FRAME;

	mainmgl->LastKeyPressed();
	runCount=0;
	titmsx=240;
	titmsy=430;
	MakeSound(SND_CONTINUE,1200);
	keys=0xFF;

	while(!quit)
	{
		lastTime+=endclock-clock;
		clock=GetTime();

		if(ContinueRun(&lastTime)==0)
			quit=1;

		AWAIT ContinueRender();

		endclock=GetTime();
		if(runCount>=30*10-1)	// 10 second countdown
		{
			runCount=0;
			MakeSound(SND_CONTNO,1200);
			CO_RETURN 0;	// didn't continue
		}
	}
	CO_RETURN menuCursor;
}

byte VictoryUpdate(int *lastTime)
{
	int i;

	while(*lastTime>=TIME_PER_FRAME)
	{
		//JamulSoundUpdate();
		if(victoryMode==0)
		{
			if(screenY>0)
				screenY-=2;
			if(eddieY>108)
				eddieY-=4;
			else	// done scrolling in
			{
				if(extermBright<6)
					extermBright++;

				runCount++;
				if(runCount==320)
				{
					victoryMode=1;
					runCount=0;
				}
			}

		}
		else if(victoryMode==1)
		{
			if(extermBright>-32)
				extermBright--;
			else
			{
				if(screenY<480)
					screenY+=2;
				if(eddieY<108+480*2)
					eddieY+=4;
				else	// done scrolling away
				{
					if(theEndBright<10)
						theEndBright++;

					runCount++;
					if(runCount==180)
					{
						victoryMode=2;
						runCount=0;
					}
				}
			}
		}
		else	// going away for good
		{
			if(theEndBright>-32)
				theEndBright--;

			for(i=0;i<64;i++)
			{
				if(starcolor[i]>0 && GetRand(4)==0)
					starcolor[i]--;

				if(starx[i]<240)
					starx[i]-=(((240-starx[i])/128)+1);
				else
					starx[i]+=(((starx[i]-240)/128)+1);
				if(stary[i]<240)
					stary[i]-=(((240-stary[i])/128)+1);
				else
					stary[i]+=(((stary[i]-240)/128)+1);

			}

			runCount++;
			if(runCount==140)
			{
				return 0;
			}
		}
		RetractLogo();

		(*lastTime)-=TIME_PER_FRAME;
	}
	return 1;
}

TASK(void) VictoryRender(void)
{
	int i;

	SpecialDrawBackgd(screenY);

	for(i=0;i<64;i++)
		RenderParticle(starx[i],stary[i]-480+screenY,starcolor[i],0);

#ifndef DEMO
	ShowSprite(SPR_TITLE,12,eddieX,eddieY);
#endif

	if(extermBright>-32)
	{
		CenterPrint(16+extermBright*2,"CONGRATULATIONS!",2,-32);
		CenterPrint(10+extermBright*2,"CONGRATULATIONS!",2,extermBright);
#ifdef JUNIOR
		PrintBright(10,380,"Hooray! You managed to defeat",extermBright,1);
		PrintBright(10,400,"all the evil space critters",extermBright,1);
		PrintBright(10,420,"that were threatening the",extermBright,1);
		PrintBright(10,440,"galaxy!  Nice work, Eddie.",extermBright,1);
		PrintBright(10,460,"Looks like it's Glexnor time.",extermBright,1);
#elif	TRIAL
		PrintBright(10,380,"Hooray! You managed to defeat",extermBright,1);
		PrintBright(10,400,"all the evil space critters",extermBright,1);
		PrintBright(10,420,"that were threatening the",extermBright,1);
		PrintBright(10,440,"galaxy!  Nice work, Eddie.",extermBright,1);
		PrintBright(10,460,"Looks like it's Glexnor time.",extermBright,1);
#elif	DEMO
		CenterPrint(280,"Hooray! You managed to defeat",1,extermBright);
		CenterPrint(300,"all the evil space critters",1,extermBright);
		CenterPrint(320,"that were threatening Varakkis!",1,extermBright);
		CenterPrint(340,"Now all you have to do is to",1,extermBright);
		CenterPrint(360,"buy the complete version of",1,extermBright);
		CenterPrint(380,"the game in order to save the",1,extermBright);
		CenterPrint(400,"rest of the galaxy!  And believe",1,extermBright);
		CenterPrint(420,"me - it's one big, dangerous",1,extermBright);
		CenterPrint(440,"galaxy.  Hurry up, there are",1,extermBright);
		CenterPrint(460,"nine more worlds to save!!!",1,extermBright);
#else
		PrintBright(10,280,"The alien menace is finally",extermBright,1);
		PrintBright(10,300,"exterminated, and the universe",extermBright,1);
		PrintBright(10,320,"is safe once again.  To the",extermBright,1);
		PrintBright(10,340,"people of the planets he saved,",extermBright,1);
		PrintBright(10,360,"it seems a true act of heroism.",extermBright,1);
		PrintBright(10,380,"  To Eddie, it's just another",extermBright,1);
		PrintBright(10,400,"day on the job. Though the whole",extermBright,1);
		PrintBright(10,420,"'battle to the death with the",extermBright,1);
		PrintBright(10,440,"alien queen' business did earn",extermBright,1);
		PrintBright(10,460,"him a tidy bonus.",extermBright,1);
#endif
	}
	if(theEndBright>-32)
	{
		if(victoryMode==1)
			CenterPrint(200,"THE END",2,theEndBright);
		else
			CenterPrint(200+runCount,"THE END",2,theEndBright);
	}
	RenderLogo();
	RenderHiScoreDisplay();
	AWAIT FlipScreen();
}

TASK(void) Victory(void)
{
	int i;
	byte quit;
	int lastTime;
	dword clock,endclock;

	for(i=0;i<64;i++)
	{
		starx[i]=GetRand(480);
		stary[i]=GetRand(480);
		starcolor[i]=4+GetRand(28);
	}
#ifndef DEMO
	LoadBackgd("graphics/final2.bmp");
#else
	LoadBackgd("graphics/astro1.bmp");
#endif
	SetBackScroll(0);

	quit=0;
	clock=0;
	endclock=0;
	lastTime=TIME_PER_FRAME;
	screenY=480;
	eddieX=220;
	eddieY=108+480*2;
	extermBright=-32;
	theEndBright=-32;
	mainmgl->LastKeyPressed();
	PlaySong(100);
	victoryMode=0;
	runCount=0;

	while(!quit)
	{
		lastTime+=endclock-clock;
		clock=GetTime();

		if(VictoryUpdate(&lastTime)==0)
			quit=1;

		AWAIT VictoryRender();

		endclock=GetTime();
	}
#ifdef JUNIOR
	PlaySong(106);
#elif TRIAL
	PlaySong(106);
#elif DEMO
	PlaySong(106);
#else
	PlaySong(107);
#endif
	AWAIT Credits();
}
