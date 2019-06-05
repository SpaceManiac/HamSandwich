#include "title.h"
#include "game.h"
#include "jamulfmv.h"
#include "pause.h"
#include "challenge.h"
#include "options.h"

// special codes in the credits:
// @ = use GirlsRWeird font
// # = draw a major horizontal line
// % = draw a minor horizontal line
// $ = last line of the whole deal
char credits[][32]={
	"The Adventures Of",
	"@KID MYSTIC",
	"",
	"",
	"A Hamumu Software Production",
	"www.hamumu.com",
	"#",
	"Programming",
	"Mike Hommel",
	"%",
	"Art",
	"Mike Hommel",
	"%",
	"Music",
	"Brent Christian",
	"%",
	"Level Design",
	"Mike Hommel",
	"%",
	"Sound Effects",
	"Brent Christian",
	"Mike Hommel",
	"%",
	"Testers",
	"Corey 'Coolguy' Connolly",
	"Micah Green",
	"Bryan Harrington",
	"Dave 'Custom Title' Hettel",
	"Wesley Jakab",
	"Lisa Keck",
	"TD Miller",
	"Barry Pennington",
	"Betty Scherber",
	"#",
	"Thanks for playing!",
	"Visit us at www.hamumu.com!",
	"","","","","","","","","","","","","","",
	"@THE END",
	"$"
	};

char victoryTxt[][64]={
	"With the shield restored,",
	"the people of Tulipton could",
	"return to their peaceful,",
	"monster-free lives.",
	"But not before holding a",
	"week-long party in Kid",
	"Mystic's honor and creating",
	"a giant statue of him in the",
	"village square.",
	"",
	"#",
	"",
	"Mr. Wilkins had used the evil",
	"warlord disguise so that he",
	"could steal the mystical orbs",
	"to protect Spooky Castle from",
	"monsters.  That way, he'd get",
	"many more tourists!  When Kid",
	"Mystic foiled his scheme, Mr.",
	"Wilkins was banished to the",
	"dungeon.  He spent his days",
	"plotting revenge, vowing one",
	"day to return and reclaim",
	"Spooky Castle as his own.",
	"",
	"$"
	};

char madcapWinTxt[][64]={
	"Wow!  You actually completed",
	"Madcap Mode!  That's almost",
	"disturbing.  Congratulations,",
	"and now enjoy the Happy Stick",
	"Dancers!",
	"",
	"$"
};

char demoWinTxt[][64]={
	"Having defeated the Optimum",
	"Octon, Kid Mystic is well",
	"on his way to the fearsome",
	"Spooky Castle.",
	"",
	"But to finish the journey,",
	"you'll have to purchase the",
	"full version of Kid Mystic.",
	"available at our website",
	"http://hamumu.com!",
	"$",
	};

// once the credits have scrolled to END_OF_CREDITS pixels, they end
#define END_OF_CREDITS 480*2+40
#define END_OF_VICTORY 480*2-400
#define END_OF_DEMOWIN 480-250
#define END_OF_MADCAPWIN 480-300

// the most custom worlds it will handle
#define MAX_CUSTOM 64

typedef struct title_t
{
	int bouaphaX;
	int blueY;
	char titleBright;
	char titleDir;
	int moonY;
	int optionsX;
	byte cursor;
	byte savecursor;
	byte saveChapter[5],saveHour[5],saveMin[5],saveLevel[5],saveNightmare[5];
} title_t;

sprite_set_t *planetSpr;
static int numRunsToMakeUp;
byte pickerpos;
char pickeroffset;
byte offsetdir;
byte curCustom;

static byte oldc=0;
mfont_t pickerFont;
char customName[MAX_CUSTOM][32];
char customFname[MAX_CUSTOM][32];
static byte creditsOrIntro;

static byte keyAnim=0;
static byte *backgd;

byte starColorTable[]={214,81,63,49,33,21,32,83,93};
static dword startTime;


void MainMenuDisplay(MGLDraw *mgl,title_t title)
{
	int i;
	int orbY[]={217,268,319,364,419};

	memcpy(mgl->GetScreen(),backgd,640*480);

	Print(388,462,"Copyright 2004, Hamumu Software",0,1);
	Print(2,462,VERSION_NO,0,1);

	for(i=0;i<5;i++)
	{
		if(title.cursor==i)
			planetSpr->GetSprite(0)->DrawGlow(50,orbY[i],mgl,title.titleBright);
		if(i!=2 || opt.challenge)
		{
			if(title.cursor==i)
				planetSpr->GetSprite(i*2+1+(title.cursor!=i))->DrawGlow(80,190,mgl,title.titleBright);
			else
				planetSpr->GetSprite(i*2+1+(title.cursor!=i))->Draw(80,190,mgl);

		}
	}

#ifdef BETA
	CenterPrint(320,40,"*BETA VERSION*",MGL_random(48)-24,0);
	CenterPrint(320,80,"DO NOT DISTRIBUTE!",0,1);
#endif
#ifdef CHEAT
	CenterPrint(320,80,"*CHEAT EDITION*",MGL_random(48)-24,0);
#endif
#ifdef DEMO
	CenterPrint(400,140,"DEMO VERSION",title.titleBright,0);
#endif
#ifdef VALUE
	CenterPrint(480,90,"*VALUE VERSION*",0,0);
#endif
}

byte MainMenuUpdate(MGLDraw *mgl,title_t *title,int *lastTime)
{
	byte c;
	static byte reptCounter=10;

	while(*lastTime>=TIME_PER_FRAME)
	{
		// update graphics
		title->titleBright+=title->titleDir;
		if(title->titleBright>8)
		{
			title->titleDir=-1;
			title->titleBright=8;
		}
		if(title->titleBright<-8)
		{
			title->titleDir=1;
			title->titleBright=-8;
		}

		// now real updating
		c=GetControls();

		reptCounter++;
		if((!oldc) || (reptCounter>10))
			reptCounter=0;

		if((c&CONTROL_UP) && (!reptCounter))
		{
			(title->cursor)--;
			if(title->cursor==255)
				title->cursor=4;
			if(!opt.challenge && title->cursor==2)
				title->cursor=1;
			MakeNormalSound(SND_MENUCLICK);
			startTime=timeGetTime();	// reset the clock if any key is pressed
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			(title->cursor)++;
			if(title->cursor==5)
				title->cursor=0;
			if(!opt.challenge && title->cursor==2)
				title->cursor=3;
			MakeNormalSound(SND_MENUCLICK);
			startTime=timeGetTime();	// reset the clock if any key is pressed
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
		   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			MakeNormalSound(SND_MENUSELECT);
			startTime=timeGetTime();	// reset the clock if any key is pressed
			return 1;
		}
		oldc=c;

		c=mgl->LastKeyPressed();
		if(c==27)
		{
			MakeNormalSound(SND_MENUSELECT);
			startTime=timeGetTime();	// reset the clock if any key is pressed
			return 3;
		}
//#ifdef _DEBUG
		if(c=='e')
		{
			title->cursor=5;
			return 1;
		}
//#endif
		*lastTime-=TIME_PER_FRAME;
	}

	JamulSoundUpdate();

	return 0;
}

byte MainMenu(MGLDraw *mgl)
{
	dword now;
	byte b=0;
	title_t title;
	int lastTime=1;

	creditsOrIntro=0;
	mgl->LoadBMP("graphics/title.bmp");
	backgd=(byte *)malloc(640*480);
	if(!backgd)
		FatalError("Out of memory!!");
	memcpy(backgd,mgl->GetScreen(),640*480);

	mgl->LastKeyPressed();
	mgl->ClearScreen();
	oldc=CONTROL_B1|CONTROL_B2;
	planetSpr=new sprite_set_t("graphics/title.jsp");

	title.bouaphaX=640;
	title.optionsX=-300;
	title.titleBright=-32;
	title.titleDir=4;
	title.cursor=0;
	title.blueY=1;
	title.moonY=480;
	startTime=timeGetTime();
	StartClock();
	if(CurrentSong()!=SONG_SHOP && CurrentSong()!=SONG_INTRO)
		PlaySong(SONG_SHOP);
	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();
		b=MainMenuUpdate(mgl,&title,&lastTime);
		MainMenuDisplay(mgl,title);
		mgl->Flip();
		if(!mgl->Process())
		{
			delete planetSpr;
			free(backgd);
			return 255;
		}
		if(b==1 && title.cursor==1)	// selected Continue
		{
			if(!GameSlotPicker(mgl,&title))	// pressed ESC on the slot picker
			{
				b=0;
				startTime=timeGetTime();
			}
		}
		now=timeGetTime();
		if(now-startTime>1000*10)
		{
			// alternate between showing credits and the intro movie if the user sits at the title
			if(creditsOrIntro)
				ShowVictoryAnim(11);
			else
				Credits(mgl,0);
			creditsOrIntro=1-creditsOrIntro;

			startTime=timeGetTime();
		}
		EndClock();
	}
	delete planetSpr;
	free(backgd);
	if(b==1)	// something was selected
	{
		if(title.cursor==4)	// exit
			return 255;
		else
			return title.cursor;
	}
	else
		return 255;	// ESC was pressed
}

void GameSlotPickerDisplay(MGLDraw *mgl,title_t title)
{
	int i;
	char s[32];

	MainMenuDisplay(mgl,title);

	for(i=0;i<5;i++)
	{
		sprintf(s,"%d",i+1);
		PrintBrightGlow(380,200+i*50,s,-16+(title.savecursor==i)*16,0);
		if(title.saveChapter[i]==0 || title.saveLevel[i]==0)
		{
			PrintBrightGlow(450,200+i*50,"Unused",-16+(title.savecursor==i)*16,2);
		}
		else
		{
			if(title.saveNightmare[i])
				sprintf(s,"Chapter %d!!!",title.saveChapter[i]);
			else
				sprintf(s,"Chapter %d",title.saveChapter[i]);
			PrintBrightGlow(450,200+i*50,s,-16+(title.savecursor==i)*16,2);
			sprintf(s,"%02d:%02d  Lvl: %02d",title.saveHour[i],title.saveMin[i],title.saveLevel[i]);
			PrintBrightGlow(450,200+i*50+20,s,-16+(title.savecursor==i)*16,2);
		}
	}
}

byte GameSlotPickerUpdate(MGLDraw *mgl,title_t *title,int *lastTime)
{
	byte c;
	static byte reptCounter=0;

	while(*lastTime>=TIME_PER_FRAME)
	{
		// update graphics
		title->titleBright+=title->titleDir;
		if(title->titleBright>8)
		{
			title->titleDir=-2;
			title->titleBright=8;
		}
		if(title->titleBright<-8)
		{
			title->titleDir=2;
			title->titleBright=-8;
		}

		// now real updating
		c=GetControls();

		reptCounter++;
		if((!oldc) || (reptCounter>10))
			reptCounter=0;

		if((c&CONTROL_UP) && (!reptCounter))
		{
			(title->savecursor)--;
			if(title->savecursor==255)
				title->savecursor=4;
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			(title->savecursor)++;
			if(title->savecursor==5)
				title->savecursor=0;
			MakeNormalSound(SND_MENUCLICK);
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
		   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			if(title->saveChapter[title->savecursor]==0 || title->saveLevel[title->savecursor]==0)
				MakeNormalSound(SND_ACIDSPLAT);
			else
			{
				MakeNormalSound(SND_MENUSELECT);
				return 1;
			}
		}
		oldc=c;
		*lastTime-=TIME_PER_FRAME;
	}

	if(mgl->LastKeyPressed()==27)
	{
		MakeNormalSound(SND_MENUSELECT);
		return 2;
	}

	JamulSoundUpdate();
	return 0;
}

void InitGameSlotPicker(MGLDraw *mgl,title_t *title)
{
	FILE *f;
	player_t p;
	int i;

	f=fopen("mystic.sav","rb");
	if(!f)
	{
		for(i=0;i<5;i++)
		{
			title->saveLevel[i]=0;
			title->saveChapter[i]=0;
			title->saveHour[i]=0;
			title->saveMin[i]=0;
			title->saveNightmare[i]=0;
		}
	}
	else
	{
		for(i=0;i<5;i++)
		{
			fread(&p,sizeof(player_t),1,f);
			title->saveLevel[i]=p.level;
			title->saveChapter[i]=p.worldNum+1;
			title->saveHour[i]=(byte)(p.gameClock/(30*60*60));
			title->saveMin[i]=(byte)((p.gameClock/(30*60))%60);
			title->saveNightmare[i]=p.nightmare;
		}
		fclose(f);
	}
	mgl->LastKeyPressed();
	oldc=CONTROL_B1|CONTROL_B2;
}

byte GameSlotPicker(MGLDraw *mgl,title_t *title)
{
	byte b=0;
	int lastTime=1;

	title->savecursor=0;
	InitGameSlotPicker(mgl,title);

	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();
		b=GameSlotPickerUpdate(mgl,title,&lastTime);
		GameSlotPickerDisplay(mgl,*title);
		mgl->Flip();
		if(!mgl->Process())
			return 0;
		EndClock();
	}
	if(b==1)	// something was selected
	{
		InitPlayer(INIT_GAME,0,0);
		PlayerLoadGame(title->savecursor);
		// make it remember which was picked so the pause menu will start on the same
		SetSubCursor(title->savecursor);
		return 1;
	}
	else
		return 0;
}

void CreditsRender(int y,byte mode)
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
				if(s[1]!='T' || mode!=0)
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
				CenterPrint(320,ypos-y,s,0,2);
		}
		ypos+=20;
		i++;
		if(ypos-y>=480)
			return;
	}
}

void Credits(MGLDraw *mgl,byte mode)
{
	int y=-470;
	static byte flip=0;
	byte scroll=1;

	dword startTime,endTime;

	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/pal.bmp");
	while(1)
	{
		startTime=timeGetTime();
		mgl->ClearScreen();
		CreditsRender(y,mode);

		if(scroll)
			y+=1;

		if(y==END_OF_CREDITS-320 && mode==1)
			scroll=0;

		mgl->Flip();
		if(!mgl->Process())
			return;
		if(mgl->LastKeyPressed())
			return;
		if(y==END_OF_CREDITS-320 && mode==0)
			return;

		JamulSoundUpdate();

		endTime=timeGetTime();
		while((endTime-startTime)<TIME_PER_FRAME)
			endTime=timeGetTime();
	}
}

void VictoryTextRender(int y,byte type)
{
	int i,ypos;
	char *s;

	i=0;

	ypos=0;

	while(1)
	{
		switch(type)
		{
			case 0:
				s=victoryTxt[i];
				break;
			case 1:
				s=madcapWinTxt[i];
				break;
			case 2:
				s=demoWinTxt[i];
				break;
		}
		if(s[0]=='$')
			break;
		if(ypos-y>-60)
		{
			if(s[0]=='@')
			{
				CenterPrintGlow(ypos-y,&s[1],0);
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
				CenterPrintGlow(ypos-y,s,2);
		}

		ypos+=20;
		i++;
		if(ypos-y>=480)
			return;
	}
}

void VictoryText(MGLDraw *mgl,byte victoryType)
{
	int y=-470;
	int darkY,i,j,k;
	byte *scr;
	dword startTime,endTime;
	byte flip=0;
	int length;

	scr=(byte *)malloc(640*480);
	if(!scr)
		return;

	switch(victoryType)
	{
		case 0:
			mgl->LoadBMP("graphics/zoobo.bmp");
			length=END_OF_VICTORY;
			break;
		case 1:
			mgl->LoadBMP("graphics/zoobo.bmp");
			length=END_OF_MADCAPWIN;
			break;
		case 2:
			mgl->LoadBMP("graphics/zoobo2.bmp");
			length=END_OF_DEMOWIN;
			break;
	}

	mgl->LastKeyPressed();
	memcpy(scr,mgl->GetScreen(),640*480);
	darkY=0;
	while(1)
	{
		startTime=timeGetTime();
		memcpy(mgl->GetScreen(),scr,640*480);
		VictoryTextRender(y,victoryType);
		if((flip=1-flip) || victoryType==1 || victoryType==2)
			y++;
		mgl->Flip();
		if(!mgl->Process())
			break;
		if(mgl->LastKeyPressed()==27)
			break;
		if(y==length)
			break;

		if(y>0)
		{
			for(k=1;k<20;k++)
			{
				j=darkY*640;
				for(i=0;i<640;i++)
				{
					if(scr[i+j]>0)
						scr[i+j]--;
				}
				darkY+=19;
				if(darkY>479)
					darkY-=480;
			}
		}
		JamulSoundUpdate();

		endTime=timeGetTime();
		while((endTime-startTime)<TIME_PER_FRAME)
			endTime=timeGetTime();
	}
	free(scr);
}

void SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound,byte specialdeal)
{
	int i,j,clock;
	RGB desiredpal[256],curpal[256];
	byte mode,done;
	dword tick;

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

	StartClock();
	mode=0;
	clock=0;
	done=0;
	tick=0;
	while(!done)
	{

		mgl->Flip();
		if(!mgl->Process())
			return;
		if(mgl->LastKeyPressed())
			mode=2;
		EndClock();
		tick+=TimeLength();
		StartClock();
		if(tick>1000/30)
		{
			clock++;
			tick-=1000/30;
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
						if(sound)
							MakeNormalSound(sound);
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
	}
	mgl->ClearScreen();
	mgl->Flip();
}
