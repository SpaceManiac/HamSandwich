#include "stars.h"
#include "profile.h"
#include "highscore.h"

typedef struct star_t
{
	int x,y;
	int dx,dy;
	int clock;
} star_t;

static star_t star[NUM_STARS];
static byte starBulge;
static byte level;
static int starSrcX,starSrcY;
static word starCnt;
static byte colTab[]={0,2,0,5};

void InitStars(byte lvl,word minStar)
{
	int i;

	if(lvl>0)
		level=lvl-1;
	else
		level=0;

	starCnt=minStar;
	starBulge=0;
	for(i=0;i<NUM_STARS;i++)
		star[i].clock=-1;
}

void UpdateStars(void)
{
	int i;

	if(starBulge)
		starBulge--;

	for(i=0;i<NUM_STARS;i++)
	{
		if(star[i].clock>-1)
		{
			if(star[i].clock)
			{
				star[i].clock--;
				Dampen(&star[i].dx,FIXAMT/8);
				Dampen(&star[i].dy,FIXAMT/8);
			}
			else
			{
				if(star[i].x<STARDESTX)
					star[i].dx+=FIXAMT/2;
				if(star[i].x>STARDESTX)
					star[i].dx-=FIXAMT/2;
				if(star[i].y<STARDESTY)
					star[i].dy+=FIXAMT/2;
				if(star[i].y>STARDESTY)
					star[i].dy-=FIXAMT/2;
				Clamp(&star[i].dx,FIXAMT*8);
				Clamp(&star[i].dy,FIXAMT*8);
			}
			star[i].x+=star[i].dx;
			star[i].y+=star[i].dy;
			if(star[i].x>STARDESTX-10*FIXAMT && star[i].y>STARDESTY-10*FIXAMT &&
			   star[i].x<STARDESTX+60*FIXAMT && star[i].y<STARDESTY+60*FIXAMT)
			{
				star[i].clock=-1;
				starCnt++;
				starBulge=4;
				MakeNormalSound(SND_STARSTOP);
			}
		}
	}
}

void LaunchStar(int x,int y)
{
	int i;
	byte ang;

	for(i=0;i<NUM_STARS;i++)
	{
		if(star[i].clock==-1)
		{
			star[i].x=x;
			star[i].y=y;
			ang=(byte)Random(256);
			star[i].clock=15;
			star[i].dx=Cosine(ang)*5;
			star[i].dy=Sine(ang)*5;
			return;
		}
		MakeNormalSound(SND_STARGO);
	}
}

void LaunchStarAngle(int x,int y,byte ang)
{
	int i;

	for(i=0;i<NUM_STARS;i++)
	{
		if(star[i].clock==-1)
		{
			star[i].x=x+Cosine(ang)*100;
			star[i].y=y+Sine(ang)*80;
			star[i].clock=15;
			star[i].dx=Cosine(ang)*10;
			star[i].dy=Sine(ang)*10;
			return;
		}
		MakeNormalSound(SND_STARGO);
	}
}

byte StarsLeft(void)
{
	int i;

	for(i=0;i<NUM_STARS;i++)
		if(star[i].clock>-1)
			return 1;
	return 0;
}

void RenderStartStar(int x,int y,MGLDraw *mgl)
{
	if(level>0)
		RenderStar(colTab[level],3,x,y,mgl);
}

void RenderEndStar(MGLDraw *mgl)
{
	char s[10];

	RenderStar(0,5+starBulge,STARDESTX,STARDESTY,mgl);
	sprintf(s,"%03d",starCnt);
	CenterPrint(STARDESTX/FIXAMT-1,STARDESTY/FIXAMT+21,s,-32,0);
	CenterPrint(STARDESTX/FIXAMT+1,STARDESTY/FIXAMT+19,s,-32,0);
	CenterPrint(STARDESTX/FIXAMT,STARDESTY/FIXAMT+20,s,0,0);
}

void RenderEndStar2(MGLDraw *mgl)
{
	char s[10];

	RenderStar(0,5+starBulge,STARDESTX,STARDESTY,mgl);
	sprintf(s,"%03d",starCnt);
	CenterPrintUnGlow(STARDESTX/FIXAMT,STARDESTY/FIXAMT+20,s,0);
}

void RenderStars(MGLDraw *mgl)
{
	int i;

	for(i=0;i<NUM_STARS;i++)
		if(star[i].clock>-1)
		{
			RenderStar(colTab[level],4,star[i].x,star[i].y,mgl);
		}
}