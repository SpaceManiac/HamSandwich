#include "empmonth.h"
#include "player.h"
#include "floor.h"
#include "sound.h"
#include "highscore.h"
#include "options.h"
#include "stars.h"
#include "music.h"

#define EMPYEARY	230

static byte *backScr=NULL;
static sprite_set_t *yearSpr;
static byte oldMsBtn;
static byte oldc;
static byte starClock;
static byte starAdd;
static byte level;
static byte endClock,typeClock,namePos,burning;
static char tmpName[32];
static byte doYear,doYearTime;
static int yearY,yearDY;

void SetupEmpMonth(void)
{
	int i;
	byte m;

	GetDisplayMGL()->LoadBMP("graphics/empmonth.bmp");
	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);

	GetTaps();
	GetArrowTaps();
	GetDisplayMGL()->LastKeyPressed();
	MakeNormalSound(SND_EMPMONTH);
	InitStars(4,profile.starsLeft);
	oldc=255;
	oldMsBtn=1;
	starAdd=EMPMONTH_STARS;
	starClock=30;
	endClock=30*5;
	profile.starsLeft+=EMPMONTH_STARS;
	profile.stars+=EMPMONTH_STARS;
	tmpName[0]='\0';
	namePos=0;
	typeClock=10;
	burning=1;
	yearSpr=new sprite_set_t("graphics/empyear.jsp");

	doYear=0;
	if(GameType()==GAME_STOCKROOM)
	{
		if(!profile.empYear)
		{
			m=0;
			for(i=0;i<12;i++)
			{
				if(profile.empMonth[i])
					m++;
			}
			if(m==11 && level!=12)
				doYear=1;
		}
	}
	else
	{
		if(!profile.empYear2)
		{
			m=0;
			for(i=0;i<12;i++)
			{
				if(profile.empMonth2[i])
					m++;
			}
			if(m==11 && level!=12)
				doYear=1;
		}
	}
	doYearTime=30*3;
	yearY=-320;
	yearDY=0;
	Music_Stop();

}

void ExitEmpMonth(void)
{
	SaveProfile(opt.curProfile);
	SaveOptions();

	if(backScr)
	{
		free(backScr);
		backScr=NULL;
	}
	delete yearSpr;
}

byte UpdateEmpMonth(int *lastTime,MGLDraw *mgl)
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

		if(doYear==2)
		{
			yearY+=yearDY;
			yearDY+=2;

			if(yearY>EMPYEARY)
			{
				yearY=EMPYEARY;
				yearDY=-yearDY/2;
				doYear=3;
				starAdd=EMPYEAR_STARS;
				starClock=30;
				profile.starsLeft+=EMPYEAR_STARS;
				profile.stars+=EMPYEAR_STARS;
				MakeNormalSound(SND_BOMBBOOM2);
			}
		}
		else if(doYear==3)
		{
			yearY+=yearDY;
			yearDY+=2;

			if(yearY>EMPYEARY)
			{
				yearY=EMPYEARY;
				yearDY=-yearDY/2;
				if(yearDY<-4)
					MakeNormalSound(SND_BOMBBOOM2);
				else
					yearDY=0;
			}
		}

		typeClock--;
		if(!typeClock)
		{
			typeClock=10;
			if(namePos<strlen(profile.name))
			{
				namePos++;
				strncpy(tmpName,profile.name,namePos);
				tmpName[namePos]='\0';
				MakeNormalSound(SND_BOMBFIZZ);	// burning the letter in
			}
			else
			{
				burning=0;
				if(GameType()==GAME_PARALLEL)
					profile.empMonth2[level]=1;
				else
					profile.empMonth[level]=1;
			}
		}
		c=mgl->LastKeyPressed();
		c2=GetArrows()|GetControls();
		b=mgl->MouseDown();

		if(!burning)
		{
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
		}

		if(!burning && !StarsLeft() && doYear==1)
		{
			doYearTime--;
			if(!doYearTime)
			{
				doYear=2;
				if(GameType()==GAME_STOCKROOM)
					profile.empYear=1;
				else
					profile.empYear2=1;
				MakeNormalSound(SND_EMPYEAR);
			}
		}

		if((!burning && !StarsLeft() && (doYear==0 || doYear==3)) && (c || (c2&&(!oldc)) || (b&&(!oldMsBtn))))
		{
			return 1;
		}

		*lastTime-=TIME_PER_FRAME;
		oldc=c2;
		oldMsBtn=b;
	}
	return 0;
}

void RenderEmpMonth(MGLDraw *mgl)
{
	int i;

	/*
	if(!GM_doDraw)
		return;
	*/

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	if(GameType()==GAME_PARALLEL)
	{
		// special smarch block only shown if smarch is there
		if(level==12 || profile.empMonth2[12])
			RenderStar(0,32,307*FIXAMT,238*FIXAMT,mgl);

		for(i=0;i<6;i++)
		{
			if(profile.empMonth2[i])
			{
				CenterPrintUnGlow(168,108+60*i,profile.name,0);
				CenterPrintGlow(166,106+60*i,profile.name,-10,0);
			}
			if(profile.empMonth2[i+6])
			{
				CenterPrintUnGlow(640-168,108+60*i,profile.name,0);
				CenterPrintGlow(640-170,106+60*i,profile.name,-10,0);
			}
		}
		if(profile.empMonth2[12])
			PrintAngle2(275,313,profile.name,0);
	}
	else
	{
		// special smarch block only shown if smarch is there
		if(level==12 || profile.empMonth[12])
			RenderStar(0,32,307*FIXAMT,238*FIXAMT,mgl);

		for(i=0;i<6;i++)
		{
			if(profile.empMonth[i])
			{
				CenterPrintUnGlow(168,108+60*i,profile.name,0);
				CenterPrintGlow(166,106+60*i,profile.name,-10,0);
			}
			if(profile.empMonth[i+6])
			{
				CenterPrintUnGlow(640-168,108+60*i,profile.name,0);
				CenterPrintGlow(640-170,106+60*i,profile.name,-10,0);
			}
		}
		if(profile.empMonth[12])
			PrintAngle2(275,313,profile.name,0);
	}

	if(burning)
	{
		if(level==12)
		{
			// smarch name
			PrintAngle2(275,313,tmpName,0);
			PrintAngle2(273+Random(5),311+Random(5),tmpName,0);
		}
		else if(level<6)
		{
			CenterPrintUnGlow(168,108+60*level,tmpName,0);
			CenterPrintGlow(166,106+60*level,tmpName,-10,0);
			CenterPrintGlow(164+Random(5),104+60*level+Random(5),tmpName,16,0);
		}
		else
		{
			CenterPrintUnGlow(640-168,108+60*(level-6),tmpName,0);
			CenterPrintGlow(640-170,106+60*(level-6),tmpName,-10,0);
			CenterPrintGlow(640-172+Random(5),104+60*(level-6)+Random(5),tmpName,16,0);
		}
	}

	if(doYear>=2)
	{
		yearSpr->GetSprite(2)->DrawShadow(320,EMPYEARY+100,mgl);
		yearSpr->GetSprite(0)->Draw(320,yearY,mgl);
		yearSpr->GetSprite(1)->Draw(320,yearY,mgl);
		CenterPrintUnGlow(320+1,yearY+140+1,profile.name,0);
		CenterPrintGlow(320-1,yearY+140-1,profile.name,-10,0);
	}
	RenderEndStar(mgl);
	RenderStartStar(STARSRCX,STARSRCY,mgl);
	RenderStars(mgl);
}

TASK(void) EmpMonth(byte lvl,MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	SetupEmpMonth();
	lastTime=1;
	level=lvl;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateEmpMonth(&lastTime,mgl);
		RenderEmpMonth(mgl);
		AWAIT mgl->Flip();
		EndClock();

		if(!mgl->Process())
			done=1;

	}

	ExitEmpMonth();
}
