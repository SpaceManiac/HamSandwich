#include "endgame.h"
#include "display.h"
#include "player.h"
#include "fireworks.h"
#include "badge.h"
#include "bossbash.h"
#include "highscore.h"
#include "log.h"

char rankText[][32]={
	"Scalliwag",
	"Cad",
	"Ruffian",
	"Weenie",
	"Peon",
	"Peasant",
	"Citizen",
	"Upper Crust",
	"High Class",
	"Knight",
	"Baron",
	"Duke",
	"Lord",
	"King",
	"Emperor",
	"Hero",
	"Superhero",
	"Ultrahero",
	"Uberhero",
	"Gyro Sandwich",
	"Jamulio",
};

#define ST_NUM	0
#define ST_TIME 1
#define ST_PCT	2
#define ST_BASH 3	// bash power level

typedef struct stat_t
{
	byte type;
	char name[32];
	int val;
	char xtra[32];
	int points;
	int curPts;
	byte visible;
} stat_t;

stat_t stat[16];
byte numStats;
byte curStat;
int totalScore;

void Commaize(char *s,int n)
{
	char s2[8];
	byte zero;

	s[0]='\0';
	zero=0;

	if(n<0)
	{
		n=-n;
		s[0]='-';
		s[1]='\0';
	}
	if(n>=1000000)
	{
		zero=1;
		sprintf(s2,"%d,",n/1000000);
		strcat(s,s2);
	}
	if(n>=1000)
	{
		if(!zero)
			sprintf(s2,"%d,",(n/1000)%1000);
		else
			sprintf(s2,"%03d,",(n/1000)%1000);
		strcat(s,s2);
		zero=1;
	}
	if(!zero)
		sprintf(s2,"%d",n%1000);
	else
		sprintf(s2,"%03d",n%1000);
	strcat(s,s2);
}

void ShowStat(byte w,int x,int y)
{
	char s[88];

	Print(x,y,stat[w].name,0,0);
	switch(stat[w].type)
	{
		case ST_NUM:
			// straight number
			Commaize(s,stat[w].val);
			RightPrint(x+210,y,s,0,0);
			break;
		case ST_TIME:
			// time
			sprintf(s,"%02d:%02d:%02d",
				stat[w].val/(30*60*60),(stat[w].val/(30*60))%60,(stat[w].val/30)%60);
			RightPrint(x+210,y,s,0,0);
			break;
		case ST_PCT:
			// percent
			sprintf(s,"%0.2f%%",(float)stat[w].val/10000);
			RightPrint(x+210,y,s,0,0);
			break;
		case ST_BASH:
			// bash power level
			RightPrint(x+210,y,BashPowerName(stat[w].val),0,0);
			break;
	}
	Print(x+220,y,stat[w].xtra,0,0);
	Print(x+360,y,"=",0,0);
	Commaize(s,stat[w].curPts);
	RightPrint(x+500,y,s,0,0);
}

byte GetRank(int score)
{
	if(score<0)
		score=0;

	score/=100000;
	if(score>20)
		score=20;
	return (byte)score;
}

int GetTimePoints(dword time,dword best,dword bestScore,dword betterPts,dword worsePts)
{
	int result;

	if(time<=best)
	{
		result=bestScore+(betterPts*(best-time))/30;
	}
	else
	{
		result=bestScore-(worsePts*(time-best))/30;
	}
	return result;
}

void InitEndGame(void)
{
	int i;

	DBG("InitEndGame");
	for(i=0;i<16;i++)
	{
		stat[i].curPts=0;
		stat[i].type=ST_NUM;
		stat[i].val=0;
		stat[i].visible=0;
		stat[i].xtra[0]='\0';
		stat[i].name[0]='\0';
	}
	switch(player.worldNum)
	{
		case WORLD_NORMAL:
		case WORLD_REMIX:
			numStats=10;
			// percent done
			strcpy(stat[0].name,"Complete");
			stat[0].val=(int)(CalcPercent(&player)*10000);
			strcpy(stat[0].xtra,"x10,000");
			stat[0].points=(int)(CalcPercent(&player)*10000);
			stat[0].type=ST_PCT;
			// time to finish
			if((player.cheatsOn&PC_HARDCORE) && player.hearts==0)
			{
				strcpy(stat[1].name,"Time");
				stat[1].val=player.timeToFinish;
				stat[1].points=0;
				stat[1].type=ST_TIME;
				strcpy(stat[1].xtra,"(failed)");
			}
			else
			{
				strcpy(stat[1].name,"Time");
				stat[1].val=player.timeToFinish;
				stat[1].points=GetTimePoints(player.timeToFinish,30*60*60*8,1000000,200,200);
				stat[1].type=ST_TIME;
				strcpy(stat[1].xtra,"(8:00:00 par)");
			}
			// monster points
			strcpy(stat[2].name,"Monster Pts");
			stat[2].val=player.monsterPoints;
			strcpy(stat[2].xtra,"x10");
			stat[2].points=player.monsterPoints*10;
			// gems collected
			strcpy(stat[3].name,"Gems Obtained");
			stat[3].val=player.gemsGotten;
			strcpy(stat[3].xtra,"x10");
			stat[3].points=player.gemsGotten*10;
			// best combo
			strcpy(stat[4].name,"Best Combo");
			stat[4].val=player.bestCombo;
			strcpy(stat[4].xtra,"x5,000");
			stat[4].points=player.bestCombo*5000;
			// badguys killed
			strcpy(stat[5].name,"Badguys Slain");
			stat[5].val=player.guysKilled;
			strcpy(stat[5].xtra,"x5");
			stat[5].points=player.guysKilled*5;
			// shotsFired
			strcpy(stat[6].name,"Shots Fired");
			stat[6].val=player.shotsFired;
			strcpy(stat[6].xtra,"x-10");
			stat[6].points=player.shotsFired*-10;
			// special shots fired
			strcpy(stat[7].name,"Special Shots");
			stat[7].val=player.specialShotsFired;
			strcpy(stat[7].xtra,"x-10");
			stat[7].points=player.specialShotsFired*-10;
			// number of saves
			strcpy(stat[8].name,"Saved Games");
			stat[8].val=player.numSaves;
			strcpy(stat[8].xtra,"x-5,000");
			stat[8].points=player.numSaves*-5000;
			// hits taken
			strcpy(stat[9].name,"Hits Taken");
			stat[9].val=player.hitsTaken;
			strcpy(stat[9].xtra,"x-500");
			stat[9].points=player.hitsTaken*-500;
			break;
		case WORLD_SURVIVAL:
			numStats=9;
			// level reached
			strcpy(stat[0].name,"Level Passed");
			stat[0].val=player.numSaves;
			strcpy(stat[0].xtra,"x50,000");
			stat[0].points=player.numSaves*50000;
			if(player.cheatsOn&PC_INFSURV)
			{
				strcpy(stat[0].xtra,"x25,000");
				stat[0].points=player.numSaves*25000;
			}
			// time to finish
			if(player.cheatsOn&PC_INFSURV)
			{
				strcpy(stat[1].name,"Time");
				stat[1].val=player.timeToFinish;
				stat[1].points=0;
				stat[1].type=ST_TIME;
				strcpy(stat[1].xtra,"(no points)");
			}
			else
			{
				if(player.numSaves==25)
				{
					strcpy(stat[1].name,"Time");
					stat[1].val=player.timeToFinish;
					stat[1].points=GetTimePoints(player.timeToFinish,30*60*10,1000000,1000,1000);
					stat[1].type=ST_TIME;
					strcpy(stat[1].xtra,"(10:00 par)");
				}
				else
				{
					strcpy(stat[1].name,"Time");
					stat[1].val=player.timeToFinish;
					stat[1].points=-1000000;
					stat[1].type=ST_TIME;
					strcpy(stat[1].xtra,"(failed)");
				}
			}
			// monster points
			strcpy(stat[2].name,"Monster Pts");
			stat[2].val=player.monsterPoints;
			strcpy(stat[2].xtra,"x100");
			stat[2].points=player.monsterPoints*100;
			if(player.cheatsOn&PC_INFSURV)
			{
				stat[2].val=player.monsterPoints;
				strcpy(stat[2].xtra,"x500");
				stat[2].points=player.monsterPoints*500;
			}
			// gems collected
			strcpy(stat[3].name,"Gems Obtained");
			stat[3].val=player.gemsGotten;
			strcpy(stat[3].xtra,"x100");
			stat[3].points=player.gemsGotten*100;
			// best combo
			strcpy(stat[4].name,"Best Combo");
			stat[4].val=player.bestCombo;
			strcpy(stat[4].xtra,"x10,000");
			stat[4].points=player.bestCombo*10000;
			// badguys killed
			strcpy(stat[5].name,"Badguys Slain");
			stat[5].val=player.guysKilled;
			strcpy(stat[5].xtra,"x500");
			stat[5].points=player.guysKilled*500;
			// shotsFired
			strcpy(stat[6].name,"Shots Fired");
			stat[6].val=player.shotsFired;
			strcpy(stat[6].xtra,"x-500");
			stat[6].points=player.shotsFired*-500;
			// special shots fired
			strcpy(stat[7].name,"Special Shots");
			stat[7].val=player.specialShotsFired;
			strcpy(stat[7].xtra,"x-200");
			stat[7].points=player.specialShotsFired*-200;
			// hits taken
			strcpy(stat[8].name,"Hits Taken");
			stat[8].val=player.hitsTaken;
			strcpy(stat[8].xtra,"x-5000");
			stat[8].points=player.hitsTaken*-5000;
			if(player.cheatsOn&PC_INFSURV)
			{
				stat[8].val=player.hitsTaken;
				strcpy(stat[8].xtra,"x-2000");
				stat[8].points=player.hitsTaken*-2000;
			}
			break;
		case WORLD_BOWLING:
			numStats=5;
			// points
			strcpy(stat[0].name,"Points");
			stat[0].val=player.monsterPoints;
			strcpy(stat[0].xtra,"x10,000");
			stat[0].points=player.monsterPoints*10000;
			// strikes
			strcpy(stat[1].name,"Strikes");
			stat[1].val=player.gemsGotten;
			strcpy(stat[1].xtra,"x100,000");
			stat[1].points=player.gemsGotten*100000;
			// gutterballs
			strcpy(stat[2].name,"Gutterballs");
			stat[2].val=player.specialShotsFired;
			strcpy(stat[2].xtra,"x-50,000");
			stat[2].points=player.specialShotsFired*-50000;
			// used balls
			strcpy(stat[3].name,"Balls Used");
			stat[3].val=player.shotsFired;
			strcpy(stat[3].xtra,"x-50,000");
			stat[3].points=player.shotsFired*-50000;
			// time
			strcpy(stat[4].name,"Time");
			stat[4].val=player.timeToFinish;
			stat[4].points=GetTimePoints(player.timeToFinish,30*60*2,100000,5000,1000);
			stat[4].type=ST_TIME;
			strcpy(stat[4].xtra,"(2:00 par)");
			break;
		case WORLD_LOONYBALL:
			numStats=5;
			// points
			strcpy(stat[0].name,"Points");
			stat[0].val=(player.gemsGotten-5000);
			strcpy(stat[0].xtra,"x100,000");
			stat[0].points=(player.gemsGotten-5000)*100000;
			// strikes
			strcpy(stat[1].name,"Penalties");
			stat[1].val=player.numSaves;
			strcpy(stat[1].xtra,"x-100,000");
			stat[1].points=player.numSaves*-100000;
			// gutterballs
			strcpy(stat[2].name,"Games Played");
			stat[2].val=player.levelNum;
			strcpy(stat[2].xtra,"x250,000");
			stat[2].points=player.levelNum*250000;
			// used balls
			strcpy(stat[3].name,"Out Of Bounds");
			player.guysKilled=player.oob;
			stat[3].val=player.guysKilled;
			strcpy(stat[3].xtra,"x-100,000");
			stat[3].points=player.guysKilled*-100000;
			// time
			if(player.levelNum==5)
			{
				strcpy(stat[4].name,"Time");
				stat[4].val=player.timeToFinish;
				stat[4].points=GetTimePoints(player.timeToFinish,30*60*6,100000,10000,10000);
				stat[4].type=ST_TIME;
				strcpy(stat[4].xtra,"(6:00 par)");
			}
			else
			{
				strcpy(stat[4].name,"Time");
				stat[4].val=player.timeToFinish;
				stat[4].points=-100000;
				stat[4].type=ST_TIME;
				strcpy(stat[4].xtra,"(failed)");
			}
			break;
		case WORLD_BOSSBASH:
			DBG("BB-Setup");
			numStats=3;
			// time
			DBG("BB1");
			if(player.hearts>0)
			{
				DBG("BB2");
				strcpy(stat[0].name,"Time");
				stat[0].val=player.timeToFinish;
				DBG("BB3");
				i=player.numSaves-30*30*BashPower();
				if(i<0)
					i=10*30;
				DBG("BB4");
				stat[0].points=GetTimePoints(player.timeToFinish,i,2000000,40000,10000);
				stat[0].type=ST_TIME;
				DBG("BB5");
				sprintf(stat[0].xtra,"(%d:%02d par)",i/(30*60),(i/30)%60);
			}
			else
			{
				strcpy(stat[0].name,"Time");
				stat[0].val=player.timeToFinish;
				stat[0].points=0;
				stat[0].type=ST_TIME;
				strcpy(stat[0].xtra,"(failed)");
			}
			DBG("BB6");
			// power level
			strcpy(stat[1].name,"Power");
			stat[1].val=BashPower();
			DBG("BB7");
			strcpy(stat[1].xtra,"x-100,000");
			stat[1].points=BashPower()*-100000;
			stat[1].type=ST_BASH;
			DBG("BB8");
			// hits taken
			strcpy(stat[2].name,"Hits Taken");
			stat[2].val=player.hitsTaken;
			DBG("BB9");
			strcpy(stat[2].xtra,"x-100,000");
			stat[2].points=player.hitsTaken*-100000;
			DBG("BB10");
			break;
	}
	DBG("BB-Setup2");
	player.rank=0;
	curStat=0;
	totalScore=0;
	GetDisplayMGL()->LastKeyPressed();
	DBG("BB-Setup3");
	GetTaps();
	DBG("BB-Setup4");
	InitFireworks();
	DBG("BB-Setup5");
}

void RenderFinalScore(int x,int y,MGLDraw *mgl)
{
	char s[48];

	DrawBox(x,y,x+500,y+1,31);

	RightPrint(x+340,y+5,"Total Score",0,0);
	Commaize(s,totalScore);
	RightPrint(x+500,y+5,s,0,0);

	RightPrint(x+340,y+35,"Rank",0,0);
	sprintf(s,"%s",rankText[player.rank]);
	RightPrint(x+500,y+35,s,0,0);
}

void ShowHighScore(int x,int y,int score,byte on,byte col,MGLDraw *mgl)
{
	char s[48];

	Commaize(s,score);
	if(on)
	{
		PrintGlow(x,y,s,0,0);
		PrintGlow(x+120,y,rankText[GetRank(score)],0,0);
	}
	else
	{
		PrintColor(x,y,s,col,-10,0);
		PrintColor(x+120,y,rankText[GetRank(score)],col,-10,0);
	}
}

void ShowScoreStats(int x,int y,highScore_t *me,MGLDraw *mgl)
{
	char s[32];

	switch(me->mode)
	{
		case WORLD_NORMAL:
		case WORLD_REMIX:
			PrintGlow(x,y,"Percent Done",0,0);
			sprintf(s,"%2.1f%%",(float)me->spclValue/FIXAMT);
			RightPrintGlow(x+250,y,s,0,0);

			PrintGlow(x,y+25,"Time",0,0);
			sprintf(s,"%02d:%02d:%02d",
				me->time/(30*60*60),(me->time/(30*60))%60,(me->time/30)%60);
			RightPrintGlow(x+250,y+25,s,0,0);

			PrintGlow(x,y+25*2,"Monster Pts",0,0);
			sprintf(s,"%d",me->monsterPoints);
			RightPrintGlow(x+250,y+25*2,s,0,0);

			PrintGlow(x,y+25*3,"Gems Collected",0,0);
			sprintf(s,"%d",me->gemsGotten);
			RightPrintGlow(x+250,y+25*3,s,0,0);

			PrintGlow(x,y+25*4,"Best Combo",0,0);
			sprintf(s,"%d",me->bestCombo);
			RightPrintGlow(x+250,y+25*4,s,0,0);

			PrintGlow(x,y+25*5,"Badguys Slain",0,0);
			sprintf(s,"%d",me->guysKilled);
			RightPrintGlow(x+250,y+25*5,s,0,0);

			PrintGlow(x,y+25*6,"Shots Fired",0,0);
			sprintf(s,"%d",me->shotsFired);
			RightPrintGlow(x+250,y+25*6,s,0,0);

			PrintGlow(x,y+25*7,"Special Shots",0,0);
			sprintf(s,"%d",me->specialShotsFired);
			RightPrintGlow(x+250,y+25*7,s,0,0);

			PrintGlow(x,y+25*8,"Saved Games",0,0);
			sprintf(s,"%d",me->numSaves);
			RightPrintGlow(x+250,y+25*8,s,0,0);

			PrintGlow(x,y+25*9,"Hits Taken",0,0);
			sprintf(s,"%d",me->hitsTaken);
			RightPrintGlow(x+250,y+25*9,s,0,0);
			break;
		case WORLD_SURVIVAL:
		case WORLD_INFSURV:
			PrintGlow(x,y+25*0,"Level Passed",0,0);
			sprintf(s,"%d",me->numSaves);
			RightPrintGlow(x+250,y+25*0,s,0,0);

			PrintGlow(x,y+25,"Time",0,0);
			sprintf(s,"%02d:%02d:%02d",
				me->time/(30*60*60),(me->time/(30*60))%60,(me->time/30)%60);
			RightPrintGlow(x+250,y+25,s,0,0);

			PrintGlow(x,y+25*2,"Monster Pts",0,0);
			sprintf(s,"%d",me->monsterPoints);
			RightPrintGlow(x+250,y+25*2,s,0,0);

			PrintGlow(x,y+25*3,"Gems Collected",0,0);
			sprintf(s,"%d",me->gemsGotten);
			RightPrintGlow(x+250,y+25*3,s,0,0);

			PrintGlow(x,y+25*4,"Best Combo",0,0);
			sprintf(s,"%d",me->bestCombo);
			RightPrintGlow(x+250,y+25*4,s,0,0);

			PrintGlow(x,y+25*5,"Badguys Slain",0,0);
			sprintf(s,"%d",me->guysKilled);
			RightPrintGlow(x+250,y+25*5,s,0,0);

			PrintGlow(x,y+25*6,"Shots Fired",0,0);
			sprintf(s,"%d",me->shotsFired);
			RightPrintGlow(x+250,y+25*6,s,0,0);

			PrintGlow(x,y+25*7,"Special Shots",0,0);
			sprintf(s,"%d",me->specialShotsFired);
			RightPrintGlow(x+250,y+25*7,s,0,0);

			PrintGlow(x,y+25*8,"Hits Taken",0,0);
			sprintf(s,"%d",me->hitsTaken);
			RightPrintGlow(x+250,y+25*8,s,0,0);
			break;
		case WORLD_BOWLING:
			PrintGlow(x,y+25*0,"Points",0,0);
			sprintf(s,"%d",me->monsterPoints);
			RightPrintGlow(x+250,y+25*0,s,0,0);

			PrintGlow(x,y+25*1,"Strikes",0,0);
			sprintf(s,"%d",me->gemsGotten);
			RightPrintGlow(x+250,y+25*1,s,0,0);

			PrintGlow(x,y+25*2,"Gutterballs",0,0);
			sprintf(s,"%d",me->specialShotsFired);
			RightPrintGlow(x+250,y+25*2,s,0,0);

			PrintGlow(x,y+25*3,"Balls Used",0,0);
			sprintf(s,"%d",me->shotsFired);
			RightPrintGlow(x+250,y+25*3,s,0,0);

			PrintGlow(x,y+25*4,"Time",0,0);
			sprintf(s,"%02d:%02d:%02d",
				me->time/(30*60*60),(me->time/(30*60))%60,(me->time/30)%60);
			RightPrintGlow(x+250,y+25*4,s,0,0);
			break;
		case WORLD_LOONYBALL:
			PrintGlow(x,y+25*0,"Points",0,0);
			sprintf(s,"%d",me->gemsGotten-5000);
			RightPrintGlow(x+250,y+25*0,s,0,0);

			PrintGlow(x,y+25*1,"Penalties",0,0);
			sprintf(s,"%d",me->numSaves);
			RightPrintGlow(x+250,y+25*1,s,0,0);

			PrintGlow(x,y+25*2,"Games Played",0,0);
			sprintf(s,"%d",me->bestCombo);
			RightPrintGlow(x+250,y+25*2,s,0,0);

			PrintGlow(x,y+25*3,"Out Of Bounds",0,0);
			sprintf(s,"%d",me->guysKilled);
			RightPrintGlow(x+250,y+25*3,s,0,0);

			PrintGlow(x,y+25*4,"Time",0,0);
			sprintf(s,"%02d:%02d:%02d",
				me->time/(30*60*60),(me->time/(30*60))%60,(me->time/30)%60);
			RightPrintGlow(x+250,y+25*4,s,0,0);
			break;
		case WORLD_BOSSBASH:
			//PrintGlow(x,y,"Victim",0,0);
			PrintGlow(x,y,BashLevelName(me->spclValue),0,0);

			PrintGlow(x,y+25*1,"Time",0,0);
			sprintf(s,"%02d:%02d:%02d",
				me->time/(30*60*60),(me->time/(30*60))%60,(me->time/30)%60);
			RightPrintGlow(x+250,y+25*1,s,0,0);

			PrintGlow(x,y+25*2,"Power Level",0,0);
			sprintf(s,"%d",me->gemsGotten);
			RightPrintGlow(x+250,y+25*2,BashPowerName((byte)me->gemsGotten),0,0);

			PrintGlow(x,y+25*3,"Hits Taken",0,0);
			sprintf(s,"%d",me->hitsTaken);
			RightPrintGlow(x+250,y+25*3,s,0,0);
			break;
	}
}

void RenderEndGame(MGLDraw *mgl)
{
	int i,x,y;

	DBG("RenderEndGame");
	mgl->ClearScreen();
	RenderFireworks(mgl);

	CenterPrint(320,2,"Final Score",0,2);

	x=75;
	y=70;

	i=0;
	while(i<numStats)
	{
		if(stat[i].visible)
			ShowStat(i,x,y);
		y+=30;
		i++;
	}

	y+=10;
	RenderFinalScore(x,y,mgl);
}

byte UpdateEndGame(int *lastTime,MGLDraw *mgl)
{
	int i;
	char c;
	byte t;
	static byte fwClock;

	DBG("UpdateEndGame");
	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		c=mgl->LastKeyPressed();
		t=GetTaps();

		if(curStat<numStats)
		{
			MakeNormalSound(SND_MENUCLICK);
			// make this stat visible
			stat[curStat].visible=1;

			// start cranking up the points
			if(stat[curStat].curPts<stat[curStat].points)
			{
				if(stat[curStat].points-stat[curStat].curPts>100000)
				{
					stat[curStat].curPts+=10000;
					totalScore+=10000;
				}
				else if(stat[curStat].points-stat[curStat].curPts>1000)
				{
					stat[curStat].curPts+=1000;
					totalScore+=1000;
				}
				else if(stat[curStat].points-stat[curStat].curPts>100)
				{
					stat[curStat].curPts+=100;
					totalScore+=100;
				}
				else if(stat[curStat].points-stat[curStat].curPts>10)
				{
					stat[curStat].curPts+=10;
					totalScore+=10;
				}
				else
				{
					stat[curStat].curPts++;
					totalScore++;
				}
			}
			else if(stat[curStat].curPts>stat[curStat].points)
			{
				if(stat[curStat].points-stat[curStat].curPts<-100000)
				{
					stat[curStat].curPts-=10000;
					totalScore-=10000;
				}
				else if(stat[curStat].points-stat[curStat].curPts<-1000)
				{
					stat[curStat].curPts-=1000;
					totalScore-=1000;
				}
				else if(stat[curStat].points-stat[curStat].curPts<-100)
				{
					stat[curStat].curPts-=100;
					totalScore-=100;
				}
				else if(stat[curStat].points-stat[curStat].curPts<-10)
				{
					stat[curStat].curPts-=10;
					totalScore-=10;
				}
				else
				{
					stat[curStat].curPts--;
					totalScore--;
				}
			}
			else
				curStat++;

			if(c || t)
			{
				curStat=numStats;
				totalScore=0;
				for(i=0;i<numStats;i++)
				{
					stat[i].visible=1;
					stat[i].curPts=stat[i].points;
					totalScore+=stat[i].curPts;
				}
			}
		}
		else if(c || t)
		{
			return 0;
		}
		player.rank=GetRank(totalScore);

		if(fwClock>0)
		{
			fwClock--;
			if(fwClock==0)
			{
				if(Random(100)<player.rank)
					LaunchShell(player.rank);
			}
		}
		else
			fwClock=10;

		UpdateFireworks();

		*lastTime-=TIME_PER_FRAME;
	}
	return 1;
}

void EndGameTally(MGLDraw *mgl)
{
	byte b;
	int lastTime=1;
	dword now,then,hangon;
	highScore_t hs;

	DBG("EndGameTally");
	EndClock();
	hangon=TimeLength();

	now=timeGetTime();
	InitEndGame();
	DBG("SurvivedInitEndGame");
	b=1;
	while(b==1)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdateEndGame(&lastTime,mgl);
		RenderEndGame(mgl);
		mgl->Flip();
		if(!mgl->Process())
		{
			return;
		}
		EndClock();
	}
	DBG("ExitEndGameTally1");
	ExitFireworks();
	DBG("ExitEndGameTally2");
	then=timeGetTime();
	ResetClock(hangon);
	//AddGarbageTime(then-now);
	MakeHighScore(&hs,totalScore);
	CheckForHighScore(hs);
	if(totalScore>=10000000)
		EarnBadge(BADGE_SCORE);
	BadgeCheck(BE_ENDGAME,0,curMap);
	DBG("ExitEndGameTally3");
}
