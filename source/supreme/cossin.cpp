#include "winpch.h"
#include "cossin.h"
#include <math.h>
#include "jamultypes.h"

#define DEGREES  (256)

int costab[DEGREES];
int sintab[DEGREES];

void InitCosSin(void)
{
	int i;

	for(i=0;i<DEGREES;i++)
	{
		costab[i]=(int)(cos((float)i*3.14159*2/DEGREES)*FIXAMT);
		sintab[i]=(int)(sin((float)i*3.14159*2/DEGREES)*FIXAMT);
	}
}

int Cosine(int angle)
{
	return costab[(angle&255)];
}

int Sine(int angle)
{
	return sintab[(angle&255)];
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

byte AngleFrom(int fromx,int fromy,int tox,int toy)
{
	float ang;

	ang=(float)atan2(-(fromy-toy),-(fromx-tox));
	ang=(ang*256.0f)/(3.14159f*2.0f);
	while(ang<0)
		ang+=256;
	while(ang>=256)
		ang-=256;

	return (byte)ang;
}

int Distance(int fromx,int fromy,int tox,int toy)
{
	float f,f2;

	f=(float)(fromx-tox)*(fromx-tox);
	f2=(float)(fromy-toy)*(fromy-toy);

	return (int)(sqrt(f+f2));
}

byte TurnToward(byte faceNow,byte newFace,byte spd)
{
	int diff,dir;

	if(newFace==faceNow)
		return faceNow;

	if(newFace>faceNow)
	{
		diff=newFace-faceNow;
		if(diff>128)
		{
			dir=-1;
			diff=256-diff;
		}
		else
			dir=1;
	}
	else
	{
		diff=faceNow-newFace;
		if(diff>128)
		{
			dir=1;
			diff=256-diff;
		}
		else
			dir=-1;
	}
	if(diff<=spd)
		return newFace;	// close enough
	else
		return ((faceNow+spd*dir)&255);
}
