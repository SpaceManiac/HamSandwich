#include "plasma.h"

static byte plasma[PLASMA_WIDTH*PLASMA_HEIGHT];
static byte color;

void InitPlasma(byte col)
{
	color=col;
	memset(plasma,color*32+2,PLASMA_WIDTH*PLASMA_HEIGHT);
}

void ExitPlasma(void)
{
}

void AddHotSpot(void)
{
	int x,i;

	for(i=0;i<10;i++)
	{
		x=Random(PLASMA_WIDTH);
		plasma[x]=31+color*32;
	}

//	if(Random(2)==1)
//		plasma[305+Random(8)]=31+color*32;
}

void ScrollPlasma(void)
{
	int i;

	// scroll it down
	for(i=PLASMA_HEIGHT-1;i>0;i--)
		memcpy(&plasma[i*PLASMA_WIDTH],&plasma[(i-1)*PLASMA_WIDTH],PLASMA_WIDTH);
}

void SmoothPlasma(void)
{
	int x,y;
	int total;
	int pos;

	pos=0;
	for(pos=0;pos<PLASMA_WIDTH*PLASMA_HEIGHT;pos++)
	{
		total=0;
		if(x>0)
			total+=(plasma[pos-1]&31)*2;
		if(x<PLASMA_WIDTH-1)
			total+=(plasma[pos+1]&31)*2;
		if(y>0)
			total+=(plasma[pos-PLASMA_WIDTH]&31)*2;
		total+=(plasma[pos]&31)*25;
		total/=32;
		if(total>31)
			total=31;
		if(total<2)
			total=2;
		plasma[pos]=(plasma[pos]&(~31))+total;

		x++;
		if(x==PLASMA_WIDTH)
		{
			x=0;
			y++;
		}
	}
}

void UpdatePlasma(void)
{
	ScrollPlasma();

	AddHotSpot();
	AddHotSpot();
	AddHotSpot();
	AddHotSpot();
	AddHotSpot();

	SmoothPlasma();
}

void UpdatePlasma2(void)
{
	ScrollPlasma();

	AddHotSpot();
	AddHotSpot();
	AddHotSpot();
	AddHotSpot();
	AddHotSpot();
	AddHotSpot();
	AddHotSpot();
	AddHotSpot();
	AddHotSpot();
	AddHotSpot();
	AddHotSpot();
	AddHotSpot();

	SmoothPlasma();
}

void RenderPlasma(MGLDraw *mgl,byte bottom)
{
	int i,x,y;
	byte *scrn;
	int wid;

	scrn=mgl->GetScreen();
	wid=mgl->GetWidth();
	if(bottom)
	{
		scrn+=wid*479;
		for(i=2;i<PLASMA_HEIGHT;i++)
		{
			y=i*PLASMA_WIDTH;
			for(x=0;x<PLASMA_WIDTH;x++)
			{
				*scrn=plasma[y+x];
				*(scrn-wid)=plasma[y+x];
				scrn++;
				*scrn=plasma[y+x];
				*(scrn-wid)=plasma[y+x];
				scrn++;
			}
			scrn-=(640+wid*2);
		}
	}
	else
	{
		for(i=2;i<PLASMA_HEIGHT;i++)
		{
			y=i*PLASMA_WIDTH;
			for(x=0;x<PLASMA_WIDTH;x++)
			{
				*scrn=plasma[y+x];
				*(scrn+wid)=plasma[y+x];
				scrn++;
				*scrn=plasma[y+x];
				*(scrn+wid)=plasma[y+x];
				scrn++;
			}
			scrn+=wid*2-640;
		}
	}
}

void RenderPlasma2(MGLDraw *mgl)
{
	int i,j,x,y;
	byte *scrn;
	int wid,hei;

	scrn=mgl->GetScreen();
	wid=mgl->GetWidth();
	scrn+=wid*479;
	
	for(x=0;x<PLASMA_WIDTH;x++)
	{
		hei=abs(x-160)/8+1;
		if(hei*PLASMA_HEIGHT>400)
			hei=400/PLASMA_HEIGHT;
		for(i=2;i<PLASMA_HEIGHT;i++)
		{
			y=i*PLASMA_WIDTH;
			
			for(j=0;j<hei;j++)
			{
				*(scrn-wid*j)=plasma[y+x];
			}
			scrn++;
			for(j=0;j<hei;j++)
			{
				*(scrn-wid*j)=plasma[y+x];
			}
			scrn-=(640+wid*(hei-1)+1);
		}
		scrn=mgl->GetScreen()+wid*479+2*(x+1);
	}
}

void RenderPlasma3(MGLDraw *mgl,byte bottom)
{
	int i,x,y;
	byte *scrn;
	int wid;

	scrn=mgl->GetScreen();
	wid=mgl->GetWidth();
	if(bottom)
	{
		scrn+=wid*479;
		for(i=2;i<PLASMA_HEIGHT;i++)
		{
			y=i*PLASMA_WIDTH;
			for(x=0;x<PLASMA_WIDTH;x++)
			{
				*scrn=plasma[y+x];
				scrn++;
				*scrn=plasma[y+x];
				scrn++;
			}
			scrn-=(640+wid);
		}
	}
	else
	{
		for(i=2;i<PLASMA_HEIGHT;i++)
		{
			y=i*PLASMA_WIDTH;
			for(x=0;x<PLASMA_WIDTH;x++)
			{
				*scrn=plasma[y+x];
				scrn++;
				*scrn=plasma[y+x];
				scrn++;
			}
			scrn+=wid-640;
		}
	}
}

void PlasmaColor(byte c)
{
	color=c;
}