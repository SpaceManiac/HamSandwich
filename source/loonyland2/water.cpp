#include "water.h"
#include "display.h"
#include "player.h"
#include "options.h"

#define WATER_WIDTH	(256)
#define WATER_HEIGHT (256)
#define WATERFIX (32)
#define WATERDAMPEN (31)

short *water1,*water2;
static byte *waterbkgd,scrollX,scrollY;
static byte waterColor;
static char waterAdj,scrollAmtX,scrollAmtY;

void InitWater(void)
{
	int i;
	byte *src;
	int width;

	if(config.shading==0)
		return;

	water1=(short *)malloc(WATER_WIDTH*WATER_HEIGHT*sizeof(short));
	if(!water1)
		FatalError("out of memory!");
	water2=(short *)malloc(WATER_WIDTH*WATER_HEIGHT*sizeof(short));
	if(!water2)
		FatalError("out of memory!");
	waterbkgd=(byte *)malloc(WATER_WIDTH*WATER_HEIGHT*sizeof(byte));
	if(!waterbkgd)
		FatalError("out of memory!");

	GetDisplayMGL()->LoadBMP("graphics/water.bmp");

	width=GetDisplayMGL()->GetWidth();
	src=GetDisplayMGL()->GetScreen();
	for(i=0;i<WATER_WIDTH*WATER_HEIGHT;i++)
	{
		water1[i]=0;
		water2[i]=0;
		waterbkgd[i]=src[(i%WATER_WIDTH)+(i/WATER_WIDTH)*width];
	}
}

void ExitWater(void)
{
	if(config.shading==0)
		return;

	free(water1);
	free(water2);
	free(waterbkgd);
}

void SetupWater(void)
{
	byte *src;
	int width,i;

	if(config.shading==0)
		return;

	// different pictures and colors for different levels
	switch(player.levelNum)
	{
		case 0:
		default:
			GetDisplayMGL()->LoadBMP("graphics/water.bmp");
			waterColor=32*3;	// blue
			waterAdj=5;	// 5 levels brightened
			scrollAmtX=-1;
			scrollAmtY=0;
			break;
	}

	width=GetDisplayMGL()->GetWidth();
	src=GetDisplayMGL()->GetScreen();
	for(i=0;i<WATER_WIDTH*WATER_HEIGHT;i++)
	{
		water1[i]=0;
		water2[i]=0;
		waterbkgd[i]=src[(i%WATER_WIDTH)+(i/WATER_WIDTH)*width];
	}
}

inline short GetWaterBit(int x,int y)
{
	if(config.shading==0)
		return 0;

	while(x<0)
		x+=WATER_WIDTH;
	while(x>=WATER_WIDTH)
		x-=WATER_WIDTH;
	while(y<0)
		y+=WATER_HEIGHT;
	while(y>=WATER_HEIGHT)
		y-=WATER_HEIGHT;

	return water1[x+y*WATER_WIDTH];
}

inline byte GetWaterBkgdBit(int x,int y)
{
	if(config.shading==0)
		return 0;

	while(x<0)
		x+=WATER_WIDTH;
	while(x>=WATER_WIDTH)
		x-=WATER_WIDTH;
	while(y<0)
		y+=WATER_HEIGHT;
	while(y>=WATER_HEIGHT)
		y-=WATER_HEIGHT;

	return waterbkgd[x+y*WATER_WIDTH];
}

void UpdateWater(void)
{
	int i,j;
	short *tmp;

	if(config.shading==0)
		return;

	scrollX+=scrollAmtX;
	scrollY+=scrollAmtY;

	for(i=0;i<40;i++)
		WaterBlop((byte)Random(256),(byte)Random(256),(byte)Random(32));

	for(i=0;i<WATER_WIDTH;i++)
		for(j=0;j<WATER_HEIGHT;j++)
		{
			water2[i+j*WATER_WIDTH]=(GetWaterBit(i-1,j)+GetWaterBit(i+1,j)+GetWaterBit(i,j-1)+GetWaterBit(i,j+1))/2-water2[i+j*WATER_WIDTH];
			water2[i+j*WATER_WIDTH]=water2[i+j*WATER_WIDTH]*WATERDAMPEN/WATERFIX;
		}

	tmp=water1;
	water1=water2;
	water2=tmp;	// swap the pointers
}

byte WaterPixel(int x,int y)
{
	int camx,camy;
	short s,xofs,yofs;

	if(config.shading==0)
		return waterColor;

	GetCamera(&camx,&camy);
	x+=camx;
	y+=camy;

	x/=2;
	y/=2;

	x+=scrollX;
	y+=scrollY;

	x=x%WATER_WIDTH;
	y=y%WATER_HEIGHT;

	xofs=GetWaterBit(x-1,y)-GetWaterBit(x+1,y);
	yofs=GetWaterBit(x,y-1)-GetWaterBit(x,y+1);

	s=GetWaterBkgdBit(x+xofs/WATERFIX-scrollX,y+yofs/WATERFIX-scrollY)&31;
	s+=xofs/WATERFIX+waterAdj;	// shade it

    if(s<0)
		s=0;
	if(s>31)
		s=31;

	return (byte)(s+waterColor);
}

void WaterRipple(int x,int y,short amt)
{
	int camx,camy;

	if(config.shading==0)
		return;

	GetCamera(&camx,&camy);
	if((x-camx+320)<0 || (x-camx+320)>640 ||
	   (y-camy+240)<0 || (y-camy+240)>480)
		return;

	x+=320;
	y+=240;

	x/=2;
	y/=2;

	y+=scrollY;
	x+=scrollX;

	x=x%WATER_WIDTH;
	y=y%WATER_HEIGHT;

	water2[x+y*WATER_WIDTH]+=amt;
	x-=1;
	if(x<0)
		x+=WATER_WIDTH;
	water2[x+y*WATER_WIDTH]+=amt/2;
	x+=2;
	if(x>=WATER_WIDTH)
		x-=WATER_WIDTH;
	water2[x+y*WATER_WIDTH]+=amt/2;
	x--;
	if(x<0)
		x+=WATER_WIDTH;
	y++;
	if(y>=WATER_HEIGHT)
		y-=WATER_HEIGHT;
	water2[x+y*WATER_WIDTH]+=amt/2;
	y-=2;
	if(y<0)
		y+=WATER_HEIGHT;
	water2[x+y*WATER_WIDTH]+=amt/2;
}

void WaterBlop(byte x,byte y,byte width)
{
	int i,x2;
	short s;

	if(config.shading==0)
		return;

	s=1;
	for(i=x;i<=x+width;i++)
	{
		x2=i;
		if(x2>WATER_WIDTH)
			x2-=WATER_WIDTH;
		water2[((byte)(x2))+((byte)(y))*WATER_WIDTH]+=s;
		if((i-x)<width/2)
			s+=WATERFIX*3/4;
		else if(s>1)
			s-=WATERFIX*3/4;
	}
}
