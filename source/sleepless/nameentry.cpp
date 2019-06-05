#include "nameentry.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "dialogbits.h"
#include "progress.h"
#include "customworld.h"
#include "lsdir.h"

static byte *backgd;
static int textBright;
static char entry[16];
static char cursorBright;
static byte curLine,error;
static int errorBright;

#define FILENAME_LEN	32
#define MAX_WORLDS		64
#define DISP_CHOICES	12

static byte customWorldEnabled;
static byte choice;
static char worldFilenames[MAX_WORLDS * FILENAME_LEN];
static char worldTitles[MAX_WORLDS * FILENAME_LEN];
static int numFiles;

/*void SortWorlds(void)
{
	byte flip;
	int i;
	char tmp[FILENAME_LEN];

	flip=1;

	while(flip)
	{
		flip=0;
		for(i=0;i<numFiles-1;i++)
		{
			if(strcmp(&fileList[i*FILENAME_LEN],&fileList[(i+1)*FILENAME_LEN])>0)
			{
				strcpy(tmp,&fileList[i*FILENAME_LEN]);
				strcpy(&fileList[i*FILENAME_LEN],&fileList[(i+1)*FILENAME_LEN]);
				strcpy(&fileList[(i+1)*FILENAME_LEN],tmp);
				flip=1;
			}
		}
	}
}*/

void InputWorld(const char* fname)
{
	if(strlen(fname)>=FILENAME_LEN)
		return;	// won't add long names

	if(numFiles>=MAX_WORLDS)
		return;	// can't add more of them

	strcpy(&worldFilenames[numFiles*FILENAME_LEN],fname);

	strcpy(&worldTitles[numFiles*FILENAME_LEN],CustomWorldTitle(fname));
	numFiles++;
}

void ScanWorlds(void)
{
	int i;

	for(i=0;i<MAX_WORLDS;i++)
		worldFilenames[i*FILENAME_LEN]='\0';
	numFiles=0;

	InputWorld("hollow.shw");

	for (const char* name : filterdir("worlds", ".shw", FILENAME_LEN))
	{
		if (strcmp(name, "hollow.shw") &&
				strcmp(name, "backup_save.shw") &&
				strcmp(name, "backup_load.shw") &&
				strcmp(name, "backup_exit.shw"))
			InputWorld(name);
	}
	//SortWorlds();

	if (numFiles == 1)
		customWorldEnabled = 0;

	choice=0;
}

void InitNameEntry(MGLDraw *mgl)
{
	int i;

	textBright=-250;
	curLine=0;

	entry[0]='\0';
	cursorBright=1;
	error=0;
	errorBright=-200;

	if (customWorldEnabled)
		ScanWorlds();

	mgl->LoadBMP("graphics/pause.bmp");
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
	int raw;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();

		cursorBright=1-cursorBright;

		*lastTime-=TIME_PER_FRAME;
	}

	c=mgl->LastKeyPressed();
	raw = LastScanCode();
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

	if (customWorldEnabled)
	{
		if(raw == SDL_SCANCODE_UP)
		{
			if (--choice == 255)
				choice = numFiles - 1;
		}
		else if (raw == SDL_SCANCODE_DOWN)
		{
			if (++choice == numFiles)
				choice = 0;
		}
	}

	return 0;
}

void RenderLine(int y,const char *txt,int num)
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

	if (customWorldEnabled)
	{
		PrintUnGlow(330,20,"Select a world to play:",2);

		int start = choice - DISP_CHOICES/2;
		if (start < 0)
			start = 0;
		else if (start > numFiles - DISP_CHOICES)
			start = numFiles - DISP_CHOICES;

		for (i=0;i<DISP_CHOICES;++i)
		{
			char* txt=&worldTitles[FILENAME_LEN*(start + i)];
			int x=330 -2, y=50+20*i -2+5;

			PrintUnGlow(x+2,y+2-5,txt,2);
			if(choice==start+i)
			{
				PrintUnGlow(x+1,y+1-5,txt,2);
				PrintUnGlow(x+3,y+3-5,txt,2);
				PrintUnGlow(x+1,y+3-5,txt,2);
				PrintUnGlow(x+3,y+1-5,txt,2);
				PrintGlow(x+2,y+2-5,txt,0,2);
			}
		}
	}

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

#ifdef DEMO
	customWorldEnabled = 0;
#else
	customWorldEnabled = (makeNew == 1);
#endif

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
		// tell the profile what world it is
		if (customWorldEnabled)
			strcpy(profile.lastWorld, &worldFilenames[choice*FILENAME_LEN]);
		else
			strcpy(profile.lastWorld, "hollow.shw");
		SaveProfile();
	}
}

const char *GetEnteredName(void)
{
	return entry;
}
