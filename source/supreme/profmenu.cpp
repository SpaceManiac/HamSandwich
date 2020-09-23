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
#include "lsdir.h"
#if __linux__
#include <unistd.h>
#endif

#define PBTN_HEIGHT	19

#define PROF_NORMAL	0
#define PROF_DELETE	1
#define PROF_KEYCONFIG 2

#define BTN_PLAYLIST	1
#define BTN_DEL_PROF	2
#define BTN_BRAINRADAR	3
#define BTN_CANDLERADAR	4
#define BTN_MOVENSHOOT	5
#define BTN_EXIT		6
#define BTN_KEYCONFIG	7
#define BTN_DIFFICULTY	8
#define BTN_HYPERMODE	9
#define BTN_SUPREME		10
#define BTN_CHARACTER	11
#define BTN_RECORDS		12

#define MAX_PROFS	20
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
	{20,60,122,"Difficulty",BTN_DIFFICULTY},
	{20,90,122,"Character",BTN_CHARACTER},
	{20,120,122,"Brain Radar",BTN_BRAINRADAR},
	{20,150,122,"Candle Radar",BTN_CANDLERADAR},
	{20,180,122,"Hyper Mode",BTN_HYPERMODE},
	{20,210,122,"Supreme Mode",BTN_SUPREME},
	{20,240,122,"Move N' Shoot",BTN_MOVENSHOOT},
	{20,270,200,"Record Book",BTN_RECORDS},
	{20,300,200,"Configure Keys",BTN_KEYCONFIG},
	{20,330,200,"Edit Song Playlists",BTN_PLAYLIST},
	{20,360,200,"Delete This Profile",BTN_DEL_PROF},
	{20,390,200,"Save & Exit",BTN_EXIT},
};
#define NUM_PROF_BTNS	12

static profButton_t kcBtn[]={
	{90,90,102,"",BTN_K1+0},
	{90,112,102,"",BTN_K1+1},
	{90,134,102,"",BTN_K1+2},
	{90,156,102,"",BTN_K1+3},
	{90,178,102,"",BTN_K1+4},
	{90,200,102,"",BTN_K1+5},

	{196,90,102,"",BTN_K2+0},
	{196,112,102,"",BTN_K2+1},
	{196,134,102,"",BTN_K2+2},
	{196,156,102,"",BTN_K2+3},
	{196,178,102,"",BTN_K2+4},
	{196,200,102,"",BTN_K2+5},

	{20,300,200,"Default Controls",BTN_DEFAULT},
	{20,350,200,"Exit",BTN_EXITKEYS},
};
#define NUM_KC_BTNS	14

static byte mode,kcMode,kBtnNum,canHitKeys;
static int msx,msy;

static byte *backgd;
static sprite_set_t *plSpr;
static char msBright,msDBright;

static char fileList[PRFNAME_LEN*MAX_PROFS];
static int numFiles;
static byte profChoice;

static char diffName[][16]={"Wimpy","Normal","Hard","Lunatic","Jamulio"};
static char charName[][16]={"Bouapha","Happy Stick Man","Dr. Lunatic","Shtupid Shroom","LunaChick","MechaBouapha","Wolfman","Wild Wizard","Kid Mystic","Young Loony"};

static byte recordBook,candleRadar,brainRadar,moveNShoot,hyperMode,supremeMode;

static char *ShortName(const char *name)
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

void InitProfMenu(MGLDraw *mgl)
{
	int i;

	mode=PROF_NORMAL;

	msBright=0;
	msDBright=1;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);
	plSpr=new sprite_set_t("graphics/pause.jsp");

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	ScanProfiles();
	recordBook=ItemPurchased(SHOP_MAJOR,MAJOR_RECORDBOOK);
	candleRadar=ItemPurchased(SHOP_ABILITY,ABIL_CANDLE);
	brainRadar=ItemPurchased(SHOP_ABILITY,ABIL_BRAIN);
	moveNShoot=ItemPurchased(SHOP_ABILITY,ABIL_MOVESHOOT);
	hyperMode=ItemPurchased(SHOP_ABILITY,ABIL_HYPER);
	supremeMode=ItemPurchased(SHOP_ABILITY,ABIL_SUPREME);
}

void ExitProfMenu(void)
{
	free(backgd);
	delete plSpr;
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
							case BTN_BRAINRADAR:
								if(ItemPurchased(SHOP_ABILITY,ABIL_BRAIN))
									profile.brainRadar=1-profile.brainRadar;
								break;
							case BTN_CANDLERADAR:
								if(ItemPurchased(SHOP_ABILITY,ABIL_CANDLE))
									profile.candleRadar=1-profile.candleRadar;
								break;
							case BTN_MOVENSHOOT:
								if(ItemPurchased(SHOP_ABILITY,ABIL_MOVESHOOT))
									profile.moveNShoot=1-profile.moveNShoot;
								break;
							case BTN_HYPERMODE:
								if(ItemPurchased(SHOP_ABILITY,ABIL_HYPER))
									profile.hyperMode=1-profile.hyperMode;
								break;
							case BTN_SUPREME:
								if(ItemPurchased(SHOP_ABILITY,ABIL_SUPREME))
									profile.supremeMode=1-profile.supremeMode;
								break;
							case BTN_RECORDS:
								if(recordBook)
									RecordBook(mgl);
								break;
							case BTN_DIFFICULTY:
								profile.difficulty++;
								if(profile.difficulty>4)
									profile.difficulty=0;
								break;
							case BTN_CHARACTER:
								profile.playAs++;
								if(profile.playAs>PLAY_LOONY)
									profile.playAs=0;
								while(profile.playAs!=PLAY_BOUAPHA && !ItemPurchased(SHOP_PLAYABLE,profile.playAs))
								{
									profile.playAs++;
									if(profile.playAs>5)
										profile.playAs=0;
								}
								break;
							case BTN_PLAYLIST:
								return 2;
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
					i=(msy-39)/18;
					if(i<numFiles)	// selecting a profile
					{
						MakeNormalSound(SND_MENUSELECT);
						SaveProfile();
						FreeProfile();
						profChoice=i;
						LoadProfile(ShortName(&fileList[i*PRFNAME_LEN]));
						recordBook=ItemPurchased(SHOP_MAJOR,MAJOR_RECORDBOOK);
						candleRadar=ItemPurchased(SHOP_ABILITY,ABIL_CANDLE);
						brainRadar=ItemPurchased(SHOP_ABILITY,ABIL_BRAIN);
						moveNShoot=ItemPurchased(SHOP_ABILITY,ABIL_MOVESHOOT);
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

					mode=PROF_NORMAL;
					sprintf(s,"profiles/%s.prf",profile.name);
					FreeProfile();
					unlink(s);
					if(numFiles==1)	// this was the only profile
					{
						return 3;	// so force them to create a new profile
					}
					else
					{
						if(profChoice>0)
							LoadProfile(ShortName(&fileList[0]));
						else	// load profile #1 if you just deleted #0
							LoadProfile(ShortName(&fileList[PRFNAME_LEN]));
						ScanProfiles();
						recordBook=ItemPurchased(SHOP_MAJOR,MAJOR_RECORDBOOK);
						candleRadar=ItemPurchased(SHOP_ABILITY,ABIL_CANDLE);
						brainRadar=ItemPurchased(SHOP_ABILITY,ABIL_BRAIN);
						moveNShoot=ItemPurchased(SHOP_ABILITY,ABIL_MOVESHOOT);
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

void RenderProfButton(int x,int y,int wid,const char *txt,MGLDraw *mgl)
{
	if((mode==PROF_NORMAL || mode==PROF_KEYCONFIG) && PointInRect(msx,msy,x,y,x+wid,y+PBTN_HEIGHT))
	{
		mgl->Box(x,y,x+wid,y+PBTN_HEIGHT,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+PBTN_HEIGHT-1,32+8);
	}
	else
		mgl->Box(x,y,x+wid,y+PBTN_HEIGHT,32+16);

	PrintGlow(x+2,y+3,txt,0,2);
}

void RenderProfList(int x,int y,MGLDraw *mgl)
{
	int i;

	mgl->FillBox(x,y,x+150,y+361,32*1+4);
	mgl->Box(x,y,x+150,y+361,32*1+16);

	for(i=0;i<numFiles;i++)
	{
		if(i==profChoice)
			mgl->FillBox(x+1,y+1+i*18,x+149,y+i*18+18,32*1+8);
		if(PointInRect(msx,msy,x,y+i*18,x+150,y+i*18+18))
			mgl->Box(x+1,y+1+i*18,x+149,y+i*18+18,32*1+12);
		PrintGlowLimited(x+2,y+2+i*18,x+150,ShortName(&fileList[i*PRFNAME_LEN]),0,2);
	}

	if(i<MAX_PROFS)
	{
		if(PointInRect(msx,msy,x,y+i*18,x+150,y+i*18+18))
			mgl->Box(x+1,y+1+i*18,x+149,y+i*18+18,32*1+12);
		PrintGlowLimited(x+2,y+2+i*18,x+150,"<New Profile>",0,2);
	}
}

void RenderProfMenu(MGLDraw *mgl)
{
	int i;
	int msx2,msy2;
	char s[64];
	static char yesno[][5]={"Off","On"};

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	for(i=0;i<NUM_PROF_BTNS;i++)
	{
		if(btn[i].id==BTN_RECORDS && !recordBook)
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,"?????",mgl);
		else if(btn[i].id==BTN_CANDLERADAR && !candleRadar)
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,"?????",mgl);
		else if(btn[i].id==BTN_BRAINRADAR && !brainRadar)
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,"?????",mgl);
		else if(btn[i].id==BTN_MOVENSHOOT && !moveNShoot)
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,"?????",mgl);
		else if(btn[i].id==BTN_HYPERMODE && !hyperMode)
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,"?????",mgl);
		else if(btn[i].id==BTN_SUPREME && !supremeMode)
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,"?????",mgl);
		else
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,btn[i].txt,mgl);
	}
	
	/*
	for(i=0;i<8;i++)
	{
		if(profile.progress.purchase[modeShopNum[i]]&SIF_ACTIVE)
		PrintGlow(160,64+32*i,"ACTIVATED",0,1);
	}*/

	PrintGlow(472,20,"Available Profiles",0,2);
	RenderProfList(470,38,mgl);

	sprintf(s,"Username: %s",profile.name);
	PrintGlow(20,20,s,0,2);

	PrintGlow(146,62,diffName[profile.difficulty],0,2);
	PrintGlow(146,92,charName[profile.playAs],0,2);
	if(brainRadar)
		PrintGlow(166,122,yesno[profile.brainRadar],0,2);
	if(candleRadar)
		PrintGlow(166,152,yesno[profile.candleRadar],0,2);
	if(hyperMode)
		PrintGlow(166,182,yesno[profile.hyperMode],0,2);
	if(supremeMode)
		PrintGlow(166,212,yesno[profile.supremeMode],0,2);
	if(moveNShoot)
		PrintGlow(166,242,yesno[profile.moveNShoot],0,2);

	if(mode==PROF_DELETE)
	{
		RenderYesNoDialog2(msx,msy,mgl);
	}
	// mouse cursor
	SetSpriteConstraints(13,13,627,467);
	msx2=msx;
	msy2=msy;
	if(msx2<13)
		msx2=13;
	if(msy2<13)
		msy2=13;
	if(msx2>622)
		msx2=622;
	if(msy2>462)
		msy2=462;
	plSpr->GetSprite(0)->DrawBright(msx2,msy2,mgl,msBright/2);
	SetSpriteConstraints(0,0,639,479);
}

void RenderKeyConfigMenu(MGLDraw *mgl)
{
	int i;
	int msx2,msy2;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	for(i=0;i<NUM_KC_BTNS;i++)
		RenderProfButton(kcBtn[i].x,kcBtn[i].y,kcBtn[i].wid,kcBtn[i].txt,mgl);

	PrintGlow(20,20,"Configure Controls",0,2);

	PrintGlow(20,90,"Up",0,2);
	PrintGlow(20,112,"Down",0,2);
	PrintGlow(20,134,"Left",0,2);
	PrintGlow(20,156,"Right",0,2);
	PrintGlow(20,178,"Hammer",0,2);
	PrintGlow(20,200,"Weapon",0,2);

	PrintGlow(90,68,"Keyboard1",0,2);
	PrintGlow(196,68,"Keyboard2",0,2);

	if(kcMode>0)
	{
		PrintGlow(20,228,"Press a key to assign, or ESC to cancel",0,2);
	}

	// mouse cursor
	SetSpriteConstraints(13,13,627,467);
	msx2=msx;
	msy2=msy;
	if(msx2<13)
		msx2=13;
	if(msy2<13)
		msy2=13;
	if(msx2>622)
		msx2=622;
	if(msy2>462)
		msy2=462;
	plSpr->GetSprite(0)->DrawBright(msx2,msy2,mgl,msBright/2);
	SetSpriteConstraints(0,0,639,479);
}

//----------------

void ProfMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	InitProfMenu(mgl);
	PlaySongForce("SWC_02_worldpicker.ogg");

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
