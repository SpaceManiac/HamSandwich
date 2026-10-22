#include "score.h"
#include "player.h"

#define CUR_SCORE_X	460
#define CUR_SCORE_Y	2
#define DST_SCORE_X	600
#define DST_SCORE_Y	2
#define COMBO_TIME	10

#define COMBO_X		200
#define COMBO_DX	30

typedef struct pointDisp_t
{
	int pts;
	int x,y;
	int dx,dy;
} pointDisp_t;

byte comboTimer;
word curPoints[8];
byte mult;

pointDisp_t pointDisp[8];

void Score_Init(void)
{
	int i;

	comboTimer=0;
	mult=0;

	for(i=0;i<8;i++)
	{
		curPoints[i]=0;

		pointDisp[i].pts=0;
	}
	player.score=0;
}

void Score_ApplyPoints(void)
{
	int i,j;

	for(i=0;i<8;i++)
		if(pointDisp[i].pts==0)
		{
			for(j=0;j<8;j++)
				pointDisp[i].pts+=curPoints[j];
			pointDisp[i].pts*=mult;
			pointDisp[i].x=CUR_SCORE_X*FIXAMT;
			pointDisp[i].y=CUR_SCORE_Y*FIXAMT;
			pointDisp[i].dx=0;
			pointDisp[i].dy=FIXAMT*4;
			return;
		}

	// if you got here, they are all occupied
	// so dump the lowest one directly the score
	// and slide the others down, so the new one will be at the end
	player.score+=pointDisp[0].pts;
	for(i=0;i<7;i++)
		pointDisp[i]=pointDisp[i+1];

	pointDisp[7].pts=0;
	for(j=0;j<8;j++)
		pointDisp[7].pts+=curPoints[j];
	pointDisp[7].pts*=mult;
	pointDisp[7].x=CUR_SCORE_X*FIXAMT;
	pointDisp[7].y=CUR_SCORE_Y*FIXAMT;
	pointDisp[7].dx=0;
	pointDisp[7].dy=FIXAMT*4;
}

byte Score_UpdatePointDisp(pointDisp_t *me)
{
	if(me->pts==0)
		return 1;

	me->x+=me->dx;
	me->y+=me->dy;

	me->dx+=FIXAMT/2;
	me->dy-=FIXAMT/4;

	if(me->x>(DST_SCORE_X*FIXAMT))
	{
		player.score+=me->pts;
		me->pts=0;
	}
	return 0;
}

byte Score_Update(void)
{
	int i;
	byte done;

	if(comboTimer)
	{
		comboTimer--;
		if(!comboTimer)
		{
			Score_ApplyPoints();
			mult=0;
		}
	}

	done=0;
	for(i=0;i<8;i++)
		done+=Score_UpdatePointDisp(&pointDisp[i]);
	return (done<8);
}

void Score_BlowUp(byte pts,byte color)
{
	int i;

	if(GameType()==GAME_CLEARANCE)
	{
		if(!comboTimer)
		{
			for(i=0;i<8;i++)
				curPoints[i]=0;

			comboTimer=1;
			mult=1;
			curPoints[color]=1;
		}
		else
		{
			comboTimer=1;
			mult++;
			curPoints[color]++;
		}
		return;
	}
	if(!comboTimer)
	{
		mult=1;
		comboTimer=COMBO_TIME;
		for(i=0;i<8;i++)
			curPoints[i]=0;
		if(curPoints[color]<65535-pts)
			curPoints[color]+=pts;
		else
			curPoints[color]=65535;
	}
	else
	{
		comboTimer=COMBO_TIME;
		if(curPoints[color]==0)
			mult++;
		if(curPoints[color]<65535-pts)
			curPoints[color]+=pts;
		else
			curPoints[color]=65535;
	}
}

void Score_Render(void)
{
	int i,n;
	int x;
	char s[32];
	byte started;

	if(comboTimer)
	{
		started=0;
		x=CUR_SCORE_X-COMBO_DX*2;
		for(i=7;i>=0;i--)
			if(curPoints[i])
			{
				if(!started)
					started=1;
				else
				{
					CenterPrint(x-1,2-1,"+",-32,0);
					CenterPrint(x+1,2+1,"+",-32,0);
					CenterPrint(x,2,"+",0,0);
					x-=COMBO_DX;
				}
				sprintf(s,"%d",curPoints[i]);
				CenterPrint(x+1,2+1,s,-32,0);
				CenterPrint(x-1,2-1,s,-32,0);
				CenterPrintColor(x,2,s,i,-8,0);
				x-=COMBO_DX;
			}
		x=CUR_SCORE_X-COMBO_DX;
		CenterPrint(x-1,2-1,"=",-32,0);
		CenterPrint(x+1,2+1,"=",-32,0);
		CenterPrint(x,2,"=",0,0);
		x+=COMBO_DX;
		n=0;
		for(i=0;i<8;i++)
			n+=curPoints[i];
		sprintf(s,"%d x %d",n,mult);
		Print(x-1,2-1,s,-32,0);
		Print(x+1,2+1,s,-32,0);
		Print(x,2,s,0,0);
	}

	for(i=0;i<8;i++)
	{
		if(pointDisp[i].pts)
		{
			sprintf(s,"%d",pointDisp[i].pts);
			PrintGlow(pointDisp[i].x/FIXAMT,pointDisp[i].y/FIXAMT,s,0,0);
		}
	}
}

byte Score_ComboGoing(void)
{
	return (comboTimer>0);
}
