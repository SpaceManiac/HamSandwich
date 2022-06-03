#include "ironman.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "pause.h"
#include "music.h"
#include "options.h"
#include "quest.h"
#include "achieve.h"

#pragma pack(push, 1)
typedef struct ironMan_t
{
	dword score;
	byte  madcap;	// 0=not, 1=in madcap, 2=finished
	float percentAmt;
	dword moneyBonus;
	dword percentBonus;
	word  crystals;
	dword timeBonus;
} ironMan_t;
static_assert(sizeof(ironMan_t) == 23);
#pragma pack(pop)

static ironMan_t ironMan;

static byte subcursor=0;
static char lastKey=0;
static byte submode=0;
static byte oldc;

void IronmanScore(byte madcapDone)
{
	int i;

	ironMan.score=player.xp;
	if(player.var[VAR_MADCAP])
	{
		if(madcapDone)
			ironMan.madcap=2;
		else
			ironMan.madcap=1;
		// made it into madcap mode, so add a bonus for regular mode, then the madcap percent
		// 2,000pts per percent in madcap
		ironMan.percentAmt=CalcPercent(&player);
		ironMan.percentBonus=(dword)(ironMan.percentAmt*2000.0f);
		ironMan.score+=ironMan.percentBonus;
	}
	else
	{
		ironMan.madcap=0;
		ironMan.percentAmt=CalcPercent(&player);
		// 1,000pts per percent in normal
		ironMan.percentBonus=(dword)(ironMan.percentAmt*1000.0f);
		ironMan.score+=ironMan.percentBonus;
	}
	ironMan.moneyBonus=player.money;
	ironMan.score+=ironMan.moneyBonus;

	ironMan.crystals=0;
	for(i=0;i<11;i++)
		ironMan.crystals+=player.var[VAR_MADGEM+i];
	ironMan.score+=ironMan.crystals*100;	// 100 points per madcap crystal bought, doesn't make up for money spent, but it helps

	// time bonus
	if(player.playClock<30*60*60*10 && madcapDone)
	{
		ironMan.timeBonus=(player.playClock-(30*60*60*10));	// you'd get just over 1 million points for finishing instantly.  Lose 30pts per second
	}
	else
		ironMan.timeBonus=0;
	// no time bonus if you died!  Dying quickly is not good
	ironMan.score+=ironMan.timeBonus;
}

void ExitIronman(void)
{
}

void InitIronman(void)
{
	MakeNormalSound(SND_PAUSE);
	lastKey=0;
	subcursor=0;
	submode=0;
	oldc=255;
	IronmanScore((byte)((player.var[VAR_MADCAP]>0) && (player.var[VAR_QUESTDONE+QUEST_HAPPYSTICK])));
}

void HandleIronmanKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

byte UpdateIronman(MGLDraw *mgl)
{
	byte c;
	static byte reptCounter=0;

	c=GetControls()|GetArrows();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if(submode==0)	// looking at the score
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{

		}
		if((c&CONTROL_DN) && (!reptCounter))
		{

		}
		if((c&CONTROL_LF) && (!reptCounter))
		{

		}
		if((c&CONTROL_RT) && (!reptCounter))
		{

		}
		if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
		{
		}
		if((c&CONTROL_B2) && (!(oldc&CONTROL_B2)))
		{
			// exit the options menu
			return 1;
		}
	}
	else if(submode==1)	// waiting for it to submit to web
	{

	}

	oldc=c;

	HandleIronmanKeyPresses(mgl);
	if(lastKey==27)	// hit ESC to exit
	{
		if(submode==1)
			submode=0;
		else
			return 1;

		lastKey=0;
	}
	return 0;
}

static void IronmanButton(int x,int y,int wid,char *txt,byte spot)
{
	if(subcursor!=spot || submode!=0)
		PauseBox(x,y,x+wid,y+28,240);
	else if(submode==0)
		PauseBox(x,y,x+wid,y+28,32*5+16);
	CenterPrintDark(x+wid/2,y+2,txt,0);
}

void NumPrint(char *s,int x,int y)
{
	int i;
	char s2[2];

	s2[1]='\0';
	for(i=0;i<(int)strlen(s);i++)
	{
		s2[0]=s[strlen(s)-1-i];
		RightPrintGlow(x,y,s2,0,0);
		x-=15;
	}
}

void RenderIronman(void)
{
	char s[64];
	int i,y;

	GetDisplayMGL()->ClearScreen();

	PauseBox(0,0,638,40,234);
	CenterPrintGlow(320,8,"Ironman Results",0,0);

	PauseBox(0,45,638,470,234);

	y=50;
	RightPrintGlow(300,y,"Total Score",0,0);
	sprintf(s,"%d",ironMan.score);
	NumPrint(s,340+15*8,y);
	DrawFillBox(100,y+30,540,y+31,31);
	y+=40;
	sprintf(s,"%0.1f%% Complete",ironMan.percentAmt);
	RightPrintGlow(300,y,s,0,0);
	sprintf(s,"%d",ironMan.percentBonus);
	NumPrint(s,340+15*8,y);
	y+=30;
	if(ironMan.madcap)
		RightPrintGlow(300,y,"(x2,000 Madcap Bonus)",0,0);
	else
		RightPrintGlow(300,y,"(x1,000 points)",0,0);
	y+=30;

	RightPrintGlow(300,y,"XP Bonus",0,0);
	sprintf(s,"%d",player.xp);
	NumPrint(s,340+15*8,y);
	y+=30;
	RightPrintGlow(300,y,"Moneybags Bonus",0,0);
	sprintf(s,"%d",ironMan.moneyBonus);
	NumPrint(s,340+15*8,y);
	y+=30;
	if(ironMan.madcap)
	{
		RightPrintGlow(300,y,"Madcap Crystal Bonus",0,0);
		sprintf(s,"%d",ironMan.crystals*100);
		NumPrint(s,340+15*8,y);
		y+=30;
	}
	if(ironMan.madcap==2)
	{
		RightPrintGlow(300,y,"Victory Time Bonus",0,0);
		sprintf(s,"%d",ironMan.timeBonus);
		NumPrint(s,340+15*8,y);
		y+=30;
		sprintf(s,"(%d:%02d Total Time)",player.playClock/(30*60*60),(player.playClock/(30*60))%60);
		RightPrintGlow(300,y,s,0,0);
	}
	i=0;
	strcpy(s,"Modifiers: ");
	if(player.var[VAR_MODIFIER+0])
	{
		i++;
		strcat(s,ModifierName(player.var[VAR_MODIFIER+0]));
	}
	if(player.var[VAR_MODIFIER+1])
	{
		if(i>0)
			strcat(s,", ");
		i++;
		strcat(s,ModifierName(player.var[VAR_MODIFIER+1]));
	}
	if(player.var[VAR_MODIFIER+2])
	{
		if(i>0)
			strcat(s,", ");
		i++;
		strcat(s,ModifierName(player.var[VAR_MODIFIER+2]));
	}
	if(i==0)
		strcat(s,"None");
	CenterPrintGlow(320,y,s,0,0);
	y+=30;
	if(ironMan.madcap!=2)
	{
		sprintf(s,"Time played: %d:%02d",player.playClock/(30*60*60),(player.playClock/(30*60))%60);
		CenterPrintGlow(320,y,s,0,0);
	}
}

TASK(void) IronmanScreen(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	InitIronman();
	lastTime=1;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		if(lastTime>TIME_PER_FRAME*30)
			lastTime=TIME_PER_FRAME*30;

		while(lastTime>=TIME_PER_FRAME && !done)
		{
			done=UpdateIronman(mgl);
			lastTime-=TIME_PER_FRAME;
		}
		RenderIronman();
		AWAIT mgl->Flip();
		EndClock();

		if(!mgl->Process())
			done=1;
	}

	ExitIronman();
}
