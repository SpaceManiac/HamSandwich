#include "winpch.h"
#include "racer.h"
#include "display.h"
#include "progress.h"
#include "game.h"
#include "goal.h"

#define RACE_MAPWIDTH	(32)
#define RACE_MAPHEIGHT	(32)

#define RACE_TILEWIDTH	(20)
#define RACE_TILEHEIGHT	(20)

#define RACER_Y	(460*FIXAMT)

typedef struct racer_t
{
	int x,y;
	int dx,dy;
	byte life;
	int speed;
	byte cash;

	byte level;
	dword distDone;
} racer_t;

static int roadX[RACE_MAPHEIGHT],roadX2[RACE_MAPHEIGHT];
static int roadY;

static byte oldc;
static int score,curScore,coinsEarned,minSpeed,minWidth,scoreTick,maxSize;
static byte gameOver,gameOverClock;
static racer_t racer;
static int titleY;

void NewRaceLevel(void)
{
	int i;

	racer.distDone=0;
	racer.level++;
	racer.x=320*FIXAMT;
	racer.y=500*FIXAMT;
	racer.dx=0;
	racer.dy=0;
	minWidth=200*FIXAMT;
	minSpeed=FIXAMT;
	racer.speed=minSpeed;
	maxSize=FIXAMT;
	scoreTick=0;
	for(i=0;i<RACE_MAPHEIGHT;i++)
	{
		roadX[i]=320-minWidth/FIXAMT;
		roadX2[i]=320+minWidth/FIXAMT;
	}
	roadY=0;
	titleY=0;
}

void InitRaceGame(MGLDraw *mgl)
{
	mgl->LastKeyPressed();
	gameOver=0;

	score=0;
	curScore=0;
	racer.cash=0;
	racer.life=1;
	racer.level=255;

	NewRaceLevel();
}

void ExitRaceGame(void)
{
}

byte RaceDie(void)
{
	racer.life--;
	if(racer.life==0)
	{
		gameOver=1;
		gameOverClock=30;
		GetDisplayMGL()->LastKeyPressed();
		GetTaps();

		if(score>=75000)
			CompleteGoal(8);

		coinsEarned=score/1000;
		profile.progress.totalCoins+=coinsEarned;
		MakeNormalSound(SND_BOMBBOOM);
		return 1;
	}
	racer.speed=minSpeed;
	racer.dx=0;
	racer.dy=0;
	return 1;
}

void RenderRoad(MGLDraw *mgl)
{
	int i,y;

	y=480+RACE_TILEHEIGHT-RACE_MAPHEIGHT*RACE_TILEHEIGHT+(roadY/FIXAMT);
	for(i=0;i<RACE_MAPHEIGHT-1;i++)
	{
		DrawLine(roadX[i],y,0,y+RACE_TILEHEIGHT,16);
		DrawLine(roadX[i],y,roadX[i+1],y+RACE_TILEHEIGHT,24);
		DrawLine(roadX2[i],y,639,y+RACE_TILEHEIGHT,16);
		DrawLine(roadX2[i],y,roadX2[i+1],y+RACE_TILEHEIGHT,24);
		y+=RACE_TILEHEIGHT;
	}
}

void RenderRacer(MGLDraw *mgl)
{
	int x,y;

	x=racer.x/FIXAMT;
	y=racer.y/FIXAMT;
	DrawLine(x-10,y,x,y-10,31);
	DrawLine(x,y-10,x+10,y,31);
	DrawLine(x+10,y,x-10,y,31);
}

void RenderRaceGame(MGLDraw *mgl)
{
	byte *scrn;
	int pitch,spd;
	char s[32];

	mgl->ClearScreen();
	pitch=mgl->GetWidth();
	scrn=mgl->GetScreen();

	RenderRoad(mgl);

	if(gameOver)
	{
		CenterPrint(320,100,"GAME OVER!",0,2);
		sprintf(s,"Survived for %lu kilometers!",racer.distDone/1000);
		CenterPrint(320,150,s,0,1);
		sprintf(s,"Total Score: %d",score);
		CenterPrint(320,180,s,0,1);

		sprintf(s,"Coins Earned: %d",coinsEarned);
		CenterPrint(320,240,s,0,1);
	}
	else
	{
		RenderRacer(mgl);

		// interface
		sprintf(s,"Score: %08d",curScore);
		PrintGlow(2,470,s,0,1);
		sprintf(s,"Distance: %08lu",racer.distDone);
		PrintGlow(500,470,s,0,1);

		DrawBox(256,470,256+130,479,31);
		spd=(racer.speed)*128/(FIXAMT*40);
		DrawFillBox(257,471,257+spd,478,20);
		spd=(minSpeed)*128/(FIXAMT*40);
		DrawFillBox(257+spd,471,257+spd,478,31);
	}
}

byte UpdateRacer(MGLDraw *mgl)
{
	byte c;

	racer.x+=racer.dx;
	if(racer.y>RACER_Y)
		racer.y-=8*FIXAMT;

	c=GetControls();

	if(c&CONTROL_LF)
	{
		if(racer.dx>0)
			racer.dx=0;
		racer.dx-=FIXAMT;
	}
	if(c&CONTROL_RT)
	{
		if(racer.dx<0)
			racer.dx=0;
		racer.dx+=FIXAMT;
	}

	if(c&CONTROL_UP)
	{
		racer.speed+=FIXAMT/4;
		if(racer.speed>FIXAMT*40)
			racer.speed=FIXAMT*40;
	}

	if(c&CONTROL_DN)
	{
		racer.speed-=FIXAMT;
	}

	if(racer.speed<minSpeed)
		racer.speed=minSpeed;

	if(racer.y>RACER_Y)
		racer.y-=8*FIXAMT;

	Clamp(&racer.dx,FIXAMT*5);

	Dampen(&racer.dx,FIXAMT/8);

	if(racer.x<roadX[RACE_MAPHEIGHT-(480-RACER_Y/FIXAMT)/RACE_TILEHEIGHT]*FIXAMT ||
		racer.x>roadX2[RACE_MAPHEIGHT-(480-RACER_Y/FIXAMT)/RACE_TILEHEIGHT]*FIXAMT)
		RaceDie();

	oldc=c;

	c=mgl->LastKeyPressed();
	if(c==27)
		return 1;

	return 0;
}

void UpdateRoad(void)
{
	int i,r,size;

	roadY+=racer.speed;
	racer.distDone+=racer.speed/FIXAMT;

	scoreTick+=racer.speed/FIXAMT;

	while(roadY>(RACE_TILEHEIGHT*FIXAMT))
	{
		roadY-=RACE_TILEHEIGHT*FIXAMT;

		for(i=RACE_MAPHEIGHT-1;i>0;i--)
		{
			roadX[i]=roadX[i-1];
			roadX2[i]=roadX2[i-1];
		}
		r=Random(256);
		if(r<64)
		{
			size=Random(maxSize)/FIXAMT;
			if(size>(roadX2[1]-roadX[1])/2)
				size=(roadX2[1]-roadX[1])/2;

			if(Random(2)==0)
				roadX[0]=roadX[1]-size;
			else
				roadX[0]=roadX[1]+size;

			if(roadX2[0]-roadX[0]<minWidth/FIXAMT)
			{
				roadX[0]=roadX2[0]-minWidth/FIXAMT;
			}

			if(roadX[0]<0)
			{
				roadX2[0]+=(-roadX[0]);
				roadX[0]=0;
			}
		}
		else
		{
			roadX[0]=roadX[1];
		}
		r=Random(256);
		if(r<64)
		{
			size=Random(maxSize)/FIXAMT;
			if(size>(roadX2[1]-roadX[1])/2)
				size=(roadX2[1]-roadX[1])/2;
			if(Random(2)==0)
				roadX2[0]=roadX2[1]-size;
			else
				roadX2[0]=roadX2[1]+size;

			if(roadX2[0]-roadX[0]<minWidth/FIXAMT)
			{
				roadX2[0]=roadX[0]+minWidth/FIXAMT;
			}

			if(roadX2[0]>639)
			{
				roadX[0]+=(640-roadX2[0]);
				roadX2[0]=639;
			}
		}
		else
		{
			roadX2[0]=roadX2[1];
		}
	}
}

byte UpdateRaceGame(int *lastTime,MGLDraw *mgl)
{
	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		if(curScore<score)
			curScore++;
		if(curScore<score-10)
			curScore+=5;
		if(curScore<score-100)
			curScore+=50;
		if(curScore<score-1000)
			curScore+=500;
		if(curScore<score-10000)
			curScore+=5000;
		if(curScore>score)
			curScore--;

		UpdateRoad();
		if(UpdateRacer(mgl))
		{
			return 1;
		}

		while(scoreTick>=20)
		{
			score+=racer.speed/FIXAMT;
			scoreTick-=20;
			if(minWidth>30*FIXAMT)
				minWidth-=5;
			if(minSpeed<10*FIXAMT)
				minSpeed+=1;
			if(maxSize<100*FIXAMT)
				maxSize+=15;
		}

		*lastTime-=TIME_PER_FRAME;
	}

	return 0;
}

static byte UpdateGameOver(int *lastTime,MGLDraw *mgl)
{
	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		*lastTime-=TIME_PER_FRAME;
		if(gameOverClock)
			gameOverClock--;
	}

	if(mgl->LastKeyPressed() && gameOverClock==0)
		return 1;

	if((GetTaps()&(CONTROL_B1|CONTROL_B2)) && gameOverClock==0)
		return 1;

	return 0;
}

void RaceGame(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	InitRaceGame(mgl);
	lastTime=1;
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		if(!gameOver)
			done=UpdateRaceGame(&lastTime,mgl);
		else
			done=UpdateGameOver(&lastTime,mgl);
		RenderRaceGame(mgl);
		mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	ExitRaceGame();
}

