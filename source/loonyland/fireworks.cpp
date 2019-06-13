#include "fireworks.h"
#include "cossin.h"
#include "sound.h"
#include "player.h"
#include "log.h"

fwork_t *fwork;
byte going;
int firstFree,lastFwork;

void InitFireworks(void)
{
	DBG("InitFireworks");
	going=1;
	fwork=(fwork_t *)calloc(sizeof(fwork_t)*MAX_FWORK,1);
	if(!fwork)
		going=0;
	firstFree=0;
	lastFwork=-1;
	DBG("SurvivedInitFireworks");
}

void ExitFireworks(void)
{
	if(fwork)
		free(fwork);
}

void RenderFirework(fwork_t *me,MGLDraw *mgl)
{
	char img[]={
		32,32,32,16,32,32,32,
		32,24,12, 8,12,24,32,
		32,12, 6, 4, 6,12,32,
		16, 8, 4, 0, 4, 8,16,
		32,12, 6, 4, 6,12,32,
		32,24,12, 8,12,24,32,
		32,32,32,16,32,32,32,
	};

	int bright;
	int width;
	byte *scrn;
	int i,j;

	if(me->x<3*FIXAMT || me->y<3*FIXAMT || me->x>639*FIXAMT-4*FIXAMT || me->y>479*FIXAMT-4*FIXAMT)
		return;

	width=mgl->GetWidth();
	scrn=mgl->GetScreen()+(me->x/FIXAMT-3)+(me->y/FIXAMT-3)*width;

	for(j=0;j<7;j++)
	{
		for(i=0;i<7;i++)
		{
			if(img[i+j*7]<32)
			{
				bright=(*scrn)&31;
				bright-=img[i+j*7]*2;
				bright+=me->bright/FIXAMT;
				if(bright>0)
				{
					if(bright>31)
						bright=31;
					*scrn=(me->color)+bright;
				}
			}
			scrn++;
		}
		scrn+=width-7;
	}
}

void RenderFireworks(MGLDraw *mgl)
{
	int i;

	DBG("RenderingFireworks");
	if(!going)
		return;

	for(i=0;i<lastFwork;i++)
	{
		if(fwork[i].type)
			RenderFirework(&fwork[i],mgl);
	}
}

int AddFwork(byte type,byte exType,byte color,int x,int y,int force)
{
	fwork_t *me;
	int i;
	int f;

	if(firstFree>=MAX_FWORK)
		return -1;

	me=&fwork[firstFree];

	me->type=type;
	me->tag=firstFree;
	me->exType=exType;
	me->color=color;
	me->x=x;
	me->y=y;
	me->timer=10+Random(40);
	i=Random(256);
	if(force>0)
		f=force*3/4+Random(force/2);
	else
		f=0;

	me->power=f;
	me->dx=Cosine(i)*f/FIXAMT;
	me->dy=Sine(i)*f/FIXAMT;
	// slight upward trend
	me->dy-=FIXAMT*2;

	me->bright=FIXAMT*10+Random(FIXAMT*40);

	for(i=me->tag+1;i<MAX_FWORK;i++)
		if(fwork[i].type==0)
		{
			firstFree=i;
			break;
		}
	if(i==MAX_FWORK)
		firstFree=i;

	if(me->tag>lastFwork)
		lastFwork=me->tag;

	return me->tag;
}

byte RandomColor(void)
{
	byte tab[]={32*1,32*3,32*4,32*5,32*6,32*7};

	return tab[Random(6)];
}

void LaunchShell(byte rank)
{
	int t,type,boom;
	int i;

	i=Random(rank+1);

	type=FWORK_SHELL;

	if(i<1)
		boom=FE_DUD;
	else if(i<10)
		boom=FE_SINGLE;
	else if(i<17)
		boom=FE_RANDOM;
	else if(i<19)
		boom=FE_SHELLS;
	else
		boom=FE_CRACKLER;


	t=AddFwork(type,boom,RandomColor(),Random(640)*FIXAMT,480*FIXAMT,FIXAMT*5);
	if(t!=-1)
	{
		MakeNormalSound(SND_FWORK+Random(2));
		fwork[t].dx=-FIXAMT*5+Random(FIXAMT*10);
		fwork[t].dy=-FIXAMT*8-Random(FIXAMT*8);
		fwork[t].timer=20+Random(60);
		fwork[t].bright=FIXAMT*64;
	}
}

void RemoveMe(fwork_t *me)
{
	int i;

	me->type=FWORK_NONE;
	if(me->tag<firstFree)
		firstFree=me->tag;
	if(me->tag==lastFwork)
	{
		for(i=me->tag;i>=0;i--)
			if(fwork[i].type)
			{
				lastFwork=i;
				break;
			}
		if(i==-1)
			lastFwork=-1;
	}
}

void FworkExplode(fwork_t *me)
{
	int i,t;
	int amt;

	// no exploding at the bottom of the screen
	if(me->y>440*FIXAMT)
		me->exType=FE_DUD;

	switch(me->exType)
	{
		case FE_DUD:
			break;
		case FE_SHELLS:
			amt=(me->power/FIXAMT)*10+Random(me->power/FIXAMT*20+1);
			for(i=0;i<amt;i++)
			{
				if(Random(10)==0)
				{
					t=AddFwork(FWORK_SHELL,FE_SINGLE,me->color,me->x,me->y,me->power);
					if(t!=-1)
					{
						// angle it uppish maybe
						fwork[t].dy-=Random(FIXAMT*10);
					}
				}
				else
					AddFwork(FWORK_NORMAL,FE_DUD,me->color,me->x,me->y,me->power);
			}
			break;
		case FE_SINGLE:
			MakeNormalSound(SND_CRUMP+Random(2));
			amt=(me->power/FIXAMT)*10+Random(me->power/FIXAMT*20+1);
			for(i=0;i<amt;i++)
				AddFwork(FWORK_NORMAL,FE_DUD,me->color,me->x,me->y,me->power);
			break;
		case FE_CRACKLER:
			MakeNormalSound(SND_CRUMP2);
			amt=(me->power/FIXAMT)*10+Random(me->power/FIXAMT*20+1);
			for(i=0;i<amt;i++)
				AddFwork(FWORK_CRACKLE,FE_DUD,me->color,me->x,me->y,me->power);
			break;
		case FE_RANDOM:
			MakeNormalSound(SND_CRUMP+Random(2));
			amt=(me->power/FIXAMT)*10+Random(me->power/FIXAMT*20+1);
			for(i=0;i<amt;i++)
				AddFwork(FWORK_NORMAL,FE_SINGLE,RandomColor(),me->x,me->y,me->power);
			break;
	}
	RemoveMe(me);
}

void UpdateFirework(fwork_t *me)
{
	me->x+=me->dx;
	me->y+=me->dy;
	me->dy+=FIXAMT/4;

	if(me->y>FIXAMT*550)
	{
		RemoveMe(me);
	}
	switch(me->type)
	{
		case FWORK_SHELL:
			me->timer--;
			if(me->timer==0)
			{
				FworkExplode(me);
			}
			if(player.rank==20)
			{
				if(Random(5)==0)
				{
					AddFwork(FWORK_NORMAL,FE_DUD,me->color,me->x,me->y,0);
				}
			}
			break;
		case FWORK_NORMAL:
		case FWORK_CRACKLE:
			me->bright-=FIXAMT;
			me->timer--;
			if(Random(128)==0)
				me->bright+=FIXAMT*Random(50);
			if(me->bright<-32*FIXAMT)
				FworkExplode(me);
			if(me->type==FWORK_CRACKLE)
			{
				if(Random(10)==0)
				{
					AddFwork(FWORK_NORMAL,FE_DUD,me->color,me->x,me->y,0);
				}
			}
			break;
	}
}

void UpdateFireworks(void)
{
	int i;

	if(!going)
		return;

	for(i=0;i<=lastFwork;i++)
	{
		if(fwork[i].type)
			UpdateFirework(&fwork[i]);
	}
}
