#include "flock.h"
#include "cossin.h"
#include "particle.h"

#define FLOCK_SIZE	128
#define CIRCLE_SIZE	32
#define FLOCK_MINDIST	(FIXAMT*20)

sprite_set_t *fsp;

flock_t flock[FLOCK_SIZE];
circle_t circle[2];
static word tick=0;

void CreateFlockMember(byte n)
{
	/*
	flock[n].x=Random(640*FIXAMT);
	flock[n].y=Random(480*FIXAMT);
	flock[n].dx=-FIXAMT*2+Random(FIXAMT*4);
	flock[n].dy=-FIXAMT*2+Random(FIXAMT*4);
	*/

	flock[n].bright=abs(32-(n%64));
	flock[n].brightChg=(n/32)%2;
	if(flock[n].brightChg==0)
		flock[n].brightChg=-1;

	flock[n].color=(byte)Random(8);
	flock[n].type=n%4;//(byte)Random(4);
	//flock[n].angAdj=(char)(-4+Random(9));
	//flock[n].angle=(byte)Random(256);

	flock[n].x=320*FIXAMT+Cosine(n*256/FLOCK_SIZE)*100;
	flock[n].y=240*FIXAMT+Sine(n*256/FLOCK_SIZE)*100;
	flock[n].dx=0;
	flock[n].dy=0;
	flock[n].angle=n*256/FLOCK_SIZE+64;
	flock[n].angAdj=2;
}

void SetupCircle(byte n)
{
	circle[n].angAdj=2-4*(n==1);
	circle[n].angle=0+128*(n==1);
	circle[n].width=0+FIXAMT*(n==1);
	circle[n].height=FIXAMT-FIXAMT*(n==1);
	circle[n].dWidth=2-4*(n==1);
	circle[n].dHeight=-2+4*(n==1);
}

void InitFlock(sprite_set_t *spr)
{
	int i;

	tick=0;
	fsp=spr;
	for(i=0;i<FLOCK_SIZE;i++)
		CreateFlockMember(i);
	SetupCircle(0);
	SetupCircle(1);
}

void UpdateFlockMember(flock_t *me)
{
	int dxAdj,dyAdj;

	me->x+=me->dx;
	me->y+=me->dy;

	Clamp(&me->dx,FIXAMT*4);
	Clamp(&me->dy,FIXAMT*4);
	me->bright+=me->brightChg;

	if(me->bright>=32)
	{
		me->brightChg=-1;
	}
	else if(me->bright<=0)
		me->brightChg=1;

	me->angle+=me->angAdj;
	
	dxAdj=Cosine(me->angle)/2;
	dyAdj=Sine(me->angle)/2;

	me->dx+=dxAdj;
	me->dy+=dyAdj;
}

void UpdateCircle(circle_t *me)
{
	me->angle+=me->angAdj;
	me->width+=me->dWidth;
	me->height+=me->dHeight;
	if(me->width>=FIXAMT)
	{
		me->width=FIXAMT;
		me->dWidth=-me->dWidth;
	}
	if(me->width<=-FIXAMT)
	{
		me->width=-FIXAMT;
		me->dWidth=-me->dWidth;
	}
	if(me->height>=FIXAMT)
	{
		me->height=FIXAMT;
		me->dHeight=-me->dHeight;
	}
	if(me->height<=-FIXAMT)
	{
		me->height=-FIXAMT;
		me->dHeight=-me->dHeight;
	}
}

void UpdateFlock(void)
{
	int i;

	for(i=0;i<FLOCK_SIZE;i++)
	{
		UpdateFlockMember(&flock[i]);
	}

	UpdateCircle(&circle[0]);
	UpdateCircle(&circle[1]);

	tick++;
	if(tick==30*17)
	{
		tick=0;
		for(i=0;i<FLOCK_SIZE;i++)
			flock[i].angAdj=-flock[i].angAdj;
	}
}

void RenderFlockItem(byte type,int x,int y,byte color,char bright,MGLDraw *mgl)
{
	switch(type)
	{
		case 0:
			fsp->GetSprite(2)->DrawOffColor(x,y,mgl,0,color,bright);
			break;
		case 1:
			fsp->GetSprite(3)->DrawOffColor(x,y,mgl,2,color,bright);
			break;
		case 2:
			fsp->GetSprite(4)->DrawOffColor(x,y,mgl,4,color,bright);
			break;
		case 3:
			fsp->GetSprite(5)->DrawOffColor(x,y,mgl,6,color,bright);
			break;
	}

}

void RenderCircle(byte n,circle_t *me,MGLDraw *mgl)
{
	int i;
	int x,y;
	int xMult,yMult;

	byte type;
	byte color;
	byte bright;
	byte angAdd;
	char brightAdd;

	type=0;
	color=0;

	bright=abs(32-(me->angle%64));
	brightAdd=(me->angle/32)%2;
	if(brightAdd==0)
		brightAdd=-1;

	angAdd=0;

	if(n==0)
	{
		xMult=(300*me->width/FIXAMT)*Cosine(me->angle)/FIXAMT;
		yMult=(220*me->height/FIXAMT)*Sine(me->angle)/FIXAMT;
	}
	else
	{
		xMult=(300*me->width/FIXAMT);
		yMult=(220*me->height/FIXAMT);
	}

	for(i=0;i<CIRCLE_SIZE;i++)
	{
		type++;
		if(type>3)
			type=0;
		color++;
		if(color>7)
			color=0;
		bright+=brightAdd;
		if(bright==31)
			brightAdd=-1;
		if(bright==0)
			brightAdd=1;

		x=Cosine(me->angle+angAdd)*xMult;
		y=Sine(me->angle+angAdd)*yMult;
		x=320+x/FIXAMT;
		y=240+y/FIXAMT;
		RenderFlockItem(type,x,y,color,bright,mgl);
		angAdd+=(256/CIRCLE_SIZE);
	}
}

void RenderFlock(MGLDraw *mgl)
{
	int i;

	for(i=0;i<2;i++)
		RenderCircle(i,&circle[i],mgl);

	for(i=0;i<FLOCK_SIZE;i++)
	{
		RenderFlockItem(flock[i].type,flock[i].x/FIXAMT,flock[i].y/FIXAMT,flock[i].color,flock[i].bright,mgl);
	}
}

