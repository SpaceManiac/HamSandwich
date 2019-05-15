#include "nag.h"
#include "game.h"
#include "jamulfmv.h"
#include "pause.h"
#include "options.h"

sprite_set_t *nagSpr;
static int numRunsToMakeUp;

static byte oldc=0;
static byte cursor;
static int scrY,scrOffY;

void NagDisplay(MGLDraw *mgl)
{
	int i,n;

	mgl->ClearScreen();
	nagSpr->GetSprite(0)->Draw(0,0,mgl);

	n=scrY;
	for(i=0;i<3;i++)
	{
		nagSpr->GetSprite(5+n)->Draw(0,0+i*240-scrOffY,mgl);
		n++;
		if(n>5)
			n=0;
	}

	nagSpr->GetSprite(1+(cursor==0))->Draw(0,0,mgl);
	nagSpr->GetSprite(3+(cursor==1))->Draw(0,0,mgl);
}

byte NagUpdate(MGLDraw *mgl,int *lastTime)
{
	byte c;
	static byte reptCounter=10;

	while(*lastTime>=TIME_PER_FRAME)
	{
		// update graphics
		scrOffY++;
		if(scrOffY>=240)
		{
			scrOffY-=240;
			scrY++;
			if(scrY==6)
				scrY=0;
		}

		// now real updating
		c=GetControls();

		reptCounter++;
		if((!oldc) || (reptCounter>10))
			reptCounter=0;

		if((c&(CONTROL_LF|CONTROL_RT)) && (!reptCounter))
		{
			cursor=1-cursor;
			MakeNormalSound(SND_MENUCLICK);
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
		   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			MakeNormalSound(SND_MENUSELECT);
			return 1;
		}
		oldc=c;

		c=mgl->LastKeyPressed();
		if(c==27)
		{
			cursor=1;
			MakeNormalSound(SND_MENUSELECT);
			return 1;
		}
		*lastTime-=TIME_PER_FRAME;
	}

	JamulSoundUpdate();

	return 0;
}

byte Nag(MGLDraw *mgl)
{
	byte b=0;
	int lastTime=1;

	mgl->LastKeyPressed();
	mgl->ClearScreen();
	oldc=CONTROL_B1|CONTROL_B2;
	nagSpr=new sprite_set_t("graphics/nag.jsp");
	cursor=1;

	scrY=MGL_random(6);
	scrOffY=MGL_random(240);
	StartClock();
	PlaySong(SONG_CHAP12FIGHT);
	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();
		b=NagUpdate(mgl,&lastTime);
		NagDisplay(mgl);
		mgl->Flip();
		if(!mgl->Process())
		{
			delete nagSpr;
			return 255;
		}
		EndClock();
	}
	delete nagSpr;
	return cursor;
}
