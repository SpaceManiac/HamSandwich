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
#include "appdata.h"
#if __linux__ || __EMSCRIPTEN__
#include <unistd.h>
#endif

constexpr int PBTN_HEIGHT = 19;

constexpr int MAX_PROFS = 20;
constexpr int PRFNAME_LEN = 32;

enum Mode
{
	PROF_NORMAL,
	PROF_DELETE,
	PROF_KEYCONFIG,
};

// dorky enum-in-namespace to scope the entries but allow math
namespace ButtonIdNs
{
	enum ButtonId
	{
		None,
		Playlist,
		DelProf,
		BrainRadar,
		CandleRadar,
		MoveNShoot,
		Exit,
		KeyConfig,
		Difficulty,
		Character,
		Records,
		// delete profile
		Yes,
		No,
		// keyconfig buttons
		Keys1_0,
		Keys1_1,
		Keys1_2,
		Keys1_3,
		Keys1_4,
		Keys1_5,
		Keys2_0,
		Keys2_1,
		Keys2_2,
		Keys2_3,
		Keys2_4,
		Keys2_5,
		KeysDefault,
		KeysExit,
		// Profile buttons
		Profile_New,
		Profile_0,
		Profile_End = Profile_0 + MAX_PROFS,
	};
}
using ButtonIdNs::ButtonId;

struct profButton_t
{
	int x,y;
	int wid;
	char txt[32];
	ButtonId id;
};

static const profButton_t btn[]={
	{20,60,122,"Difficulty", ButtonId::Difficulty},
	{20,90,122,"Character", ButtonId::Character},
	{20,120,122,"Brain Radar", ButtonId::BrainRadar},
	{20,150,122,"Candle Radar", ButtonId::CandleRadar},
	{20,180,122,"Move N' Shoot", ButtonId::MoveNShoot},
	{20,210,122,"Record Book", ButtonId::Records},
	{20,374,200,"Configure Keys", ButtonId::KeyConfig},
	{20,396,200,"Edit Song Playlists", ButtonId::Playlist},
	{20,418,200,"Delete This Profile", ButtonId::DelProf},
	{20,440,200,"Save & Exit", ButtonId::Exit},
};
constexpr int NUM_PROF_BTNS = SDL_arraysize(btn);

static profButton_t kcBtn[]={
	{90,90,102,"", ButtonId::Keys1_0},
	{90,112,102,"",ButtonId::Keys1_1},
	{90,134,102,"",ButtonId::Keys1_2},
	{90,156,102,"",ButtonId::Keys1_3},
	{90,178,102,"",ButtonId::Keys1_4},
	{90,200,102,"",ButtonId::Keys1_5},

	{196,90,102,"", ButtonId::Keys2_0},
	{196,112,102,"",ButtonId::Keys2_1},
	{196,134,102,"",ButtonId::Keys2_2},
	{196,156,102,"",ButtonId::Keys2_3},
	{196,178,102,"",ButtonId::Keys2_4},
	{196,200,102,"",ButtonId::Keys2_5},

	{20,300,200,"Default Controls", ButtonId::KeysDefault},
	{20,350,200,"Exit", ButtonId::KeysExit},
};
constexpr int NUM_KC_BTNS = SDL_arraysize(kcBtn);

static Mode mode;
static byte kcMode,kBtnNum,canHitKeys;
static int msx,msy;

static byte *backgd;
static std::unique_ptr<sprite_set_t> plSpr;
static char msBright,msDBright;

static char fileList[PRFNAME_LEN*MAX_PROFS];
static int numFiles;
static byte profChoice;

static byte recordBook,candleRadar,brainRadar,moveNShoot;

static ButtonId curButton;
static byte oldc;

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

	auto files = ListDirectory("profiles", ".prf", PRFNAME_LEN);
	for (const auto& str : files)
	{
		const char* name = str.c_str();
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
	plSpr = std::make_unique<sprite_set_t>("graphics/pause.jsp");

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	ScanProfiles();
	recordBook=ItemPurchased(SHOP_MAJOR,MAJOR_RECORDBOOK);
	candleRadar=ItemPurchased(SHOP_ABILITY,ABIL_CANDLE);
	brainRadar=ItemPurchased(SHOP_ABILITY,ABIL_BRAIN);
	moveNShoot=ItemPurchased(SHOP_ABILITY,ABIL_MOVESHOOT);

	curButton = ButtonId::None;
	oldc = ~0;
}

void ExitProfMenu(void)
{
	free(backgd);
	plSpr.reset();
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

	int KEY_MAX = 0;
	const byte* key = SDL_GetKeyboardState(&KEY_MAX);

	int oldMsx = msx, oldMsy = msy;
	mgl->GetMouse(&msx,&msy);
	bool mb = mgl->MouseTap();
	if (mb || msx != oldMsx || msy != oldMsy)
	{
		curButton = ButtonId::None;

		if (mode == PROF_NORMAL)
		{
			for(i=0;i<NUM_PROF_BTNS;i++)
			{
				if(PointInRect(msx,msy,btn[i].x,btn[i].y,btn[i].x+btn[i].wid,btn[i].y+PBTN_HEIGHT))
				{
					curButton = btn[i].id;
				}
			}

			int x = 470, y = 38;
			for(i=0;i<numFiles;i++)
			{
				if(PointInRect(msx,msy,x,y+i*18,x+150,y+i*18+18))
					curButton = ButtonId(ButtonId::Profile_0 + i);
			}
			if(i<MAX_PROFS)
			{
				if(PointInRect(msx,msy,x,y+i*18,x+150,y+i*18+18))
					curButton = ButtonId::Profile_New;
			}
		}
		else if (mode == PROF_DELETE)
		{
			if (PointInRect(msx, msy, 320-100-60,270,320-100-60+60,270+15))
				curButton = ButtonId::Yes;
			else if (PointInRect(msx, msy, 320+100,270,320+100+60,270+15))
				curButton = ButtonId::No;
		}
		else if (mode == PROF_KEYCONFIG)
		{
			if (kcMode == 0)
			{
				for(i=0;i<NUM_KC_BTNS;i++)
				{
					if(PointInRect(msx,msy,kcBtn[i].x,kcBtn[i].y,kcBtn[i].x+kcBtn[i].wid,kcBtn[i].y+PBTN_HEIGHT))
					{
						curButton = kcBtn[i].id;
					}
				}
			}
		}
	}

	byte c = GetControls() | GetArrows();

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;
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

	char k = mgl->LastKeyPressed();

	switch(mode)
	{
		case PROF_NORMAL:
			if (c & ~oldc & CONTROL_UP)
			{
				switch (curButton)
				{
					case ButtonId::Character:
						curButton = ButtonId::Difficulty;
						break;
					case ButtonId::BrainRadar:
						curButton = ButtonId::Character;
						break;
					case ButtonId::CandleRadar:
						curButton = ButtonId::BrainRadar;
						break;
					case ButtonId::MoveNShoot:
						curButton = ButtonId::CandleRadar;
						break;
					case ButtonId::Records:
						curButton = ButtonId::MoveNShoot;
						break;
					case ButtonId::KeyConfig:
						curButton = ButtonId::Records;
						break;
					case ButtonId::Playlist:
						curButton = ButtonId::KeyConfig;
						break;
					case ButtonId::DelProf:
						curButton = ButtonId::Playlist;
						break;
					case ButtonId::Exit:
						curButton = ButtonId::DelProf;
						break;
					case ButtonId::None:
					case ButtonId::Difficulty:
						curButton = ButtonId::Exit;
						break;
					case ButtonId::Profile_New:
						curButton = ButtonId(ButtonId::Profile_0 + numFiles - 1);
						break;
					default:
						if (curButton >= ButtonId::Profile_0 && curButton < ButtonId::Profile_0 + numFiles)
						{
							// NB: Profile_0 rolls down to Profile_New for wraparound
							curButton = ButtonId(curButton - 1);
						}
						break;
				}
			}

			if (c & ~oldc & CONTROL_DN)
			{
				switch (curButton)
				{
					case ButtonId::None:
					case ButtonId::Exit:
						curButton = ButtonId::Difficulty;
						break;
					case ButtonId::Difficulty:
						curButton = ButtonId::Character;
						break;
					case ButtonId::Character:
						curButton = ButtonId::BrainRadar;
						break;
					case ButtonId::BrainRadar:
						curButton = ButtonId::CandleRadar;
						break;
					case ButtonId::CandleRadar:
						curButton = ButtonId::MoveNShoot;
						break;
					case ButtonId::MoveNShoot:
						curButton = ButtonId::Records;
						break;
					case ButtonId::Records:
						curButton = ButtonId::KeyConfig;
						break;
					case ButtonId::KeyConfig:
						curButton = ButtonId::Playlist;
						break;
					case ButtonId::Playlist:
						curButton = ButtonId::DelProf;
						break;
					case ButtonId::DelProf:
						curButton = ButtonId::Exit;
						break;
					case ButtonId::Profile_New:
						curButton = ButtonId::Profile_0;
						break;
					default:
						if (curButton >= ButtonId::Profile_0 && curButton < ButtonId::Profile_0 + numFiles)
						{
							curButton = ButtonId(curButton + 1);
							if (curButton >= ButtonId::Profile_0 + numFiles)
								curButton = ButtonId::Profile_New;
						}
						break;
				}
			}

			if (c & ~oldc & CONTROL_LF)
			{
				if (curButton == ButtonId::Profile_New || (curButton >= ButtonId::Profile_0 && curButton < ButtonId::Profile_End))
				{
					curButton = ButtonId::Difficulty;
				}
			}

			if (c & ~oldc & CONTROL_RT)
			{
				switch (curButton)
				{
					case ButtonId::None:
					case ButtonId::Difficulty:
					case ButtonId::Character:
					case ButtonId::BrainRadar:
					case ButtonId::CandleRadar:
					case ButtonId::MoveNShoot:
					case ButtonId::Records:
					case ButtonId::KeyConfig:
					case ButtonId::Playlist:
					case ButtonId::DelProf:
					case ButtonId::Exit:
						curButton = ButtonId(ButtonId::Profile_0 + profChoice);
						break;
					default: break;
				}
			}

			if (mb || (c & ~oldc & CONTROL_B1))
			{
				if (curButton != ButtonId::None)
					MakeNormalSound(SND_MENUSELECT);
				switch (curButton)
				{
					case ButtonId::BrainRadar:
						if(ItemPurchased(SHOP_ABILITY,ABIL_BRAIN))
							profile.brainRadar=1-profile.brainRadar;
						break;
					case ButtonId::CandleRadar:
						if(ItemPurchased(SHOP_ABILITY,ABIL_CANDLE))
							profile.candleRadar=1-profile.candleRadar;
						break;
					case ButtonId::MoveNShoot:
						if(ItemPurchased(SHOP_ABILITY,ABIL_MOVESHOOT))
							profile.moveNShoot=1-profile.moveNShoot;
						break;
					case ButtonId::Records:
						if(recordBook)
							return 4;
						break;
					case ButtonId::Difficulty:
						profile.difficulty++;
						if(profile.difficulty >= MAX_DIFFICULTY)
							profile.difficulty = 0;
						break;
					case ButtonId::Character:
						profile.playAs++;
						if(profile.playAs>5)
							profile.playAs=0;
						while(profile.playAs!=PLAY_BOUAPHA && !ItemPurchased(SHOP_PLAYABLE,profile.playAs))
						{
							profile.playAs++;
							if(profile.playAs>5)
								profile.playAs=0;
						}
						break;
					case ButtonId::Playlist:
						return 2;
						break;
					case ButtonId::KeyConfig:
						InitKeyConfig();
						mode=PROF_KEYCONFIG;
						curButton = mb ? ButtonId::None : ButtonId::Keys1_0;
						kcMode=0;
						break;
					case ButtonId::DelProf:
						mode=PROF_DELETE;
						curButton = mb ? ButtonId::None : ButtonId::No;
						InitYesNoDialog("Really delete this profile?","Yes","No");
						break;
					case ButtonId::Exit:
						return 1;
						break;
					case ButtonId::Profile_New:
						if (numFiles < MAX_PROFS)
						{
							SaveProfile();
							return 3;
						}
						break;
					default: break;
				}

				if (curButton >= ButtonId::Profile_0 && curButton < ButtonId::Profile_0 + numFiles)
				{
					i = curButton - ButtonId::Profile_0;
					// selecting a profile
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
			}

			if(k==27)
				return 1;	// exit
			break;
		case PROF_DELETE:
			if(mb)
			{
				YesNoDialogClick(msx,msy);
			}
			if(k!=0)
				YesNoDialogKey(k);
			int command;
			command = YesNoDialogCommand();

			if (c & ~oldc & (CONTROL_LF | CONTROL_RT))
			{
				curButton = curButton == ButtonId::No ? ButtonId::Yes : ButtonId::No;
			}

			if (c & ~oldc & CONTROL_B1)
			{
				switch (curButton)
				{
					case ButtonId::Yes:
						command = YNM_YES;
						break;
					case ButtonId::No:
						command = YNM_NO;
						break;
					default: break;
				}
			}

			switch(command)
			{
				case YNM_YES:
					char s[64];

					mode=PROF_NORMAL;
					sprintf(s,"profiles/%s.prf",profile.name);
					AppdataDelete(s);
					if(numFiles==1)	// this was the only profile
					{
						AppdataDelete("profile.cfg");
						InitProfile();
						return 3;	// so force them to create a new profile
					}
					else
					{
						FreeProfile();
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
					curButton = mb ? ButtonId::None : ButtonId::Exit;
					break;
				case YNM_NO:
					mode=PROF_NORMAL;
					curButton = mb ? ButtonId::None : ButtonId::DelProf;
					break;
			}
			break;
		case PROF_KEYCONFIG:
			if(kcMode==0)
			{
				if (c & ~oldc & CONTROL_UP)
				{
					switch (curButton)
					{
						case ButtonId::None:
							curButton = ButtonId::KeysExit;
							break;
						case ButtonId::Keys1_1:
							curButton = ButtonId::Keys1_0;
							break;
						case ButtonId::Keys1_2:
							curButton = ButtonId::Keys1_1;
							break;
						case ButtonId::Keys1_3:
							curButton = ButtonId::Keys1_2;
							break;
						case ButtonId::Keys1_4:
							curButton = ButtonId::Keys1_3;
							break;
						case ButtonId::Keys1_5:
							curButton = ButtonId::Keys1_4;
							break;
						case ButtonId::Keys2_1:
							curButton = ButtonId::Keys2_0;
							break;
						case ButtonId::Keys2_2:
							curButton = ButtonId::Keys2_1;
							break;
						case ButtonId::Keys2_3:
							curButton = ButtonId::Keys2_2;
							break;
						case ButtonId::Keys2_4:
							curButton = ButtonId::Keys2_3;
							break;
						case ButtonId::Keys2_5:
							curButton = ButtonId::Keys2_4;
							break;
						case ButtonId::KeysDefault:
							curButton = ButtonId::Keys1_5;
							break;
						case ButtonId::KeysExit:
							curButton = ButtonId::KeysDefault;
							break;
						default: break;
					}
				}

				if (c & ~oldc & CONTROL_DN)
				{
					switch (curButton)
					{
						case ButtonId::None:
							curButton = ButtonId::Keys1_0;
							break;
						case ButtonId::Keys1_0:
							curButton = ButtonId::Keys1_1;
							break;
						case ButtonId::Keys1_1:
							curButton = ButtonId::Keys1_2;
							break;
						case ButtonId::Keys1_2:
							curButton = ButtonId::Keys1_3;
							break;
						case ButtonId::Keys1_3:
							curButton = ButtonId::Keys1_4;
							break;
						case ButtonId::Keys1_4:
							curButton = ButtonId::Keys1_5;
							break;
						case ButtonId::Keys2_0:
							curButton = ButtonId::Keys2_1;
							break;
						case ButtonId::Keys2_1:
							curButton = ButtonId::Keys2_2;
							break;
						case ButtonId::Keys2_2:
							curButton = ButtonId::Keys2_3;
							break;
						case ButtonId::Keys2_3:
							curButton = ButtonId::Keys2_4;
							break;
						case ButtonId::Keys2_4:
							curButton = ButtonId::Keys2_5;
							break;
						case ButtonId::Keys1_5:
						case ButtonId::Keys2_5:
							curButton = ButtonId::KeysDefault;
							break;
						case ButtonId::KeysDefault:
							curButton = ButtonId::KeysExit;
							break;
						default: break;
					}
				}

				if (c & ~oldc & CONTROL_RT)
				{
					switch (curButton)
					{
						case ButtonId::None:
							curButton = ButtonId::Keys1_0;
							break;
						case ButtonId::Keys1_0:
							curButton = ButtonId::Keys2_0;
							break;
						case ButtonId::Keys1_1:
							curButton = ButtonId::Keys2_1;
							break;
						case ButtonId::Keys1_2:
							curButton = ButtonId::Keys2_2;
							break;
						case ButtonId::Keys1_3:
							curButton = ButtonId::Keys2_3;
							break;
						case ButtonId::Keys1_4:
							curButton = ButtonId::Keys2_4;
							break;
						case ButtonId::Keys1_5:
							curButton = ButtonId::Keys2_5;
							break;
						default: break;
					}
				}

				if (c & ~oldc & CONTROL_LF)
				{
					switch (curButton)
					{
						case ButtonId::None:
							curButton = ButtonId::Keys1_0;
							break;
						case ButtonId::Keys2_0:
							curButton = ButtonId::Keys1_0;
							break;
						case ButtonId::Keys2_1:
							curButton = ButtonId::Keys1_1;
							break;
						case ButtonId::Keys2_2:
							curButton = ButtonId::Keys1_2;
							break;
						case ButtonId::Keys2_3:
							curButton = ButtonId::Keys1_3;
							break;
						case ButtonId::Keys2_4:
							curButton = ButtonId::Keys1_4;
							break;
						case ButtonId::Keys2_5:
							curButton = ButtonId::Keys1_5;
							break;
						default: break;
					}
				}

				if(mb || (c & ~oldc & CONTROL_B1))
				{
					kBtnNum = 0;
					for (i = 0; i < NUM_KC_BTNS; i++)
					{
						if (kcBtn[i].id == curButton)
						{
							kBtnNum = i;
							break;
						}
					}

					MakeNormalSound(SND_MENUSELECT);
					if(curButton>=ButtonId::Keys1_0 && curButton<=ButtonId::Keys1_5)
					{
						kcMode=(byte)curButton-(byte)ButtonId::Keys1_0+1;
						canHitKeys=0;
						mgl->ClearKeys();
					}
					else if(curButton>=ButtonId::Keys2_0 && curButton<=ButtonId::Keys2_5)
					{
						kcMode=(byte)curButton-(byte)ButtonId::Keys2_0+7;
						canHitKeys=0;
						mgl->ClearKeys();
					}
					else if(curButton==ButtonId::KeysExit)
					{
						mode=PROF_NORMAL;
						if(!mb)
							curButton = ButtonId::KeyConfig;
						SaveProfile();
					}
					else if(curButton==ButtonId::KeysDefault)
					{
						DefaultControls();
						InitKeyConfig();
					}
				}

				if(k==27)
				{
					mode=PROF_NORMAL;
					curButton = ButtonId::KeyConfig;
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

				if(k==27 || (GetGamepadButtons() & ~(1 << SDL_CONTROLLER_BUTTON_A)))
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

	oldc = c;

	return 0;
}

void RenderProfButton(int x,int y,int wid,const char *txt,MGLDraw *mgl, ButtonId buttonId)
{
	if((mode==PROF_NORMAL || mode==PROF_KEYCONFIG) && curButton == buttonId)
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
		if(curButton == ButtonId::Profile_0 + i)
			mgl->Box(x+1,y+1+i*18,x+149,y+i*18+18,32*1+12);
		PrintGlowLimited(x+2,y+2+i*18,x+150,ShortName(&fileList[i*PRFNAME_LEN]),0,2);
	}

	if(i<MAX_PROFS)
	{
		if(curButton == ButtonId::Profile_New)
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
		if(btn[i].id==ButtonId::Records && !recordBook)
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,"?????",mgl,btn[i].id);
		else if(btn[i].id==ButtonId::CandleRadar && !candleRadar)
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,"?????",mgl,btn[i].id);
		else if(btn[i].id==ButtonId::BrainRadar && !brainRadar)
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,"?????",mgl,btn[i].id);
		else if(btn[i].id==ButtonId::MoveNShoot && !moveNShoot)
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,"?????",mgl,btn[i].id);
		else
			RenderProfButton(btn[i].x,btn[i].y,btn[i].wid,btn[i].txt,mgl,btn[i].id);
	}

	PrintGlow(472,20,"Available Profiles",0,2);
	RenderProfList(470,38,mgl);

	sprintf(s,"Username: %s",profile.name);
	PrintGlow(20,20,s,0,2);

	PrintGlow(146,62,GetDifficultyName(profile.difficulty),0,2);
	PrintGlow(146,92,GetPlayableCharacterName(profile.playAs),0,2);
	if(brainRadar)
		PrintGlow(146,122,yesno[profile.brainRadar],0,2);
	if(candleRadar)
		PrintGlow(146,152,yesno[profile.candleRadar],0,2);
	if(moveNShoot)
		PrintGlow(146,182,yesno[profile.moveNShoot],0,2);

	if(mode==PROF_DELETE)
	{
		RenderYesNoDialog2(msx, msy, mgl, curButton == ButtonId::Yes ? 0 : curButton == ButtonId::No ? 1 : -1);
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
	ClearSpriteConstraints();
}

void RenderKeyConfigMenu(MGLDraw *mgl)
{
	int i;
	int msx2,msy2;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	for(i=0;i<NUM_KC_BTNS;i++)
		RenderProfButton(kcBtn[i].x,kcBtn[i].y,kcBtn[i].wid,kcBtn[i].txt,mgl,kcBtn[i].id);

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
	ClearSpriteConstraints();
}

//----------------

TASK(void) ProfMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	InitProfMenu(mgl);
	PlaySongForce("003worldpicker.ogg");

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateProfMenu(&lastTime,mgl);
		if(mode!=PROF_KEYCONFIG)
			RenderProfMenu(mgl);
		else
			RenderKeyConfigMenu(mgl);

		AWAIT mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();

		if(done==2)
		{
			AWAIT PlayListMenu(mgl);
			oldc = ~0;
			done=0;
		}
		if(done==3)
		{
			AWAIT NameEntry(mgl,1);
			done=0;
			ExitProfMenu();
			InitProfMenu(mgl);
		}
		if(done==4)
		{
			AWAIT RecordBook(mgl);
			done=0;
		}
	}

	SaveProfile();
	ExitProfMenu();
}
