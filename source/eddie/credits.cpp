#include "credits.h"
#include "options.h"
#include "control.h"
#include "display.h"
#include "tile.h"
#include "backgd.h"
#include "status.h"
#include "game.h"
#include "random.h"
#include "sound.h"

// credit codes:
// % = huge-ass exterminator logo
// @ = big blue font
// # = blue drop shadow on regular white text
// lack of a code means regular white text
static char credTxt[][32]={
	"%",
	"@EDDIE GALAXY",
	"#Intergalactic Exterminator",
	"@",
	"@",
	"@",
	"@",
	"#A Hamumu Software Production",
	"* www.hamumu.com *",
	"",
	"#Design and Programming",
	"Mike Hommel",
	"",
	"#3D Modeling and 2D Art",
	"Mike Hommel",
	"",
	"#Background Art",
	"Angela Finer",
	"",
	"#Sound Effects",
	"Mike Hommel",
	"",
	"#Music",
	"Brent Christian",
	"",
	"$"
};

int credY;

byte CreditsRun(int *lastTime)
{
	while(*lastTime>=TIME_PER_FRAME)
	{
		UpdateBackgd();
		JamulSoundUpdate();

		credY++;

		*lastTime-=TIME_PER_FRAME;
	}
	if(mainmgl->LastKeyPressed() || mainmgl->MouseTap())
	{
		return 0;
	}

	return 1;
}

byte CreditsRender(void)
{
	int ypos,i;
	char *s;

	DrawBackgd();

	ypos=0;
	i=0;
	while(credTxt[i][0]!='$')
	{
		s=credTxt[i];

		if(s[0]=='%')
		{
			if(ypos-credY>-480)
				ShowSprite(SPR_LOGO,7,240,ypos-credY+200);
			ypos+=140;
		}
		else if(s[0]=='@')
		{
			if(ypos-credY>-60)
				CenterPrint(ypos-credY,&s[1],2,0);
			ypos+=60;
		}
		else if(s[0]=='#')
		{
			if(ypos-credY>-20)
				ColorCenterPrint(ypos-credY,&s[1],1,96+10);
			ypos+=20;
		}
		else
		{
			if(ypos-credY>-20)
				CenterPrint(ypos-credY,s,1,0);
			ypos+=20;
		}

		i++;
		if(ypos-credY>=480)
			break;
	}
	if(credTxt[i][0]=='$' && ypos-credY<0)
		return 0;

	RenderHiScoreDisplay();
	FlipScreen();
	return 1;
}

void Credits(void)
{
	byte quit;
	int lastTime;
	dword clock,endclock;

	quit=0;
	clock=0;
	endclock=0;
	lastTime=TIME_PER_FRAME;
	credY=-480;

	//LoadBackgd("graphics/space.bmp");
	//SetBackScroll(1);

	mainmgl->LastKeyPressed();

	while(!quit)
	{
		lastTime+=endclock-clock;
		clock=GetTime();

		if(CreditsRun(&lastTime)==0)
			quit=1;

		if(!CreditsRender())
			quit=1;

		endclock=GetTime();
	}
}
