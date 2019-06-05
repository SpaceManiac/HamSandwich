#include "title.h"
#include "game.h"
#include <stdio.h>
#include "jamulfmv.h"
#include "pause.h"
#include "nameentry.h"
#include "progress.h"
#include "shop.h"
#include "customworld.h"

#define COPYRIGHT_YEARS "2007-2012"

// special codes in the credits:
// @ = use GirlsRWeird font
// # = draw a major horizontal line
// % = draw a minor horizontal line
// $ = last line of the whole deal
char credits[][32]={
	"The Adventures Of Bouapha",
	"@SLEEPLESS HOLLOW",
	"",
	"",
	"Copyright " COPYRIGHT_YEARS ",",
	"By Hamumu Software",
	"#",
	"&Everything Not Mentioned Below",
	"",
	"Mike Hommel",
	"%",
	"&Modernizing",
	"",
	"Tad \"SpaceManiac\" Hardesty",
	"Best Halloween Ever 2012!",
	"%",
	"&Some Tiles Borrowed From",
	"",
	"Supreme World Builders",
	"%",
	"&Sound Effects",
	"",
	"Brent Christian",
	"Mike Hommel",
	"Solange Hommel",
	"%",
	"&Music",
	"",
	"Mike Hommel",
	"Pierre Langer",
	"Bjorn Lynne",
	"Paul Sumpter",
	"Stobierski Thomas",
	"%",
	"&Testing",
	"",
	"Barry Pennington",
	"Coolguy",
	"Hammered",
	"%",
	"&Special Thanks",
	"",
	"The Supreme Fans!",
	"%",
	"",
	"Best Halloween Ever 2007!",
	"Just wait until next year...",
	"",
	"#",
	"","","","","","","",
	"$"
	};

char victoryTxt[][64]={
	"@The Headless Horseradish grabbed the",
	"",
	"",
	"@grassy object from the ground and",
	"",
	"",
	"@roared in triumph.",
	"",
	"",
	"",
	"@\"My head!  You found my head!\"",
	"",
	"",
	"",
	"@He thanked Bouapha profusely, then",
	"",
	"",
	"@turned and stepped into a swirling",
	"",
	"",
	"@portal.",
	"",
	"",
	"@\"Well, I'm off to my home dimension.",
	"",
	"",
	"@Sorry if I caused any trouble.\"",
	"",
	"",
	"@And with that, the Formerly Headless",
	"",
	"",
	"@Horseradish vanished, never to be seen",
	"",
	"",
	"@in our world again...",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"@Until the next time he lost his head.",
	"",
	"",
	"#",
	"",
	"",
	"Congratulations!",
	"Now you can go back and get candles and",
	"stuff that you missed, if you like!",
	"$",
	};

// once the credits have scrolled to END_OF_CREDITS pixels, they end
#define END_OF_CREDITS 480*2 + 30
#define END_OF_VICTORY 480*2+50

sprite_set_t *planetSpr;
static int numRunsToMakeUp;
byte pickerpos;
char pickeroffset;
byte offsetdir;
byte curCustom;

static byte oldc=0;
mfont_t pickerFont;

static byte keyAnim=0;
char lvlName[32];

byte starColorTable[]={214,81,63,49,33,21,32,83,93};

byte demoTextCounter;
static byte canEditor;

static byte *backgd;
static int titleRuns;
static int msx,msy,oldmsx,oldmsy;
static byte cursor;
#ifdef DEMO
char menuTxt[][16]={"Play","Profiles","Buy Now!","Exit"};
#else
char menuTxt[][16]={"Play","Profiles","Editor","Exit"};
#endif

void MainMenuDisplay(MGLDraw *mgl)
{
	char s[32];
	int i,x,y;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	// version #:
	Print(550,2,VERSION_NO,1,1);
	Print(549,2,VERSION_NO,0,1);
	// Copyright:
	Print(3,467,"Copyright " COPYRIGHT_YEARS ", Hamumu Software",1,1);
	Print(2,466,"Copyright " COPYRIGHT_YEARS ", Hamumu Software",0,1);

#ifdef DEMO
	Print(350,170,"DEMO VERSION!",10,0);
#endif
#ifdef _DEBUG
	Print(300,170,"DEBUG VERSION!",10,0);
#endif

	for(i=0;i<4;i++)
	{
		if (i == 2 && !canEditor) continue;
		x=230+i*20;
		y=240+i*60;
		Print(x,y,menuTxt[i],(cursor==i)*10,0);
	}
	sprintf(s,"Profile: %s",profile.name);
	Print(638-GetStrLength(s,1)-1,467-1,s,-32,1);
	Print(638-GetStrLength(s,1)+1,467+1,s,-32,1);
	Print(638-GetStrLength(s,1),467,s,0,1);
	DrawMouseCursor(msx,msy);
}

byte MainMenuUpdate(int *lastTime,MGLDraw *mgl)
{
	byte c;
	static byte reptCounter=0;
	byte formerCursor;
	int i;

	mgl->GetMouse(&msx,&msy);

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	c=GetControls()|GetArrows();

	while(*lastTime>=TIME_PER_FRAME)
	{
		titleRuns++;
		reptCounter++;

		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
	}

	formerCursor=cursor;

	if(msx!=oldmsx || msy!=oldmsy)
	{
		titleRuns=0;

		// select stuff with cursor here
		for(i=0;i<4;i++)
		{
			if(msx>=230 && msx<=500 && msy>=240+i*60 && msy<=240+i*60+55 && (i != 2 || canEditor))
				cursor=i;
		}
		if (msx>=430 && msx<=640 && msy>=240 && msy<=295 && canEditor)
			cursor=10;
	}
	oldmsx=msx;
	oldmsy=msy;

	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if((c&(CONTROL_UP)) && !(oldc&(CONTROL_UP)))
	{
		cursor--;
		if(cursor==255)
			cursor=3;
		if(cursor==2 && !canEditor)
			cursor=1;

		titleRuns=0;
		MakeNormalSound(SND_MENUCLICK);
	}
	if((c&(CONTROL_DN)) && !(oldc&(CONTROL_DN)))
	{
		cursor++;
		if(cursor==4)
			cursor=0;
		if(cursor==2 && !canEditor)
			cursor=3;

		titleRuns=0;
		MakeNormalSound(SND_MENUCLICK);
	}
	if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
	   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
	{
		titleRuns=0;
		MakeNormalSound(SND_MENUSELECT);
		return 1;
	}
	oldc=c;

	c=mgl->LastKeyPressed();
	if(c==27)
	{
		MakeNormalSound(SND_MENUSELECT);
		cursor=3;
		return 1;
	}

	if(mgl->MouseTap())
		return 1;

	return 0;
}

byte MainMenu(MGLDraw *mgl)
{
	byte cmd;
	int lastTime=1;
	int i;

	if(FirstTime())
		NameEntry(mgl,2);

	mgl->LoadBMP("graphics/title.bmp");
	backgd=(byte *)malloc(640*480);
	if(!backgd)
		FatalError("Out of memory!");

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	canEditor=0;
	FILE* f = fopen("profiles/editor.dat", "rt");
	if (f)
	{
		canEditor=1;
		fclose(f);
	}
#ifdef _DEBUG
	canEditor=1;
#endif

	mgl->LastKeyPressed();
	mgl->MouseTap();
	oldc=CONTROL_B1|CONTROL_B2;

	PlaySongForce("unspeakable.ogg");

	cmd=0;
	while(cmd==0)
	{
		lastTime+=TimeLength();
		StartClock();
		cmd=MainMenuUpdate(&lastTime,mgl);
		MainMenuDisplay(mgl);
		mgl->Flip();

		if(!mgl->Process())
		{
			cursor=255;
			cmd=1;
		}

		EndClock();

		if(titleRuns>30*15)
		{
			Credits(mgl);
			titleRuns=0;
			mgl->LastKeyPressed();
			mgl->MouseTap();
			oldc=CONTROL_B1|CONTROL_B2;
		}
	}
	free(backgd);

	if(cursor==3)	// exit
		return 255;
	else if(cursor==2)  // editor
		return 5;
	else
		return cursor;
}

void CreditsRender(int y)
{
	int i,ypos;
	char *s;

	i=0;

	ypos=0;
	while(credits[i][0]!='$')
	{
		s=credits[i];
		if(ypos-y>-60)
		{
			if(s[0]=='@')
			{
				CenterPrint(320,ypos-y,&s[1],0,0);
			}
			else if(s[0]=='&')
			{
				CenterPrint(320,ypos-y,&s[1],0,2);
			}
			else if(s[0]=='#')
			{
				DrawFillBox(320-200,ypos-y+8,320+200,ypos-y+11,255);
			}
			else if(s[0]=='%')
			{
				DrawFillBox(320-70,ypos-y+8,320+70,ypos-y+9,255);
			}
			else
				CenterPrint(320,ypos-y,s,0,1);
		}
		ypos+=20;
		i++;
		if(ypos-y>=480)
			return;
	}
}

void Credits(MGLDraw *mgl)
{
	int y=-470;
	static byte cmd=0;
	dword lastTime;

	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");

	GetTaps();
	cmd=0;
	StartClock();
	EndClock();
	lastTime=0;
	while(cmd==0)
	{
		lastTime+=TimeLength();
		StartClock();
		while(lastTime>=TIME_PER_FRAME)
		{
			lastTime-=TIME_PER_FRAME;
			y++;
		}
		mgl->ClearScreen();
		CreditsRender(y);
		mgl->Flip();
		if(!mgl->Process())
			cmd=1;
		if(mgl->LastKeyPressed())
			cmd=1;
		if(y==END_OF_CREDITS)
			cmd=1;
		if(GetTaps())
			cmd=1;

		EndClock();
	}
}

void VictoryTextRender(int y)
{
	int i,ypos;
	char *s;

	i=0;

	ypos=0;
	while(victoryTxt[i][0]!='$')
	{
		s=victoryTxt[i];
		if(ypos-y>-60)
		{
			if(s[0]=='@')
			{
				CenterPrint(320,ypos-y,&s[1],0,0);
			}
			else if(s[0]=='#')
			{
				DrawFillBox(320-200,ypos-y+8,320+200,ypos-y+11,255);
			}
			else if(s[0]=='%')
			{
				DrawFillBox(320-70,ypos-y+8,320+70,ypos-y+9,255);
			}
			else
				CenterPrint(320,ypos-y,s,0,1);
		}

		ypos+=20;
		i++;
		if(ypos-y>=480)
			return;
	}
}

void VictoryText(MGLDraw *mgl)
{
	dword lastTime;
	int y=-470;
	int i;
	byte *backgd;

	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/final.bmp");
	backgd=(byte *)malloc(640*480);
	if(!backgd)
		FatalError("Out of memory!");

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&GetDisplayMGL()->GetScreen()[i*GetDisplayMGL()->GetWidth()],640);

	StartClock();
	EndClock();
	lastTime=0;
	while(1)
	{
		lastTime+=TimeLength();
		StartClock();
		while(lastTime>=TIME_PER_FRAME)
		{
			lastTime-=TIME_PER_FRAME;
			y++;
		}
		for(i=0;i<480;i++)
		{
			memcpy(&GetDisplayMGL()->GetScreen()[(i)*GetDisplayMGL()->GetWidth()],&backgd[i*640],640);
		}

		VictoryTextRender(y);
		mgl->Flip();
		if(!mgl->Process())
			return;
		if(mgl->LastKeyPressed()==27)
			return;
		if(y==END_OF_VICTORY)
			return;
		EndClock();
	}
}

byte SpeedSplash(MGLDraw *mgl,const char *fname)
{
	int i,j,clock;
	PALETTE desiredpal,curpal;
	byte mode,done;
	byte c,oldc;


	for(i=0;i<256;i++)
	{
		curpal[i].r=0;
		curpal[i].g=0;
		curpal[i].b=0;
	}
	mgl->SetPalette(curpal);
	mgl->RealizePalette();

	mgl->LastKeyPressed();
	oldc=GetControls()|GetArrows();

	if (!mgl->LoadBMP(fname, desiredpal))
		return false;

	mode=0;
	clock=0;
	done=0;
	while(!done)
	{
		mgl->Flip();
		if(!mgl->Process())
			return 0;
		c=mgl->LastKeyPressed();

		if(c==27)
			return 0;
		else if(c)
			mode=2;

		c=GetControls()|GetArrows();
		if((c&(CONTROL_B1|CONTROL_B2)) && (!(oldc&(CONTROL_B1|CONTROL_B2))))
			mode=2;
		oldc=c;

		clock++;
		switch(mode)
		{
			case 0:	// fading in
				for(j=0;j<16;j++)
					for(i=0;i<256;i++)
					{
						if(curpal[i].r<desiredpal[i].r)
							curpal[i].r++;
						if(curpal[i].g<desiredpal[i].g)
							curpal[i].g++;
						if(curpal[i].b<desiredpal[i].b)
							curpal[i].b++;
					}
				mgl->SetPalette(curpal);
				mgl->RealizePalette();
				if(clock>16)
				{
					mode=1;
					clock=0;
				}
				break;
			case 1:
				// sit around
				break;
			case 2:	// fading out
				clock=0;
				for(j=0;j<16;j++)
					for(i=0;i<256;i++)
					{
						if(curpal[i].r>0)
							curpal[i].r--;
						else
							clock++;
						if(curpal[i].g>0)
							curpal[i].g--;
						else
							clock++;
						if(curpal[i].b>0)
							curpal[i].b--;
						else
							clock++;
					}
				mgl->SetPalette(curpal);
				mgl->RealizePalette();
				if(clock==256*3*16)
					done=1;
				break;
		}
	}
	mgl->ClearScreen();
	mgl->Flip();
	return 1;
}

void HelpScreens(MGLDraw *mgl)
{
	char name[32];

	sprintf(name,"docs/help.bmp");
	if(!SpeedSplash(mgl,name))
		return;
}

void DemoSplashScreens(MGLDraw *mgl)
{
	if(!SpeedSplash(mgl,"docs/demosplash.bmp"))
		return;
	if(!SpeedSplash(mgl,"docs/demosplash2.bmp"))
		return;
}

void SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound)
{
	int i,j,clock;
	PALETTE desiredpal,curpal;
	byte mode,done;
	dword lastTime;

	for(i=0;i<256;i++)
	{
		curpal[i].r=0;
		curpal[i].g=0;
		curpal[i].b=0;
	}
	mgl->SetPalette(curpal);
	mgl->RealizePalette();

	mgl->LastKeyPressed();

	if (!mgl->LoadBMP(fname, desiredpal))
		return;

	mode=0;
	clock=0;
	done=0;
	StartClock();
	EndClock();
	lastTime=0;
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		while(lastTime>=TIME_PER_FRAME)
		{
			lastTime-=TIME_PER_FRAME;
			clock++;
			switch(mode)
			{
				case 0:	// fading in
					for(j=0;j<8;j++)
						for(i=0;i<256;i++)
						{
							if(curpal[i].r<desiredpal[i].r)
								curpal[i].r++;
							if(curpal[i].g<desiredpal[i].g)
								curpal[i].g++;
							if(curpal[i].b<desiredpal[i].b)
								curpal[i].b++;
						}
					mgl->SetPalette(curpal);
					mgl->RealizePalette();
					if(clock==32)
					{
						if(sound==2)
							MakeNormalSound(SND_HAMUMU);
					}
					if(clock>64)
					{
						mode=1;
						clock=0;
					}
					break;
				case 1:
					if(clock>delay)
					{
						mode=2;
						clock=0;
					}
					break;
				case 2:	// fading out
					clock=0;
					for(j=0;j<8;j++)
						for(i=0;i<256;i++)
						{
							if(curpal[i].r>0)
								curpal[i].r--;
							else
								clock++;
							if(curpal[i].g>0)
								curpal[i].g--;
							else
								clock++;
							if(curpal[i].b>0)
								curpal[i].b--;
							else
								clock++;
						}
					mgl->SetPalette(curpal);
					mgl->RealizePalette();
					if(clock==256*3*8)
						done=1;
					break;
			}
		}
		mgl->Flip();
		if(!mgl->Process())
			return;
		if(mgl->LastKeyPressed())
			mode=2;

		EndClock();
	}
	mgl->ClearScreen();
	mgl->Flip();
}
