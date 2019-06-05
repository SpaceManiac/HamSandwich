#include "winpch.h"
#include "arcade.h"
#include "dialogbits.h"
#include "spacegame.h"
#include "racer.h"

#include "goal.h"

#define MAX_CAVEBUL	1024
#define MAX_CAVEGUY	256

#define CG_PLAYER	1
#define CG_DIAMOND	2
#define CG_VOLCANO  3
#define CG_ANTHILL	4
#define CG_ANT		5

#define LAVA_BASE	(32*4+4)
#define LAVA_MAX	(8)

static byte guyPic[]=
{
	00,00,47,00,00,
	47,31,31,31,47,
	00,00,31,00,00,
	00,31,00,31,00,
	47,00,00,00,47,
};

typedef struct caveBul_t
{
	int x,y;
	int dx,dy;
	byte type;
	byte clock;
} caveBul_t;

typedef struct caveGuy_t
{
	int x,y;
	int dx,dy;
	int life;
	byte anim;
	byte type;
	byte mind;
	byte reload;
	byte stand;
} caveGuy_t;

static byte oldc;
static byte *map;
static dword timer;
static int score,curScore,lives,firePower,antTimer,life,pressure,coinsEarned;
static byte gameOver,gameOverClock,gameLevel,fireAng;
static int totalTime,diamondsGot;
static caveGuy_t *guy;
static caveBul_t *bul;

void MapBox(int x,int y,int x2,int y2,byte c)
{
	int i,j;

	for(j=y;j<=y2;j++)
		for(i=x;i<=x2;i++)
		{
			if(c==0)
				map[i+j*640]=0;
			else if(c==32*4)
				map[i+j*640]=32*4+LAVA_MAX;
			else
			{
				if(c==32*2)
					map[i+j*640]=32*2+1+Random(10+j/24);
			}
		}
}

void MapSmooth(int x,int y,int x2,int y2)
{
	int i,j;
	int t;

	for(j=y;j<=y2;j++)
		for(i=x;i<=x2;i++)
		{
			t=map[i+j*640]*4;
			if(i>0)
				t+=map[i-1+j*640];
			else
				t+=map[i+j*640];
			if(i<639)
				t+=map[i+1+j*640];
			else
				t+=map[i+j*640];
			if(j>0)
				t+=map[i+(j-1)*640];
			else
				t+=map[i+j*640];
			if(j<479)
				t+=map[i+(j+1)*640];
			else
				t+=map[i+j*640];
			map[i+j*640]=t/8;
		}
}

void NewCaveLevel(void)
{
	int i,j,x,y;

	diamondsGot=0;
	timer=60*30*2;

	MapBox(0,0,639,479,32*2);	// fill in dirt
	MapSmooth(0,0,639,479);
	MapSmooth(0,0,639,479);
	MapBox(0,0,639,20,0);	// black out the score portion

	firePower=8;
	antTimer=1;
	life=100;

	for(i=0;i<MAX_CAVEGUY;i++)
		guy[i].type=0;
	for(i=0;i<MAX_CAVEBUL;i++)
		bul[i].type=0;

	guy[0].type=CG_PLAYER;
	guy[0].x=320*FIXAMT;
	guy[0].y=5*FIXAMT;
	guy[0].dx=0;
	guy[0].dy=0;
	guy[0].stand=0;

	for(i=0;i<gameLevel+3;i++)
	{
		guy[i+1].type=CG_DIAMOND;
		guy[i+1].x=20*FIXAMT+Random(600)*FIXAMT;
		guy[i+1].y=30*FIXAMT+Random((gameLevel*15)+100)*FIXAMT;
		if(guy[i+1].y>460*FIXAMT)
			guy[i+1].y=460*FIXAMT;
	}

	if(gameLevel>=3)
	{
		j=gameLevel+4;

		for(i=0;i<(gameLevel-3);i++)
		{
			guy[j+i].type=CG_ANTHILL;
			guy[j+i].life=500;
			guy[j+i].x=20*FIXAMT+Random(600)*FIXAMT;
			guy[j+i].y=475*FIXAMT-Random(gameLevel*30)*FIXAMT;
			if(guy[j+i].y<40*FIXAMT)
				guy[j+i].y=40*FIXAMT+Random(100*FIXAMT);
			guy[j+i].mind=Random(120);
		}
		if(gameLevel>=5)
		{
			for(i=0;i<(gameLevel-5);i++)
			{
				x=20+Random(600);
				y=475-Random(gameLevel*10);
				if(y<40)
					y=40+Random(100);
				MapBox(x-1,y-1,x+1,y+1,32*4);
			}
		}
	}
}

void InitCaveGame(MGLDraw *mgl)
{
	map=new byte[640*480];

	score=0;
	fireAng=0;
	curScore=0;
	lives=2;
	gameLevel=4;

	mgl->LastKeyPressed();
	gameOver=0;
	totalTime=0;

	guy=new caveGuy_t[MAX_CAVEGUY];
	bul=new caveBul_t[MAX_CAVEBUL];

	NewCaveLevel();
}

void ExitCaveGame(void)
{
	delete[] map;
	delete[] guy;
	delete[] bul;
}

byte CaveDie(void)
{
	if(lives==0)
		return 0;
	lives--;
	if(lives==0)
	{
		gameOver=1;
		gameOverClock=30;
		if(score>=500000)
			CompleteGoal(27);

		GetDisplayMGL()->LastKeyPressed();
		GetTaps();
		coinsEarned=score/3000;
		profile.progress.totalCoins+=coinsEarned;
	}
	NewCaveLevel();
	return 1;
}

void RenderCaveBullet(MGLDraw *mgl,caveBul_t *me)
{
	byte *scrn;
	int pitch;
	byte b;

	pitch=mgl->GetWidth();
	scrn=mgl->GetScreen();

	if(me->type==1)
	{
		b=32*3+me->clock;
		if(b>32*3+31)
			b=32*3+31;

		scrn[me->x/FIXAMT+(me->y/FIXAMT)*pitch]=b;
	}
	else if(me->type==2)	// lava bullet
	{
		b=32*4+me->clock;
		if(b>32*4+31)
			b=32*4+31;

		scrn[me->x/FIXAMT+(me->y/FIXAMT)*pitch]=b;
	}
	else if(me->type==3)	// steam
	{
		b=scrn[me->x/FIXAMT+(me->y/FIXAMT)*pitch];
		if((b+me->clock)/32!=(b/32))
			b=31;
		else
			b=(b&31)+me->clock;
		scrn[me->x/FIXAMT+(me->y/FIXAMT)*pitch]=b;
	}
}

void RenderDiamond(MGLDraw *mgl,caveGuy_t *me)
{
	int x,y;

	x=me->x/FIXAMT;
	y=me->y/FIXAMT;

	DrawLine(x-2,y,x,y-2,31);
	DrawLine(x,y-2,x+2,y,31);
	DrawLine(x+2,y,x,y+2,31);
	DrawLine(x,y+2,x-2,y,31);
}

inline void PlotPixel(int x,int y,byte c,byte *scrn)
{
	if(x>0 && x<640 && y>0 && y<480)
		scrn[x+y*640]=c;
}

void RenderCaveGuy(MGLDraw *mgl,caveGuy_t *me)
{
	int x,y,i,j,k,ptr;
	byte *scrn;
	int pitch;
	pitch=mgl->GetWidth();
	scrn=mgl->GetScreen();

	if(me->type==CG_DIAMOND)
		RenderDiamond(mgl,me);
	else if(me->type==CG_ANT)
	{
		scrn[me->x/FIXAMT+(me->y/FIXAMT)*640]=32*7+16;

		if(me->anim<3)
		{
			scrn[me->x/FIXAMT-1+(me->y/FIXAMT)*640]=32*7+10;
			scrn[me->x/FIXAMT+1+(me->y/FIXAMT)*640]=32*7+10;
			scrn[me->x/FIXAMT+(me->y/FIXAMT-1)*640]=32*7+10;
			scrn[me->x/FIXAMT+(me->y/FIXAMT+1)*640]=32*7+10;
		}
		else
		{
			scrn[me->x/FIXAMT-1+(me->y/FIXAMT-1)*640]=32*7+13;
			scrn[me->x/FIXAMT+1+(me->y/FIXAMT-1)*640]=32*7+13;
			scrn[me->x/FIXAMT+1+(me->y/FIXAMT-1)*640]=32*7+13;
			scrn[me->x/FIXAMT-1+(me->y/FIXAMT+1)*640]=32*7+13;
		}
	}
	else if(me->type==CG_ANTHILL)
	{
		x=me->x/FIXAMT;
		y=me->y/FIXAMT;

		if(me->mind==0)
			i=32*7+14;
		else
			i=32*4+14;
		PlotPixel(x,y,i+6,scrn);
		PlotPixel(x-1,y,i,scrn);
		PlotPixel(x+1,y,i,scrn);
		PlotPixel(x,y-1,i,scrn);
		PlotPixel(x,y+1,i,scrn);
		PlotPixel(x-2,y-1,i,scrn);
		PlotPixel(x-2,y+1,i,scrn);
		PlotPixel(x+2,y-1,i,scrn);
		PlotPixel(x+2,y+1,i,scrn);
		PlotPixel(x-1,y-2,i,scrn);
		PlotPixel(x+1,y-2,i,scrn);
		PlotPixel(x-1,y+2,i,scrn);
		PlotPixel(x+1,y+2,i,scrn);

	}
	else if(me->type==CG_PLAYER)
	{

		x=(me->x+Cosine(fireAng)*4)/FIXAMT;
		y=(me->y-2+Sine(fireAng)*4)/FIXAMT;
		PlotPixel(x-1,y,32*6+10,scrn);
		PlotPixel(x+1,y,32*6+10,scrn);
		PlotPixel(x,y-1,32*6+10,scrn);
		PlotPixel(x,y+1,32*6+10,scrn);

		x=me->x/FIXAMT;
		y=me->y/FIXAMT;
		x-=2;
		y-=4;

		for(j=0;j<5;j++)
		{
			if(y+j>=0 && y+j<480)
			{
				k=((y+j)*pitch);
				ptr=j*5;
				for(i=0;i<5;i++)
				{
					if(x+i>=0 && x+i<639 && guyPic[ptr+i])
						scrn[(x+i)+k]=guyPic[i+ptr];
				}
			}
		}
	}
}

void RenderCaveGame(MGLDraw *mgl)
{
	int i;
	byte *scrn;
	int pitch;
	char s[32];

	mgl->ClearScreen();
	pitch=mgl->GetWidth();
	scrn=mgl->GetScreen();

	mgl->FillBox(0,0,639,19,0);
	for(i=20;i<480;i++)
		memcpy(&scrn[pitch*i],&map[i*640],640);	// paint the map

	if(gameOver)
	{
		CenterPrint(320,100,"GAME OVER!",0,2);
		sprintf(s,"Survived for %d:%02d",totalTime/(30*60),(totalTime/30)%60);
		CenterPrint(320,150,s,0,1);
		sprintf(s,"Total Score: %d",score);
		CenterPrint(320,180,s,0,1);

		sprintf(s,"Coins Earned: %d",coinsEarned);
		CenterPrint(320,240,s,0,1);
	}
	else
	{
		for(i=0;i<MAX_CAVEBUL;i++)
			if(bul[i].type)
				RenderCaveBullet(mgl,&bul[i]);
		for(i=0;i<MAX_CAVEGUY;i++)
			if(guy[i].type)
				RenderCaveGuy(mgl,&guy[i]);

		// interface
		sprintf(s,"Score: %08d",curScore);
		PrintGlow(2,2,s,0,1);

		sprintf(s,"Level: %d",gameLevel-3);
		PrintGlow(300,2,s,0,1);

		sprintf(s,"Time: %lu:%02lu",(timer/(30*60)),(timer/30)%60);
		PrintGlow(400,2,s,0,1);

		sprintf(s,"Lives: %d",lives);
		PrintGlow(638-GetStrLength(s,1),2,s,0,1);

		DrawBox(180,1,281,12,31);
		if(life>0)
			DrawFillBox(181,2,180+life,11,32*4+16);
	}
}

inline byte IsLava(byte n)
{
	return (n>=LAVA_BASE && n<=LAVA_BASE+LAVA_MAX);
}

inline byte IsDirt(byte n)
{
	return (n>32*2 && n<=32*2+31);
}

byte CanBeHere(caveGuy_t *me)
{
	int i,j,x,y;

	x=me->x/FIXAMT-2;
	y=me->y/FIXAMT-4;

	for(i=x;i<x+5;i++)
	{
		for(j=y;j<y+5;j++)
		{
			if(i<0 || j<0 || i>639 || j>479)
				return 0;	// can't be offscreen
			if(map[i+j*640]>32*2 && map[i+j*640]<=32*2+31 && guyPic[(i-x)+(j-y)*5])
				return 0;
		}
	}
	return 1;
}

void UpdateFireAngle(byte c)
{
	static byte f;

	f=255;
	if(c&CONTROL_RT)
	{
		if(c&CONTROL_UP)
			f=7;
		else if(c&CONTROL_DN)
			f=1;
		else
			f=0;
	}
	else if(c&CONTROL_LF)
	{
		if(c&CONTROL_UP)
			f=5;
		else if(c&CONTROL_DN)
			f=3;
		else
			f=4;
	}
	else
	{
		if(c&CONTROL_UP)
			f=6;
		else if(c&CONTROL_DN)
			f=2;
	}
	if(f!=255)
	{
		int diff,dir;

		f*=32;

		if(f!=fireAng)
		{
			if(f>fireAng)
			{
				diff=f-fireAng;
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
				diff=fireAng-f;
				if(diff>128)
				{
					dir=1;
					diff=256-diff;
				}
				else
					dir=-1;
			}

			if(diff<2)
				fireAng=f;
			else
				fireAng+=dir*2;
		}
	}
}

void CaveFireBullet(byte c,caveGuy_t *me)
{
	int i,j;
	int num,spread;

	spread=2+firePower/4;
	if(spread>16)
		spread=16;
	num=0;
	for(i=0;i<MAX_CAVEBUL;i++)
	{
		if(!bul[i].type)
		{
			bul[i].x=me->x-FIXAMT*3+Random(FIXAMT*6+1);
			bul[i].y=me->y+FIXAMT*1-Random(FIXAMT*6+1);
			bul[i].clock=firePower+Random(10);
			j=(fireAng-spread+Random(spread*2+1))&255;
			bul[i].dx=Cosine(j);
			bul[i].dy=Sine(j);
			bul[i].type=1;
			num++;
			if(num==firePower)
				return;
		}
	}
}

byte GetNeighborLava(int x,int y)
{
	if(x<0 || y<0 || x>639 || y>479)
		return 255;	// cannot interact with that space at all

	return map[x+y*640];
}

void CaveLavaSpew(int x,int y)
{
	int i,j;

	for(i=0;i<MAX_CAVEBUL;i++)
	{
		if(!bul[i].type)
		{
			bul[i].x=x*FIXAMT;
			bul[i].y=y*FIXAMT;
			bul[i].clock=10+Random(20);
			j=Random(128)+128;
			bul[i].dx=Cosine(j);
			bul[i].dy=Sine(j);
			bul[i].type=2;
			return;
		}
	}
}

void AddAnt(int x,int y)
{
	int i;

	for(i=0;i<MAX_CAVEGUY;i++)
	{
		if(!guy[i].type)
		{
			guy[i].type=CG_ANT;
			guy[i].x=x;
			guy[i].y=y;
			guy[i].mind=Random(256);
			guy[i].anim=Random(6);
			return;
		}
	}
}

byte AntAngle(caveGuy_t *me,caveGuy_t *you)
{
	byte a;

	a=255;

	if(you->x<me->x-FIXAMT*16)
	{
		if(you->y<me->y-FIXAMT*16)
			a=5;
		else if(you->y>me->y+FIXAMT*16)
			a=3;
		else
			a=4;
	}
	else if(you->x>me->x+FIXAMT*16)
	{
		if(you->y<me->y-FIXAMT*16)
			a=7;
		else if(you->y>me->y+FIXAMT*16)
			a=1;
		else
			a=0;
	}
	else
	{
		if(you->y<me->y-FIXAMT*16)
			a=6;
		else if(you->y>me->y+FIXAMT*16)
			a=2;
	}

	if(a==255)
		a=Random(256);
	else
		a=a*32;

	return a;
}

void AntTurnTo(byte desired,caveGuy_t *me)
{
	int diff,dir;

	if(desired==me->mind)
		return;

	if(desired>me->mind)
	{
		diff=desired-me->mind;
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
		diff=me->mind-desired;
		if(diff>128)
		{
			dir=1;
			diff=256-diff;
		}
		else
			dir=-1;
	}
	me->mind=(me->mind+dir)&255;
}

void AntAI(caveGuy_t *me)
{
	byte b;
	int i,mx,my;

	me->anim++;
	if(me->anim==6)
		me->anim=0;

	if(PointInRect(me->x,me->y,guy[0].x-2*FIXAMT,guy[0].y-4*FIXAMT,guy[0].x+2*FIXAMT,guy[0].y))
	{
		me->type=0;	// die hurting the player
		MakeNormalSound(SND_BOUAPHAOUCH);
		life-=100;
		if(life<=0)
			CaveDie();
	}

	for(i=0;i<MAX_CAVEBUL;i++)
	{
		if(bul[i].type==1 && abs(bul[i].x-me->x)<FIXAMT && abs(bul[i].y-me->y)<FIXAMT)
		{
			me->type=0;	// die from the water
			score+=1000;
			MakeNormalSound(SND_SPD1DIE);
		}
	}

	me->x+=me->dx;
	if(me->x<1*FIXAMT || me->x>638*FIXAMT)
	{
		me->mind=Random(256);
		me->x-=me->dx;
	}
	me->y+=me->dy;
	if(me->y<FIXAMT*20 || me->y>478*FIXAMT)
	{
		me->mind=Random(256);
		me->y-=me->dy;
	}

	AntTurnTo(AntAngle(me,&guy[0]),me);

	me->dx=Cosine(me->mind)/2;
	me->dy=Sine(me->mind)/2;

	me->mind+=4-Random(9);
	if(Random(50)==0)
		me->mind=Random(256);

	mx=me->x/FIXAMT;
	my=me->y/FIXAMT;

	b=map[(mx)+(my)*640];

	if(IsLava(b))
		me->type=0;	// burn to death
	if(IsDirt(b))
	{
		map[(mx)+(my)*640]=0;
	}
	b=map[(mx-1)+(my)*640];
	if(IsDirt(b))
	{
		map[(mx-1)+(my)*640]=0;
	}
	b=map[(mx+1)+(my)*640];
	if(IsDirt(b))
	{
		map[(mx+1)+(my)*640]=0;
	}
	b=map[(mx)+(my-1)*640];
	if(IsDirt(b))
	{
		map[(mx)+(my-1)*640]=0;
	}
}

byte UpdateCaveGuy(MGLDraw *mgl,caveGuy_t *me)
{
	byte c;
	int i;
	int mx,my;

	if(me->type==CG_VOLCANO)
	{
		CaveLavaSpew(me->x/FIXAMT,me->y/FIXAMT);
		return 0;
	}
	if(me->type==CG_ANTHILL)
	{
		me->mind=0;
		antTimer--;
		if(!antTimer)
		{
			AddAnt(me->x,me->y);
			antTimer=(60*2-gameLevel*3);
			if(antTimer<0)
				antTimer=1;
		}
		for(i=0;i<MAX_CAVEBUL;i++)
		{
			if(bul[i].type==1 && abs(bul[i].x-me->x)<FIXAMT && abs(bul[i].y-me->y)<FIXAMT)
			{
				me->life--;
				me->mind=1;
				if(me->life==0)
				{
					score+=10000;
					me->type=0;
					MakeNormalSound(SND_BOMBBOOM);
				}
			}
		}

		mx=me->x/FIXAMT;
		my=me->y/FIXAMT;

		c=map[(mx)+(my)*640];

		if(IsLava(c))
		{
			me->type=0;	// burn to death
			MakeNormalSound(SND_BOMBBOOM);
		}

		return 0;
	}
	if(me->type==CG_ANT)
	{
		AntAI(me);
		return 0;
	}

	if(me->reload)
		me->reload--;

	me->x+=me->dx;
	if(!CanBeHere(me))
	{
		me->x-=me->dx;
		me->dx=0;
	}
	me->y+=me->dy;
	if(!CanBeHere(me))
	{
		me->y-=me->dy;
		if(me->dy>0)
		{
			me->dy=0;
			me->stand=1;
		}
		else
			me->dy=0;
	}
	else
		me->stand=0;

	me->dy+=FIXAMT/32;

	if(me->x<3*FIXAMT)
	{
		me->x=3*FIXAMT;
		me->dx=-me->dx;
	}
	if(me->x>(640-3)*FIXAMT)
	{
		me->x=(640-3)*FIXAMT;
		me->dx=-me->dx;
	}
	if(me->y<5*FIXAMT)
		me->y=5*FIXAMT;

	if(me->y>(480-1)*FIXAMT)
		me->y=(480-1)*FIXAMT;

	if(me->type==CG_PLAYER)
	{
		c=GetControls();
		UpdateFireAngle(c);

		if(c&CONTROL_LF)
		{
			if(me->dx>0)
				me->dx=0;
			me->dx-=FIXAMT/8;
		}
		if(c&CONTROL_RT)
		{
			if(me->dx<0)
				me->dx=0;
			me->dx+=FIXAMT/8;
		}
		if(me->stand && (c&CONTROL_B2))
		{
			me->dy=-FIXAMT;
		}
		if(c&CONTROL_B1)
		{
			CaveFireBullet(c,me);

		}

		Clamp(&me->dx,FIXAMT);

		Dampen(&me->dx,FIXAMT/32);
		Clamp(&me->dy,FIXAMT);

		oldc=c;

		c=mgl->LastKeyPressed();
		if(c==27)
			return 1;

		if(me->y<21*FIXAMT && diamondsGot==gameLevel+3)
		{
			gameLevel++;
			NewCaveLevel();
		}
		if(IsLava(map[me->x/FIXAMT+(me->y/FIXAMT)*640]))
		{
			map[me->x/FIXAMT+(me->y/FIXAMT)*640]=0;
			MakeNormalSound(SND_BOUAPHAOUCH);
			life--;
			if(life<=0)
				CaveDie();
		}
	}
	else if(me->type==CG_DIAMOND)
	{
		if(abs(me->x-guy[0].x)<5*FIXAMT && abs(me->y-guy[0].y)<5*FIXAMT)
		{
			MakeNormalSound(SND_COINGET);
			me->type=0;
			firePower+=4;
			diamondsGot++;
			score+=5000;
		}
		if(IsLava(map[me->x/FIXAMT+(me->y/FIXAMT)*640]))
		{
			me->type=0;
			diamondsGot++;
			score-=5000;
		}
	}
	return 0;
}

void UpdateCaveBul(MGLDraw *mgl,caveBul_t *me)
{
	byte *m;

	if(!--me->clock)
	{
		me->type=0;
	}

	if(me->type==0)
		return;

	me->x+=me->dx;
	me->y+=me->dy;

	if(me->type!=3)
		me->dy+=4;

	if(me->x<0 || me->y<0 || me->x>=(640*FIXAMT) || me->y>=(480*FIXAMT))
	{
		me->type=0;
		return;
	}
	m=&map[me->x/FIXAMT+(me->y/FIXAMT)*640];

	if(me->type==1)	// water
	{
		if((*m)>32*2 && (*m)<=32*2+31)
		{
			(*m)-=2;
			if((*m)<=32*2)
			{
				(*m)=0;
				score++;
			}
			me->type=0;
		}
		if((*m)>=LAVA_BASE && (*m)<=LAVA_BASE+LAVA_MAX)
		{
			(*m)=0;
			me->type=3;	// steam
			me->dx=0;
			me->dy=-FIXAMT/2;
			me->clock=32;
			score+=5;
			MakeSpaceSound(SND_MISSILELAUNCH,1);
		}
	}
	else if(me->type==2)	// lava
	{
		if((*m)>32*2 && (*m)<=32*2+31)
		{
			(*m)-=(me->clock/2);
			if((*m)<=32*2)
				(*m)=0;
			//if(me->clock>1)
			//	me->clock--;
		}
		else if((*m)>LAVA_BASE && (*m)<LAVA_BASE+LAVA_MAX)
		{
			(*m)+=me->clock;
			if((*m)>LAVA_BASE+LAVA_MAX)
				(*m)=LAVA_BASE+LAVA_MAX;
			me->type=0;
		}
		if((*m)==0)
		{
			(*m)=LAVA_BASE+1;
		}
	}
}

void TransferLava(byte *src,byte *dst,byte amt)
{
	if(dst!=NULL && (*dst)==LAVA_BASE+LAVA_MAX)
		return;	// can't dump there!

	if(src!=NULL)
	{
		if((*src)<LAVA_BASE+amt)
			amt=(*src)-LAVA_BASE;
		if(dst!=NULL && (*dst)>=LAVA_BASE && (*dst)<=LAVA_BASE+LAVA_MAX)
		{
			if((*dst)+amt>LAVA_BASE+LAVA_MAX)
				amt=LAVA_BASE+LAVA_MAX-(*dst);
		}
		(*src)-=amt;
	}
	if(dst!=NULL)
	{
		if((*dst)==0 || ((*dst)>=32*2 && (*dst)<=32*2+31))
			(*dst)=LAVA_BASE+amt;
		else
		{
			(*dst)+=amt;
			if((*dst)>LAVA_BASE+LAVA_MAX)
				(*dst)=LAVA_BASE+LAVA_MAX;
		}
	}
}

void UpdateLava(void)
{
	static int line=0;
	int i;
	byte *m,nbr[4];

	line--;
	if(line<0)
		line=479;

	m=&map[line*640];

	for(i=0;i<640;i++)
	{
		if((*m)>=LAVA_BASE && (*m)<=LAVA_BASE+LAVA_MAX)	// we have lava!
		{
			//if((*m)<LAVA_BASE+LAVA_MAX)
			//	(*m)++;

			nbr[0]=GetNeighborLava(i+1,line);
			nbr[1]=GetNeighborLava(i,line+1);
			nbr[2]=GetNeighborLava(i-1,line);
			nbr[3]=GetNeighborLava(i,line-1);

			if((*m)==LAVA_BASE+LAVA_MAX && (nbr[0]==LAVA_BASE+LAVA_MAX || nbr[0]==255) &&
										   (nbr[1]==LAVA_BASE+LAVA_MAX || nbr[1]==255) &&
										   (nbr[2]==LAVA_BASE+LAVA_MAX || nbr[2]==255) &&
										   (nbr[3]==LAVA_BASE+LAVA_MAX || nbr[3]==255))
			{
				m++;
				continue;
			}

			if(nbr[1]==0)	// empty space below, dump 4 points downward
			{
				TransferLava(m,&map[i+(line+1)*640],4);
			}
			else if(nbr[1]>=LAVA_BASE && nbr[1]<LAVA_BASE+LAVA_MAX)	// non-maxed lava below
			{
				TransferLava(m,&map[i+(line+1)*640],1);
			}
			if(Random(2)==0)
			{
				if((*m)>LAVA_BASE && !IsDirt(nbr[1]) && nbr[1]!=LAVA_BASE+LAVA_MAX && nbr[1]!=255)
				{
					TransferLava(m,m-1,4);
				}
			}
			else
			{
				if((*m)>LAVA_BASE && !IsDirt(nbr[0]) && nbr[0]!=LAVA_BASE+LAVA_MAX && nbr[0]!=255)
				{
					TransferLava(m,m+1,4);
				}
			}
			if((*m)==LAVA_BASE+LAVA_MAX && (IsDirt(nbr[1])))
			{
				(*(m+640))--;
				if((*(m+640))<=32*2)
				{
					(*(m+640))=0;
					TransferLava(m,m+640,1);
				}
			}
			if((*m)==LAVA_BASE+LAVA_MAX && (IsDirt(nbr[0])))
			{
				(*(m+1))--;
				if((*(m+1))<=32*2)
				{
					(*(m+1))=0;
					TransferLava(m,m+1,1);
				}
			}
			if((*m)==LAVA_BASE+LAVA_MAX && (IsDirt(nbr[2])))
			{
				(*(m-1))--;
				if((*(m-1))<=32*2)
				{
					(*(m-1))=0;
					TransferLava(m,m-1,1);
				}
			}
			if((*m)==LAVA_BASE+LAVA_MAX && (nbr[1]==LAVA_BASE+LAVA_MAX || IsDirt(nbr[1]) || nbr[1]==255) &&
				((nbr[0]==LAVA_BASE+LAVA_MAX || IsDirt(nbr[0]) || nbr[0]==255) ||
				(nbr[2]==LAVA_BASE+LAVA_MAX || IsDirt(nbr[2]) || nbr[2]==255)) &&
				(nbr[3]==0 || IsDirt(nbr[3])))
			{
				(*m)--;
				CaveLavaSpew(i,line);
			}

			if((*m)<=LAVA_BASE)
				*m=0;	// emptied lava is now empty space
		}
		m++;
	}
}

byte UpdateCaveGame(int *lastTime,MGLDraw *mgl)
{
	int i,j;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		if(curScore<score)
			curScore++;
		if(curScore<score-10)
			curScore+=5;
		if(curScore<score-100)
			curScore+=50;
		if(curScore<score-1000)
			curScore+=500;
		if(curScore<score-10000)
			curScore+=5000;
		if(curScore>score)
			curScore--;

		for(j=0;j<240;j++)
			UpdateLava();

		for(i=0;i<MAX_CAVEGUY;i++)
			if(guy[i].type)
			{
				if(guy[i].type==CG_PLAYER)
				{
					if(UpdateCaveGuy(mgl,&guy[i]))
						return 1;
					if(UpdateCaveGuy(mgl,&guy[i]))
						return 1;
				}
				else
					UpdateCaveGuy(mgl,&guy[i]);
			}

		for(i=0;i<MAX_CAVEBUL;i++)
			if(bul[i].type)
			{
				UpdateCaveBul(mgl,&bul[i]);
				UpdateCaveBul(mgl,&bul[i]);
			}

		timer--;
		if(!timer)
			if(!CaveDie())
				return 1;

		totalTime++;
		*lastTime-=TIME_PER_FRAME;
	}

	return 0;
}

static byte UpdateGameOver(int *lastTime,MGLDraw *mgl)
{
	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		*lastTime-=TIME_PER_FRAME;
		if(gameOverClock)
			gameOverClock--;
	}

	if(mgl->LastKeyPressed() && gameOverClock==0)
		return 1;

	if((GetTaps()&(CONTROL_B1|CONTROL_B2)) && gameOverClock==0)
		return 1;

	return 0;
}

void CaveGame(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	InitCaveGame(mgl);
	lastTime=1;
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		if(!gameOver)
			done=UpdateCaveGame(&lastTime,mgl);
		else
			done=UpdateGameOver(&lastTime,mgl);
		RenderCaveGame(mgl);
		mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	ExitCaveGame();
}

// -----------------------------------

static byte *backgd;
static sprite_set_t *plSpr;
static byte game,dispMode,cursor;
static int msx,msy;
static char msBright,msDBright;

void InitArcade(MGLDraw *mgl)
{
	int i;

	msBright=0;
	msDBright=1;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);
	plSpr=new sprite_set_t("graphics/pause.jsp");

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);
	dispMode=0;
	cursor=0;
	GetTaps();
	GetArrowTaps();
}

void ExitArcade(void)
{
	free(backgd);
	delete plSpr;
}

byte UpdateArcade(int *lastTime,MGLDraw *mgl)
{
	char k;
	byte c;

	mgl->GetMouse(&msx,&msy);

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	k=mgl->LastKeyPressed();

	while(*lastTime>=TIME_PER_FRAME)
	{
		msBright+=msDBright;
		if(msBright>10)
			msDBright=-1;
		if(msBright<-2)
			msDBright=1;

		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
	}

	c=GetTaps()|GetArrowTaps();

	if(c&CONTROL_LF)
	{
		cursor--;
		if(cursor>2)
			cursor=2;
	}
	if(c&CONTROL_RT)
	{
		cursor++;
		if(cursor>2)
			cursor=0;
	}
	if(c&CONTROL_B1)
	{
		if(cursor==1)
			dispMode=1-dispMode;
		if(cursor==0)
		{
			if(profile.progress.totalCoins-profile.progress.coinsSpent<25)
				MakeNormalSound(SND_TURRETBZZT);
			else
			{
				profile.progress.coinsSpent+=25;
				return 2;	// 2 means I want to play
			}
		}
		if(cursor==2)
			return 1;
	}

	if(PointInRect(msx,msy,230,440,230+180,440+19))
	{
		cursor=1;
	}
	if(PointInRect(msx,msy,20,440,20+180,440+19))
	{
		cursor=0;
	}
	if(PointInRect(msx,msy,440,440,440+180,440+19))
	{
		cursor=2;
	}

	if(mgl->MouseTap())
	{
		if(PointInRect(msx,msy,230,440,230+180,440+19))
		{
			dispMode=1-dispMode;
		}
		if(PointInRect(msx,msy,20,440,20+180,440+19))
		{
			if(profile.progress.totalCoins-profile.progress.coinsSpent<25)
				MakeNormalSound(SND_TURRETBZZT);
			else
			{
				profile.progress.coinsSpent+=25;
				return 2;	// 2 means I want to play
			}
		}
		if(PointInRect(msx,msy,440,440,440+180,440+19))
		{
			return 1;	// 1 means exit
		}
	}
	if(k==27)
		return 1;	// exit

	return 0;
}

void RenderArcButton(int x,int y,int wid,const char *txt,MGLDraw *mgl,byte on)
{
	if(on || PointInRect(msx,msy,x,y,x+wid,y+19))
	{
		mgl->Box(x,y,x+wid,y+19,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+19-1,32+8);
	}
	else
		mgl->Box(x,y,x+wid,y+19,32+16);

	PrintGlow(x+2,y+3,txt,0,2);
}

void RenderArcade(MGLDraw *mgl)
{
	int i;
	int msx2,msy2;
	static char gameDesc[3][1024]={
		"Face off against an endless horde of aliens in this disturbingly "
		"fast paced space battle!  There's no time to think, but at least "
		"try not to shoot the power-ups - you need them!  Watch out, because "
		"every once in a while, the alien mother ship will arrive.  Destroy "
		"it quickly, because it launches ships like crazy!  You have a limited "
		"supply of Super Bombs, use them sparingly when you need to save yourself. "
		"Button 1 fires, button 2 uses a Super Bomb.  Steer your ship with the "
		"joystick or arrows.",

		"Dig Man is a greedy little man... so collect all the diamonds you "
		"can!  On each level, you must collect all the diamonds and return to "
		"the surface.  If an ant eats you, or you fall in lava, or you run out "
		"of time, you lose one try and must start the level over.  But you can "
		"fight back with your high-pressure water cannon!  Each diamond you "
		"collect increases the water pressure.  Use it to excavate, "
		"wash away ants, and even cool off the lava!  You can rack up higher scores "
		"by doing more digging, defeating Ant Queens, and blasting lava, but don't "
		"go too crazy - you want to get out alive with your diamonds, right? "
		"Button 1 is your water cannon, button 2 is jump.  You can rotate the water "
		"cannon with the joystick or arrows.",

		"For some entirely unknown reason, you feel the need to rush through a cave "
		"at extremely high speed in your high-tech rocket ship!  How long will you "
		"last before you hit the walls?  The cave gets much more dangerous the further "
		"you go, and due to a malfunction in your engine, you gradually go faster too! "
		"Steer left and right with the arrow keys.  Press the up arrow to accelerate, "
		"and the down arrow to brake.  You earn more points the faster you go!",
	};
	static char gameNotes[3][1024]={
		"This game was originally made as the Super Secret Bonus Extra in Loonyland, "
		"added in the patch.  So now you know what that is if you were always dying to "
		"find out!  You'll still have to figure out how to access it yourself. But in "
		"this game, it's easy - just pay 25c! The original version in Loonyland was much "
		"harder than what you're playing now. Seriously! It was really ridiculous.  There "
		"aren't a lot of good tips I can offer you on this game... it's really hard to collect "
		"powerups, but they're really useful. Don't forget about your Super Bombs - If "
		"you are feeling swamped, you can clear out a lot of space with them.  They also rake "
		"in the points, and therefore the powerups.  If the enemies appear to be moving in slow "
		"motion, that's because they are.  Imagine how hard the Loonyland version was!",

		"This is an original creation, just for Supreme!  It started out as just the basic "
		"idea of a character who could tunnel freely through dirt.  The end result is really "
		"odd for a game, and almost trivially easy most of the time.  But the reason I didn't "
		"change it any further is that I didn't want to lose that beautiful and mesmerizing "
		"lava effect.  Now that is cool.  And it is fun to run around grabbing the diamonds. It's "
		"more fun, but dangerous, to douse the lava.  This game is about greed - it's extremely "
		"easy for the first few levels if you just grab diamonds and leave, but you can "
		"get an exponentially higher score if you use up your remaining time dousing fires and "
		"ants.  So you have to balance your greed for points (and coins!) against your survival. "
		"I hope that ends up being an interesting challenge for you!  Enjoy!",

		"The arcade was looking a little sparse, and I was feeling a little guilty that I had "
		"to release a patch with nothing in it but one serious bug fix.  So I thought I'd throw "
		"in a new arcade game!  Not a lot to say about this one, it's a really basic game, but "
		"it's probably the most fun of the 3 games.  It gets very intense once you get moving!  "
		"One thing I really like about it is that the skill doesn't come in with moving left and "
		"right - that part's easy.  The skill is applying your brakes quickly and properly, and "
		"returning to speed when its safe, so your score climbs quickly before the cave gets too "
		"lethal.  Good luck!",
	};

	char s[20];

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	switch(game)
	{
		case 0:
			PrintGlow(20,20,"ZAPPO RAIDERS",0,2);
			plSpr->GetSprite(9)->Draw(200,100,mgl);
			break;
		case 1:
			PrintGlow(20,20,"DIG MAN IV",0,2);
			plSpr->GetSprite(8)->Draw(210,100,mgl);
			break;
		case 2:
			PrintGlow(20,20,"CAVE ZOOMER",0,2);
			plSpr->GetSprite(9)->Draw(210,100,mgl);
			break;
	}

	RenderArcButton(20,440,180,"Insert 25c To Play",mgl,(cursor==0));
	if(dispMode==0)
	{
		PrintGlow(500,20,"How To Play",0,2);
		PrintGlowRect(230,40,619,459,18,gameDesc[game],2);
		RenderArcButton(230,440,180,"Designer's Notes",mgl,(cursor==1));
	}
	else
	{
		PrintGlow(460,20,"Designer's Notes",0,2);
		PrintGlowRect(230,40,619,459,18,gameNotes[game],2);
		RenderArcButton(230,440,180,"Game Rules",mgl,(cursor==1));
	}
	RenderArcButton(440,440,180,"Quit",mgl,(cursor==2));

	sprintf(s,"Coins: %lu",profile.progress.totalCoins-profile.progress.coinsSpent);
	PrintGlow(620-GetStrLength(s,2),415,s,0,2);

	// mouse cursor
	SetSpriteConstraints(13,13,627,467);
	msx2=msx;
	msy2=msy;
	if(msx2<13)
		msx2=13;
	if(msy2<13)
		msy2=13;
	if(msx2>622)
		msx2=622;
	if(msy2>462)
		msy2=462;
	plSpr->GetSprite(0)->DrawBright(msx2,msy2,mgl,msBright/2);
	SetSpriteConstraints(0,0,639,479);
}

void Arcade(MGLDraw *mgl, byte gm)
{
	byte done=0;
	int lastTime=1;

	game=gm;

	InitArcade(mgl);

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateArcade(&lastTime,mgl);
		RenderArcade(mgl);

		mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();

		if(done==2)
		{
			switch(game)
			{
				case 0:
					SpaceGame(mgl);
					break;
				case 1:
					CaveGame(mgl);
					break;
				case 2:
					RaceGame(mgl);
					break;
			}
			done=0;
			GetTaps();
		}
	}

	SaveProfile();
	ExitArcade();
	GoalPurchase();
}
