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
static char cursorBright,dCBright;
static byte curLine,error;
static int errorBright;

void InitNameEntry(MGLDraw *mgl)
{
	int i;

	textBright=-250;
	curLine=0;

	entry[0]='\0';
	dCBright=1;
	cursorBright=-31;
	error=0;
	errorBright=-200;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);
}

void ExitNameEntry(void)
{
	free(backgd);
}

byte CheckForExistingName(const char *name)
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
		if(textBright<40 && curLine<9)
		{
			if(textBright<0 && Random(2)==0)
				MakeNormalSound(SND_BULLETHIT);
			textBright+=8;
			if(textBright>=40 && curLine<9)
			{
				curLine++;
				textBright=-250;
			}
		}
		if(error && errorBright<40)
		{
			if(errorBright<0 && Random(2)==0)
				MakeNormalSound(SND_BULLETHIT);
			errorBright+=8;
		}
		cursorBright+=dCBright;
		if(cursorBright>8)
			dCBright=-2;
		else if(cursorBright<-30)
			dCBright=2;

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
	else if(c == SDLK_ESCAPE)
	{
		return 255;  // cancel
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

void RenderLine(int y,const char *txt,int num)
{
	if(curLine==num)
		PrintProgressiveGlow(20,y,txt,textBright,2);
	else if(curLine>num)
		PrintGlow(20,y,txt,0,2);
}
void RenderNameEntry(MGLDraw *mgl)
{
	int i;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	RenderLine(20,"LunaticOS v3.7b",0);
	RenderLine(40,"Copyright 2012, by Hamumu Software",1);

	RenderLine(100,"SPISPOPD United Plan for Really Evil Maniac Extermination",2);
	RenderLine(120,"(Project S.U.P.R.E.M.E.) *** TOP SECRET ***",3);
	RenderLine(140,"with Colossal Hordes of Extremely Evil Spooky Enemies (C.H.E.E.S.E.)",4);
	RenderLine(180,"Please enter your login name, and press Enter for system access.",5);

	RenderLine(220,"(Note: pick a name you like, because you can't change it without",6);
	RenderLine(240,"losing all of your progress in the game!)",7);

	RenderLine(260,"(Note2: Inappropriate names will be banned from online high score list!)",8);

	PrintGlow(20,320,"LOGIN>",0,2);
	PrintGlow(75,320,entry,0,2);
	PrintGlow(75+GetStrLength(entry,2)+1,320,"_",cursorBright,2);

	if(error==1)
		PrintProgressiveGlow(20,350,"Login name must be at least 3 characters long.",errorBright,2);
	else if(error==2)
		PrintProgressiveGlow(20,350,"Login name may only contain letters and numbers.",errorBright,2);
	else if(error==3)
		PrintProgressiveGlow(20,350,"That name's already in use on this computer!",errorBright,2);
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
			done=255;
		EndClock();
	}

	ExitNameEntry();
	if(makeNew && done != 255)
	{
		FreeProfile();
		LoadProfile(entry);
		SaveProfile();
	}
}

const char *GetEnteredName(void)
{
	return entry;
}
