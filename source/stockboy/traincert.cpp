#include "traincert.h"
#include "player.h"
#include "floor.h"
#include "sound.h"
#include "highscore.h"
#include "options.h"
#include "stars.h"
#include "music.h"

static byte *backScr=NULL;
static byte oldMsBtn;
static byte oldc;
static byte starClock;
static byte starAdd;
static byte level;
static byte endClock;

void SetupTrainCert(void)
{
	int i;

	GetDisplayMGL()->LoadBMP("graphics\\training_done.bmp");
	backScr=(byte *)malloc(640*480);
	if(!backScr)
		GetDisplayMGL()->FatalError("Out of memory!");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);
	
	GetTaps();
	GetArrowTaps();
	GetDisplayMGL()->LastKeyPressed();
	MakeNormalSound(SND_WINLEVEL);
	InitStars(4,profile.starsLeft);
	oldc=255;
	oldMsBtn=1;
	starAdd=TRAINING_STARS;
	starClock=30;
	endClock=30*5;
	profile.starsLeft+=TRAINING_STARS;
	profile.stars+=TRAINING_STARS;
	Music_Stop();
}

void ExitTrainCert(void)
{
	SaveProfile(opt.curProfile);
	SaveOptions();

	if(backScr)
	{
		free(backScr);
		backScr=NULL;
	}
}

byte UpdateTrainCert(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte c2;
	byte b;
	
	JamulSoundUpdate();

	if(*lastTime>TIME_PER_FRAME*MAX_RUNS)
		*lastTime=TIME_PER_FRAME*MAX_RUNS;
	while(*lastTime>=TIME_PER_FRAME)
	{
		Music_Update();

		c=mgl->LastKeyPressed();
		c2=GetArrows()|GetControls();
		b=mgl->MouseDown(0);

		UpdateStars();
		if(starClock)
		{
			starClock--;
		}
		else
		{
			if(starAdd)
			{
				starAdd--;
				LaunchStar(STARSRCX,STARSRCY);
			}
			starClock=starAdd+1;
		}
		
		if(c || (c2&&(!oldc)) || (b&&(!oldMsBtn)))
		{
			return 1;
		}

		*lastTime-=TIME_PER_FRAME;
		oldc=c2;
		oldMsBtn=b;
	}
	return 0;
}

void RenderTrainCert(MGLDraw *mgl)
{
	int i;
	char levelName[10][32]={
		"Basic Shoving",
		"Roly Poly",
		"Blowing Stuff Up",
		"Arrows & Cranks",
		"Doors & Switches",
		"Critters",
		"Gadgets",
		"Holes, Pits & Spikes",
		"Quantum Physics",
		"Scoring",
	};

	if(!GM_doDraw)
		return;

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	PrintUnGlow(212,125,profile.name,0);
	PrintUnGlow(212,229,levelName[level],0);
	PrintUnGlow(311,335,"10 Star Points.",0);
	RenderEndStar(mgl);
	RenderStartStar(STARSRCX,STARSRCY,mgl);
	RenderStars(mgl);
}

void TrainCert(byte lvl,MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	SetupTrainCert();
	lastTime=1;
	level=lvl;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateTrainCert(&lastTime,mgl);
		RenderTrainCert(mgl);
		mgl->Flip();
		EndClock();
		
		if(!mgl->Process())
			done=1;
		
	}

	ExitTrainCert();
}
