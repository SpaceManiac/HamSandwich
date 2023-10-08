#include "winpch.h"
#include <stdio.h>
#include <math.h>
#include "title.h"
#include "game.h"
#include "jamulfmv.h"
#include "pause.h"
#include "nameentry.h"
#include "progress.h"
#include "shop.h"
#include "textgame.h"
#include "steam.h"
#include "softjoystick.h"

// special codes in the credits:
// @ = use GirlsRWeird font
// # = draw a major horizontal line
// % = draw a minor horizontal line
// $ = last line of the whole deal
char credits[][32]={
	"SPISPOPD II",
	"@DR. LUNATIC",
	"",
	"",
	"@SUPREME WITH CHEESE",
	"",
	"",
	("Copyright " COPYRIGHT_YEARS ","),
	("By " COPYRIGHT_COMPANY),
	"#",
	"&Original Concept",
	"Mike Hommel",
	"%",
	"&Programming",
	"Mike Hommel",
	"Tad \"SpaceManiac\" Hardesty",
	"%",
	"&Steam Edition Art",
	"Ben Rose",
	"%",
	"&HamSandwich Contributors",
	"Alex Walley",
	"AutomaticFrenzy",
	"CPE Gaebler",
	"Hypexion",
	"Sam Atkins",
	"%",
	"&Character Design",
	"Jack Darby",
	"Lisa Dash",
	"Mike Hommel",
	"Master Jobee",
	"%",
	"&Level Design",
	"Devon",
	"Mike Hommel",
	"Lisa Keck",
	"Ryan Keck",
	"TD Miller",
	"Pants Of Power",
	"Barry Pennington",
	"Mark Stevens",
	"World Tester Guy!?",
	"Randy Zook",
	"%",
	"&In-Game Graphics",
	"Mike Hommel",
	"TD Miller",
	"Barry Pennington",
	"Mark Stevens",
	"%",
	"&Gallery Art",
	"Lisa Dash",
	"Mike Hommel",
	"Solange Hommel",
	"Master Jobee",
	"Mike Kasprzak",
	"TD Miller",
	"SpiderPumpkin",
	"%",
	"&Sound Effects",
	"Brent Christian",
	"Mike Hommel",
	"Solange Hommel",
	"%",
	"&Music",
	"Brent Christian",
	"Mike Hommel",
	"%",
	"&Producer/Designer/Director",
	"Mike Hommel",
	"%",
	"&Gaffer",
	"Mike Hommel",
	"%",
	"&Testing",
	"Baba",
	"Brent Christian",
	"Jim Crawford",
	"Chris Dillman",
	"Mark Fassett",
	"Angela Finer",
	"Tim Finer",
	"Dawn Genge",
	"Mattie Goodman",
	"Matt Guest",
	"Solange Hommel",
	"Suzanne Hommel",
	"Brad Kasten",
	"Lisa Keck",
	"Ryan Keck",
	"Geoff Michell",
	"TD Miller",
	"Britt Morris",
	"Barry Pennington",
	"Trevor Strohman",
	"Peter Young",
	"%",
	"&Ideas & Hamumu Theme",
	"Mattie Goodman",
	"%",
	"&More Ideas",
	"The Hamumu.com Forum Players!",
	"%",
	"&Technical Assistance",
	"Trevor Strohman",
	"'Grubby Children'",
	"%",
	"&Special Thanks",
	"The Supremizing Crew",
	"World Tester Guy",
	"Joss Whedon",
	"%",
	"\"it's certainly pointless",
	"and annoying, but i say you",
	"should keep it anyways.\"",
	"The words of a true SpisFan",
	"%",
	"#",
#ifndef ARCADETOWN
	"&Stop by hamumu.com!",
#endif
	"","","","","","","","","","","","","","","","",
	"$"
	};

char victoryTxt[][64]={
	"@With Dr. Lunatic vanquished, the",
	"",
	"",
	"@zombie menace was ended. Never again",
	"",
	"",
	"@would someone take the brains out of",
	"",
	"",
	"@zombies and put them into other",
	"",
	"",
	"@zombies to create a race of super",
	"",
	"",
	"@zombies.",
	"",
	"",
	"#",
	"",
	"@Bouapha was the hero of the hour,",
	"",
	"",
	"@loved and respected by all. There",
	"",
	"",
	"@were parades and parties for days.",
	"",
	"",
	"@Until the president got himself into",
	"",
	"",
	"@another madcap scandal, and everyone",
	"",
	"",
	"@forgot about Bouapha and the zombies",
	"",
	"",
	"@altogether.",
	"",
	"",
	"#",
	"$"
	};

// once the credits have scrolled to END_OF_CREDITS pixels, they end
#define END_OF_CREDITS 480*4+130
#define END_OF_VICTORY 480*2

sprite_set_t *planetSpr;
byte pickerpos;
char pickeroffset;
byte offsetdir;
byte curCustom;

static byte oldc=0;
mfont_t pickerFont;

char lvlName[32];
static byte secretClicks,secretDir;

byte starColorTable[]={214,81,63,49,33,21,32,83,93};

byte demoTextCounter;

static byte *backgd;
static int titleRuns;
static int msx,msy,msx2,msy2;
static byte cursor;
static byte sliceOut[8];
char menuTxt[][16]={"Play","Profile","Tutorial","Instructions","Exit","Internet","Shop","Editor"};
int igfX,igfY,igfDX,igfDY;

void MainMenuDisplay(MGLDraw *mgl)
{
	byte ang[]={192+16,224+16,0+16,32+16,64+16,96+16,128+16,160+16};
	char s[32];
	int i,x,y;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	// version #:
	Print(SCRWID - GetStrLength(VERSION_NO, 1), 3, VERSION_NO, 1, 1);
	Print(SCRWID - GetStrLength(VERSION_NO, 1) - 1, 3-1, VERSION_NO, 0, 1);
	// Copyright:
	Print(3,467,"Copyright " COPYRIGHT_YEARS ", " COPYRIGHT_COMPANY,1,1);
	Print(2,466,"Copyright " COPYRIGHT_YEARS ", " COPYRIGHT_COMPANY,0,1);
	// Steam edition info
	/*const char* edition = SteamManager::Get()->DescribeEdition();
	Print(SCRWID - GetStrLength(edition, 1), 20, edition, 1, 1);
	Print(SCRWID - GetStrLength(edition, 1) - 1, 20-1, edition, 0, 1);*/
	const char* workshopStatus = SteamManager::Get()->DescribeWorkshopStatus();
	Print(3, 3, workshopStatus, 1, 1);
	Print(3-1, 3-1, workshopStatus, 0, 1);

	for(i=0;i<8;i++)
	{
		x=((Cosine(ang[i])*sliceOut[i])/FIXAMT);
		y=((Sine(ang[i])*sliceOut[i])/FIXAMT);
		planetSpr->GetSprite(i)->Draw(320+x,280+y,mgl);
	}

	for(i=0;i<8;i++)
	{
		x=((Cosine(ang[i])*sliceOut[i])/FIXAMT);
		y=((Sine(ang[i])*sliceOut[i])/FIXAMT);
		x+=((Cosine(ang[i])*100)/FIXAMT);
		y+=((Sine(ang[i])*70)/FIXAMT);
		if(i<2 || i>5)
			y-=40;
		if(i>3)
			x-=GetStrLength(menuTxt[i],0);
		if(i==0 || i==7)
			y-=30;
		if(i==3 || i==4)
			y+=30;
		Print(320+x,280+y,menuTxt[i],(cursor==i)*10,0);
	}
	sprintf(s,"Profile: %s",profile.name);
	Print(638-GetStrLength(s,2)-1,460-1,s,-32,2);
	Print(638-GetStrLength(s,2)+1,460+1,s,-32,2);
	Print(638-GetStrLength(s,2),460,s,0,2);
	DrawMouseCursor(msx2,msy2);

#ifdef IGF
	CenterPrint(igfX-1,igfY-1,"IGF Judge Edition!  Do Not Distribute",-32,2);
	CenterPrint(igfX+1,igfY+1,"IGF Judge Edition!  Do Not Distribute",-32,2);
	CenterPrint(igfX,igfY,"IGF Judge Edition!  Do Not Distribute",0,2);
#endif
}

TASK(byte) MainMenuUpdate(int *lastTime,MGLDraw *mgl)
{
	byte c;
	static byte reptCounter = 0;
	int i;
	byte formerCursor;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;
	while(*lastTime>=TIME_PER_FRAME)
	{
		igfX+=igfDX;
		igfY+=igfDY;

		if(igfX<30 || igfX>610)
			igfDX=-igfDX;
		if(igfY<0 || igfY>460)
			igfDY=-igfDY;

		titleRuns++;
		for(i=0;i<8;i++)
		{
			if(cursor!=i && sliceOut[i]>0)
				sliceOut[i]--;
			else if(cursor==i && sliceOut[i]<20)
				sliceOut[i]+=2;
		}
		reptCounter++;

		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
	}

	formerCursor=cursor;

	int16_t lsx, lsy;
	byte dpad;  // dpad + buttons, but not left stick
	GetLeftStick(&lsx, &lsy, &dpad);
	c = GetKeyControls() | GetArrows() | SoftJoystickState() | dpad;

	int oldmsx = msx, oldmsy = msy;
	mgl->GetMouse(&msx, &msy);
	bool doMouse = false;
	if(msx!=oldmsx || msy!=oldmsy)
	{
		doMouse = true;
		msx2 = msx;
		msy2 = msy;
	}

	if (lsx*lsx + lsy*lsy > (INT16_MAX/2)*(INT16_MAX/2))
	{
		double angle = atan2(lsy, lsx);
		msx2 = 320 + cos(angle) * 200;
		msy2 = 280 + sin(angle) * 200;
		doMouse = true;
	}

	if(doMouse)
	{
		titleRuns=0;

		if(msx2>320)
		{
			if(msy2<280)
			{
				// upper right quadrant
				if(abs(msx2-320)>abs(msy2-280))
					cursor=1;
				else
					cursor=0;
			}
			else
			{
				// lower right quadrant
				if(abs(msx2-320)>abs(msy2-280))
					cursor=2;
				else
					cursor=3;
			}
		}
		else
		{
			if(msy2<280)
			{
				// upper left quadrant
				if(abs(msx2-320)>abs(msy2-280))
					cursor=6;
				else
				{
					if(ItemPurchased(SHOP_MAJOR,MAJOR_EDITOR))
						cursor=7;
					else
						cursor=6;
				}
			}
			else
			{
				// lower left quadrant
				if(abs(msx2-320)>abs(msy2-280))
					cursor=5;
				else
					cursor=4;
			}
		}
	}

	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if((c & ~oldc) & (CONTROL_UP|CONTROL_LF))
	{
		cursor--;
		if(cursor==255)
			cursor=7;
		if(cursor==7 && !ItemPurchased(SHOP_MAJOR,MAJOR_EDITOR))
			cursor=6;

		titleRuns=0;
		MakeNormalSound(SND_MENUCLICK);
	}
	if((c & ~oldc) & (CONTROL_DN|CONTROL_RT))
	{
		cursor++;
		if(cursor==8)
			cursor=0;
		if(cursor==7 && !ItemPurchased(SHOP_MAJOR,MAJOR_EDITOR))
			cursor=0;

		titleRuns=0;
		MakeNormalSound(SND_MENUCLICK);
	}
	if((c & ~oldc) & CONTROL_B1)
	{
		titleRuns=0;
		MakeNormalSound(SND_MENUSELECT);
		CO_RETURN 1;
	}
	oldc=c;

	c=mgl->LastKeyPressed();
	if(c==27)
	{
		MakeNormalSound(SND_MENUSELECT);
		cursor=4;
		CO_RETURN 1;
	}

	if(formerCursor!=cursor)
	{
		if((formerCursor==cursor-1) || (formerCursor==7 && cursor==0))
		{
			// moved up
			if(secretDir==1)
				secretClicks++;
			else
			{
				secretDir=1;
				secretClicks=1;
			}
		}
		if((formerCursor==cursor+1) || (formerCursor==0 && cursor==7))
		{
			// moved down
			if(secretDir==2)
				secretClicks++;
			else
			{
				secretDir=2;
				secretClicks=1;
			}
		}
	}

	if(secretClicks>=16)
	{
		AWAIT TextGame(mgl);
		secretClicks=0;
		secretDir=0;
	}

	if(mgl->MouseTap())
		CO_RETURN 1;

	CO_RETURN 0;
}

TASK(byte) MainMenu(MGLDraw *mgl)
{
	byte cmd;
	int lastTime=1;
	int i;

	secretDir=0;
	secretClicks=0;

	if(ItemPurchased(SHOP_MAJOR,MAJOR_EDITOR))
		strcpy(menuTxt[7],"Editor");
	else
		strcpy(menuTxt[7],"??????");

	if(FirstTime())
		AWAIT NameEntry(mgl,1);

	if(doShop)
	{
		doShop=0;
		if(shopping)
			CO_RETURN 8;	// start playing the last level again
		else
			CO_RETURN 6;	// start shopping!
	}

	mgl->LoadBMP(SteamManager::Get()->IsSteamEdition() ? "graphics/title_steam.bmp" : "graphics/title.bmp");
	backgd=(byte *)malloc(640*480);
	if(!backgd)
		FatalError("Out of memory!");

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	mgl->LastKeyPressed();
	mgl->MouseTap();
	oldc=~0;
	planetSpr=new sprite_set_t("graphics/pizza.jsp");

	PlaySongForce("002title.ogg");

	igfX=Random(500)+100;
	igfY=Random(300)+100;
	igfDX=4;
	igfDY=2;
	cmd=0;
	while(cmd==0)
	{
		lastTime+=TimeLength();
		StartClock();
		cmd=AWAIT MainMenuUpdate(&lastTime,mgl);
		MainMenuDisplay(mgl);
		AWAIT mgl->Flip();

		if(!mgl->Process())
		{
			cursor=4;
			cmd=1;
		}

		EndClock();

		if(titleRuns>30*15)
		{
			AWAIT Credits(mgl);
			titleRuns=0;
			mgl->LastKeyPressed();
			mgl->MouseTap();
			oldc=~0;
		}
	}
	delete planetSpr;
	free(backgd);

	if(cursor==4)	// exit
		CO_RETURN 255;
	else
		CO_RETURN cursor;
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

TASK(void) Credits(MGLDraw *mgl)
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
		AWAIT mgl->Flip();
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

TASK(void) VictoryText(MGLDraw *mgl)
{
	int y=-470;

	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");
	while(1)
	{
		mgl->ClearScreen();
		VictoryTextRender(y);
		y+=1;
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN;
		if(mgl->LastKeyPressed()==27)
			CO_RETURN;
		if(y==END_OF_VICTORY)
			CO_RETURN;
	}
}

TASK(byte) SpeedSplash(MGLDraw *mgl,const char *fname)
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
		CO_RETURN false;

	mode=0;
	clock=0;
	done=0;
	while(!done)
	{
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN 0;
		c=mgl->LastKeyPressed();

		if(c==27)
			CO_RETURN 0;
		else if(c || mgl->MouseTap())
			mode=2;

		c=GetControls()|GetArrows();
		if((c & ~oldc) & (CONTROL_B1 | CONTROL_B2))
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
	AWAIT mgl->Flip();
	CO_RETURN 1;
}

TASK(void) HelpScreens(MGLDraw *mgl)
{
	char name[32];

	sprintf(name,"docs/help.bmp");
	if(!AWAIT SpeedSplash(mgl,name))
		CO_RETURN;
}

TASK(void) DemoSplashScreens(MGLDraw *mgl)
{
	if(!AWAIT SpeedSplash(mgl,"docs/demosplash.bmp"))
		CO_RETURN;
	if(!AWAIT SpeedSplash(mgl,"docs/demosplash2.bmp"))
		CO_RETURN;
}

TASK(void) SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound)
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
		CO_RETURN;

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
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN;
		if(mgl->LastKeyPressed() || GetJoyButtons())
			mode=2;

		EndClock();
	}
	mgl->ClearScreen();
	AWAIT mgl->Flip();
}
