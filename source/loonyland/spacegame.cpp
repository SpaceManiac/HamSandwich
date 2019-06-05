#include "spacegame.h"

#define NUMSTARS	256
#define NUMBULLETS	512
#define NUMBADGUYS	128

typedef struct spacePlayer_t
{
	int x,y;
	int dx,dy;
	byte face;
	byte reload,reload2;
	byte bombs;
	int powerup[8];
	byte comboClock;
	byte combo;
	byte invinc;
} spacePlayer_t;

typedef struct spaceBullet_t
{
	int x,y;
	int dx,dy;
	byte clock;
	byte type;
	byte face;
} spaceBullet_t;

typedef struct spaceBadguy_t
{
	int x,y;
	int dx,dy;
	byte type;
	byte face;
	byte hp;
	byte timer;
} spaceBadguy_t;

// power ups
#define PU_BOMB		0
#define PU_RAPID	1
#define PU_EXPLODE	2
#define PU_TRIPLE	3
#define PU_REAR		4
#define PU_SHIELD	5
#define PU_WINGMAN	6
#define PU_LIFE		7

// space game objects
#define SO_PLAYER	1
#define SO_PBULLET  2
#define SO_EXHAUST	3
#define SO_UFO		4
#define SO_DEBRIS	5
#define SO_POWERUP	6
#define SO_WINGMAN	7
#define SO_MINI		8
#define SO_MAXI		9
#define SO_MOTHER	10
#define SO_DIVER	11
#define SO_EBULLET	12

// bullet types
#undef SB_NONE
#define SB_NONE		0
#define SB_EXHAUST	1	// player ship exhaust
#define SB_PBULLET	2	// player bullet
#define SB_DEBRIS	3	// debris of enemy ship
#define SB_PBULLET2	4	// just like normal bullets, except they can't explode with the explode powerup
#define SB_DEBRIS2	5	// white debris, for player
#define SB_DEBRIS3	6	// red debris, for Mini
#define SB_DEBRIS4	7	// other debris colors...
#define SB_DEBRIS5	8
#define SB_DEBRIS6	9
#define SB_EBULLET	10	// enemy bullet
#define SB_EXHAUST2	11	// red exhaust for enemies

// badguy types
#define SG_NONE		0
#define SG_UFO		1
#define SG_POWERUP	2
#define SG_WINGMAN	3
#define SG_MINI		4
#define SG_MAXI		5
#define SG_MOTHER	6
#define SG_DIVER	7

static byte oldc;

static int starX[NUMSTARS],starY[NUMSTARS],starCol[NUMSTARS];
static int score,curScore,lives,nextLev;
static byte gameOver;
static int totalTime;
static int addTime;

static spacePlayer_t spcPlyr;
static spaceBullet_t spcBullet[NUMBULLETS];
static spaceBadguy_t spcBadguy[NUMBADGUYS];
static byte gameLevel;
static char msg[32];
static byte msgTimer;
static byte numBadguys;

void InitSpaceGame(MGLDraw *mgl)
{
	int i;

	for(i=0;i<NUMSTARS;i++)
	{
		starX[i]=Random(640)*FIXAMT;
		starY[i]=Random(480)*FIXAMT;
		starCol[i]=Random(32);
	}
	score=0;
	curScore=0;
	lives=3;
	gameLevel=0;
	msgTimer=0;

	mgl->LastKeyPressed();
	spcPlyr.x=320*FIXAMT;
	spcPlyr.y=240*FIXAMT;
	spcPlyr.dx=0;
	spcPlyr.dy=0;
	spcPlyr.face=192;
	spcPlyr.reload=5;
	spcPlyr.reload2=5;
	spcPlyr.bombs=3;
	spcPlyr.invinc=60;
	for(i=0;i<8;i++)
		spcPlyr.powerup[i]=0;
	spcPlyr.powerup[PU_SHIELD]=200;
	spcPlyr.combo=0;
	spcPlyr.comboClock=0;
	nextLev=50;	// need 50 points to go up a level
	numBadguys=0;
	gameOver=0;

	for(i=0;i<NUMBULLETS;i++)
	{
		spcBullet[i].type=SB_NONE;
	}
	for(i=0;i<NUMBADGUYS;i++)
	{
		spcBadguy[i].type=SG_NONE;
	}
	for(i=0;i<5;i++)
	{
		addTime=0;
		SpaceAddBadguys();
	}
	addTime=30;
}

void ExitSpaceGame(void)
{
}

void SpaceMessage(const char *s)
{
	strcpy(msg,s);
	msgTimer=120;
}

void RenderSpaceObject(MGLDraw *mgl,byte obj,byte face,byte xtra,int x,int y)
{
	int x2,y2,x3,y3,x4,y4;

	switch(obj)
	{
		case SO_PLAYER:
			if(spcPlyr.powerup[PU_SHIELD])
				RenderCircleParticle(x,y,Random(22)+2,3*32+16,mgl->GetScreen());

			x2=x+(Cosine(face)*18)/FIXAMT;
			y2=y+(Sine(face)*18)/FIXAMT;
			x3=x+(Cosine(face+96)*8)/FIXAMT;
			y3=y+(Sine(face+96)*8)/FIXAMT;
			x4=x+(Cosine(face-96)*8)/FIXAMT;
			y4=y+(Sine(face-96)*8)/FIXAMT;
			if(!spcPlyr.invinc)
				xtra=31;
			else
				xtra=(byte)Random(256);
			DrawLine(x2,y2,x3,y3,xtra);
			DrawLine(x3,y3,x,y,xtra);
			DrawLine(x,y,x4,y4,xtra);
			DrawLine(x4,y4,x2,y2,xtra);
			break;
		case SO_PBULLET:
			DrawLine(x,y,x+(Cosine(face+128)*8)/FIXAMT,y+(Sine(face+128)*8)/FIXAMT,6*32+30-xtra);
			break;
		case SO_EBULLET:
			if(xtra>20)
				xtra=20;
			DrawLine(x,y,x+(Cosine(face+128)*8)/FIXAMT,y+(Sine(face+128)*8)/FIXAMT,4*32+30-xtra);
			break;
		case SO_EXHAUST:
			if(x>=0 && x<SCRWID && y>=0 && y<SCRHEI)
				mgl->GetScreen()[x+y*mgl->GetWidth()]=face;
			break;
		case SO_UFO:
			DrawLine(x-10,y-5,x+10,y-5,xtra);
			DrawLine(x+10,y-5,x+15,y,xtra);
			DrawLine(x+15,y,x+10,y+5,xtra);
			DrawLine(x+10,y+5,x-10,y+5,xtra);
			DrawLine(x-10,y+5,x-15,y,xtra);
			DrawLine(x-15,y,x-10,y-5,xtra);
			DrawLine(x-15,y,x+15,y,xtra);
			break;
		case SO_DEBRIS:
			x2=x+(Cosine(face)*8)/FIXAMT;
			y2=y+(Sine(face)*8)/FIXAMT;
			DrawLine(x,y,x2,y2,xtra);
			break;
		case SO_POWERUP:
			RenderCircleParticle(x,y,Random(10)+2,face*32+16,mgl->GetScreen());
			x-=3;
			if(Random(2))
				x2=31;
			else
				x2=0;
			DrawLine(x,y+6,x,y-6,x2);
			DrawLine(x,y-6,x+4,y-6,x2);
			DrawLine(x+4,y-6,x+6,y-4,x2);
			DrawLine(x+6,y-4,x+6,y,x2);
			DrawLine(x+6,y,x+4,y+2,x2);
			DrawLine(x+4,y+2,x,y+2,x2);
			break;
		case SO_WINGMAN:
			x2=x+(Cosine(face)*9)/FIXAMT;
			y2=y+(Sine(face)*9)/FIXAMT;
			x3=x+(Cosine(face+96)*4)/FIXAMT;
			y3=y+(Sine(face+96)*4)/FIXAMT;
			x4=x+(Cosine(face-96)*4)/FIXAMT;
			y4=y+(Sine(face-96)*4)/FIXAMT;
			DrawLine(x2,y2,x3,y3,31);
			DrawLine(x3,y3,x,y,31);
			DrawLine(x,y,x4,y4,31);
			DrawLine(x4,y4,x2,y2,31);
			break;
		case SO_MINI:
			DrawLine(x-5,y-2,x+5,y-2,xtra);
			DrawLine(x+5,y-2,x+7,y,xtra);
			DrawLine(x+7,y,x+5,y+2,xtra);
			DrawLine(x+5,y+2,x-5,y+2,xtra);
			DrawLine(x-5,y+2,x-7,y,xtra);
			DrawLine(x-7,y,x-5,y-2,xtra);
			DrawLine(x-7,y,x+7,y,xtra);
			break;
		case SO_MAXI:
			DrawLine(x-15,y-15,x+15,y-15,xtra);
			DrawLine(x+15,y-15,x+20,y,xtra);
			DrawLine(x+20,y,x+15,y+15,xtra);
			DrawLine(x+15,y+15,x-15,y+15,xtra);
			DrawLine(x-15,y+15,x-20,y,xtra);
			DrawLine(x-20,y,x-15,y-15,xtra);
			DrawLine(x-20,y,x+20,y,xtra);
			break;
		case SO_MOTHER:
			DrawLine(x-40,y-5,x+40,y-5,xtra);
			DrawLine(x-40,y+5,x+40,y+5,xtra);
			DrawLine(x-40,y-5,x-20,y-20,xtra);
			DrawLine(x-40,y+5,x-20,y+20,xtra);
			DrawLine(x-20,y-20,x+20,y-20,xtra);
			DrawLine(x-20,y+20,x+20,y+20,xtra);
			DrawLine(x+20,y-20,x+40,y-5,xtra);
			DrawLine(x+20,y+20,x+40,y+5,xtra);
			DrawLine(x-10,y-5,x-10,y+5,xtra);
			DrawLine(x+10,y-5,x+10,y+5,xtra);
			break;
		case SO_DIVER:
			DrawLine(x,y-10,x+10,y,xtra);
			DrawLine(x+10,y,x,y+10,xtra);
			DrawLine(x,y+10,x-10,y,xtra);
			DrawLine(x-10,y,x,y-10,xtra);
			DrawLine(x,y-10,x,y+10,xtra);
			break;
	}
}

void RenderPowerUpLevel(MGLDraw *mgl,int x,int y,byte amt,byte c)
{
	int x2;

	RenderCircleParticle(x,y,amt,c*32+16,mgl->GetScreen());
	x-=3;
	if(Random(2))
		x2=31;
	else
		x2=0;
	DrawLine(x,y+6,x,y-6,x2);
	DrawLine(x,y-6,x+4,y-6,x2);
	DrawLine(x+4,y-6,x+6,y-4,x2);
	DrawLine(x+6,y-4,x+6,y,x2);
	DrawLine(x+6,y,x+4,y+2,x2);
	DrawLine(x+4,y+2,x,y+2,x2);
}

void RenderSpaceBullet(MGLDraw *mgl,int n)
{
	switch(spcBullet[n].type)
	{
		case SB_PBULLET:
		case SB_PBULLET2:
			RenderSpaceObject(mgl,SO_PBULLET,spcBullet[n].face,(30-spcBullet[n].clock),spcBullet[n].x/FIXAMT,spcBullet[n].y/FIXAMT);
			break;
		case SB_EBULLET:
			RenderSpaceObject(mgl,SO_EBULLET,spcBullet[n].face,(30-spcBullet[n].clock),spcBullet[n].x/FIXAMT,spcBullet[n].y/FIXAMT);
			break;
		case SB_EXHAUST:
			RenderSpaceObject(mgl,SO_EXHAUST,spcBullet[n].face,0,spcBullet[n].x/FIXAMT,spcBullet[n].y/FIXAMT);
			break;
		case SB_EXHAUST2:
			RenderSpaceObject(mgl,SO_EXHAUST,spcBullet[n].face,0,spcBullet[n].x/FIXAMT,spcBullet[n].y/FIXAMT);
			break;
		case SB_DEBRIS:
			RenderSpaceObject(mgl,SO_DEBRIS,spcBullet[n].face,3*32+spcBullet[n].clock,spcBullet[n].x/FIXAMT,spcBullet[n].y/FIXAMT);
			break;
		case SB_DEBRIS2:
			RenderSpaceObject(mgl,SO_DEBRIS,spcBullet[n].face,spcBullet[n].clock,spcBullet[n].x/FIXAMT,spcBullet[n].y/FIXAMT);
			break;
		case SB_DEBRIS3:
			RenderSpaceObject(mgl,SO_DEBRIS,spcBullet[n].face,4*32+spcBullet[n].clock,spcBullet[n].x/FIXAMT,spcBullet[n].y/FIXAMT);
			break;
		case SB_DEBRIS4:
			RenderSpaceObject(mgl,SO_DEBRIS,spcBullet[n].face,5*32+spcBullet[n].clock,spcBullet[n].x/FIXAMT,spcBullet[n].y/FIXAMT);
			break;
		case SB_DEBRIS5:
			RenderSpaceObject(mgl,SO_DEBRIS,spcBullet[n].face,6*32+spcBullet[n].clock,spcBullet[n].x/FIXAMT,spcBullet[n].y/FIXAMT);
			break;
		case SB_DEBRIS6:
			RenderSpaceObject(mgl,SO_DEBRIS,spcBullet[n].face,7*32+spcBullet[n].clock,spcBullet[n].x/FIXAMT,spcBullet[n].y/FIXAMT);
			break;
	}
}

void RenderSpaceBadguy(MGLDraw *mgl,int n)
{
	switch(spcBadguy[n].type)
	{
		case SG_UFO:
			RenderSpaceObject(mgl,SO_UFO,0,3*32+16,spcBadguy[n].x/FIXAMT,spcBadguy[n].y/FIXAMT);
			break;
		case SG_MINI:
			RenderSpaceObject(mgl,SO_MINI,0,4*32+16,spcBadguy[n].x/FIXAMT,spcBadguy[n].y/FIXAMT);
			break;
		case SG_MAXI:
			RenderSpaceObject(mgl,SO_MAXI,0,5*32+16,spcBadguy[n].x/FIXAMT,spcBadguy[n].y/FIXAMT);
			break;
		case SG_MOTHER:
			RenderSpaceObject(mgl,SO_MOTHER,0,6*32+16,spcBadguy[n].x/FIXAMT,spcBadguy[n].y/FIXAMT);
			break;
		case SG_DIVER:
			RenderSpaceObject(mgl,SO_DIVER,0,7*32+16,spcBadguy[n].x/FIXAMT,spcBadguy[n].y/FIXAMT);
			break;
		case SG_POWERUP:
			RenderSpaceObject(mgl,SO_POWERUP,spcBadguy[n].face,0,spcBadguy[n].x/FIXAMT,spcBadguy[n].y/FIXAMT);
			break;
		case SG_WINGMAN:
			RenderSpaceObject(mgl,SO_WINGMAN,spcBadguy[n].face,0,spcBadguy[n].x/FIXAMT,spcBadguy[n].y/FIXAMT);
			break;
	}
}

void RenderSpaceGame(MGLDraw *mgl)
{
	int i;
	byte *scrn;
	int pitch;
	char s[32];

	mgl->ClearScreen();
	pitch=mgl->GetWidth();
	scrn=mgl->GetScreen();

	for(i=0;i<NUMSTARS;i++)
	{
		scrn[(starX[i]/FIXAMT)+(starY[i]/FIXAMT)*pitch]=starCol[i];
	}

	if(gameOver)
	{
		CenterPrint(320,100,"GAME OVER!",0,2);
		sprintf(s,"Survived for %d:%02d",totalTime/(30*60),(totalTime/30)%60);
		CenterPrint(320,150,s,0,1);
		sprintf(s,"Total Score: %d",score);
		CenterPrint(320,180,s,0,1);
	}
	else
	{
		for(i=0;i<NUMBULLETS;i++)
			if(spcBullet[i].type)
				RenderSpaceBullet(mgl,i);
		for(i=0;i<NUMBADGUYS;i++)
			if(spcBadguy[i].type)
				RenderSpaceBadguy(mgl,i);

		RenderSpaceObject(mgl,SO_PLAYER,spcPlyr.face,0,spcPlyr.x/FIXAMT,spcPlyr.y/FIXAMT);

		// interface
		sprintf(s,"Score: %08d",curScore);
		PrintGlow(2,2,s,0,1);

		sprintf(s,"Bombs: %d",spcPlyr.bombs);
		PrintGlow(150,2,s,0,1);

		sprintf(s,"Level: %d",gameLevel+1);
		PrintGlow(300,2,s,0,1);

		sprintf(s,"Lives: %d",lives);
		RightPrintGlow(638,2,s,0,1);

		if(spcPlyr.combo>1)
		{
			sprintf(s,"Combo x%d",spcPlyr.combo);
			PrintGlow(2,32,s,0,1);
		}

		for(i=0;i<8;i++)
		{
			if(spcPlyr.powerup[i])
			{
				RenderPowerUpLevel(mgl,620,40+30*i,spcPlyr.powerup[i]/32,i);
			}
		}
	}
	if(msgTimer)
		Print(2,460,msg,0,1);
}

void SpaceFireBullet(byte type,byte face,int x,int y)
{
	int i,p;

	for(i=0;i<NUMBULLETS;i++)
	{
		if(spcBullet[i].type==SB_NONE)
		{
			spcBullet[i].type=type;
			spcBullet[i].x=x;
			spcBullet[i].y=y;
			switch(spcBullet[i].type)
			{
				case SB_EXHAUST:
					spcBullet[i].dx=Cosine(face+128-8+Random(17));
					spcBullet[i].dy=Sine(face+128-8+Random(17));
					spcBullet[i].clock=15;
					spcBullet[i].face=6*32+31;
					break;
				case SB_EXHAUST2:
					spcBullet[i].dx=Cosine(face+128-8+Random(17));
					spcBullet[i].dy=Sine(face+128-8+Random(17));
					spcBullet[i].clock=15;
					spcBullet[i].face=4*32+31;
					break;
				case SB_PBULLET:
				case SB_PBULLET2:
					spcBullet[i].dx=Cosine(face-4+Random(9))*12;
					spcBullet[i].dy=Sine(face-4+Random(9))*12;
					spcBullet[i].clock=30;
					spcBullet[i].face=face;
					break;
				case SB_EBULLET:
					p=4+(gameLevel/4);
					if(p>16)
						p=16;
					spcBullet[i].dx=Cosine(face-4+Random(9))*p;
					spcBullet[i].dy=Sine(face-4+Random(9))*p;
					spcBullet[i].clock=30;
					spcBullet[i].face=face;
					break;
				case SB_DEBRIS:
				case SB_DEBRIS2:
				case SB_DEBRIS3:
				case SB_DEBRIS4:
				case SB_DEBRIS5:
				case SB_DEBRIS6:
					spcBullet[i].dx=-4*FIXAMT+Random(9*FIXAMT);
					spcBullet[i].dy=-4*FIXAMT+Random(9*FIXAMT);
					spcBullet[i].clock=20;
					spcBullet[i].face=face;
					break;
			}
			return;
		}
	}
}

byte CheckRect(int x,int y,int x2,int y2,int ax,int ay,int ax2,int ay2)
{
	return (x<=ax2 && x2>=ax && y<=ay2 && y2>=ay);
}

void BadguyHit(int me)
{
	int i;

	spcBadguy[me].hp--;
	if(spcBadguy[me].hp==0)
	{
		switch(spcBadguy[me].type)
		{
			case SG_POWERUP:
				MakeSpaceSound(SND_BOOM,800);
				for(i=0;i<8;i++)
					SpaceFireBullet(SB_DEBRIS3,(byte)Random(256),spcBadguy[me].x,spcBadguy[me].y);
				break;
			case SG_UFO:
				MakeSpaceSound(SND_BOOM,800);
				for(i=0;i<8;i++)
					SpaceFireBullet(SB_DEBRIS,(byte)Random(256),spcBadguy[me].x,spcBadguy[me].y);

				if(spcPlyr.combo<255)
					spcPlyr.combo++;
				spcPlyr.comboClock=60;
				score+=5*spcPlyr.combo;
				break;
			case SG_MINI:
				MakeSpaceSound(SND_BOOM,800);
				for(i=0;i<8;i++)
					SpaceFireBullet(SB_DEBRIS3,(byte)Random(256),spcBadguy[me].x,spcBadguy[me].y);

				if(spcPlyr.combo<255)
					spcPlyr.combo++;
				spcPlyr.comboClock=60;
				score+=10*spcPlyr.combo;
				break;
			case SG_MAXI:
				MakeSpaceSound(SND_BIGBOOM,800);
				for(i=0;i<32;i++)
					SpaceFireBullet(SB_DEBRIS4,(byte)Random(256),spcBadguy[me].x,spcBadguy[me].y);

				if(spcPlyr.combo<255)
					spcPlyr.combo++;
				spcPlyr.comboClock=60;
				score+=30*spcPlyr.combo;
				break;
			case SG_DIVER:
				MakeSpaceSound(SND_BOOM,800);
				for(i=0;i<8;i++)
					SpaceFireBullet(SB_DEBRIS6,(byte)Random(256),spcBadguy[me].x,spcBadguy[me].y);

				if(spcPlyr.combo<255)
					spcPlyr.combo++;
				spcPlyr.comboClock=60;
				score+=15*spcPlyr.combo;
				break;
			case SG_MOTHER:
				MakeSpaceSound(SND_BIGBOOM,800);
				MakeSpaceSound(SND_BIGBOOM,800);
				MakeSpaceSound(SND_BIGBOOM,800);
				for(i=0;i<256;i++)
					SpaceFireBullet(SB_DEBRIS5,(byte)Random(256),
						spcBadguy[me].x-20*FIXAMT+Random(41*FIXAMT),
						spcBadguy[me].y-12*FIXAMT+Random(25*FIXAMT));

				if(spcPlyr.combo<255)
					spcPlyr.combo++;
				spcPlyr.comboClock=60;
				score+=1000;
				break;
		}

		spcBadguy[me].type=0;
	}
	else
		MakeSpaceSound(SND_METALSMACK,400);
}

void BulletPop(int me)
{
	int i,amt;

	switch(spcBullet[me].type)
	{
		case SB_PBULLET:
		case SB_PBULLET2:
			amt=8;

			for(i=0;i<amt;i++)
			{
				SpaceFireBullet(SB_EXHAUST,(byte)Random(256),spcBullet[me].x,spcBullet[me].y);
			}
			if(spcPlyr.powerup[PU_EXPLODE] && spcBullet[me].type==SB_PBULLET)
			{
				for(i=0;i<4;i++)
				{
					SpaceFireBullet(SB_PBULLET2,(byte)Random(256),spcBullet[me].x,spcBullet[me].y);
				}
			}
			spcBullet[me].type=SB_NONE;
			break;
		case SB_EBULLET:
			amt=8;

			for(i=0;i<amt;i++)
			{
				SpaceFireBullet(SB_EXHAUST2,(byte)Random(256),spcBullet[me].x,spcBullet[me].y);
			}
			spcBullet[me].type=SB_NONE;
			break;
	}
}

void SpaceBulletHitCheck(int me)
{
	int i;
	int mex,mey,mex2,mey2;

	if(spcBullet[me].type==SB_PBULLET || spcBullet[me].type==SB_PBULLET2)
	{
		mex=spcBullet[me].x-2*FIXAMT;
		mey=spcBullet[me].y-2*FIXAMT;
		mex2=spcBullet[me].x+2*FIXAMT;
		mey2=spcBullet[me].y+2*FIXAMT;
		for(i=0;i<NUMBADGUYS;i++)
		{
			if(spcBadguy[i].type)
			{
				if(spcBadguy[i].type==SG_UFO)
				{
					if(CheckRect(mex,mey,mex2,mey2,spcBadguy[i].x-15*FIXAMT,spcBadguy[i].y-5*FIXAMT,
						spcBadguy[i].x+15*FIXAMT,spcBadguy[i].y+5*FIXAMT))
					{
						BadguyHit(i);
						BulletPop(me);
					}
				}
				else if(spcBadguy[i].type==SG_MINI)
				{
					if(CheckRect(mex,mey,mex2,mey2,spcBadguy[i].x-7*FIXAMT,spcBadguy[i].y-2*FIXAMT,
						spcBadguy[i].x+7*FIXAMT,spcBadguy[i].y+2*FIXAMT))
					{
						BadguyHit(i);
						BulletPop(me);
					}
				}
				else if(spcBadguy[i].type==SG_MAXI)
				{
					if(CheckRect(mex,mey,mex2,mey2,spcBadguy[i].x-20*FIXAMT,spcBadguy[i].y-15*FIXAMT,
						spcBadguy[i].x+20*FIXAMT,spcBadguy[i].y+15*FIXAMT))
					{
						BadguyHit(i);
						BulletPop(me);
					}
				}
				else if(spcBadguy[i].type==SG_MOTHER)
				{
					if(CheckRect(mex,mey,mex2,mey2,spcBadguy[i].x-40*FIXAMT,spcBadguy[i].y-20*FIXAMT,
						spcBadguy[i].x+40*FIXAMT,spcBadguy[i].y+20*FIXAMT))
					{
						BadguyHit(i);
						BulletPop(me);
					}
				}
				else if(spcBadguy[i].type==SG_DIVER)
				{
					if(CheckRect(mex,mey,mex2,mey2,spcBadguy[i].x-10*FIXAMT,spcBadguy[i].y-10*FIXAMT,
						spcBadguy[i].x+10*FIXAMT,spcBadguy[i].y+10*FIXAMT))
					{
						BadguyHit(i);
						BulletPop(me);
					}
				}
				else if(spcBadguy[i].type==SG_POWERUP)
				{
					if(CheckRect(mex,mey,mex2,mey2,spcBadguy[i].x-8*FIXAMT,spcBadguy[i].y-8*FIXAMT,
						spcBadguy[i].x+8*FIXAMT,spcBadguy[i].y+8*FIXAMT))
					{
						BadguyHit(i);
						BulletPop(me);
					}
				}
				else if(spcBadguy[i].type==SG_WINGMAN)
				{
					// can't hurt them
				}
			}
		}
	}
	else if(spcBullet[me].type==SB_EBULLET)
	{
		mex=spcBullet[me].x-2*FIXAMT;
		mey=spcBullet[me].y-2*FIXAMT;
		mex2=spcBullet[me].x+2*FIXAMT;
		mey2=spcBullet[me].y+2*FIXAMT;

		if(CheckRect(mex,mey,mex2,mey2,spcPlyr.x-10*FIXAMT,spcPlyr.y-10*FIXAMT,
					 spcPlyr.x+10*FIXAMT,spcPlyr.y+10*FIXAMT))
		{
			SpaceGoodguyHit();
			BulletPop(me);
		}
	}
}

void LaunchPowerup(void)
{
	int i;

	for(i=0;i<NUMBADGUYS;i++)
	{
		if(spcBadguy[i].type==SG_NONE)
		{
			spcBadguy[i].type=SG_POWERUP;
			PutOnEdge(i);
			spcBadguy[i].hp=1;
			spcBadguy[i].face=(byte)Random(8);
			spcBadguy[i].dx=0;
			spcBadguy[i].dy=0;
			return;
		}
	}
}

void SpacePlayerShoot(void)
{
	MakeSpaceSound(SND_BATSHOOT,200);

	SpaceFireBullet(SB_PBULLET,spcPlyr.face,spcPlyr.x+Cosine(spcPlyr.face)*12,spcPlyr.y+Sine(spcPlyr.face)*12);

	if(spcPlyr.powerup[PU_TRIPLE])
	{
		SpaceFireBullet(SB_PBULLET,spcPlyr.face+32,spcPlyr.x+Cosine(spcPlyr.face+32)*12,spcPlyr.y+Sine(spcPlyr.face+32)*12);
		SpaceFireBullet(SB_PBULLET,spcPlyr.face-32,spcPlyr.x+Cosine(spcPlyr.face-32)*12,spcPlyr.y+Sine(spcPlyr.face-32)*12);
	}
	if(spcPlyr.powerup[PU_REAR])
	{
		SpaceFireBullet(SB_PBULLET,spcPlyr.face+128,spcPlyr.x+Cosine(spcPlyr.face+128)*12,spcPlyr.y+Sine(spcPlyr.face+128)*12);
	}

	if(spcPlyr.powerup[PU_RAPID])
		spcPlyr.reload=0;
	else
		spcPlyr.reload=4;
}

void RemoveWingman(void)
{
	int i;

	for(i=0;i<NUMBADGUYS;i++)
		if(spcBadguy[i].type==SG_WINGMAN)
			spcBadguy[i].type=SG_NONE;
}

byte SpaceUpdatePlayerShip(MGLDraw *mgl)
{
	byte c;
	int i;

	if(spcPlyr.reload)
		spcPlyr.reload--;
	if(spcPlyr.reload2)
		spcPlyr.reload2--;

	if(spcPlyr.comboClock)
		spcPlyr.comboClock--;
	else
		spcPlyr.combo=0;

	if(spcPlyr.invinc)
		spcPlyr.invinc--;

	spcPlyr.x+=spcPlyr.dx;
	spcPlyr.y+=spcPlyr.dy;
	if(spcPlyr.x<12*FIXAMT)
	{
		spcPlyr.x=12*FIXAMT;
		spcPlyr.dx=-spcPlyr.dx;
	}
	if(spcPlyr.x>(640-12)*FIXAMT)
	{
		spcPlyr.x=(640-12)*FIXAMT;
		spcPlyr.dx=-spcPlyr.dx;
	}
	if(spcPlyr.y<12*FIXAMT)
	{
		spcPlyr.y=12*FIXAMT;
		spcPlyr.dy=-spcPlyr.dy;
	}
	if(spcPlyr.y>(480-12)*FIXAMT)
	{
		spcPlyr.y=(480-12)*FIXAMT;
		spcPlyr.dy=-spcPlyr.dy;
	}

	c=GetControls();

	if(c&CONTROL_LF)
	{
		spcPlyr.face-=8;
	}
	if(c&CONTROL_RT)
	{
		spcPlyr.face+=8;
	}
	if(c&CONTROL_UP)
	{
		spcPlyr.dx+=Cosine(spcPlyr.face)/2;
		spcPlyr.dy+=Sine(spcPlyr.face)/2;
	}
	if(c&CONTROL_DN)
	{
		spcPlyr.dx-=Cosine(spcPlyr.face)/2;
		spcPlyr.dy-=Sine(spcPlyr.face)/2;
	}
	Dampen(&spcPlyr.dx,FIXAMT/8);
	Dampen(&spcPlyr.dy,FIXAMT/8);
	Clamp(&spcPlyr.dx,FIXAMT*14);
	Clamp(&spcPlyr.dy,FIXAMT*14);

	if(spcPlyr.dx!=0 || spcPlyr.dy!=0)
	{
		SpaceFireBullet(SB_EXHAUST,spcPlyr.face,spcPlyr.x+Cosine(spcPlyr.face+128)*4,spcPlyr.y+Sine(spcPlyr.face+128)*4);
		SpaceFireBullet(SB_EXHAUST,spcPlyr.face-4,spcPlyr.x+Cosine(spcPlyr.face+128-8)*4,spcPlyr.y+Sine(spcPlyr.face+128-8)*4);
		SpaceFireBullet(SB_EXHAUST,spcPlyr.face+4,spcPlyr.x+Cosine(spcPlyr.face+128+8)*4,spcPlyr.y+Sine(spcPlyr.face+128+8)*4);
	}
	if((c&CONTROL_B1) && spcPlyr.reload==0)
	{
		SpacePlayerShoot();
	}

	if((c&CONTROL_B2) && spcPlyr.reload2==0 && spcPlyr.bombs)
	{
		spcPlyr.bombs--;
		spcPlyr.reload2=15;
		spcPlyr.invinc=60;
		MakeSpaceSound(SND_BIGBOOM,2000);
		for(i=0;i<256;i++)
			SpaceFireBullet(SB_PBULLET,i,spcPlyr.x,spcPlyr.y);
	}

	if(curScore>=nextLev && gameLevel<100)
	{
		nextLev+=50;
		LaunchPowerup();
		gameLevel++;
	}

	for(i=0;i<8;i++)
	{
		if(spcPlyr.powerup[i] && i!=PU_SHIELD)
		{
			spcPlyr.powerup[i]--;
			if(i==PU_WINGMAN && spcPlyr.powerup[i]==0)
				RemoveWingman();
		}
	}
	oldc=c;

	c=mgl->LastKeyPressed();
	if(c==27)
		return 1;

	return 0;
}

void SpaceUpdateBullets(void)
{
	int i;

	for(i=0;i<NUMBULLETS;i++)
	{
		if(spcBullet[i].type)
		{
			spcBullet[i].x+=spcBullet[i].dx;
			spcBullet[i].y+=spcBullet[i].dy;
			if(spcBullet[i].x<-10*FIXAMT)
				spcBullet[i].type=SB_NONE;
			if(spcBullet[i].y<-10*FIXAMT)
				spcBullet[i].type=SB_NONE;
			if(spcBullet[i].x>650*FIXAMT)
				spcBullet[i].type=SB_NONE;
			if(spcBullet[i].y>650*FIXAMT)
				spcBullet[i].type=SB_NONE;

			spcBullet[i].clock--;
			if(spcBullet[i].clock==0)
				spcBullet[i].type=SB_NONE;

			switch(spcBullet[i].type)
			{
				case SB_PBULLET:
				case SB_PBULLET2:
				case SB_EBULLET:
					// hit badguys
					SpaceBulletHitCheck(i);
					break;
				case SB_EXHAUST:
				case SB_EXHAUST2:
					spcBullet[i].face-=2;
					break;
				case SB_DEBRIS:
				case SB_DEBRIS2:
				case SB_DEBRIS3:
				case SB_DEBRIS4:
				case SB_DEBRIS5:
				case SB_DEBRIS6:
					spcBullet[i].face+=12;
					break;
			}
		}
	}
}

void LaunchWingman(void)
{
	int i;

	for(i=0;i<NUMBADGUYS;i++)
		if(spcBadguy[i].type==SG_NONE)
		{
			spcBadguy[i].type=SG_WINGMAN;
			spcBadguy[i].x=spcPlyr.x-FIXAMT*20+Random(FIXAMT*41);
			spcBadguy[i].y=spcPlyr.y-FIXAMT*20+Random(FIXAMT*41);
			spcBadguy[i].dx=0;
			spcBadguy[i].dy=0;
			spcBadguy[i].face=(byte)Random(256);
			return;
		}
}

void SpacePowerUp(byte type)
{
	int i;

	if(type==PU_BOMB)
	{
		if(spcPlyr.bombs<9)
			spcPlyr.bombs++;
	}
	else if(type==PU_LIFE)
	{
		if(lives<9)
			lives++;
	}
	else
		spcPlyr.powerup[type]=30*20;

	if(type==PU_WINGMAN)
		LaunchWingman();

	for(i=0;i<32;i++)
		SpaceFireBullet(SB_EXHAUST,(byte)Random(256),spcPlyr.x,spcPlyr.y);

	MakeSpaceSound(SND_ZOMBIESDEAD,500);
	switch(type)
	{
		case PU_BOMB:
			SpaceMessage("Free Bomb!");
			break;
		case PU_LIFE:
			SpaceMessage("Free Life!");
			break;
		case PU_RAPID:
			SpaceMessage("Rapid Fire Cannon!");
			break;
		case PU_TRIPLE:
			SpaceMessage("Triple Blaster!");
			break;
		case PU_EXPLODE:
			SpaceMessage("Explosive Warheads!");
			break;
		case PU_REAR:
			SpaceMessage("Reverse Blaster!");
			break;
		case PU_WINGMAN:
			SpaceMessage("Crazed Wingman!");
			break;
		case PU_SHIELD:
			SpaceMessage("Energy Shield!");
			break;
	}
}

void SpaceGoodguyHit(void)
{
	int i;

	if(spcPlyr.invinc)
		return;

	if(spcPlyr.powerup[PU_SHIELD])
	{
		MakeSpaceSound(SND_ZAP,800);
		spcPlyr.powerup[PU_SHIELD]/=2;
		if(spcPlyr.powerup[PU_SHIELD]<100)
			spcPlyr.powerup[PU_SHIELD]=0;
		return;
	}
	else
	{
		MakeSpaceSound(SND_BIGBOOM,5000);
		for(i=0;i<12;i++)
			SpaceFireBullet(SB_DEBRIS2,(byte)Random(256),spcPlyr.x,spcPlyr.y);

		lives--;
		if(lives==0)
		{
			gameOver=1;
			GetDisplayMGL()->LastKeyPressed();
			oldc=255;
		}
		spcPlyr.x=320*FIXAMT;
		spcPlyr.y=240*FIXAMT;
		spcPlyr.dx=0;
		spcPlyr.dy=0;
		for(i=0;i<8;i++)
			spcPlyr.powerup[i]=0;
		spcPlyr.face=192;
		spcPlyr.combo=0;
		spcPlyr.comboClock=0;
		spcPlyr.reload=0;
		spcPlyr.reload2=10;
		spcPlyr.invinc=60;
		RemoveWingman();
	}
}

void SpaceUpdateBadguys(void)
{
	int i,p,a;

	for(i=0;i<NUMBADGUYS;i++)
	{
		if(spcBadguy[i].type)
		{
			spcBadguy[i].x+=spcBadguy[i].dx;
			spcBadguy[i].y+=spcBadguy[i].dy;
			if(spcBadguy[i].x<-60*FIXAMT)
				spcBadguy[i].type=SB_NONE;
			if(spcBadguy[i].y<-60*FIXAMT)
				spcBadguy[i].type=SB_NONE;
			if(spcBadguy[i].x>700*FIXAMT)
				spcBadguy[i].type=SB_NONE;
			if(spcBadguy[i].y>700*FIXAMT)
				spcBadguy[i].type=SB_NONE;

			switch(spcBadguy[i].type)
			{
				case SG_UFO:
					spcBadguy[i].timer--;
					if(spcBadguy[i].timer==0)
					{
						p=gameLevel*FIXAMT/4;
						if(p>FIXAMT*6)
							FIXAMT*6;
						if(p<FIXAMT/2)
							p=FIXAMT/2;
						a=Random(256);
						spcBadguy[i].dx=Cosine(a)*p/FIXAMT;
						spcBadguy[i].dy=Sine(a)*p/FIXAMT;
						spcBadguy[i].timer=30;
						MakeSpaceSound(SND_MUMMYSHOOT,300);
						SpaceFireBullet(SB_EBULLET,(byte)Random(256),spcBadguy[i].x,spcBadguy[i].y);
					}
					if(CheckRect(spcBadguy[i].x-FIXAMT*15,spcBadguy[i].y-FIXAMT*5,
								 spcBadguy[i].x+FIXAMT*15,spcBadguy[i].y+FIXAMT*5,
								 spcPlyr.x-FIXAMT*10,spcPlyr.y-FIXAMT*10,
								 spcPlyr.x+FIXAMT*10,spcPlyr.y+FIXAMT*10))
					{
						SpaceGoodguyHit();
						spcBadguy[i].hp=1;
						BadguyHit(i);
					}
					break;
				case SG_MINI:
					if(spcBadguy[i].timer)
						spcBadguy[i].timer--;
					else
					{
						spcBadguy[i].dx+=((-2+(int)Random(5))*FIXAMT);
						spcBadguy[i].dy+=((-2+(int)Random(5))*FIXAMT);
						p=gameLevel*FIXAMT;
						if(p>FIXAMT*10)
							FIXAMT*10;
						if(p<FIXAMT*2)
							p=FIXAMT*2;
						Clamp(&spcBadguy[i].dx,p);
						Clamp(&spcBadguy[i].dy,p);
					}
					if(CheckRect(spcBadguy[i].x-FIXAMT*7,spcBadguy[i].y-FIXAMT*2,
								 spcBadguy[i].x+FIXAMT*7,spcBadguy[i].y+FIXAMT*2,
								 spcPlyr.x-FIXAMT*10,spcPlyr.y-FIXAMT*10,
								 spcPlyr.x+FIXAMT*10,spcPlyr.y+FIXAMT*10))
					{
						SpaceGoodguyHit();
						spcBadguy[i].hp=1;
						BadguyHit(i);
					}

					SpaceFireBullet(SB_EXHAUST2,(byte)Random(256),spcBadguy[i].x,spcBadguy[i].y);
					break;
				case SG_MAXI:
					if(spcBadguy[i].x<spcPlyr.x)
						spcBadguy[i].dx+=FIXAMT/8;
					if(spcBadguy[i].x>spcPlyr.x)
						spcBadguy[i].dx-=FIXAMT/8;
					if(spcBadguy[i].y<spcPlyr.y)
						spcBadguy[i].dy+=FIXAMT/8;
					if(spcBadguy[i].y>spcPlyr.y)
						spcBadguy[i].dy-=FIXAMT/8;

					p=gameLevel*FIXAMT/8;
					if(p>FIXAMT*4)
						FIXAMT*4;
					if(p<FIXAMT*2)
						p=FIXAMT*2;
					Clamp(&spcBadguy[i].dx,p);
					Clamp(&spcBadguy[i].dy,p);
					if(spcBadguy[i].timer)
						spcBadguy[i].timer--;
					else
					{
						if(spcBadguy[i].x<spcPlyr.x)
						{
							if(spcBadguy[i].y<spcPlyr.y)
								p=0;
							else
								p=192;
						}
						else
						{
							if(spcBadguy[i].y<spcPlyr.y)
								p=64;
							else
								p=128;
						}
						MakeSpaceSound(SND_MUMMYSHOOT,300);
						SpaceFireBullet(SB_EBULLET,(byte)Random(64)+p,spcBadguy[i].x,spcBadguy[i].y);
						p=gameLevel/2;
						if(p>30)
							p=30;
						spcBadguy[i].timer=60-p;
					}
					if(CheckRect(spcBadguy[i].x-FIXAMT*20,spcBadguy[i].y-FIXAMT*15,
									 spcBadguy[i].x+FIXAMT*20,spcBadguy[i].y+FIXAMT*15,
									 spcPlyr.x-FIXAMT*10,spcPlyr.y-FIXAMT*10,
									 spcPlyr.x+FIXAMT*10,spcPlyr.y+FIXAMT*10))
					{
						SpaceGoodguyHit();
						spcBadguy[i].hp=1;
						BadguyHit(i);
					}
					break;
				case SG_DIVER:
					if(spcBadguy[i].x<spcPlyr.x)
						spcBadguy[i].dx+=FIXAMT/2;
					if(spcBadguy[i].x>spcPlyr.x)
						spcBadguy[i].dx-=FIXAMT/2;
					if(spcBadguy[i].y<spcPlyr.y)
						spcBadguy[i].dy+=FIXAMT/2;
					if(spcBadguy[i].y>spcPlyr.y)
						spcBadguy[i].dy-=FIXAMT/2;

					p=(gameLevel-20)*FIXAMT/2;
					if(p>FIXAMT*8)
						FIXAMT*8;
					if(p<FIXAMT*4)
						p=FIXAMT*4;
					Clamp(&spcBadguy[i].dx,p);
					Clamp(&spcBadguy[i].dy,p);
					if(CheckRect(spcBadguy[i].x-FIXAMT*10,spcBadguy[i].y-FIXAMT*10,
									 spcBadguy[i].x+FIXAMT*10,spcBadguy[i].y+FIXAMT*10,
									 spcPlyr.x-FIXAMT*10,spcPlyr.y-FIXAMT*10,
									 spcPlyr.x+FIXAMT*10,spcPlyr.y+FIXAMT*10))
					{
						SpaceGoodguyHit();
						spcBadguy[i].hp=1;
						BadguyHit(i);
					}
					break;
				case SG_MOTHER:
					if(spcBadguy[i].x<0*FIXAMT)
					{
						spcBadguy[i].x=0*FIXAMT;
						spcBadguy[i].dx=-spcBadguy[i].dx;
					}
					if(spcBadguy[i].x>640*FIXAMT)
					{
						spcBadguy[i].x=640*FIXAMT;
						spcBadguy[i].dx=-spcBadguy[i].dx;
					}
					if(spcBadguy[i].y<0*FIXAMT)
					{
						spcBadguy[i].y=0*FIXAMT;
						spcBadguy[i].dy=-spcBadguy[i].dy;
					}
					if(spcBadguy[i].y>480*FIXAMT)
					{
						spcBadguy[i].y=480*FIXAMT;
						spcBadguy[i].dy=-spcBadguy[i].dy;
					}
					spcBadguy[i].timer--;
					if(spcBadguy[i].timer==0)
					{
						a=Random(256);
						spcBadguy[i].dx=Cosine(a);
						spcBadguy[i].dy=Sine(a);
						spcBadguy[i].timer=30;
						MakeSpaceSound(SND_FRANKMISSILE,1000);
						LaunchBadguy(spcBadguy[i].x+FIXAMT*20,spcBadguy[i].y,FIXAMT*4);
						LaunchBadguy(spcBadguy[i].x-FIXAMT*20,spcBadguy[i].y,-FIXAMT*4);
					}
					if(CheckRect(spcBadguy[i].x-FIXAMT*40,spcBadguy[i].y-FIXAMT*20,
								 spcBadguy[i].x+FIXAMT*40,spcBadguy[i].y+FIXAMT*20,
								 spcPlyr.x-FIXAMT*10,spcPlyr.y-FIXAMT*10,
								 spcPlyr.x+FIXAMT*10,spcPlyr.y+FIXAMT*10))
					{
						SpaceGoodguyHit();
						BadguyHit(i);
					}
					break;
				case SG_POWERUP:
					if(spcBadguy[i].x<320*FIXAMT)
					{
						if(spcBadguy[i].y<240*FIXAMT)
						{
							spcBadguy[i].dx+=FIXAMT/2;
							spcBadguy[i].dy+=FIXAMT/4;
						}
						else
						{
							spcBadguy[i].dx+=FIXAMT/4;
							spcBadguy[i].dy-=FIXAMT/2;
						}
					}
					else
					{
						if(spcBadguy[i].y<240*FIXAMT)
						{
							spcBadguy[i].dx-=FIXAMT/4;
							spcBadguy[i].dy+=FIXAMT/2;
						}
						else
						{
							spcBadguy[i].dx-=FIXAMT/2;
							spcBadguy[i].dy-=FIXAMT/4;
						}
					}
					Clamp(&spcBadguy[i].dx,FIXAMT*10);
					Clamp(&spcBadguy[i].dy,FIXAMT*10);
					if(CheckRect(spcBadguy[i].x-FIXAMT*10,spcBadguy[i].y-FIXAMT*10,
								 spcBadguy[i].x+FIXAMT*10,spcBadguy[i].y+FIXAMT*10,
								 spcPlyr.x-FIXAMT*10,spcPlyr.y-FIXAMT*10,
								 spcPlyr.x+FIXAMT*10,spcPlyr.y+FIXAMT*10))
					{
						SpacePowerUp(spcBadguy[i].face);
						spcBadguy[i].type=SG_NONE;
					}
					break;
				case SG_WINGMAN:
					if(spcBadguy[i].x<0*FIXAMT)
					{
						spcBadguy[i].x=0*FIXAMT;
						spcBadguy[i].dx=-spcBadguy[i].dx;
					}
					if(spcBadguy[i].x>640*FIXAMT)
					{
						spcBadguy[i].x=640*FIXAMT;
						spcBadguy[i].dx=-spcBadguy[i].dx;
					}
					if(spcBadguy[i].y<0*FIXAMT)
					{
						spcBadguy[i].y=0*FIXAMT;
						spcBadguy[i].dy=-spcBadguy[i].dy;
					}
					if(spcBadguy[i].y>480*FIXAMT)
					{
						spcBadguy[i].y=480*FIXAMT;
						spcBadguy[i].dy=-spcBadguy[i].dy;
					}
					if(spcBadguy[i].x<spcPlyr.x)
						spcBadguy[i].dx+=FIXAMT/8;
					if(spcBadguy[i].x>spcPlyr.x)
						spcBadguy[i].dx-=FIXAMT/8;
					if(spcBadguy[i].y<spcPlyr.y)
						spcBadguy[i].dy+=FIXAMT/8;
					if(spcBadguy[i].y>spcPlyr.y)
						spcBadguy[i].dy-=FIXAMT/8;
					Clamp(&spcBadguy[i].dx,FIXAMT*8);
					Clamp(&spcBadguy[i].dy,FIXAMT*8);
					spcBadguy[i].face+=8;
					if(Random(6)==0)
						SpaceFireBullet(SB_PBULLET2,spcBadguy[i].face,spcBadguy[i].x,spcBadguy[i].y);
					SpaceFireBullet(SB_EXHAUST,spcBadguy[i].face,spcBadguy[i].x,spcBadguy[i].y);
					break;
			}
		}
	}
}

void PutOnEdge(int i)
{
	byte e;

	e=(byte)Random(4);
	switch(e)
	{
		case 0:
			spcBadguy[i].x=-20*FIXAMT;
			spcBadguy[i].y=Random(480)*FIXAMT;
			spcBadguy[i].dx=FIXAMT*4;
			spcBadguy[i].dy=0;
			break;
		case 1:
			spcBadguy[i].x=(640+20)*FIXAMT;
			spcBadguy[i].y=Random(480)*FIXAMT;
			spcBadguy[i].dx=-FIXAMT*4;
			spcBadguy[i].dy=0;
			break;
		case 2:
			spcBadguy[i].x=Random(640)*FIXAMT;
			spcBadguy[i].y=-20*FIXAMT;
			spcBadguy[i].dx=0;
			spcBadguy[i].dy=FIXAMT*4;
			break;
		case 3:
			spcBadguy[i].x=Random(640)*FIXAMT;
			spcBadguy[i].y=(480+20)*FIXAMT;
			spcBadguy[i].dx=0;
			spcBadguy[i].dy=-FIXAMT*4;
			break;
	}
}

void LaunchBadguy(int x,int y,int dx)
{
	int i;
	byte t;

	t=(byte)Random(2);
	if(gameLevel>=20 && Random(10)==0)
		t=2;

	for(i=0;i<NUMBADGUYS;i++)
	{
		if(spcBadguy[i].type==SG_NONE)
		{
			switch(t)
			{
				case 0:
					spcBadguy[i].type=SG_UFO;
					spcBadguy[i].hp=3;
					spcBadguy[i].x=x;
					spcBadguy[i].y=y;
					spcBadguy[i].dx=dx;
					spcBadguy[i].dy=0;
					spcBadguy[i].timer=30;
					break;
				case 1:
					spcBadguy[i].type=SG_MINI;
					spcBadguy[i].hp=1;
					spcBadguy[i].x=x;
					spcBadguy[i].y=y;
					spcBadguy[i].dx=dx;
					spcBadguy[i].dy=0;
					spcBadguy[i].timer=20;
					break;
				case 2:
					spcBadguy[i].type=SG_DIVER;
					spcBadguy[i].hp=1;
					spcBadguy[i].x=x;
					spcBadguy[i].y=y;
					spcBadguy[i].dx=dx;
					spcBadguy[i].dy=0;
					break;
			}
			return;
		}
	}
}

void SpaceAddBadguys(void)
{
	int i,j;
	byte t;

	if(addTime)
		addTime--;
	else
	{
		addTime=4*30-gameLevel;
		if(numBadguys==100)
		{
			for(i=0;i<NUMBADGUYS;i++)
			{
				if(spcBadguy[i].type==SG_NONE)
				{
					numBadguys=0;
					spcBadguy[i].type=SG_MOTHER;
					spcBadguy[i].hp=50;
					PutOnEdge(i);
					spcBadguy[i].timer=30;
					MakeSpaceSound(SND_SCARYBOSS,8000);
					return;
				}
			}
			return;
		}
		for(j=0;j<4;j++)
		{
			t=(byte)Random(2);
			if(gameLevel>=5 && Random(15)==0)
				t=2;
			if(gameLevel>=20 && Random(20)==0)
				t=3;

			for(i=0;i<NUMBADGUYS;i++)
			{
				if(spcBadguy[i].type==SG_NONE)
				{
					numBadguys++;
					switch(t)
					{
						case 0:
							spcBadguy[i].type=SG_UFO;
							spcBadguy[i].hp=3;
							PutOnEdge(i);
							spcBadguy[i].timer=30;
							break;
						case 1:
							spcBadguy[i].type=SG_MINI;
							spcBadguy[i].hp=1;
							PutOnEdge(i);
							spcBadguy[i].timer=20;
							break;
						case 2:
							spcBadguy[i].type=SG_MAXI;
							spcBadguy[i].hp=10;
							PutOnEdge(i);
							spcBadguy[i].timer=10;
							break;
						case 3:
							spcBadguy[i].type=SG_DIVER;
							spcBadguy[i].hp=1;
							PutOnEdge(i);
							spcBadguy[i].dx=0;
							spcBadguy[i].dy=0;
							break;
					}
					break;
				}
			}
		}
	}
}
byte UpdateSpaceGame(int *lastTime,MGLDraw *mgl)
{
	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		if(curScore<score)
			curScore++;
		if(curScore>score)
			curScore--;

		if(SpaceUpdatePlayerShip(mgl))
			return 1;

		SpaceUpdateBullets();
		SpaceUpdateBadguys();
		SpaceAddBadguys();
		if(msgTimer)
			msgTimer--;

		totalTime++;
		*lastTime-=TIME_PER_FRAME;
	}

	JamulSoundUpdate();
	return 0;
}

byte UpdateGameOver(int *lastTime,MGLDraw *mgl)
{
	char c;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		*lastTime-=TIME_PER_FRAME;
	}


	c=GetControls();

	if(mgl->LastKeyPressed())
		return 1;

	if((c&(CONTROL_B1|CONTROL_B2)) && !(oldc&(CONTROL_B1|CONTROL_B2)))
		return 1;

	JamulSoundUpdate();
	return 0;
}

void SpaceGame(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	InitSpaceGame(mgl);
	lastTime=1;
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		if(!gameOver)
			done=UpdateSpaceGame(&lastTime,mgl);
		else
			done=UpdateGameOver(&lastTime,mgl);
		RenderSpaceGame(mgl);
		mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	ExitSpaceGame();
}
