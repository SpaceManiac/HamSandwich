#include "winpch.h"
#include "netgame.h"
#include "display.h"
#include "sound.h"
#include "goal.h"
#include "map.h"
#include "progress.h"
#include "game.h"

#define MAX_POINTS	(1024)
#define MAX_NETGUYS	(64)

typedef struct netpt_t
{
	word x,y;
} netpt_t;

typedef struct netguy_t
{
	int x,y;
	int dx,dy;
} netguy_t;

static word numPts;
static netpt_t *points;
static byte drawing,drawBright,numGuys;
static netguy_t netGuy[MAX_NETGUYS];
static int record,caught;
static char caughtBright;
static int score,lastScore,ticker;

void Init_Netgame(void)
{
	int i;

	curMap=NULL;
	points=(netpt_t *)malloc(sizeof(netpt_t)*MAX_POINTS);
	numPts=0;
	drawing=0;
	drawBright=0;
	record=0;
	caught=0;
	score=0;
	ticker=0;
	drawing=2;
	caughtBright=-32;

	for(i=0;i<MAX_NETGUYS;i++)
		netGuy[i].x=-1;
}

void Exit_Netgame(void)
{
	free(points);
}

void AddNetGuys(void)
{
	int i;

	if(Random(20)==0)
	{
		for(i=0;i<MAX_NETGUYS;i++)
		{
			if(netGuy[i].x==-1)
			{
				netGuy[i].x=Random(640)*FIXAMT;
				netGuy[i].y=Random(480)*FIXAMT;
				netGuy[i].dx=0;
				netGuy[i].dy=0;
				return;
			}
		}
	}
}

byte PointExists(word x,word y)
{
	int i;

	for(i=0;i<numPts;i++)
		if(points[i].x==x && points[i].y==y)
			return 1;

	return 0;
}

byte AddPoint(word x,word y)
{
	if(numPts==MAX_POINTS || PointExists(x,y))
		return 0;

	points[numPts].x=x;
	points[numPts].y=y;
	numPts++;
	return 1;
}

byte LineTo(word x,word y)
{
	word i,j,startx,starty;
	int dx,dy;

	if(x<20)
		x=20;
	if(y<20)
		y=20;
	if(x>620)
		x=620;
	if(y>460)
		y=460;

	startx=points[numPts-1].x;
	starty=points[numPts-1].y;

	i=startx;
	j=starty;

	while(i!=x ||  j!=y)
	{
		if(i<x)
			dx=1;
		else if(i>x)
			dx=-1;
		else
			dx=0;

		if(j<y)
			dy=1;
		else if(j>y)
			dy=-1;
		else
			dy=0;

		if(dx!=0 && dy!=0)
		{
			if(abs(x-i)>abs(j-y))
				dy=0;
			else
				dx=0;
		}
		i+=dx;
		j+=dy;

		if(!AddPoint(i,j))
			return 0;
	}
	return 1;
}

byte PointIsOutThere(word myX,word myY,char dir)
{
	int i;

	for(i=0;i<numPts;i++)
	{
		if(points[i].y==myY)
		{
			if((points[i].x>myX && dir==1) || (points[i].x<myX && dir==-1))
				return 1;
		}
	}
	return 0;
}

byte CheckForEnclosure(word vicX,word vicY)
{
	return (PointIsOutThere(vicX,vicY,-1)*PointIsOutThere(vicX,vicY,1));
}

byte Update_Netgame(byte btn,word mouse_x,word mouse_y)
{
	int i,j;

	if(ticker>0)
		ticker++;
	if(ticker==30*30)
	{
		MakeNormalSound(SND_ALLCANDLE);
		ticker=0;
		numPts=0;
		drawing=2;
		drawBright=31;
		for(i=0;i<MAX_NETGUYS;i++)
		{
			points[numPts].x=netGuy[i].x/FIXAMT;
			points[numPts].y=netGuy[i].y/FIXAMT;
			numPts++;
			netGuy[i].x=-1;
		}

		if(score>record)
		{
			record=score;
		}
		score=0;
	}

	AddNetGuys();

	if(caughtBright>-32)
		caughtBright--;

	for(i=0;i<MAX_NETGUYS;i++)
	{
		if(netGuy[i].x!=-1)
		{
			netGuy[i].dx+=-FIXAMT/8+Random(FIXAMT/4+1);
			netGuy[i].dy+=-FIXAMT/8+Random(FIXAMT/4+1);
			netGuy[i].x+=netGuy[i].dx;
			netGuy[i].y+=netGuy[i].dy;

			if(drawing==1)
			{
				for(j=0;j<numPts;j++)
				{
					if(netGuy[i].x/FIXAMT==points[j].x && netGuy[i].y/FIXAMT==points[j].y)
					{
						drawing=2;
						drawBright=31;
						MakeNormalSound(SND_TURNEVIL);
					}
				}
			}
			if(netGuy[i].x<20*FIXAMT || netGuy[i].x>620*FIXAMT || netGuy[i].y<20*FIXAMT || netGuy[i].y>460*FIXAMT)
				netGuy[i].x=-1;
		}
	}

	if(drawing==0)
	{
		if(drawBright>8)
			drawBright--;
		else
			numPts=0;

		if(btn==1)	// start drawing
		{
			numPts=1;
			points[0].x=mouse_x;
			points[0].y=mouse_y;
			drawing=1;
			drawBright=16;
		}
	}
	else if(drawing==1)
	{
		if(btn==0)
		{
			LineTo(points[0].x,points[0].y);
			drawing=0;
			drawBright=31;
			caught=0;
			for(i=0;i<MAX_NETGUYS;i++)
			{
				if(netGuy[i].x!=-1 && CheckForEnclosure((word)(netGuy[i].x/FIXAMT),(word)(netGuy[i].y/FIXAMT)))
				{
					caught++;
					netGuy[i].x=-1;
				}
			}
			caughtBright=32;
			lastScore=caught*caught;
			score+=lastScore;
			if(caught==0)
				MakeNormalSound(SND_TURNEVIL);
			else
			{
				if(ticker==0)
				{
					ticker=1;
					MakeNormalSound(SND_SPEEDUP);
				}
				MakeNormalSound(SND_BUYITEM);
			}
		}
		else
		{
			if(mouse_x!=points[numPts-1].x || mouse_y!=points[numPts-1].y)
			{
				if(!LineTo(mouse_x,mouse_y))
				{
					drawing=2;
					drawBright=31;
					MakeNormalSound(SND_TURNEVIL);
				}
			}
		}
	}
	else if(drawing==2)	// must release the button to draw again
	{
		if(drawBright>8)
			drawBright--;
		else
			numPts=0;

		if(btn==0)
			drawing=0;
	}

	return 1;
}

static inline void PlotPixel(int x,int y,byte c,byte *scrn)
{
	if(x>0 && x<640 && y>0 && y<480)
		scrn[x+y*640]=c;
}

void Render_Netgame(void)
{
	int i;
	byte *scrn,c;
	char s[32];

	scrn=GetDisplayMGL()->GetScreen();

	for(i=0;i<MAX_NETGUYS;i++)
	{
		if(netGuy[i].x!=-1)
		{
			c=32+(abs(netGuy[i].dx)+abs(netGuy[i].dy))/(FIXAMT/32);
			if(c>63)
				c=63;
			PlotPixel(netGuy[i].x/FIXAMT,netGuy[i].y/FIXAMT,c,scrn);
		}
	}
	for(i=0;i<numPts;i++)
	{
		PlotPixel(points[i].x,points[i].y,32+drawBright,scrn);
	}

	if(caughtBright>-32)
	{
		sprintf(s,"Caught: %d",caught);
		PrintGlow(530,410,s,caughtBright,2);
		sprintf(s,"Points: %d",lastScore);
		PrintGlow(530,430,s,caughtBright,2);
	}
	if(score>0)
	{
		sprintf(s,"Score: %d",score);
		PrintGlow(530,450,s,0,2);
	}
	if(record>0)
	{
		sprintf(s,"Record: %d",record);
		PrintGlow(530,20,s,0,2);
	}
	if(ticker>0)
	{
		sprintf(s,"0:%02d",(30*30-ticker)/30);
		PrintGlow(320,450,s,0,2);
	}
}
