#include "status.h"
#include "player.h"
#include "backgd.h"
#include "display.h"
#include "game.h"
#include "options.h"
#include "sound.h"
#include "mission.h"
#include "random.h"

#define CURSOR_SPR 20

#define UPPERLEFT  0
#define UPPERRIGHT 1
#define LOWERRIGHT 2
#define LOWERLEFT  3

int monitorX;
byte monitorOn,cursorOn;
byte monitorWasOn;
byte pauseMode;
int cursorX,cursorY;
byte cursorBrt;
int statusScore;	// what is showing as the score, not necessarily the real score
byte lastQuad,timesAround;	// for spinning the mouse to cheat
byte hilited;			// which menu item is hilited

static byte keys,oldKeys;

// hi score chart
static char buffer[10];
static int  pos;

char monitorText[5][32];

void InitStatus(void)
{
	cursorOn=0;
	cursorBrt=0;
	monitorOn=0;
	monitorWasOn=0;
	monitorX=640;
	monitorText[0][0]='\0';
	monitorText[1][0]='\0';
	monitorText[2][0]='\0';
	monitorText[3][0]='\0';
	cursorX=(636-485)/2;
	cursorY=(428-321)/2;
	pauseMode=0;
	statusScore=0;
	lastQuad=0;	// upper-left
	timesAround=0;
	hilited=0;
}

void ExitStatus(void)
{
}

void RenderStatusDisplay(void)
{
	int i;
	int s;
	char txt[20];

	DrawStatus();

	// display gun power level
	ShowSprite(SPR_STATUS,GetPower(),571,146);

	// display current weapon
	s=GetWeapon();
	if(s!=WPN_NONE)
		ShowSprite(SPR_STATUS,14+s,497,119);
	
	// display current ammo
	if(s!=WPN_NONE)
	{
		s=GetAmmo();
		ShowSprite(SPR_STATUS,5+(s%10),531+16,126);
		ShowSprite(SPR_STATUS,5+(s/10),531,126);
	}

	// display score
	s=statusScore;
	for(i=0;i<9;i++)
	{
		if(s==0 && i>0)
			break;	// no leading zeroes

		ShowSprite(SPR_STATUS,5+(s%10),493+16*8-16*i,58);

		s/=10;
	}

	// display lives
	s=GetLives();
	for(i=0;i<s;i++)
	{
		if(i>=8)
			break;	// can only show 8 lives
		ShowSprite(SPR_STATUS,3,620-18*i,90);
	}
	// the monitor display
	if(monitorX<640)
	{
		ShowSprite(SPR_STATUS,4,monitorX,312);
		if(!pauseMode)
		{
			PrintGreen(monitorX+15,324,monitorText[0]);
			PrintGreen(monitorX+15,344,monitorText[1]);
			PrintGreen(monitorX+15,364,monitorText[2]);
			PrintGreen(monitorX+15,384,monitorText[3]);
			PrintGreen(monitorX+15,404,monitorText[4]);
		}
		else
		{
			// print the pause things
			PrintGreen(monitorX+15,324,"  ** Game Paused **");
			PrintBright(monitorX+15,344,"Continue",-10+10*(hilited==1),1);
			PrintBright(monitorX+100,344,"Exit Game",-10+10*(hilited==2),1);
			if(SoundIsOn())
				PrintBright(monitorX+15,384,"Sound On",-10+10*(hilited==3),1);
			else
				PrintBright(monitorX+15,384,"Sound Off",-10+10*(hilited==3),1);
			if(MusicIsOn())
				PrintBright(monitorX+100,384,"Music On",-10+10*(hilited==4),1);
			else
				PrintBright(monitorX+100,384,"Music Off",-10+10*(hilited==4),1);

			if(Cheating())	// add cheat options too
			{
				PrintBright(monitorX+15,404,"Power Up",-10+10*(hilited==5),1);
				PrintBright(monitorX+100,404,"Skip Level",-10+10*(hilited==6),1);
			}
		}
		if(cursorOn)
		{
			ShowSprite(SPR_STATUS,CURSOR_SPR,monitorX+11+cursorX,321+cursorY);
		}
	}
	// the high scores
	for(i=0;i<5;i++)
	{
		Print(495,182+24*i,GetHiScoreName(i),1);
		sprintf(txt,"%d",GetHiScore(i));
		Print(495+140-StringLength(txt,1),182+24*i,txt,1);
	}
}

void RenderHiScoreDisplay(void)
{
	int i;
	char txt[20];

	DrawStatus();

	// the high scores
	for(i=0;i<5;i++)
	{
		Print(495,182+24*i,GetHiScoreName(i),1);
		sprintf(txt,"%d",GetHiScore(i));
		Print(495+140-StringLength(txt,1),182+24*i,txt,1);
	}
}

void PauseModeUpdate(void)
{
	// check for mouse spin
	if(cursorX<152/2)
	{
		// left half of the screen
		if(cursorY<108/2)
		{
			// upper left
			if(lastQuad==LOWERLEFT)
				timesAround++;
			else if(lastQuad!=UPPERLEFT)
				timesAround=0;
			lastQuad=UPPERLEFT;
		}
		else
		{
			// lower left
			if(lastQuad==LOWERRIGHT)
				timesAround++;
			else if(lastQuad!=LOWERLEFT)
				timesAround=0;
			lastQuad=LOWERLEFT;
		}
	}
	else	// right half
	{
		if(cursorY<108/2)
		{
			// upper right
			if(lastQuad==UPPERLEFT)
				timesAround++;
			else if(lastQuad!=UPPERRIGHT)
				timesAround=0;
			lastQuad=UPPERRIGHT;
		}
		else
		{
			// lower right
			if(lastQuad==UPPERRIGHT)
				timesAround++;
			else if(lastQuad!=LOWERRIGHT)
				timesAround=0;
			lastQuad=LOWERRIGHT;
		}
	}

	if(timesAround>16)
	{
		timesAround=0;
		MakeSound(SND_CHEAT,1600);
		SetCheat(1);	// start cheating
	}

	hilited=0;
	if(cursorX<60 && cursorX>0 && cursorY>20 && cursorY<20+20)
		hilited=1;
	if(cursorX<150 && cursorX>90 && cursorY>20 && cursorY<20+20)
		hilited=2;
	if(cursorX<60 && cursorX>0 && cursorY>60 && cursorY<60+20)
		hilited=3;
	if(cursorX<150 && cursorX>90 && cursorY>60 && cursorY<60+20)
		hilited=4;
	if(Cheating())
	{
		if(cursorX<60 && cursorX>0 && cursorY>80 && cursorY<80+20)
			hilited=5;
		if(cursorX<150 && cursorX>90 && cursorY>80 && cursorY<80+20)
			hilited=6;
	}
	// this checks for clicks and such on the pause buttons
	if(MouseClick() || ((keys&CONTROL_B1) && (!(oldKeys&CONTROL_B1))))
	{
		switch(hilited)
		{
			case 0:
				break;
			case 1:
				StopPausing();
				ExitPauseMode();
				break;
			case 2:
				ExitGame();
				break;
			case 3:
				ToggleSound();
				break;
			case 4:
				ToggleMusic();
				break;
			case 5:
				MissionWin(1);
				break;
			case 6:
				StopPausing();
				ExitPauseMode();
				CheatWin();
				break;
		}
	}
}

void StatusKeyPress(void)
{
	int h=hilited;

	if((keys&CONTROL_UP) && (!(oldKeys&CONTROL_UP)))
	{
		if(hilited==0)
			hilited=1;
		else if(hilited>2)
			hilited-=2;
	}
	if((keys&CONTROL_DN) && (!(oldKeys&CONTROL_DN)))
	{
		if(hilited==0)
			hilited=1;
		else if(hilited<3+2*Cheating())
			hilited+=2;
	}
	if((keys&CONTROL_LF) && (!(oldKeys&CONTROL_LF)))
	{
		if(hilited==0)
			hilited=1;
		else if((hilited&1)==0)
			hilited--;
	}
	if((keys&CONTROL_RT) && (!(oldKeys&CONTROL_RT)))
	{
		if(hilited==0)
			hilited=1;
		else if((hilited&1)==1)
			hilited++;
	}

	if(hilited!=h)	// moved, so move the cursor
	{
		if((hilited&1)==1)
			cursorX=30;
		else
			cursorX=120;

		if(hilited<3)
			cursorY=30;
		else if(hilited<5)
			cursorY=70;
		else
			cursorY=90;
	}
}

void UpdateStatusDisplay(void)
{
	int msx,msy;
	int s;

	if(monitorOn)
	{
		if(monitorX>474)
			monitorX-=16;
		if(monitorX<474)
			monitorX=474;
	}
	else
	{
		if(monitorX<640)
			monitorX+=16;
		if(monitorX>640)
			monitorX=640;
	}

	GetMouse(&msx,&msy);
	oldKeys=keys;
	keys=GetControls();

	if(keys&(CONTROL_UP|CONTROL_LF|CONTROL_DN|CONTROL_RT))
		StatusKeyPress();
	
	cursorX+=msx/2;
	cursorY+=msy/2;
	if(cursorX>(636-485-4))
		cursorX=636-485-4;
	if(cursorX<5)
		cursorX=5;
	if(cursorY<5)
		cursorY=5;
	if(cursorY>(428-321-4))
		cursorY=428-321-4;

	s=GetScore();
	if(s>statusScore+100)
		statusScore+=100;
	if(s>statusScore+10)
		statusScore+=10;
	if(s>statusScore)
		statusScore++;

	if(pauseMode)
		PauseModeUpdate();
}

void SetMonitorText(byte line,char *s)
{
	strncpy(monitorText[line],s,32);
}

void EnterPauseMode(void)
{
	MakeSound(SND_PAUSE,1400);
	pauseMode=1;
	monitorWasOn=monitorOn;
	monitorOn=1;
	cursorOn=1;
	keys=0xFF;
}

void ExitPauseMode(void)
{
	MakeSound(SND_UNPAUSE,1400);
	pauseMode=0;
	monitorOn=monitorWasOn;
	cursorOn=0;

	if(!monitorWasOn)
	{
		// if it wasn't on before, then make sure it still shows this text,
		// so that it looks nice rolling away
		/*
		SetMonitorText(0,pauseText[0]);
		SetMonitorText(1,pauseText[1]);
		SetMonitorText(2,pauseText[2]);
		SetMonitorText(3,pauseText[3]);
		SetMonitorText(4,pauseText[4]);
		*/
	}
}

void MonitorSwitch(byte on)
{
	if(!pauseMode)
	{
		monitorOn=on;
	}
	else
	{
		monitorWasOn=on;
	}
}

static char b=0;
static char bdir=1;

// this records a high score
byte HighScoreEntryRender(char *txt)
{
	int len;

	DrawBackgd();
	RenderHiScoreDisplay();
	CenterPrint(40,"Congratulations!",2,b);
	CenterPrint(100,"You got a high score!",2,0);
	CenterPrint(160,"Enter your name:",2,0);
	len=strlen(txt);
	// throw on a blinking cursor
	txt[len]='_';
	txt[len+1]='\0';
	if((b&2)!=0)
	{
		CenterPrint(220,txt,2,0);
	}
	else
	{
		CenterPrint2(220,txt,2,0);
	}
	txt[len]='\0';

	return FlipScreen();
}

byte HighScoreEntryUpdate(byte whichScore,int *lastTime)
{
	byte done=0;
	char c;

	while(*lastTime>=TIME_PER_FRAME)
	{

		b+=bdir;
		if(b>16)
			bdir=-1;
		if(b<-8)
			bdir=1;
		(*lastTime)-=TIME_PER_FRAME;
	}

	if(c=LastKeyPressed())
	{
		if(c==8) // backspace
		{
			if(pos>0)
			{
				pos--;
				buffer[pos]='\0';					
			}
			strcpy(GetHiScoreName(whichScore),buffer);
		}
		else if(c==27)
		{
			done=1;
			buffer[0]='\0';
			strcpy(GetHiScoreName(whichScore),"????????");
		}
		else if(c==13)
		{
			done=1;
			buffer[pos]='\0';
			strcpy(GetHiScoreName(whichScore),buffer);
		}
		else if(pos<8)
		{
			buffer[pos]=c;				
			pos++;
			buffer[pos]='\0';
			strcpy(GetHiScoreName(whichScore),buffer);
		}
	}
	if((!HighScoreEntryRender(buffer)) && (done==0))
		return 1;

	return done;
}

void EnterHighScore(void)
{
	int i;
	int rank;
	rank=5;
	int lastTime;
	dword clock,endclock;

	strcpy(buffer,"");

	for(i=4;i>-1;i--)
		if(GetScore()>GetHiScore(i))
			rank=i;

	for(i=3;i>=rank;i--)
	{
		SetHiScore(i+1,GetHiScore(i));
		strcpy(GetHiScoreName(i+1),GetHiScoreName(i));
	}
	strcpy(GetHiScoreName(rank),"");
	SetHiScore(rank,GetScore());
	pos=0;

	if(rank<5)	// got a high score!
	{
		LastKeyPressed();
		lastTime=0;
		endclock=0;
		clock=0;
		while(1)
		{	
			lastTime+=endclock-clock;
			clock=GetTime();

			if(HighScoreEntryUpdate(rank,&lastTime))
			{
				if(Cheating())	// tee hee
					strcpy(GetHiScoreName(rank),"CHEATER!");
				ExitStatus();	// save high score
				return;	// entered
			}
			endclock=GetTime();
		}
		
	}
	else
	{
		// didn't get a high score
	}
}