#include "empyear.h"
#include "player.h"
#include "floor.h"
#include "sound.h"
#include "highscore.h"
#include "options.h"
#include "stars.h"
#include "music.h"

static byte *backScr=NULL;
static sprite_set_t *yearSpr;
static byte oldMsBtn;
static byte oldc;
static byte starClock;
static byte starAdd;
static byte endClock,typeClock,namePos,burning;
static char tmpName[32];

void SetupEmpYear(void)
{
	int i;

	switch(profile.character)
	{
		case CHAR_SURF:
			GetDisplayMGL()->LoadBMP("graphics/win_surfboy.bmp");
			break;
		case CHAR_GOTH:
			GetDisplayMGL()->LoadBMP("graphics/win_goth.bmp");
			break;
		case CHAR_ROBOT:
			GetDisplayMGL()->LoadBMP("graphics/win_robot.bmp");
			break;
		case CHAR_HAPPY:
			GetDisplayMGL()->LoadBMP("graphics/win_happystick.bmp");
			break;
		case CHAR_SANTA:
			GetDisplayMGL()->LoadBMP("graphics/win_santa.bmp");
			break;
		case CHAR_CHERUB:
			GetDisplayMGL()->LoadBMP("graphics/win_cherub.bmp");
			break;
		case CHAR_SHROOM:
			GetDisplayMGL()->LoadBMP("graphics/win_shroom.bmp");
			break;
		case CHAR_PIG:
			GetDisplayMGL()->LoadBMP("graphics/win_warpig.bmp");
			break;
		case CHAR_FREAK:
			GetDisplayMGL()->LoadBMP("graphics/win_freak.bmp");
			break;
		case CHAR_HIPPIE:
			GetDisplayMGL()->LoadBMP("graphics/win_hippie.bmp");
			break;
	}

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
	starAdd=EMPYEAR_STARS;
	starClock=30;
	endClock=30*5;
	profile.starsLeft+=EMPYEAR_STARS;
	profile.stars+=EMPYEAR_STARS;
	tmpName[0]='\0';
	namePos=0;
	typeClock=10;
	burning=1;
	Music_Stop();

	yearSpr=new sprite_set_t("graphics/empyear.jsp");
}

void ExitEmpYear(void)
{
	delete yearSpr;
	SaveProfile(opt.curProfile);
	SaveOptions();

	if(backScr)
	{
		free(backScr);
		backScr=NULL;
	}
}

byte UpdateEmpYear(int *lastTime,MGLDraw *mgl)
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
					profile.empYear2=1;
				else
					profile.empYear=1;
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

		if((!burning && !StarsLeft()) && (c || (c2&&(!oldc)) || (b&&(!oldMsBtn))))
		{
			return 1;
		}

		*lastTime-=TIME_PER_FRAME;
		oldc=c2;
		oldMsBtn=b;
	}
	return 0;
}

void RenderEmpYear(MGLDraw *mgl)
{
	int i;

	/*
	if(!GM_doDraw)
		return;
	*/

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	// special smarch block only shown if smarch is there
	if(GameType()==GAME_PARALLEL)
	{
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
		CenterPrintUnGlow(320+1,300+1,tmpName,0);
		CenterPrintGlow(320-1,300-1,tmpName,-10,0);
		CenterPrintGlow(320-3+Random(5),300-3+Random(5),tmpName,16,0);
	}

	RenderEndStar(mgl);
	RenderStartStar(STARSRCX,STARSRCY,mgl);
	RenderStars(mgl);
}

void EmpYear(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	SetupEmpYear();
	lastTime=1;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateEmpYear(&lastTime,mgl);
		RenderEmpYear(mgl);
		mgl->Flip();
		EndClock();

		if(!mgl->Process())
			done=1;

	}

	ExitEmpYear();
}
