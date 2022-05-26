#include "game.h"
#include "random.h"
#include "level.h"
#include "status.h"
#include "mission.h"
#include "options.h"
#include "title.h"
#include "sound.h"
#include "nag.h"

#define GAMEMODE_PLAY  1
#define GAMEMODE_PAUSE 2
#define GAMEMODE_QUIT  3
#define GAMEMODE_WIN   4
#define GAMEMODE_START 5
#define GAMEMODE_OVER  6

#define QR_LOSE	  1	// you quit because you ran out of lives
#define QR_CHOOSE 2	// you quit by choosing quit from the menu
#define QR_WIN	  3	// you quit by winning the whole damn game

int numRunsToMakeUp;
byte gameMode;
byte countDown;
int textY,textBright;
char lvlText[2][32];
byte quitReason;
byte continuing;
int naughtyCounter;
MGLDraw *gamemgl;

void CentipedeInit(MGLDraw *mgl)
{
	gamemgl=mgl;
	InitDisplay(mgl);
	InitControls();
	InitPlayer();
	InitGuys();
	InitBackgd();
	SeedRand();
	InitParticles();
	gameMode=GAMEMODE_PLAY;
	InitOptions();
}

void CentipedeExit(void)
{
	ExitOptions();
	ExitParticles();
	ExitDisplay();
	ExitPlayer();
	ExitGuys();
	ExitBackgd();
}

void GameLevelSetup(char *name,byte level)
{
	if(level!=5)
	{
		sprintf(lvlText[0],"%s - Mission %d",name,level);
		strcpy(lvlText[1],"Prepare To Exterminate");
	}
	else
	{
		if(GetWorld()!=WORLD_FINAL)
		{
			strcpy(lvlText[0],"Bonus Stage!");
			strcpy(lvlText[1],name);
		}
		else
		{
			strcpy(lvlText[0],"Final Battle!");
			strcpy(lvlText[1],"Squash the Queen!");
		}
	}
	gameMode=GAMEMODE_START;
	countDown=50;
	textY=-80;
	textBright=32;
}

TASK(byte) CentipedePlay(void)
{
	byte quit;
	int lastTime;
	dword clock,endclock;

	quit=0;
	clock=0;
	endclock=0;
	lastTime=TIME_PER_FRAME;

	if(!continuing)
		InitPlayer();
	else
	{
		ContinuePlayer();
		continuing=0;
	}

	InitStatus();

	AWAIT SetupLevel(GetLevel());
	mainmgl->LastKeyPressed();
	naughtyCounter=0;

	while(!quit)
	{
		lastTime+=endclock-clock;
		clock=GetTime();
		/*
		while(lastTime<TIME_PER_FRAME)
		{
			// this is a delaying action if it is running too fast
			// this prevents that horrible delay when it draws the
			// same frame twice
			endclock=GetTime();
			if(endclock-clock+lastTime>=TIME_PER_FRAME)
				lastTime+=endclock-clock;
		}
		*/
		if(!CentipedeRun(&lastTime))
			CO_RETURN 0;
		//if(numRunsToMakeUp)
		if(!(AWAIT CentipedeDraw()))
			CO_RETURN 0;

		endclock=GetTime();
		if(gameMode==GAMEMODE_QUIT)
			quit=1;

		if(gameMode==GAMEMODE_WIN)
		{
			// you just won a level, set up the next one
			if(AWAIT PlayerWinLevel())
			{
				// just won the whole damn game
				quit=2;
				quitReason=QR_WIN;
			}
			naughtyCounter=0;
		}
	}
	if(quit==2)
	{
		AWAIT Victory();
	}

	CO_RETURN 1;
}

TASK(byte) CentipedeGame(void)
{
	byte quit;
	byte k;
	byte ret;

	quit=0;

	InitStatus();
	continuing=0;

	//SplashScreen("graphics/intro.bmp",3*30+64,108,0);

	GetPaletteFromBMP("graphics/status.bmp");

	while(!quit)
	{
		if(!continuing)
			k=AWAIT TitleMenu();
		else
			k=0;
		switch(k)
		{
			case 0:	//play
				quit=1-(AWAIT CentipedePlay());
				if(quitReason==QR_LOSE)
				{
					AWAIT EnterHighScore();
					continuing=AWAIT Continue();
				}
				else if(quitReason==QR_WIN)
					AWAIT EnterHighScore();

				break;
			case 1: //options
				AWAIT OptionsScreen();
				break;
			case 2:
				quit=1;
				break;
		}
	}
	ret=0;
#ifdef DEMO
	ret=Nag(gamemgl);
#endif
	StopSong();

	//SplashScreen("graphics/trail1.bmp",5*30+64,109,0);
	//SplashScreen("graphics/trail2.bmp",3*30,110,1);
	CO_RETURN ret;
}

void StopPausing(void)
{
	gameMode=GAMEMODE_PLAY;
}

void ExitGame(void)
{
	quitReason=QR_CHOOSE;
	gameMode=GAMEMODE_QUIT;
}

void GameOver(void)
{
	gameMode=GAMEMODE_OVER;
	countDown=60;
	strcpy(lvlText[0],"GAME OVER");
	strcpy(lvlText[1],"Try Again!");
	textY=-80;
	textBright=32;
	MakeSound(SND_GAMEOVER,2000);
}

byte CentipedeRun(int *lastTime)
{
	byte k;

	numRunsToMakeUp=0;
	while(*lastTime>=TIME_PER_FRAME)
	{
		if(gameMode==GAMEMODE_START || gameMode==GAMEMODE_OVER)
		{
			if(textY<140)
				textY+=20;
			if(textBright>0)
				textBright-=2;
		}
		else if(textY<480)
		{
			textY+=40;
		}

		//update everything
		UpdateMouse();

		if(gameMode==GAMEMODE_PLAY)
		{
			UpdateBackgd();

			if(countDown>0)
				AdvanceLogo();
			else
			{
				UpdatePlayer();
				UpdateLevel();
				k=UpdateGuys();
			}
			if(LevelMode()==LVL_BATTLE)
			{
				if(k==0 && countDown==0)
				{
					// all target creatures have been annihilated
					StartTicking(30);	// so switch it to a timed level with 30 ticks=1 second,
										// so the level doesn't just cut out
				}
			}
			else
			{
				if(TimeIsUp() && countDown==0 && k==0)
				{
					// the player survived long enough
					countDown=30;	// play for one more second
					PlayerGetPoints(500);
					MakeSound(105,5000);
					StopSong();
				}
			}
			UpdateParticles();
			UpdateMission();
		}
		UpdateStatusDisplay();

		*lastTime-=TIME_PER_FRAME;
		numRunsToMakeUp++;
		if(gameMode==GAMEMODE_PLAY)
			naughtyCounter++;

		if((countDown) && gameMode!=GAMEMODE_PAUSE)
		{
			countDown--;
			if(gameMode==GAMEMODE_START)
			{
				if(countDown==0)
					gameMode=GAMEMODE_PLAY;
				RetractLogo();
			}
			else if(gameMode==GAMEMODE_OVER && countDown==0)
			{
				quitReason=QR_LOSE;
				gameMode=GAMEMODE_QUIT;
			}
			else if(gameMode==GAMEMODE_PLAY && countDown==0)
				gameMode=GAMEMODE_WIN;
		}
	}
	JamulSoundUpdate();

	k=mainmgl->LastKeyPressed();

	if(k==27)
	{
		if(gameMode==GAMEMODE_PLAY)
		{
			gameMode=GAMEMODE_PAUSE;
			EnterPauseMode();
		}
		else if(gameMode==GAMEMODE_PAUSE)
		{
			gameMode=GAMEMODE_PLAY;
			ExitPauseMode();
		}
	}
	if(Cheating())
	{
		if(k=='1')
		{
			CheatWin();
		}
		if(k=='2')
		{
			MonitorSwitch(1);
			MissionWin(1);	// cheat win
		}
	}
	return 1;
}

void CheatWin(void)
{
	gameMode=GAMEMODE_WIN;
}

byte NaughtyTime(void)
{
	return (naughtyCounter>30*120);	// 2 minutes
}

TASK(byte) CentipedeDraw(void)
{
	/*
	if(!GM_getDoDraw())
		return 1;		// not supposed to draw
	*/

	// draw everything
	DrawBackgd();
	PreRenderParticles();
	RenderGuys();
	RenderAllSprites();
	RenderParticles();	// particles go atop everything but the status bar
	RenderMissionTarget();
	RenderLogo();
	RenderStatusDisplay();

	if(gameMode==GAMEMODE_START || gameMode==GAMEMODE_OVER)
	{
		CenterPrint(textY,lvlText[0],2,textBright);
		CenterPrint(textY+62,lvlText[1],2,textBright);
	}
	else if(textY<480)
	{
		CenterPrint(textY,lvlText[0],2,textBright);
		CenterPrint(textY+62,lvlText[1],2,textBright);
	}

	if(!(AWAIT FlipScreen()))
		CO_RETURN 0;

	CO_RETURN 1;
}
