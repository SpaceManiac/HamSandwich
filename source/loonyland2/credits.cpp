#include "credits.h"
#include "game.h"
#include <io.h>
#include "jamulfmv.h"
#include "pause.h"
#include "options.h"
#include "pause.h"
#include "title.h"

char credits[][48]={
	"Loonyland II:",
	"Winter Woods",
#ifdef DIRECTORS
	"Collector's Edition",
#endif
	"",
	"Copyright 2007,",
	"Hamumu Software",
	"",
	"Programming",
	"Mike Hommel",
	"",
	"Graphics",
	"Mike Hommel",
	"",
	"Sound Effects",
	"Mike Hommel",
	"",
	"Voices",
	"Solange Hommel",
	"Mike Hommel",
	"",
	"Music",
	"Bjorn Lynne",
	"Damian Turnbull",
	"Project 2020",
	"",
	"Music from",
	"www.shockwave-sound.com",
	"",
	"Design And Whatnot",
	"Mike Hommel",
	"",
	"Inspiration and Ideas",
	"The Hamumu Forum Players!",
	"",
	"Testers",
	"Corey Connolly",
	"Hammered",
	"Solange Hommel",
	"Lisa Keck",
	"Ryan Keck",
	"Twila Miller",
	"Barry Pennington",
	"",
	"Stay Tuned For",
	"Loonyland III:",
	"Diwali Desert",
	"Coming... someday?",
	"",
	"",
#ifdef DIRECTORS
	"Thanks for being a",
	"collector!",
	"",
#endif
	"Don't forget to visit",
	"WWW.HAMUMU.COM",
	"a lot!  A whole lot!",
	"",
	"",
	"~",
};

typedef struct dude_t
{
	int  x,y;
	int dx,dy;
	byte who;
} dude_t;

#define NUMDUDES	(10)

static byte oldc=0;
static int credY,credEnd;
static byte *backScr;
static int numRuns;
static dude_t dude[NUMDUDES];
static sprite_set_t *dudeSpr;

void InitCredits(MGLDraw *mgl,byte ending)
{
	int i;

	if(ending)
		mgl->LoadBMP("graphics/dvfgh.bmp");
	else
		mgl->LoadBMP(TITLEBMP);
	mgl->LastKeyPressed();
	GetTaps();
	oldc=255;
	credY=480;
	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],mgl->GetScreen()+mgl->GetWidth()*i,640);
	for(i=0;i<NUMDUDES;i++)
		dude[i].who=1;	// 1 and 19 are invalid

	dudeSpr=new sprite_set_t("graphics/vgrfaces.jsp");
}

void ExitCredits(void)
{
	free(backScr);
	delete dudeSpr;
}

byte AlreadyDude(byte w)
{
	int i;

	for(i=0;i<NUMDUDES;i++)
		if(i!=w && dude[w].who==dude[i].who)
			return 1;

	return 0;
}

void AddDude(void)
{
	int i;

	for(i=0;i<NUMDUDES;i++)
	{
		if(dude[i].who==1)
		{
			while(dude[i].who==1 || dude[i].who==19 || AlreadyDude(i))
				dude[i].who=rand()%(dudeSpr->GetCount()-3);
			dude[i].x=-FIXAMT*50+FIXAMT*(rand()%((50+350)));
			dude[i].y=FIXAMT*480+FIXAMT*dudeSpr->GetSprite(dude[i].who)->height+FIXAMT*dudeSpr->GetSprite(dude[i].who)->ofsy;
			dude[i].dx=0;//-FIXAMT+(rand()%(FIXAMT*2+1));
			dude[i].dy=-(FIXAMT/2)*i;
			return;
		}
	}
}

void UpdateDudes(void)
{
	int i;

	for(i=0;i<NUMDUDES;i++)
	{
		if(dude[i].who!=1)
		{
			dude[i].x+=dude[i].dx;
			dude[i].y+=dude[i].dy;
			if(dude[i].x>740*FIXAMT || dude[i].x<-100*FIXAMT || dude[i].y<-100*FIXAMT)
				dude[i].who=1;
		}
	}
	AddDude();
}

void RenderDudes(MGLDraw *mgl)
{
	int i;

	for(i=0;i<NUMDUDES;i++)
	{
		if(dude[i].who!=1)
		{
			dudeSpr->GetSprite(dude[i].who)->Draw(dude[i].x/FIXAMT,dude[i].y/FIXAMT,mgl);
		}
	}
}

void CreditsDisplay(MGLDraw *mgl)
{
	int i;

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	RenderDudes(mgl);

	i=0;
	while(credits[i][0]!='~')
	{
		if(credY+i*30>-30)
		{
			RightPrintDark(634-1,credY+i*30-1,credits[i],0);
			RightPrintDark(634+1,credY+i*30-1,credits[i],0);
			RightPrintDark(634+1,credY+i*30+1,credits[i],0);
			RightPrintDark(634-1,credY+i*30+1,credits[i],0);
			RightPrintGlow(634,credY+i*30,credits[i],0,0);
		}
		i++;
		credEnd=credY+i*30;
		if(credY+i*30>480)
		{
			credEnd=0;
			break;
		}
	}
}

byte CreditsUpdate(int *lastTime,MGLDraw *mgl)
{
	byte c,k;
	static byte reptCounter=0;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		credY--;
		UpdateDudes();

		// now real updating
		c=GetControls()|GetArrows();

		k=mgl->LastKeyPressed();

		if((c&(CONTROL_B1|CONTROL_B2)) && !(oldc&(CONTROL_B1|CONTROL_B2)))
		{
			return 1;
		}
		if(k==27)
			return 1;

		oldc=c;

		*lastTime-=TIME_PER_FRAME;
		numRuns++;
		if(credEnd<0)
			return 1;
	}
	return 0;
}

byte Credits(MGLDraw *mgl,byte ending)
{
	byte b=0;
	int lastTime=1;

	InitCredits(mgl,ending);

	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();

		b=CreditsUpdate(&lastTime,mgl);
		CreditsDisplay(mgl);

		mgl->Flip();
		if(!mgl->Process())
		{
			ExitCredits();
			return 255;
		}
		EndClock();
	}

	ExitCredits();
	return b;
}
