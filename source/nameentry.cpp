#include "nameentry.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "dialogbits.h"
#include "progress.h"

static byte *backgd;
static int textBright;
static char entry[16];
static char cursorBright;
static byte curLine,error;
static int errorBright;

void InitNameEntry(MGLDraw *mgl)
{
	int i;

	textBright=-250;
	curLine=0;

	entry[0]='\0';
	cursorBright=1;
	error=0;
	errorBright=-200;

	mgl->LoadBMP("graphics/pause.bmp");
	backgd=(byte *)malloc(640*480);

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);
}

void ExitNameEntry(void)
{
	free(backgd);
}

byte CheckForExistingName(char *name)
{
	FILE *f;
	char s[64];

	sprintf(s,"profiles/%s.prf",name);
	f=fopen(s,"rb");
	if(f)
	{
		fclose(f);
		return 1;
	}
	return 0;
}

byte UpdateNameEntry(int *lastTime,MGLDraw *mgl)
{
	char c;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();

		cursorBright=1-cursorBright;

		*lastTime-=TIME_PER_FRAME;
	}

	c=mgl->LastKeyPressed();
	if(c==13)	// enter
	{
		MakeNormalSound(SND_MENUSELECT);
		if(strlen(entry)>2)
		{
			if(CheckForExistingName(entry))
			{
				error=3;
				errorBright=-200;
			}
			else
				return 1;
		}
		else
		{
			error=1;
			errorBright=-200;
		}
	}
	else if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'))
	{
		if(strlen(entry)<15)
		{
			entry[strlen(entry)+1]='\0';
			if(ShiftState())
			{
				if(c>='a' && c<='z')
					c+='A'-'a';
			}
			entry[strlen(entry)]=c;
			MakeNormalSound(SND_MENUCLICK);
		}
	}
	else if(c==8)
	{
		if(strlen(entry)>0)
		{
			entry[strlen(entry)-1]='\0';
			MakeNormalSound(SND_MENUCLICK);
		}
	}
	else if(c!=0)
	{
		error=2;
		errorBright=-200;
	}

	return 0;
}

void RenderLine(int y,char *txt,int num)
{
	PrintUnGlow(20,y,txt,2);
}

void RenderNameEntry(MGLDraw *mgl)
{
	int i;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	RenderLine(20,"Welcome to the Sleepless Inn!",0);
	RenderLine(60,"Check-in time:  Nowish",1);
	RenderLine(90,"Check-out time: NEVER!",2);

	PrintUnGlowRect(330,320,635,479,30,"NOTE: The management will not be responsible for any "
					"decapitation which occurs on or about our premises.  Guest agrees to assume all risk.",2);

	RenderLine(280,"Please sign our ledger:",3);

	mgl->Box(20,320,300,350,0);
	PrintUnGlow(25,322,entry,2);

	if(cursorBright)
		PrintUnGlow(25+GetStrLength(entry,2)+1,322,"_",2);

	if(error==1)
		PrintUnGlowRect(20,350,315,479,30,"Name must be at least 3 characters long.",2);
	else if(error==2)
		PrintUnGlowRect(20,350,315,479,30,"Name may only contain letters and numbers.",2);
	else if(error==3)
		PrintUnGlowRect(20,350,315,479,30,"That name's already in use on this computer!",2);
}

//----------------

void NameEntry(MGLDraw *mgl,byte makeNew)
{
	byte done=0;
	int lastTime=1;

	InitNameEntry(mgl);

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();

		done=UpdateNameEntry(&lastTime,mgl);
		RenderNameEntry(mgl);
		mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	ExitNameEntry();
	if(makeNew)
	{
		FreeProfile();
		LoadProfile(entry);
		SaveProfile();
	}
}

char *GetEnteredName(void)
{
	return entry;
}
