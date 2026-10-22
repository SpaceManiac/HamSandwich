#include "addon.h"
#include "map.h"
#include "guy.h"
#include "game.h"
#include "options.h"
#include "score.h"
#include "floor.h"
#include "world.h"
#include "stars.h"
#include "highscore.h"
#include "editmenu.h"
#include "music.h"
#include "addonweb.h"
#include "appdata.h"

#define AOTEXTX	15
#define AOTEXTY	66
#define AOHEIGHT 19
#define AOCOUNT 8

static byte *backScr;
static byte cursor;
static byte txtLevel[6];
static sprite_set_t *blowSpr;
static byte play;
static int oldmsx,oldmsy,oldMsBtn;
static byte pickMode;
static int levelSet,numSets,setPos;
static int maxFiles;
static char *filename;
static char *setname;
static char *author;
static byte level,levelPos;
static world_t world;
static byte worldLoaded;
static byte button[3];
static byte online;

const bool onlinePossible = false;

void ScanForAddOns(void);

void InitAddOnMenu(void)
{
	int i;

	online=0;
	JamulSoundPurge();
	Music_Load(profile.songChoice[SONG_MENU]);
	Music_Play();
	GetDisplayMGL()->LastKeyPressed();
	GetTaps();
	GetArrowTaps();
	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	GetDisplayMGL()->LoadBMP("graphics/addon.bmp");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);

	cursor=0;

	for(i=0;i<6;i++)
		txtLevel[i]=0;

	blowSpr=new sprite_set_t("graphics/blowout.jsp");

	GetDisplayMGL()->GetMouse(&oldmsx,&oldmsy);
	oldMsBtn=255;

	LoadOptions();
	InitStars(0,0);
	pickMode=0;
	levelSet=0;
	numSets=0;
	setPos=0;
	setname=NULL;
	filename=NULL;
	author=NULL;
	worldLoaded=0;
	button[0]=button[1]=button[2]=0;

	ScanForAddOns();
}

void ExitAddOnMenu(void)
{
	delete blowSpr;
	free(backScr);
	if(filename)
		free(filename);
	if(setname)
		free(setname);
	if(author)
		free(author);
	if(worldLoaded)
	{
		FreeWorld(&world);
		worldLoaded=0;
	}
	SaveOptions();
}

byte FileIsAddOn(const char *fname)
{
	int i;
	char s[32];

	// don't count training levels
	for(i=0;i<10;i++)
	{
		sprintf(s,"pyr%d.sbl",i+1);
		if(!strcmp(fname,s))
			return 0;
	}
	// don't count stockroom levels
	for(i=0;i<13;i++)
	{
		sprintf(s,"sr%02d.sbl",i);
		if(!strcmp(fname,s))
			return 0;
		sprintf(s,"pu%02d.sbl",i);
		if(!strcmp(fname,s))
			return 0;
	}
	// or the special levels for special modes
	if(!strcmp(fname,"pestcontrol.sbl"))
		return 0;
	if(!strcmp(fname,"clearance.sbl"))
		return 0;
	if(!strcmp(fname,"blowout.sbl"))
		return 0;

	return 1;
}

void ScanForAddOns(void)
{
	byte flip;
	int i;
	char tmp[FNAMELEN];

	numSets=0;
	maxFiles=32;

	if(filename)
		free(filename);
	if(setname)
		free(setname);
	if(author)
		free(author);

	filename=(char *)calloc(FNAMELEN*maxFiles,1);
	setname=(char *)calloc(FNAMELEN*maxFiles,1);
	author=(char *)calloc(FNAMELEN*maxFiles,1);

	if (onlinePossible)
	{
		strcpy(setname,"Auto-Download Add-On Levels");
		author[0]='\0';
		filename[0]='\0';
	}

	auto foo = ListDirectory("levels", ".sbl", FNAMELEN);
	for (const auto& file : foo)
	{
		const char* name = file.c_str();

		// completely ignore ones with filenames that are too long!
		// and ones that are not allowed to be edited
		if(FileIsAddOn(name))
		{
			SDL_utf8strlcpy(&filename[numSets*FNAMELEN],name,FNAMELEN);
			GetWorldName(&filename[numSets*FNAMELEN],&setname[numSets*FNAMELEN],
							&author[numSets*FNAMELEN]);
			numSets++;

			if(numSets==maxFiles)
			{
				maxFiles+=32;
				filename=(char *)realloc(filename,FNAMELEN*maxFiles);
				setname=(char *)realloc(setname,FNAMELEN*maxFiles);
				author=(char *)realloc(author,FNAMELEN*maxFiles);
				// clear the future space
				memset(&filename[numSets*FNAMELEN],0,FNAMELEN*maxFiles-FNAMELEN*numSets);
				memset(&setname[numSets*FNAMELEN],0,FNAMELEN*maxFiles-FNAMELEN*numSets);
				memset(&author[numSets*FNAMELEN],0,FNAMELEN*maxFiles-FNAMELEN*numSets);
			}
		}
	}

	// sort them
	flip=1;
	while(flip)
	{
		flip=0;
		for(i=1;i<numSets-1;i++)
		{
			if(strcmp(&setname[i*FNAMELEN],&setname[(i+1)*FNAMELEN])>0)
			{
				flip=1;
				strcpy(tmp,&filename[i*FNAMELEN]);
				strcpy(&filename[i*FNAMELEN],&filename[(i+1)*FNAMELEN]);
				strcpy(&filename[(i+1)*FNAMELEN],tmp);
				strcpy(tmp,&setname[i*FNAMELEN]);
				strcpy(&setname[i*FNAMELEN],&setname[(i+1)*FNAMELEN]);
				strcpy(&setname[(i+1)*FNAMELEN],tmp);
				strcpy(tmp,&author[i*FNAMELEN]);
				strcpy(&author[i*FNAMELEN],&author[(i+1)*FNAMELEN]);
				strcpy(&author[(i+1)*FNAMELEN],tmp);
			}
		}
	}
}

byte GetLevelNames(MGLDraw *mgl)
{
	char s[64];

	if(onlinePossible && levelSet==0)
		return 1;

	sprintf(s,"levels/%s",&filename[levelSet*FNAMELEN]);
	if(worldLoaded)
	{
		FreeWorld(&world);
		worldLoaded=0;
	}
	// load the world selected
	if(!LoadWorld(&world,s,mgl))
		return 0;
	else
		worldLoaded=1;
	LoadAddOnScores(s);

	level=0;
	levelPos=0;

	return 1;
}

void DeleteAddOn(void)
{
	char s[64];

	sprintf(s,"levels/%s",&filename[levelSet*FNAMELEN]);
	remove(s);
	s[strlen(s)-3]='h';
	s[strlen(s)-2]='i';
	s[strlen(s)-1]='g';
	remove(s);
}

byte UpdateAddOnMenu(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte t;
	static byte fwClock;
	byte btn;
	int msx,msy;
	int i;

	JamulSoundUpdate();

	if(*lastTime>TIME_PER_FRAME*MAX_RUNS)
		*lastTime=TIME_PER_FRAME*MAX_RUNS;

	while(*lastTime>=TIME_PER_FRAME)
	{
		Music_Update();

		for(t=0;t<6;t++)
		{
			if(cursor==t && txtLevel[t]<8)
				txtLevel[t]++;
			if(cursor!=t && txtLevel[t]>0)
				txtLevel[t]--;
		}

		mgl->GetMouse(&msx,&msy);
		btn=mgl->MouseTap();

		c=mgl->LastKeyPressed();
		t=GetTaps()|GetArrowTaps();

		button[0]=button[1]=button[2]=0;

		if(online==1)
		{
			if(!Update_Download(mgl,c,t,oldmsx,oldmsy,btn*(!oldMsBtn)))
			{
				ScanForAddOns();
				online=0;
			}
		}
		else switch(pickMode)
		{
			case 0:	// picking a levelset
				if(t&CONTROL_UP)
				{
					levelSet--;
					if(levelSet<0)
					{
						levelSet=0;
					}
					else
						MakeNormalSound(SND_MENUCLICK);
					if(levelSet<setPos)
						setPos--;
				}
				if(t&CONTROL_DN)
				{
					levelSet++;
					if(levelSet>=numSets)
					{
						levelSet=numSets-1;
					}
					else
						MakeNormalSound(SND_MENUCLICK);

					if(levelSet>setPos+AOCOUNT-1)
						setPos++;
				}
				if(t&CONTROL_B1)
				{
					MakeNormalSound(SND_MENUSELECT);
					if(onlinePossible && levelSet==0)
					{
						online=1;
						Download_Addons();
					}
					else
					{
						if(GetLevelNames(mgl))
							pickMode=1;
						else
							MakeNormalSound(SND_MENUCANCEL);
					}
				}

				if(c==27)
				{
					play=0;
					return 0;
				}

				if(msx!=oldmsx || msy!=oldmsy)
				{
					for(i=setPos;i<setPos+AOCOUNT;i++)
					{
						if(msy>AOTEXTY+AOHEIGHT*(i-setPos) && msy<AOTEXTY+AOHEIGHT*(i-setPos+1))
						{
							if(levelSet!=i && i<numSets)
							{
								MakeNormalSound(SND_MENUCLICK);
								levelSet=i;
							}
						}
					}
				}
				if(msx>600-10 && msx<600+10 && msy>AOTEXTY-20 && msy<AOTEXTY-0)
				{
					button[0]=1;
				}
				if(msx>600-10 && msx<600+10 && msy>AOTEXTY+AOHEIGHT*AOCOUNT+0 && msy<AOTEXTY+AOHEIGHT*AOCOUNT+20)
				{
					button[1]=1;
				}

				if(c==8 && levelSet!=0)
				{
					pickMode=4;
				}

				if(btn && !oldMsBtn)
				{
					for(i=setPos;i<setPos+AOCOUNT;i++)
					{
						if(msy>AOTEXTY+AOHEIGHT*(i-setPos) && msy<AOTEXTY+AOHEIGHT*(i-setPos+1))
						{
							if(i<numSets)
							{
								levelSet=i;
								MakeNormalSound(SND_MENUSELECT);
								if(onlinePossible && levelSet==0)
								{
									online=1;
									Download_Addons();
								}
								else
								{
									if(GetLevelNames(mgl))
										pickMode=1;
									else
										MakeNormalSound(SND_MENUCANCEL);
								}
							}
						}
					}
					if(msx>600-10 && msx<600+10 && msy>AOTEXTY-20 && msy<AOTEXTY-0)
					{
						button[0]=1;
						if(setPos>0)
						{
							setPos-=(AOCOUNT-2);
							if(setPos<0)
								setPos=0;
							MakeNormalSound(SND_MENUSELECT);
						}
					}
					if(msx>600-10 && msx<600+10 && msy>AOTEXTY+AOHEIGHT*AOCOUNT+0 && msy<AOTEXTY+AOHEIGHT*AOCOUNT+20)
					{
						button[1]=1;
						if(setPos<numSets-AOCOUNT)
						{
							setPos+=(AOCOUNT-2);
							if(setPos>numSets-AOCOUNT)
								setPos=numSets-AOCOUNT;
							else
								MakeNormalSound(SND_MENUSELECT);
						}
					}
				}
				break;
			case 1:	 // picking a level to play in the set
				if(t&CONTROL_UP)
				{
					level--;
					if(level>250)
					{
						level=0;
					}
					else
						MakeNormalSound(SND_MENUCLICK);

					if(level<levelPos)
						levelPos--;
				}
				if(t&CONTROL_DN)
				{
					level++;
					if(level>=world.numMaps)
					{
						level=world.numMaps-1;
					}
					else
						MakeNormalSound(SND_MENUCLICK);

					if(level>levelPos+AOCOUNT-1)
						levelPos++;
				}
				if(t&CONTROL_B1)
				{
					pickMode=2;
					MakeNormalSound(SND_MENUSELECT);
				}

				if(c==27)
				{
					pickMode=0;
					return 1;
				}

				if(msx!=oldmsx || msy!=oldmsy)
				{
					for(i=levelPos;i<levelPos+AOCOUNT;i++)
					{
						if(msy>AOTEXTY+AOHEIGHT*(i-levelPos) && msy<AOTEXTY+AOHEIGHT*(i-levelPos+1))
						{
							if(level!=i && i<world.numMaps)
							{
								MakeNormalSound(SND_MENUCLICK);
								level=i;
							}
						}
					}
				}
				if(msx>600-10 && msx<600+10 && msy>AOTEXTY-20 && msy<AOTEXTY-0)
				{
					button[0]=1;
				}
				if(msx>600-10 && msx<600+10 && msy>AOTEXTY+AOHEIGHT*AOCOUNT+0 && msy<AOTEXTY+AOHEIGHT*AOCOUNT+20)
				{
					button[1]=1;
				}

				if(btn && !oldMsBtn)
				{
					for(i=levelPos;i<levelPos+AOCOUNT;i++)
					{
						if(msy>AOTEXTY+AOHEIGHT*(i-levelPos) && msy<AOTEXTY+AOHEIGHT*(i-levelPos+1))
						{
							if(i<world.numMaps)
							{
								MakeNormalSound(SND_MENUSELECT);
								level=i;
								pickMode=2;
							}
						}
					}
					if(msx>600-10 && msx<600+10 && msy>AOTEXTY-20 && msy<AOTEXTY-0)
					{
						button[0]=1;
						if(levelPos>0)
						{
							levelPos-=(AOCOUNT-2);
							if(levelPos>200)
								levelPos=0;
							MakeNormalSound(SND_MENUSELECT);
						}
					}
					if(msx>600-10 && msx<600+10 && msy>AOTEXTY+AOHEIGHT*AOCOUNT+0 && msy<AOTEXTY+AOHEIGHT*AOCOUNT+20)
					{
						button[1]=1;
						if(levelPos<world.numMaps-AOCOUNT)
						{
							levelPos+=(AOCOUNT-2);
							if(levelPos>world.numMaps-AOCOUNT)
								levelPos=world.numMaps-AOCOUNT;
							MakeNormalSound(SND_MENUSELECT);
						}
					}
				}
				break;
			case 2: // what do you want to do with the level?
				if(t&CONTROL_DN)
				{
					cursor++;
					if(cursor>3)
						cursor=0;
					MakeNormalSound(SND_MENUCLICK);
				}
				if(t&CONTROL_UP)
				{
					cursor--;
					if(cursor>3)
						cursor=3;
					MakeNormalSound(SND_MENUCLICK);
				}

				if(t&(CONTROL_B1|CONTROL_B2))
				{
					switch(cursor)
					{
						case 0:	// begin
							play=1;
							MakeNormalSound(SND_MENUSELECT);
							return 0;
						case 1:	// chg map
							pickMode=1;
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 2:	// clear scores
							pickMode=3;
							cursor=0;
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 3:	// exit
							play=0;
							MakeNormalSound(SND_MENUSELECT);
							return 0;
					}
				}

				if(c==27)
				{
					pickMode=1;
					return 1;
				}

				if(msx!=oldmsx || msy!=oldmsy)
				{
					for(i=0;i<4;i++)
					{
						if(msx>10 && msx<280 && msy>310-16+i*35 && msy<310+19+i*35)
						{
							if(cursor!=i)
								MakeNormalSound(SND_MENUCLICK);
							cursor=i;
						}
					}
					if(msx>600-10 && msx<600+10 && msy>AOTEXTY-20 && msy<AOTEXTY-0)
					{
						button[0]=1;
					}
					if(msx>600-10 && msx<600+10 && msy>AOTEXTY+AOHEIGHT*AOCOUNT+0 && msy<AOTEXTY+AOHEIGHT*AOCOUNT+20)
					{
						button[1]=1;
					}
				}
				if(btn && !oldMsBtn)
				{
					for(i=levelPos;i<levelPos+AOCOUNT;i++)
					{
						if(msy>AOTEXTY+AOHEIGHT*(i-levelPos) && msy<AOTEXTY+AOHEIGHT*(i-levelPos+1))
						{
							if(i<world.numMaps)
							{
								MakeNormalSound(SND_MENUSELECT);
								if(level==i)
								{
									play=1;
									MakeNormalSound(SND_MENUSELECT);
									return 0;
								}
								else
								{
									level=i;
									pickMode=2;
								}
							}
						}
					}
					for(i=0;i<4;i++)
					{
						if(msx>10 && msx<280 && msy>310-16+i*35 && msy<310+19+i*35)
						{
							switch(cursor)
							{
								case 0:	// begin
									play=1;
									MakeNormalSound(SND_MENUSELECT);
									return 0;
								case 1:	// chg map
									pickMode=1;
									MakeNormalSound(SND_MENUSELECT);
									break;
								case 2:	// clear scores
									pickMode=3;
									cursor=0;
									MakeNormalSound(SND_MENUSELECT);
									break;
								case 3:	// exit
									play=0;
									MakeNormalSound(SND_MENUSELECT);
									return 0;

							}
						}
					}
					if(msx>600-10 && msx<600+10 && msy>AOTEXTY-20 && msy<AOTEXTY-0)
					{
						button[0]=1;
						if(levelPos>0)
						{
							levelPos-=(AOCOUNT-2);
							if(levelPos>200)
								levelPos=0;
							MakeNormalSound(SND_MENUSELECT);
						}
					}
					if(msx>600-10 && msx<600+10 && msy>AOTEXTY+AOHEIGHT*AOCOUNT+0 && msy<AOTEXTY+AOHEIGHT*AOCOUNT+20)
					{
						button[1]=1;
						if(levelPos<world.numMaps-AOCOUNT)
						{
							levelPos+=(AOCOUNT-2);
							if(levelPos>world.numMaps-AOCOUNT)
								levelPos=world.numMaps-AOCOUNT;
							MakeNormalSound(SND_MENUSELECT);
						}
					}
				}
				break;
			case 3:	// really want to change high scores?
				if(t&CONTROL_DN)
				{
					cursor++;
					if(cursor>1)
						cursor=0;
					MakeNormalSound(SND_MENUCLICK);
				}
				if(t&CONTROL_UP)
				{
					cursor--;
					if(cursor>1)
						cursor=1;
					MakeNormalSound(SND_MENUCLICK);
				}

				if(t&(CONTROL_B1|CONTROL_B2))
				{
					switch(cursor)
					{
						case 0:	// don't clear scores
							pickMode=2;
							cursor=2;
							MakeNormalSound(SND_MENUCANCEL);
							break;
						case 1:	// clear them
							ResetHighScore(GAME_ADDON,level);
							SaveAddOnScores();
							pickMode=2;
							cursor=2;
							MakeNormalSound(SND_MENUSELECT);
							break;
					}
				}
				if(c==27)
				{
					pickMode=2;
					cursor=2;
					MakeNormalSound(SND_MENUCANCEL);
				}

				if(msx!=oldmsx || msy!=oldmsy)
				{
					for(i=0;i<2;i++)
					{
						if(msx>10 && msx<280 && msy>310+i*40 && msy<310+25+i*40)
						{
							if(cursor!=i)
								MakeNormalSound(SND_MENUCLICK);
							cursor=i;
						}
					}
				}
				if(btn && !oldMsBtn)
				{
					for(i=0;i<2;i++)
					{
						if(msx>10 && msx<280 && msy>310+i*40 && msy<310+25+i*40)
						{
							cursor=i;
							switch(cursor)
							{
								case 0:	// don't clear scores
									pickMode=2;
									cursor=2;
									MakeNormalSound(SND_MENUCANCEL);
									break;
								case 1:	// clear them
									ResetHighScore(GAME_ADDON,level);
									SaveAddOnScores();
									pickMode=2;
									cursor=2;
									MakeNormalSound(SND_MENUSELECT);
									break;
							}
						}
					}
				}
				break;
			case 4:	// really delete this add-on?
				if(t&CONTROL_DN)
				{
					cursor++;
					if(cursor>1)
						cursor=0;
					MakeNormalSound(SND_MENUCLICK);
				}
				if(t&CONTROL_UP)
				{
					cursor--;
					if(cursor>1)
						cursor=1;
					MakeNormalSound(SND_MENUCLICK);
				}

				if(t&(CONTROL_B1|CONTROL_B2))
				{
					switch(cursor)
					{
						case 0:	// don't delete it
							pickMode=0;
							MakeNormalSound(SND_MENUCANCEL);
							break;
						case 1:	// delete it
							DeleteAddOn();
							ScanForAddOns();
							pickMode=0;
							levelSet--;
							MakeNormalSound(SND_MENUSELECT);
							break;
					}
				}
				if(c==27)
				{
					pickMode=0;
					MakeNormalSound(SND_MENUCANCEL);
				}

				if(msx!=oldmsx || msy!=oldmsy)
				{
					for(i=0;i<2;i++)
					{
						if(msx>10 && msx<280 && msy>310+i*40 && msy<310+25+i*40)
						{
							if(cursor!=i)
								MakeNormalSound(SND_MENUCLICK);
							cursor=i;
						}
					}
				}
				if(btn && !oldMsBtn)
				{
					for(i=0;i<2;i++)
					{
						if(msx>10 && msx<280 && msy>310+i*40 && msy<310+25+i*40)
						{
							cursor=i;
							switch(cursor)
							{
								case 0:	// don't delete it
									pickMode=0;
									MakeNormalSound(SND_MENUCANCEL);
									break;
								case 1:	// delete it
									DeleteAddOn();
									ScanForAddOns();
									pickMode=0;
									levelSet--;
									MakeNormalSound(SND_MENUSELECT);
									break;
							}
						}
					}
				}
				break;
		}

		oldMsBtn=btn;
		oldmsx=msx;
		oldmsy=msy;

		*lastTime-=TIME_PER_FRAME;
	}
	return 1;
}

void RenderAddOnMenu(MGLDraw *mgl)
{
	char txt[4][32]={
		"Play This Level",
		"Pick Another Level",
		"Reset High Scores",
		"Exit",
	};
	char yesnotxt[2][32]={
		"Nevermind!",
		"Really Reset High Scores",
	};
	char yesnotxt2[2][32]={
		"Nevermind!",
		"Really Delete This Add-On Set",
	};
	char s[64];

	int i,x;

	Music_Update();

	/*
	if(!GM_doDraw)
		return;
	*/

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	switch(pickMode)
	{
		case 0:	// pick a set
			PrintUnGlow(5,AOTEXTY+AOHEIGHT*AOCOUNT+15,"Select a Level Set, and press Fire",0);
			Print(5-2,AOTEXTY+AOHEIGHT*AOCOUNT+15-2,"Select a Level Set, and press Fire",0,0);
			PrintUnGlow(5,AOTEXTY+AOHEIGHT*(AOCOUNT+1)+15,"Press Backspace to delete the selected Add-On Set",0);
			Print(5-2,AOTEXTY+AOHEIGHT*(AOCOUNT+1)+15-2,"Press Backspace to delete the selected Add-On Set",0,0);
			PrintUnGlow(5,AOTEXTY+AOHEIGHT*(AOCOUNT+2)+15,"Press ESC to Exit",0);
			Print(5-2,AOTEXTY+AOHEIGHT*(AOCOUNT+2)+15-2,"Press ESC to Exit",0,0);
			if(numSets==0)
			{
				PrintUnGlow(AOTEXTX,AOTEXTY,"No Add-On Sets Available!",0);
				Print(AOTEXTX-2,AOTEXTY-2,"No Add-On Sets Available!",0,0);
			}
			else for(i=setPos;i<setPos+AOCOUNT;i++)
			{
				if(i<numSets)
				{
					PrintUnGlow(AOTEXTX,AOTEXTY+AOHEIGHT*(i-setPos),&setname[FNAMELEN*i],0);
					RightPrintUnGlow(640-AOTEXTX,AOTEXTY+AOHEIGHT*(i-setPos),&author[FNAMELEN*i],0);
					if(i==levelSet)
					{
						Print(AOTEXTX-2,AOTEXTY+AOHEIGHT*(i-setPos)-2,&setname[FNAMELEN*i],0,0);
						RightPrint(640-AOTEXTX-2,AOTEXTY+AOHEIGHT*(i-setPos)-2,&author[FNAMELEN*i],0,0);
					}
				}
			}
			if(setPos>0)
				blowSpr->GetSprite(10+button[0])->Draw(600,AOTEXTY-10,mgl);
			else
				blowSpr->GetSprite(10)->DrawOffColor(600,AOTEXTY-10,mgl,4,0,0);
			if(setPos<numSets-AOCOUNT)
				blowSpr->GetSprite(12+button[1])->Draw(600,AOTEXTY+AOHEIGHT*AOCOUNT+10,mgl);
			else
				blowSpr->GetSprite(12)->DrawOffColor(600,AOTEXTY+AOHEIGHT*AOCOUNT+10,mgl,4,0,0);
			break;
		case 1:	// pick a level
			sprintf(s,"Select a level in \"%s\"",&setname[FNAMELEN*levelSet]);
			PrintUnGlow(5,AOTEXTY+AOHEIGHT*AOCOUNT+15,s,0);
			Print(5-2,AOTEXTY+AOHEIGHT*AOCOUNT+15-2,s,0,0);
			PrintUnGlow(5,AOTEXTY+AOHEIGHT*(AOCOUNT+1)+15,"Press ESC to pick a different Level Set",0);
			Print(5-2,AOTEXTY+AOHEIGHT*(AOCOUNT+1)+15-2,"Press ESC to pick a different Level Set",0,0);
			for(i=levelPos;i<levelPos+AOCOUNT;i++)
			{
				if(i<world.numMaps)
				{
					PrintUnGlow(AOTEXTX,AOTEXTY+AOHEIGHT*(i-levelPos),world.map[i]->name,0);
					if(i==level)
						Print(AOTEXTX-2,AOTEXTY+AOHEIGHT*(i-levelPos)-2,world.map[i]->name,0,0);
				}
			}
			if(worldLoaded)
			{
				world.map[level]->RenderTiny(&world,315,235);
				RenderHighScore(GAME_ADDON,level,2,415,270,mgl);
				RenderHighScore(GAME_ADDON,level,1,415,270+60,mgl);
				RenderHighScore(GAME_ADDON,level,0,415,270+60*2,mgl);
			}
			if(levelPos>0)
				blowSpr->GetSprite(10+button[0])->Draw(600,AOTEXTY-10,mgl);
			else
				blowSpr->GetSprite(10)->DrawOffColor(600,AOTEXTY-10,mgl,4,0,0);
			if(levelPos<world.numMaps-AOCOUNT)
				blowSpr->GetSprite(12+button[1])->Draw(600,AOTEXTY+AOHEIGHT*AOCOUNT+10,mgl);
			else
				blowSpr->GetSprite(12)->DrawOffColor(600,AOTEXTY+AOHEIGHT*AOCOUNT+10,mgl,4,0,0);
			break;
		case 2:	// do something with the level
			for(i=levelPos;i<levelPos+AOCOUNT;i++)
			{
				if(i<world.numMaps)
				{
					PrintUnGlow(AOTEXTX,AOTEXTY+AOHEIGHT*(i-levelPos),world.map[i]->name,0);
					if(i==level)
						Print(AOTEXTX-2,AOTEXTY+AOHEIGHT*(i-levelPos)-2,world.map[i]->name,0,0);
				}
			}
			if(worldLoaded)
			{
				world.map[level]->RenderTiny(&world,315,235);
				RenderHighScore(GAME_ADDON,level,2,415,270,mgl);
				RenderHighScore(GAME_ADDON,level,1,415,270+60,mgl);
				RenderHighScore(GAME_ADDON,level,0,415,270+60*2,mgl);
			}
			for(i=0;i<4;i++)
			{
				PrintUnGlow(10,310+i*40,txt[i],0);
				for(x=0;x<8;x++)
				{
					if(txtLevel[i]>x)
					{
						if(x==7)
						{
							Print(10+x-1,310-x+i*40-1,txt[i],-32,0);
							Print(10+x+1,310-x+i*40+1,txt[i],-32,0);
							Print(10+x-1,310-x+i*40+1,txt[i],-32,0);
							Print(10+x+1,310-x+i*40-1,txt[i],-32,0);
						}

						Print(10+x,310-x+i*40,txt[i],-32+x*6,0);
					}
				}
			}
			if(levelPos>0)
				blowSpr->GetSprite(10+button[0])->Draw(600,AOTEXTY-10,mgl);
			else
				blowSpr->GetSprite(10)->DrawOffColor(600,AOTEXTY-10,mgl,4,0,0);
			if(levelPos<world.numMaps-AOCOUNT)
				blowSpr->GetSprite(12+button[1])->Draw(600,AOTEXTY+AOHEIGHT*AOCOUNT+10,mgl);
			else
				blowSpr->GetSprite(12)->DrawOffColor(600,AOTEXTY+AOHEIGHT*AOCOUNT+10,mgl,4,0,0);
			break;
		case 3: // clear high scores
			for(i=0;i<2;i++)
			{
				PrintUnGlow(10,310+i*40,yesnotxt[i],0);
				for(x=0;x<8;x++)
				{
					if(txtLevel[i]>x)
					{
						if(x==7)
						{
							Print(10+x-1,310-x+i*40-1,yesnotxt[i],-32,0);
							Print(10+x+1,310-x+i*40+1,yesnotxt[i],-32,0);
							Print(10+x-1,310-x+i*40+1,yesnotxt[i],-32,0);
							Print(10+x+1,310-x+i*40-1,yesnotxt[i],-32,0);
						}

						Print(10+x,310-x+i*40,yesnotxt[i],-32+x*6,0);
					}
				}
			}
			if(worldLoaded)
			{
				world.map[level]->RenderTiny(&world,315,235);
				RenderHighScore(GAME_ADDON,level,2,415,270,mgl);
				RenderHighScore(GAME_ADDON,level,1,415,270+60,mgl);
				RenderHighScore(GAME_ADDON,level,0,415,270+60*2,mgl);
			}

			break;
		case 4: // delete set
			if(numSets==0)
			{
				PrintUnGlow(AOTEXTX,AOTEXTY,"No Add-On Sets Available!",0);
				Print(AOTEXTX-2,AOTEXTY-2,"No Add-On Sets Available!",0,0);
			}
			else for(i=setPos;i<setPos+AOCOUNT;i++)
			{
				if(i<numSets)
				{
					PrintUnGlow(AOTEXTX,AOTEXTY+AOHEIGHT*(i-setPos),&setname[FNAMELEN*i],0);
					RightPrintUnGlow(640-AOTEXTX,AOTEXTY+AOHEIGHT*(i-setPos),&author[FNAMELEN*i],0);
					if(i==levelSet)
					{
						Print(AOTEXTX-2,AOTEXTY+AOHEIGHT*(i-setPos)-2,&setname[FNAMELEN*i],0,0);
						RightPrint(640-AOTEXTX-2,AOTEXTY+AOHEIGHT*(i-setPos)-2,&author[FNAMELEN*i],0,0);
					}
				}
			}
			for(i=0;i<2;i++)
			{
				PrintUnGlow(10,310+i*40,yesnotxt2[i],0);
				for(x=0;x<8;x++)
				{
					if(txtLevel[i]>x)
					{
						if(x==7)
						{
							Print(10+x-1,310-x+i*40-1,yesnotxt2[i],-32,0);
							Print(10+x+1,310-x+i*40+1,yesnotxt2[i],-32,0);
							Print(10+x-1,310-x+i*40+1,yesnotxt2[i],-32,0);
							Print(10+x+1,310-x+i*40-1,yesnotxt2[i],-32,0);
						}

						Print(10+x,310-x+i*40,yesnotxt2[i],-32+x*6,0);
					}
				}
			}
			break;
	}

	if(online)
		Render_Download(mgl,oldmsx,oldmsy);
	blowSpr->GetSprite(9)->Draw(oldmsx,oldmsy,mgl);
}

TASK(byte) AddOnMenu(MGLDraw *mgl)
{
	byte b;
	int lastTime=1;
	dword now,then,hangon;
	char s[64];
	EndClock();
	hangon=TimeLength();

	now=timeGetTime();
	InitAddOnMenu();
	b=1;
	while(b==1)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdateAddOnMenu(&lastTime,mgl);
		RenderAddOnMenu(mgl);
		AWAIT mgl->Flip();
		if(!mgl->Process())
		{
			ExitAddOnMenu();
			CO_RETURN WORLD_ABORT;
		}
		EndClock();
		if(play)
		{
			sprintf(s,"levels/%s",&filename[levelSet*FNAMELEN]);
			player.levelNum=level;
			b=AWAIT LunaticWorld(levelSet,s);
			if(b!=WORLD_ABORT)
				b=1;
			play=0;
			pickMode=1;
			mgl->LastKeyPressed();
			GetTaps();
			GetArrowTaps();
			ResetClock(hangon);
		}
	}
	then=timeGetTime();
	ResetClock(hangon);
	ExitAddOnMenu();
	CO_RETURN WORLD_ABORT;
}
