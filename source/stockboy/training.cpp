#include "training.h"
#include "display.h"
#include "player.h"
#include "game.h"
#include "profile.h"
#include "options.h"
#include "traincert.h"
#include "music.h"

#define TRN_XBASE	320
#define TRN_YBASE	115
#define TRN_XADD	55
#define TRN_YADD	88

char trainingFile[10][32]={
	"levels\\pyr1.sbl",
	"levels\\pyr2.sbl",
	"levels\\pyr3.sbl",
	"levels\\pyr4.sbl",
	"levels\\pyr5.sbl",
	"levels\\pyr6.sbl",
	"levels\\pyr7.sbl",
	"levels\\pyr8.sbl",
	"levels\\pyr9.sbl",
	"levels\\pyr10.sbl",
};

char trainingName[11][32]={
	"00 - Exit",
	"1A - Basic Shoving",
	"2A - Roly Poly",
	"2B - Blowing Stuff Up",
	"3A - Arrows & Cranks",
	"3B - Doors & Switches",
	"3C - Critters",
	"4A - Gadgets",
	"4B - Holes, Pits & Spikes",
	"4C - Quantum Physics",
	"4D - Scoring",
};

byte trainingLevel;
byte selX,selY;

typedef struct trnArrow_t
{
	int x,y;
	int dx,dy;
	int tx,ty;
} trnArrow_t;

static byte *backScr;
static sprite_set_t *trnSpr;
static trnArrow_t left,right;
static byte displayIt[10];
static int oldMsx,oldMsy;
static byte oldMsBtn;
static byte mode;
static byte subLevel;
static byte oldC;

void TrainingSetArrow(void);

void SetupDisplay(void)
{
	byte done[4]={0,0,0,0};
	int i;

	for(i=0;i<10;i++)
		displayIt[i]=0;

	displayIt[0]=1;
	if(profile.training[0]==6)
	{
		done[0]=1;
	}
	done[1]=1;
	for(i=1;i<3;i++)
	{
		if(profile.training[i]<6)
			done[1]=0;
		if(done[0])
			displayIt[i]=1;
	}
	done[2]=1;
	for(i=3;i<6;i++)
	{
		if(profile.training[i]<6)
			done[2]=0;
		if(done[1])
			displayIt[i]=1;
	}
	done[3]=1;
	for(i=6;i<10;i++)
	{
		if(profile.training[i]<6)
			done[3]=0;
		if(done[2])
			displayIt[i]=1;
	}
}

void InitTraining(void)
{
	int i;

	GetDisplayMGL()->LastKeyPressed();
	GetTaps();
	GetArrowTaps();
	backScr=(byte *)malloc(640*480);
	if(!backScr)
		GetDisplayMGL()->FatalError("Out of memory!");

	GetDisplayMGL()->LoadBMP("graphics\\training.bmp");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);

	trainingLevel=0;
	trnSpr=new sprite_set_t("graphics\\training.jsp");
	selX=0;
	selY=0;
	left.x=0;
	left.y=0;
	right.x=640*FIXAMT;
	right.y=0;
	left.dx=0;
	right.dx=0;
	left.dy=0;
	right.dy=0;
	TrainingSetArrow();
	SetupDisplay();
	oldMsBtn=255;
	oldC=255;
	GetDisplayMGL()->GetMouse(&oldMsx,&oldMsy);
	Music_Load(profile.songChoice[SONG_MENU]);
	Music_Play();
	mode=0;
}

void ExitTraining(void)
{
	free(backScr);
	delete trnSpr;
}

void ShowDescription(MGLDraw *mgl)
{

	RightPrint(630,120,trainingName[trainingLevel],-32,0);
	RightPrint(630-3,120-3,trainingName[trainingLevel],0,0);

	if(trainingLevel>0 && !displayIt[trainingLevel-1])
	{
		RightPrint(630,150,"Not Available",-32,0);
		RightPrint(630-3,150-3,"Not Available",0,0);
	}
}

void RenderTraining(MGLDraw *mgl)
{
	byte triChart[7*4]={
		0,0,0,1,0,0,0,
		0,0,2,0,3,0,0,
		0,4,0,5,0,6,0,
		7,0,8,0,9,0,10,
	};

	int i,x,y,number,letter;

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	if(mode==0)
	{
		number=0;
		letter=0;
		for(i=0;i<7*4;i++)
		{
			if(triChart[i])
			{
				x=TRN_XBASE-TRN_XADD*3+TRN_XADD*(i%7);
				y=TRN_YBASE+TRN_YADD*(i/7);

				// render the triangle
				trnSpr->GetSprite(0)->Draw(x,y,mgl);

				if(displayIt[triChart[i]-1])
				{
					// render the letter/number
					trnSpr->GetSprite(4+number)->Draw(x-15,y,mgl);
					trnSpr->GetSprite(9+letter)->Draw(x+15,y,mgl);
				}
				// X out finished ones
				if(profile.training[triChart[i]-1]>5)
					trnSpr->GetSprite(1)->Draw(x,y,mgl);

				letter++;
			}
			if(i%7==6)
			{
				letter=0;
				number++;
			}
		}
		ShowDescription(mgl);
	}
	else	// selecting which level to play
	{
		for(i=0;i<5;i++)
		{
			if(i==0 || profile.training[trainingLevel-1]>i-1)	// if have access to level
			{
				trnSpr->GetSprite(14)->Draw(200,100+i*70,mgl);
				trnSpr->GetSprite(4+i)->Draw(370,100+i*70,mgl);
			}
			else
			{
				trnSpr->GetSprite(15)->Draw(200,100+i*70,mgl);
				trnSpr->GetSprite(16+i)->Draw(370,100+i*70,mgl);
			}
		}
	}

	trnSpr->GetSprite(2)->Draw(left.x/FIXAMT,left.y/FIXAMT,mgl);
	trnSpr->GetSprite(3)->Draw(right.x/FIXAMT,right.y/FIXAMT,mgl);

	trnSpr->GetSprite(13)->Draw(oldMsx,oldMsy,mgl);
}

void TrainingSetArrow(void)
{
	int xTab[]={470,	// exit option
				TRN_XBASE,
				TRN_XBASE-TRN_XADD,
				TRN_XBASE+TRN_XADD,
				TRN_XBASE-TRN_XADD*2,
				TRN_XBASE,
				TRN_XBASE+TRN_XADD*2,
				TRN_XBASE-TRN_XADD*3,
				TRN_XBASE-TRN_XADD,
				TRN_XBASE+TRN_XADD,
				TRN_XBASE+TRN_XADD*3
				};
	byte trAdd[]={1,2,4,7};
	
	if(mode==0)
	{
		if(selY==4)
			trainingLevel=0;
		else
			trainingLevel=trAdd[selY]+selX;

		left.tx=xTab[trainingLevel]-50;
		right.tx=xTab[trainingLevel]+50;
		left.ty=TRN_YBASE+TRN_YADD*selY;
		right.ty=TRN_YBASE+TRN_YADD*selY;
		if(selY==4)
		{
			left.tx-=20;
			right.tx+=20;
			left.ty=450;
			right.ty=450;
		}
	}
	else
	{
		if(subLevel>0)
		{
			left.tx=160;
			right.tx=410;
			left.ty=30+subLevel*70;
			right.ty=30+subLevel*70;
		}
		else
		{
			left.tx=xTab[0]-50;
			right.tx=xTab[0]+50;
			left.ty=TRN_YBASE+TRN_YADD*4;
			right.ty=TRN_YBASE+TRN_YADD*4;
			left.tx-=20;
			right.tx+=20;
			left.ty=450;
			right.ty=450;
		}
	}
	left.tx*=FIXAMT;
	left.ty*=FIXAMT;
	right.tx*=FIXAMT;
	right.ty*=FIXAMT;
}

byte TrainingMouseCheck(int msx,int msy)
{
	int xTab[]={470,	// exit option
				TRN_XBASE,
				TRN_XBASE-TRN_XADD,
				TRN_XBASE+TRN_XADD,
				TRN_XBASE-TRN_XADD*2,
				TRN_XBASE,
				TRN_XBASE+TRN_XADD*2,
				TRN_XBASE-TRN_XADD*3,
				TRN_XBASE-TRN_XADD,
				TRN_XBASE+TRN_XADD,
				TRN_XBASE+TRN_XADD*3
				};
	int yTab[]={TRN_YBASE+TRN_YADD*4,	// exit option
				TRN_YBASE,
				TRN_YBASE+TRN_YADD,
				TRN_YBASE+TRN_YADD,
				TRN_YBASE+TRN_YADD*2,
				TRN_YBASE+TRN_YADD*2,
				TRN_YBASE+TRN_YADD*2,
				TRN_YBASE+TRN_YADD*3,
				TRN_YBASE+TRN_YADD*3,
				TRN_YBASE+TRN_YADD*3,
				TRN_YBASE+TRN_YADD*3,
				};
	byte selXTab[]={0,0,0,1,0,1,2,0,1,2,3};
	byte selYTab[]={4,0,1,1,2,2,2,3,3,3,3};
	int i;
	int yPos;

	yPos=TRN_YBASE;
	for(i=0;i<11;i++)
	{
		if(msx>=xTab[i]-50 && msx<=xTab[i]+50 && msy>=yTab[i]-50 && msy<=yTab[i]+50)
		{
			selX=selXTab[i];
			selY=selYTab[i];
			TrainingSetArrow();
			return 1;
		}
	}
	return 0;
}

byte SubLevelMouseCheck(int msx,int msy)
{
	int i;

	if(msx>=420 && msx<=520 && msy>=TRN_YBASE+TRN_YADD*4-50 && msy<=TRN_YBASE+TRN_YADD*4+50)
	{
		subLevel=0;
		TrainingSetArrow();
		return 1;
	}
	else
	{
		for(i=0;i<5;i++)
			if(msx>=180 && msx<=410 && msy>=100+i*70-35 && msy<=100+i*70+35)
			{
				subLevel=i+1;
				TrainingSetArrow();
				return 1;
			}
	}
	return 0;
}

void UpdateArrow(trnArrow_t *arrow)
{
	int maxSpd;

	if(arrow->x<arrow->tx)
		arrow->dx+=FIXAMT*2;
	if(arrow->x>arrow->tx)
		arrow->dx-=FIXAMT*2;
	if(arrow->y<arrow->ty)
		arrow->dy+=FIXAMT*2;
	if(arrow->y>arrow->ty)
		arrow->dy-=FIXAMT*2;

	maxSpd=(abs(arrow->tx-arrow->x)+abs(arrow->ty-arrow->y))/4;
	if(maxSpd<FIXAMT*5)
		maxSpd=FIXAMT*5;
	Clamp(&arrow->dx,maxSpd);
	Clamp(&arrow->dy,maxSpd);
	Dampen(&arrow->dx,FIXAMT);
	Dampen(&arrow->dy,FIXAMT);

	arrow->x+=arrow->dx;
	arrow->y+=arrow->dy;
}

byte UpdateTraining(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte t;
	static byte fwClock;
	int msx,msy;
	byte btn;

	JamulSoundUpdate();

	if(*lastTime>TIME_PER_FRAME*MAX_RUNS)
		*lastTime=TIME_PER_FRAME*MAX_RUNS;

	while(*lastTime>=TIME_PER_FRAME)
	{
		Music_Update();

		c=mgl->LastKeyPressed();
		t=GetControls()|GetArrows();

		mgl->GetMouse(&msx,&msy);

		if(msx!=oldMsx || msy!=oldMsy)
		{
			if(mode==0)
				TrainingMouseCheck(msx,msy);
			else
				SubLevelMouseCheck(msx,msy);
		}
		btn=mgl->MouseDown(0);
		if(btn && !oldMsBtn)
		{
			if(mode==0)
			{
				if(TrainingMouseCheck(msx,msy))
				{
					if(trainingLevel==0 || displayIt[trainingLevel-1]==1)
					{
						if(trainingLevel==0)
							return 0;
						mode=1;
						subLevel=profile.training[trainingLevel-1]+1;
						if(subLevel>5)
							subLevel=5;
						TrainingSetArrow();
					}
					else
						MakeNormalSound(SND_MENUCANCEL);
				}
			}
			else
			{
				if(SubLevelMouseCheck(msx,msy))
				{
					if(subLevel==0)
					{
						mode=0;
						TrainingSetArrow();
						MakeNormalSound(SND_MENUCANCEL);
					}
					else if(subLevel<=profile.training[trainingLevel-1]+1)
					{
						return 0;
					}
					else
					{
						MakeNormalSound(SND_MENUCANCEL);
					}
				}
			}
		}
		oldMsBtn=btn;
		oldMsx=msx;
		oldMsy=msy;

		if((t&CONTROL_DN) && !(oldC&CONTROL_DN))
		{
			if(mode==0)
			{
				selY++;
				if(selY>4)
					selY=0;
				if(selY==4)
					selX=0;
			}
			else
			{
				subLevel++;
				if(subLevel>5)
					subLevel=0;
			}
			TrainingSetArrow();
		}
		if((t&CONTROL_UP) && !(oldC&CONTROL_UP))
		{
			if(mode==0)
			{
				selY--;
				if(selY>4)
					selY=4;
				if(selY==3)
					selX=3;
				if(selX>selY)
					selX=selY;
				if(selY==4)
					selX=0;
			}
			else
			{
				subLevel--;
				if(subLevel>5)
					subLevel=5;
			}
			TrainingSetArrow();
		}
		if((t&CONTROL_LF) && !(oldC&CONTROL_LF))
		{
			if(mode==0)
			{
				selX--;
				if(selX>selY)
					selX=selY;
				if(selY==4)
					selX=0;
				TrainingSetArrow();
			}
		}
		if((t&CONTROL_RT) && !(oldC&CONTROL_RT))
		{
			if(mode==0)
			{
				selX++;
				if(selX>selY)
					selX=0;
				if(selY==4)
					selX=0;
				TrainingSetArrow();
			}
		}
		UpdateArrow(&left);
		UpdateArrow(&right);

		if((t&CONTROL_B1) && !(oldC&CONTROL_B1))
		{
			if(mode==0)
			{
				if(trainingLevel==0 || displayIt[trainingLevel-1]==1)
				{
					if(trainingLevel==0)
						return 0;
					mode=1;
					subLevel=profile.training[trainingLevel-1]+1;
					if(subLevel>5)
						subLevel=5;
					TrainingSetArrow();
				}
				else
					MakeNormalSound(SND_MENUCANCEL);
			}
			else
			{
				if(subLevel==0)
				{
					mode=0;
					MakeNormalSound(SND_MENUCANCEL);
					TrainingSetArrow();
				}
				else if(subLevel<=profile.training[trainingLevel-1]+1)
				{
					return 0;
				}
				else
				{
					MakeNormalSound(SND_MENUCANCEL);
				}
			}
		}
		if(c==27)
		{
			if(mode==1)
			{
				mode=0;
				TrainingSetArrow();
			}
			else
			{
				trainingLevel=0;
				return 0;
			}
		}
		oldC=t;

		*lastTime-=TIME_PER_FRAME;
	}
	return 1;
}

byte TrainingMenu(MGLDraw *mgl)
{
	byte b;
	int lastTime=1;
	dword now,then,hangon;

	EndClock();
	hangon=TimeLength();

	now=timeGetTime();
	InitTraining();
	b=1;
	while(b==1)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdateTraining(&lastTime,mgl);
		Music_Update();
		RenderTraining(mgl);
		mgl->Flip();
		if(!mgl->Process())
		{
			ExitTraining();
			return WORLD_ABORT;
		}
		EndClock();
		if(b==0 && trainingLevel>0)
		{
			player.levelNum=subLevel-1;
			b=LunaticWorld(trainingLevel-1,trainingFile[trainingLevel-1]);
			if(b==WORLD_WIN)
			{
				if(profile.training[trainingLevel-1]<6)
				{
					profile.training[trainingLevel-1]++;
					TrainCert(trainingLevel-1,mgl);
				}
				mode=0;
				TrainingSetArrow();
			}
			else
			{
				subLevel=profile.training[trainingLevel-1]+1;
				if(subLevel>5)
					subLevel=0;
				TrainingSetArrow();
			}
			SaveProfile(opt.curProfile);
			ResetClock(0);
			Music_Load(profile.songChoice[SONG_MENU]);
			Music_Play();
			SetupDisplay();
			GetTaps();
			GetArrowTaps();
			mgl->LastKeyPressed();
			oldMsBtn=1;
			oldC=255;
			b=1;
		}
	}
	then=timeGetTime();
	ResetClock(hangon);
	ExitTraining();

	return WORLD_ABORT;
}