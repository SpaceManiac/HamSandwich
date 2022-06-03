#include "cossin.h"
#include <math.h>

#define DEGREES  (256)

int costab[DEGREES];
int sintab[DEGREES];

void InitCosSin(void)
{
	int i;

	for(i=0;i<DEGREES;i++)
	{
		costab[i]=(int)(cos((double)i*3.14159*2/DEGREES)*FIXAMT);
		sintab[i]=(int)(sin((double)i*3.14159*2/DEGREES)*FIXAMT);
		if(costab[i]==255)
			costab[i]=256;
		if(costab[i]==-255)
			costab[i]=-256;
		if(sintab[i]==255)
			sintab[i]=256;
		if(sintab[i]==-255)
			sintab[i]=-256;
	}
}

int Cosine(int angle)
{
	angle=(angle&255);
	return costab[angle];
}

int Sine(int angle)
{
	angle=(angle&255);
	return sintab[angle];
}

void Dampen(int *value,int amt)
{
	if(*value>0)
	{
		*value-=amt;
		if(*value<0)
			*value=0;
	}
	if(*value<0)
	{
		*value+=amt;
		if(*value>0)
			*value=0;
	}
}

void Clamp(int *value,int amt)
{
	if(*value>amt)
		*value=amt;
	if(*value<-amt)
		*value=-amt;
}

byte PointInRect(int x,int y,int rx,int ry,int rx2,int ry2)
{
	return (x>=rx && y>=ry && x<=rx2 && y<=ry2);
}
