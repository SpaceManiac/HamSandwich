#include "water.h"
#include "display.h"
#include "player.h"
#include "options.h"

#define WATER_WIDTH	(512)
#define WATER_HEIGHT (512)
#define WATERFIX (32)
#define WATERDAMPEN (31)

#define WATERBMP_WIDTH (256)
#define WATERBMP_HEIGHT (256)

short *water1,*water2;
static byte* waterbkgd;
int scroll;
int blorpX, blorpY, blorpTime;

void InitWater(void)
{
	int i;
	byte *src;
	int width;

	scroll=0;
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
		waterbkgd[i] = 32*3+5;// src[(i % WATER_WIDTH) + (i / WATER_WIDTH) * width];
	}
}

void ExitWater(void)
{
	free(water1);
	free(water2);
}

void SetupWater(void)
{
	byte *src;
	int width,i,j;

	if((player.worldNum==1 && player.levelNum==15) ||
		(player.worldNum==2 && player.levelNum==20))
		GetDisplayMGL()->LoadBMP("graphics/rapid.bmp");
	else if(player.worldNum == 3 && player.levelNum == 12)
		GetDisplayMGL()->LoadBMP("graphics/water3.bmp");
	else
		GetDisplayMGL()->LoadBMP("graphics/water.bmp");

	width=GetDisplayMGL()->GetWidth();
	src=GetDisplayMGL()->GetScreen();
	for(j=0;j<WATER_HEIGHT;j++)
		for(i=0;i<WATER_WIDTH;i++)
	{
		water1[i+j*WATER_WIDTH]=0;
		water2[i+j*WATER_WIDTH]=0;
		waterbkgd[i+j*WATER_WIDTH]=src[(i%WATERBMP_WIDTH)+(j%WATERBMP_HEIGHT)*width];
	}
	blorpTime = 0;
}

inline short GetWaterBit(int x,int y)
{
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

byte flip = 0;

void UpdateWater(void)
{
	int i, j;
	short* tmp;

	if ((player.worldNum == 1 && player.levelNum == 15) ||
		(player.worldNum == 2 && player.levelNum == 20) ||
		(player.worldNum == 3 && player.levelNum == 12))
	{
		scroll--;
		blorpY--;
	}
	if (scroll < 0)
		scroll += WATER_HEIGHT;

	if (!opt.waterFX)
		return;

	if ((player.worldNum == 1 && player.levelNum == 15) ||
		(player.worldNum == 2 && player.levelNum == 20) ||
		(player.worldNum == 3 && player.levelNum == 12))
	{
		for (i = 0; i < 40; i++)
			WaterBlop(MGL_random(WATER_WIDTH), MGL_random(WATER_HEIGHT), (byte)MGL_random(WATER_WIDTH / 8));
	}
	else
		WaterBlop(MGL_random(WATER_WIDTH), MGL_random(WATER_HEIGHT), (byte)MGL_random(WATER_WIDTH / 2));

	int neighborDiv = 2;
	int dampen = WATERDAMPEN;
	for (i = 0; i < WATER_WIDTH; i++)
		for (j = 0; j < WATER_HEIGHT; j++)
		{
			water2[i + j * WATER_WIDTH] = (GetWaterBit(i - 1, j) + GetWaterBit(i + 1, j) + GetWaterBit(i, j - 1) + GetWaterBit(i, j + 1)) / neighborDiv - water2[i + j * WATER_WIDTH];
			water2[i + j * WATER_WIDTH] = water2[i + j * WATER_WIDTH] * dampen / WATERFIX;
		}
	tmp = water1;
	water1 = water2;
	water2 = tmp;	// swap the pointers	
}

byte WaterPixel(int x,int y)
{
	int camx,camy;
	short s,xofs,yofs;

	GetCamera(&camx,&camy);
	x+=camx;
	y+=camy;

	x/=2;
	y/=2;

	y+=scroll;
	if (y < 0)
		y += WATER_HEIGHT;

	x=x%WATER_WIDTH;
	y=y%WATER_HEIGHT;

	xofs=GetWaterBit(x-1,y)-GetWaterBit(x+1,y);
	yofs=GetWaterBit(x,y-1)-GetWaterBit(x,y+1);

	s=GetWaterBkgdBit(x+xofs/WATERFIX,y+yofs/WATERFIX)&31;
	
	if (player.worldNum < 3)
	{
		s += xofs / WATERFIX;	// shade it

		if (s < 0)
			s = 0;
		if (s > 31)
			s = 31;
	}
	else
	{
		s += xofs / (WATERFIX*2);	// shade it
		s += 4;
		if (s < 0) s = 0;
		if (s > 31) s = 31;
	}

	if (player.levelNum == 20 && player.worldNum == 2)
	{
		if (s - 6 < 0)
			return 0;
		else
			return (byte)s + 32 * 1 - 6;
	}
	else if (player.levelNum == 22 && player.worldNum == 2)
	{
		return (byte)s + 32 * 6;
	}
	else if (player.worldNum == 3)
	{
		if (s + 3 > 31)
			return 31;
		else
			return (byte)s + 32 * 4;
	}
	else if (player.worldNum == 0)
	{
		if (player.levelNum == 20 || (player.levelNum==1 && (GetGoodguy() && GetGoodguy()->x < 38 * FIXAMT * TILE_WIDTH && GetGoodguy()->y>29 * FIXAMT * TILE_HEIGHT)))
		{
			if (s < 4)
				return (byte)0 + 32*5;
			else
				return (byte)s - 4 + 32*5;
		}
		else
			return (byte)s + 32 * 3;
	}
	else
		return (byte)s+32*3;
}

void WaterRipple(int x,int y,short amt)
{
	int camx,camy;

	if(!opt.waterFX)
		return;

	GetCamera(&camx,&camy);
	if((x-camx+HALFWID)<0 || (x-camx+HALFWID)>SCRWID ||
	   (y-camy+HALFHEI)<0 || (y-camy+HALFHEI)>SCRHEI*2)
		return;

	x+=HALFWID;
	y+=HALFHEI;

	x/=2;
	y/=2;

	y+=scroll;
	if (y < 0)
		y += WATER_HEIGHT;

//	x=x-camx;
//	y=y-camy;

//	if(x<0 || y<0 || x>639 || y>479)
//		return;	// no rippling from offscreen

	//x+=camx;
	//y+=camy;

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

void WaterBlop(int x,int y,int width)
{
	int i,x2;
	short s;

	if(!opt.waterFX)
		return;

	s=1;
	for(i=x;i<=x+width;i++)
	{
		x2=i;
		if(x2>WATER_WIDTH)
			x2-=WATER_WIDTH;
		water2[(x2%WATER_WIDTH)+(y%WATER_HEIGHT)*WATER_WIDTH]+=s;
		if((i-x)<width/2)
			s+=WATERFIX*3/4;
		else if(s>1)
			s-=WATERFIX*3/4;
	}
}
