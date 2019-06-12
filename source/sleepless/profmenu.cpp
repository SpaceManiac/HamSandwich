#include "profmenu.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "dialogbits.h"
#include "textdialog.h"
#include "playlist.h"
#include "nameentry.h"
#include "yesnodialog.h"
#include "recordbook.h"
#include "shop.h"
#include "goal.h"
#include "comicbook.h"
#include "customworld.h"
#include "lsdir.h"
#if __linux__
#include <unistd.h>
#endif

#define PBTN_HEIGHT	26

#define PROF_NORMAL	0
#define PROF_DELETE	1
#define PROF_RESTART 2
#define PROF_KEYCONFIG 3

#define BTN_PLAYLIST	1
#define BTN_DEL_PROF	2
#define BTN_BRAINRADAR	3
#define BTN_CANDLERADAR	4
#define BTN_MOVENSHOOT	5
#define BTN_EXIT		6
#define BTN_KEYCONFIG	7
#define BTN_DIFFICULTY	8
#define BTN_CHARACTER	9
#define BTN_RECORDS		10
#define BTN_RESTART		11
#define BTN_COMIC		12

#define MAX_PROFS	16
#define PRFNAME_LEN	32

// keyconfig buttons
#define BTN_K1		10
#define BTN_K2		20
#define BTN_DEFAULT	30
#define BTN_EXITKEYS 31

typedef struct profButton_t
{
	int x,y;
	int wid;
	char txt[32];
	byte id;
} profButton_t;

static profButton_t btn[]={
	{20,320,200,"",BTN_COMIC},
	{20,350,200,"Configure Keys",BTN_KEYCONFIG},
	{20,380,200,"Restart This Profile",BTN_RESTART},
	{20,410,200,"Delete This Profile",BTN_DEL_PROF},
	{20,440,200,"Save & Exit",BTN_EXIT},
};
#define NUM_PROF_BTNS	5

static profButton_t kcBtn[]={
	{80,90,102,"",BTN_K1+0},
	{80,120,102,"",BTN_K1+1},
	{80,150,102,"",BTN_K1+2},
	{80,180,102,"",BTN_K1+3},
	{80,210,102,"",BTN_K1+4},
	{80,240,102,"",BTN_K1+5},

	{200,90,102,"",BTN_K2+0},
	{200,120,102,"",BTN_K2+1},
	{200,150,102,"",BTN_K2+2},
	{200,180,102,"",BTN_K2+3},
	{200,210,102,"",BTN_K2+4},
	{200,240,102,"",BTN_K2+5},

	{20,300,200,"Default Controls",BTN_DEFAULT},
	{20,350,200,"Exit",BTN_EXITKEYS},
};
#define NUM_KC_BTNS	14

static byte mode,kcMode,kBtnNum,canHitKeys;
static int msx,msy;

static byte *backgd;
static char msBright,msDBright;

static char fileList[PRFNAME_LEN*MAX_PROFS];
static int numFiles;
static byte profChoice;

static char *ShortName(char *name)
{
	static char result[128];

	strcpy(result,name);

	if(!strcmp(".prf",&name[strlen(name)-4]))
	{
		result[strlen(name)-4]='\0';
	}
	return result;
}

void SortProfiles(void)
{
	byte flip;
	int i;
	char tmp[PRFNAME_LEN];

	flip=1;

	while(flip)
	{
		flip=0;
		for(i=0;i<numFiles-1;i++)
		{
			if(strcmp(&fileList[i*PRFNAME_LEN],&fileList[(i+1)*PRFNAME_LEN])>0)
			{
				strcpy(tmp,&fileList[i*PRFNAME_LEN]);
				strcpy(&fileList[i*PRFNAME_LEN],&fileList[(i+1)*PRFNAME_LEN]);
				strcpy(&fileList[(i+1)*PRFNAME_LEN],tmp);
				flip=1;
			}
		}
	}
}

void ScanProfiles(void)
{
	int i;

	for(i=0;i<MAX_PROFS;i++)
		fileList[i*PRFNAME_LEN]='\0';
	numFiles=0;

	for (const char* name : filterdir("profiles", ".prf", PRFNAME_LEN))
	{
		strcpy(&fileList[numFiles*PRFNAME_LEN], name);
		if (++numFiles >= MAX_PROFS)
			break;
	}
	SortProfiles();

	profChoice=0;
	for(i=0;i<numFiles;i++)
		if(!strcmp(profile.name,ShortName(&fileList[i*PRFNAME_LEN])))
			profChoice=i;
}

void SetupProfile(void)
{
	int i;

	strcpy(btn[0].txt,"");
	for(i=0;i<NUM_GOALS;i++)
		if(profile.progress.goal[i] && !IsCustomWorld())
		{
			strcpy(btn[0].txt,"Read Comic Book");
		}
}

void InitProfMenu(MGLDraw *mgl)
{
	int i;

	mode=PROF_NORMAL;

	msBright=0;
	msDBright=1;

	mgl->LoadBMP("graphics/pause.bmp");
	backgd=(byte *)malloc(640*480);

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);
	SetupProfile();
	ScanProfiles();
}

void ExitProfMenu(void)
{
	free(backgd);
}

void InitKeyConfig(void)
{
	int i,j;

	for(j=0;j<2;j++)
	{
		for(i=0;i<6;i++)
		{
			strcpy(kcBtn[i+j*6].txt,ScanCodeText(profile.control[j][i]));
		}
	}
	ApplyControlSettings();
}

byte UpdateProfMenu(int *lastTime,MGLDraw *mgl)
{
	int i,j;
	char k;

	int KEY_MAX = 0;
	const byte* key = SDL_GetKeyboardState(&KEY_MAX);

	mgl->GetMouse(&msx,&msy);

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	k=mgl->LastKeyPressed();

	while(*lastTime>=TIME_PER_FRAME)
	{
		msBright+=msDBright;
		if(msBright>10)
			msDBright=-1;
		if(msBright<-2)
			msDBright=1;

		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
	}

	switch(mode)
	{
		case PROF_NORMAL:
			if(mgl->MouseTap())
			{
				for(i=0;i<NUM_PROF_BTNS;i++)
				{
					if(PointInRect(msx,msy,btn[i].x,btn[i].y,btn[i].x+btn[i].wid,btn[i].y+PBTN_HEIGHT))
					{
						MakeNormalSound(SND_MENUSELECT);
						switch(btn[i].id)
						{
							case BTN_COMIC:
#ifndef DEMO
								if(btn[i].txt[0]!='\0')	// only works if you are allowed!
									ComicBook();
#endif
								break;
							case BTN_RESTART:
								mode=PROF_RESTART;
								InitYesNoDialog("Erase all progress other than goals?","Yes","No");
								break;
							case BTN_KEYCONFIG:
								InitKeyConfig();
								mode=PROF_KEYCONFIG;
								kcMode=0;
								break;
							case BTN_DEL_PROF:
								mode=PROF_DELETE;
								InitYesNoDialog("Really delete this profile?","Yes","No");
								break;
							case BTN_EXIT:
								return 1;
								break;
						}
					}
				}
				if(PointInRect(msx,msy,470,39,470+150,38+361))
				{
					// profile list
					i=(msy-39)/PBTN_HEIGHT;
					if(i<numFiles)	// selecting a profile
					{
						MakeNormalSound(SND_MENUSELECT);
						SaveProfile();
						FreeProfile();
						profChoice=i;
						LoadProfile(ShortName(&fileList[i*PRFNAME_LEN]));
						SetupProfile();
					}
					else if(i==numFiles && numFiles<MAX_PROFS)
					{
						SaveProfile();
						MakeNormalSound(SND_MENUSELECT);
						return 3;
					}
				}
			}
			if(k==27)
				return 1;	// exit
			break;
		case PROF_DELETE:
		case PROF_RESTART:
			if(mgl->MouseTap())
			{
				YesNoDialogClick(msx,msy);
			}
			if(k!=0)
				YesNoDialogKey(k);
			switch(YesNoDialogCommand())
			{
				case YNM_YES:
					char s[64];

					if(mode==PROF_DELETE)
					{
						sprintf(s,"profiles/%s.prf",profile.name);
						FreeProfile();
						unlink(s);
						if(numFiles==1)	// this was the only profile
						{
							mode=PROF_NORMAL;
							return 3;	// so force them to create a new profile
						}
						else
						{
							if(profChoice>0)
								LoadProfile(ShortName(&fileList[0]));
							else	// load profile #1 if you just deleted #0
								LoadProfile(ShortName(&fileList[PRFNAME_LEN]));
							ScanProfiles();
							SetupProfile();
						}
					}
					else	// restart
					{
						worldData_t *w;

						for(i=0;i<64;i++) // used to be 19, upped to 64 to tolerate custom worlds
						{
							// delete all maps
							sprintf(s,"profiles/%s.%03d",profile.name,i);
							unlink(s);
						}
						w=GetWorldProgress(profile.lastWorld);
						w->levelOn=0;
						w->percentage=0;
						profile.progress.totalTime=0;
						profile.progress.totalPercent=0;
					}
					mode=PROF_NORMAL;
					break;
				case YNM_NO:
					mode=PROF_NORMAL;
					break;
			}
			break;
		case PROF_KEYCONFIG:
			if(kcMode==0)
			{
				if(mgl->MouseTap())
				{
					for(i=0;i<NUM_KC_BTNS;i++)
					{
						if(PointInRect(msx,msy,kcBtn[i].x,kcBtn[i].y,kcBtn[i].x+kcBtn[i].wid,kcBtn[i].y+PBTN_HEIGHT))
						{
							kBtnNum=i;
							MakeNormalSound(SND_MENUSELECT);
							if(kcBtn[i].id>=BTN_K1 && kcBtn[i].id<BTN_K1+6)
							{
								kcMode=kcBtn[i].id-BTN_K1+1;
								canHitKeys=0;
							}
							else if(kcBtn[i].id>=BTN_K2 && kcBtn[i].id<BTN_K2+6)
							{
								kcMode=kcBtn[i].id-BTN_K2+7;
								canHitKeys=0;
							}
							else if(kcBtn[i].id==BTN_EXITKEYS)
							{
								mode=PROF_NORMAL;
								SaveProfile();
							}
							else if(kcBtn[i].id==BTN_DEFAULT)
							{
								DefaultControls();
								InitKeyConfig();
							}
							if(kcMode!=0)
								mgl->ClearKeys();
						}
					}
				}

				if(k==27)
				{
					mode=PROF_NORMAL;
					SaveProfile();
				}

			}
			else	// entering a key or button (1-6=kb1, 7-12=kb2)
			{
				if(canHitKeys==0)
				{
					j=0;
					for(i=0;i<KEY_MAX;i++)
						if(key[i])
						{
							j++;
							break;
						}
					if(j==0)
						canHitKeys=1;
				}

				if(Random(3)==0)
					strcpy(kcBtn[kBtnNum].txt,"???");
				else
					strcpy(kcBtn[kBtnNum].txt,"? ? ?");

				if(k==27)
				{
					InitKeyConfig();
					kcMode=0;
				}
				else
				{
					if(canHitKeys)
					{
						for(i=0;i<KEY_MAX;i++)
						{
							if(key[i])
							{
								if(kcMode<7)
									profile.control[0][kcMode-1]=(byte)i;
								else
									profile.control[1][kcMode-7]=(byte)i;
								kcMode=0;
								InitKeyConfig();
							}
						}
					}
					/*
					i=mgl->LastRawCode()>>8;
					if(i!=0)
					{
						if(kcMode<7)
							profile.control[0][kcMode-1]=(byte)i;
						else
							profile.control[1][kcMode-7]=(byte)i;
						kcMode=0;
						InitKeyConfig();
					}
					*/
				}
			}
			break;
	}

	return 0;
}

void RenderProfButton(int x,int y,int wid,char *txt,MGLDraw *mgl)
{
	PrintUnGlow(x+2,y+2-5,txt,2);

	if((mode==PROF_NORMAL || mode==PROF_KEYCONFIG) && PointInRect(msx,msy,x,y,x+wid,y+PBTN_HEIGHT))
	{
		PrintUnGlow(x+1,y+1-5,txt,2);
		PrintUnGlow(x+3,y+3-5,txt,2);
		PrintUnGlow(x+1,y+3-5,txt,2);
		PrintUnGlow(x+3,y+1-5,txt,2);
		PrintGlow(x+2,y+2-5,txt,0,2);
	}
}

void RenderProfList(int x,int y,MGLDraw *mgl)
{
	int i;

	mgl->FillBox(x,y,x+150,y+380,32*2+22);
	mgl->Box(x,y,x+150,y+380,32*2+16);

	for(i=0;i<numFiles;i++)
	{
		if(i==profChoice)
			mgl->FillBox(x+1,y+1+i*PBTN_HEIGHT,x+149,y+i*PBTN_HEIGHT+PBTN_HEIGHT-1,31);
		if(PointInRect(msx,msy,x,y+i*PBTN_HEIGHT,x+150,y+i*PBTN_HEIGHT+PBTN_HEIGHT-1))
			mgl->Box(x+1,y+1+i*PBTN_HEIGHT,x+149,y+i*PBTN_HEIGHT+PBTN_HEIGHT-1,31);
	}

	if(i<MAX_PROFS)
	{
		if(PointInRect(msx,msy,x,y+i*PBTN_HEIGHT,x+150,y+i*PBTN_HEIGHT+PBTN_HEIGHT-1))
			mgl->Box(x+1,y+1+i*PBTN_HEIGHT,x+149,y+i*PBTN_HEIGHT+PBTN_HEIGHT-1,31);
	}

	for(i=0;i<numFiles;i++)
	{
		PrintGlowLimited(x+2,y+2+i*PBTN_HEIGHT,x+150,ShortName(&fileList[i*PRFNAME_LEN]),0,2);
	}

	if(i<MAX_PROFS)
	{
		PrintGlowLimited(x+2,y+2+i*PBTN_HEIGHT,x+150,"<New Profile>",0,2);
	}
}

void RenderProfMenu(MGLDraw *mgl)
{
	int i,x,y;
	char s[64];
	//static char yesno[][5]={"Off","On"};

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	for(i=0;i<NUM_PROF_BTNS;i++)
	{
		RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,btn[i].txt,mgl);
	}

	PrintUnGlow(472,10,"Available Profiles",2);
	RenderProfList(470,38,mgl);

	sprintf(s,"Name: %s",profile.name);
	PrintUnGlow(20,20,s,2);

	if(strcmp(profile.lastWorld, "hollow.shw"))
	{
		sprintf(s,"World: %s",CustomWorldTitle(NULL));
		PrintUnGlow(20,110,s,2);
	}

	sprintf(s,"%0.0f%% Complete",profile.progress.totalPercent);
	PrintUnGlow(20,50,s,2);
	sprintf(s,"Play time: %ld:%02ld",profile.progress.totalTime/(30*60*60),(profile.progress.totalTime/(30*60))%60);
	PrintUnGlow(20,80,s,2);

	PrintUnGlow(90,140,"Goals Completed",2);
	x=6;
	y=170;
	for(i=0;i<25;i++)
	{
		mgl->FillBox(x,y,x+58,y+15,32*2+16);
		if(profile.progress.goal[i])
			mgl->FillBox(x+2,y+2,x+56,y+13,31);
		if(PointInRect(msx,msy,x,y,x+58,y+15))
		{
#ifndef DEMO
			mgl->Box(x,y-1,x+58,y+16,31);
			mgl->Box(x-1,y,x+59,y+15,31);
			PrintGoalInfo(10,268,i);
#endif
		}
		x+=61;
		if(x>320-60)
		{
			x=6;
			y+=18;
		}
	}
#ifdef DEMO
	PrintUnGlow(90,190,"NOT IN DEMO!",2);
	PrintUnGlow(92,192,"NOT IN DEMO!",2);
	PrintGlow(91,191,"NOT IN DEMO!",0,2);
#endif

	if(mode==PROF_DELETE || mode==PROF_RESTART)
	{
		RenderYesNoDialog2(msx,msy,mgl);
	}
	// mouse cursor
	DrawMouseCursor(msx,msy);
}

void RenderKeyConfigMenu(MGLDraw *mgl)
{
	int i;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	for(i=0;i<NUM_KC_BTNS;i++)
	{
		RenderProfButton(kcBtn[i].x,kcBtn[i].y,kcBtn[i].wid,kcBtn[i].txt,mgl);
	}
	for(i=0;i<6;i++)
	{
		mgl->Box(75,88+i*30,195,88+i*30+30,0);
		mgl->Box(195,88+i*30,308,88+i*30+30,0);
	}

	PrintUnGlow(20,20,"Configure Controls",2);

	PrintUnGlow(10,90,"Up",2);
	PrintUnGlow(10,120,"Down",2);
	PrintUnGlow(10,150,"Left",2);
	PrintUnGlow(10,180,"Right",2);
	PrintUnGlow(10,210,"Hammer",2);
	PrintUnGlow(10,240,"Mystic",2);

	PrintUnGlow(80,60,"Keyboard1",2);
	PrintUnGlow(200,60,"Keyboard2",2);

	PrintUnGlowRect(330,5,635,475,30,"Click in a box to enter a new key for that action.  You can also play with "
					"a gamepad or the mouse.  Other keys you will use, but can't configure, are:",2);
	PrintUnGlow(330,180,"ESC - Pause the game",2);
	PrintUnGlow(330,220,"1-6 - Quick-select Hammers",2);

	if(kcMode>0)
	{
		PrintUnGlow(20,270,"Press a key to assign, or ESC to cancel",2);
	}

	// mouse cursor
	DrawMouseCursor(msx,msy);
}

//----------------

void ProfMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	InitProfMenu(mgl);
	PlaySong("unspeakable.ogg");

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateProfMenu(&lastTime,mgl);
		if(mode!=PROF_KEYCONFIG)
			RenderProfMenu(mgl);
		else
			RenderKeyConfigMenu(mgl);

		mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();

		if(done==2)
		{
			PlayListMenu(mgl);
			done=0;
		}
		if(done==3)
		{
			NameEntry(mgl,1);
			done=0;
			ExitProfMenu();
			InitProfMenu(mgl);
		}
	}

	SaveProfile();
	ExitProfMenu();
}
