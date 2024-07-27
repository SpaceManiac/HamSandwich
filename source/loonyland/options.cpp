#include "options.h"
#include <map>
#include "mgldraw.h"
#include "badge.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "title.h"
#include "plasma.h"
#include "appdata.h"
#include "steam.h"
#include "hammusic.h"
#include "randomizer.h"

static_assert(sizeof(options_t) == 7144, "Save compatibility at risk");

options_t opt;


static byte cursor;
static byte oldc;
static dword oldBtn, oldPad;
static byte controlX,controlY;
static byte optMode;

static const byte DEFAULT_VOLUME = 128;
static const byte volumeSpot[]={0,26,51,77,102,128,153,179,204,230,255};
static byte NextVolumeSpot(byte v)
{
	int i;

	for(i=0;i<10;i++)
	{
		if(v==volumeSpot[i])
			return volumeSpot[i+1];
	}
	if(v==volumeSpot[10])
		return volumeSpot[0];

	return 0;
}
static byte PrevVolumeSpot(byte v)
{
	int i;

	for(i=1;i<11;i++)
	{
		if(v==volumeSpot[i])
			return volumeSpot[i-1];
	}
	if(v==volumeSpot[0])
		return volumeSpot[10];
	return 0;
}

void InitOptionsMenu(void)
{
	oldc=255;
	controlX=10;
	cursor=0;
	optMode=0;
	InitPlasma(7);
}

void ExitOptionsMenu(void)
{
	SaveOptions();
	ExitPlasma();
}

void ApplyControlSettings(void)
{
	for (int i = 0; i < 2; ++i)
	{
		byte control[8];
		memcpy(&control[0], &opt.control[i], 6);
		memcpy(&control[6], &opt.moreControl[i], 2);
		SetKeyboardBindings(i, 8, control);
	}

	byte joyControl[4] = { opt.joyCtrl[0], opt.joyCtrl[1], opt.moreJoyCtrl[0], opt.moreJoyCtrl[1] };
	// Only 3 so the Android version doesn't show a 4th button that does nothing.
	SetJoystickBindings(3, joyControl);
}

byte UpdateOptionsMenu(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte c2;
	dword btn,j;
	int i;

	dword pad = GetGamepadButtons();
	dword padPressed = pad & ~oldPad;
	oldPad = pad;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;
	while(*lastTime>=TIME_PER_FRAME)
	{
		switch(optMode)
		{
			case 0:	// just going through options
				c=mgl->LastKeyPressed();
				c2=GetControls()|GetArrows();

				if(c==27 || (c2 & ~oldc & CONTROL_B2) || (padPressed & ((1 << SDL_CONTROLLER_BUTTON_BACK) | (1 << SDL_CONTROLLER_BUTTON_START))))
				{
					return 1;
				}

				if(c2 & ~oldc & CONTROL_UP)
				{
					cursor--;
					if(cursor>4)
						cursor=4;
					MakeNormalSound(SND_MENUCLICK);
				}
				if(c2 & ~oldc & CONTROL_DN)
				{
					cursor++;
					if(cursor>4)
						cursor=0;
					MakeNormalSound(SND_MENUCLICK);
				}
				if(c2 & ~oldc & CONTROL_LF)
				{
					switch(cursor)
					{
						case 0:
							opt.sound = PrevVolumeSpot(opt.sound);
							JamulSoundVolume(opt.sound);
							SetMusicVolume(opt.sound);
							if(opt.sound==0)
							{
								JamulSoundPurge();
								KillSong();
							}
							else
								LoopingSound(SND_HAMUMU);
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 1:
							MakeNormalSound(SND_MENUSELECT);
							opt.helpOn=1-opt.helpOn;
							break;
						case 2:
							MakeNormalSound(SND_MENUSELECT);
							// diffy
							opt.difficulty = PrevDifficulty(opt.difficulty);
							break;
					}
				}
				if(c2 & ~oldc & CONTROL_RT)
				{
					switch(cursor)
					{
						case 0:
							opt.sound = NextVolumeSpot(opt.sound);
							JamulSoundVolume(opt.sound);
							SetMusicVolume(opt.sound);
							if(opt.sound==0)
							{
								JamulSoundPurge();
								KillSong();
							}
							else
								LoopingSound(SND_HAMUMU);
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 1:
							MakeNormalSound(SND_MENUSELECT);
							opt.helpOn=1-opt.helpOn;
							break;
						case 2:
							MakeNormalSound(SND_MENUSELECT);
							// diffy
							opt.difficulty = NextDifficulty(opt.difficulty);
							break;
					}
				}
				if(c2 & ~oldc & CONTROL_B1)
				{
					switch(cursor)
					{
						case 0:
							opt.sound = NextVolumeSpot(opt.sound);
							JamulSoundVolume(opt.sound);
							SetMusicVolume(opt.sound);
							if(opt.sound==0)
							{
								JamulSoundPurge();
								KillSong();
							}
							else
								LoopingSound(SND_HAMUMU);
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 1:
							MakeNormalSound(SND_MENUSELECT);
							opt.helpOn=1-opt.helpOn;
							break;
						case 2:
							MakeNormalSound(SND_MENUSELECT);
							// diffy
							opt.difficulty = NextDifficulty(opt.difficulty);
							break;
						case 3:
							MakeNormalSound(SND_MENUSELECT);
							optMode=1;
							controlX=0;
							controlY=0;
							break;
						case 4:
							MakeNormalSound(SND_MENUSELECT);
							return 1;
							break;
					}
				}
				break;
			case 1:	// selecting keys to configure
				c=mgl->LastKeyPressed();
				c2=GetControls()|GetArrows();

				if(c==27 || (c2 & ~oldc & CONTROL_B2) || (padPressed & ((1 << SDL_CONTROLLER_BUTTON_BACK) | (1 << SDL_CONTROLLER_BUTTON_START))))
				{
					optMode=0;
					controlX=10;
					oldc = ~0;
					ApplyControlSettings();
					MakeNormalSound(SND_MENUCANCEL);
					return 0;
				}
				if(c==13)
					c2|=CONTROL_B1;

				if(c2 & ~oldc & CONTROL_UP)
				{
					MakeNormalSound(SND_MENUCLICK);
					controlY--;
					if(controlY>6)
						controlY=6;
				}
				if(c2 & ~oldc & CONTROL_DN)
				{
					MakeNormalSound(SND_MENUCLICK);
					controlY++;
					if(controlY>6)
						controlY=0;
				}
				if(c2 & ~oldc & CONTROL_LF)
				{
					MakeNormalSound(SND_MENUCLICK);
					controlX--;
					if(controlX>2)
						controlX=2;
				}
				if(c2 & ~oldc & CONTROL_RT)
				{
					MakeNormalSound(SND_MENUCLICK);
					controlX++;
					if(controlX>2)
						controlX=0;
				}
				if(c2 & ~oldc & CONTROL_B1)
				{
					if(controlX<2)
					{
						// keyboard
						optMode=2;
						LastScanCode();
						MakeNormalSound(SND_MENUSELECT);
					}
					else if(controlY>3)
					{
						btn=0;
						oldBtn=~0;
						optMode=3;
						MakeNormalSound(SND_MENUSELECT);
					}
				}
				break;
			case 2: // entering a specific key
				c2=LastScanCode();
				if(c2==SDL_SCANCODE_ESCAPE || (padPressed & ((1 << SDL_CONTROLLER_BUTTON_BACK) | (1 << SDL_CONTROLLER_BUTTON_START) | (1 << SDL_CONTROLLER_BUTTON_B))))
				{
					optMode=1;
					c2=255;
					oldc=~0;
					mgl->LastKeyPressed();
					MakeNormalSound(SND_MENUCANCEL);
					return 0;
				}
				if(c2!=0)
				{
					if (controlY < 6)
						opt.control[controlX][controlY]=c2;
					else
						opt.moreControl[controlX][controlY-6] = c2;
					optMode=1;
					mgl->LastKeyPressed();
					MakeNormalSound(SND_MENUSELECT);
					ApplyControlSettings();
				}
				c2=255;
				break;
			case 3: // pressing a joystick button
				c=mgl->LastKeyPressed();
				if(c==27 || (padPressed & ((1 << SDL_CONTROLLER_BUTTON_BACK) | (1 << SDL_CONTROLLER_BUTTON_START))))
				{
					optMode=1;
					c2=255;
					oldc=255;
					MakeNormalSound(SND_MENUCANCEL);
					return 0;
				}
				btn=GetJoyButtons();

				j=1;
				for(i=0;i<16;i++)
				{
					if((btn&j) && !(oldBtn&j))
					{
						if (controlY < 6)
							opt.joyCtrl[controlY-4]=i;
						else
							opt.moreJoyCtrl[controlY-6]=i;
						optMode=1;
						c2=255;
						MakeNormalSound(SND_MENUSELECT);
						ApplyControlSettings();
					}
					j*=2;
				}
				oldBtn=btn;
				break;
		}

		oldc=c2;

		UpdatePlasma();

		*lastTime-=TIME_PER_FRAME;
	}

	return 0;
}

void RenderControls(int x,int y)
{
	static const char dirName[7][12]={"Up","Down","Left","Right","Fire","Weapon","Pick Up"};
	char btnTxt[64];
	int i;

	DrawFillBox(x,y-2,x+398,y+20,16);
	CenterPrint(x+50,y+2,"Control",0,1);
	CenterPrint(x+150,y+2,"Keyboard1",0,1);
	CenterPrint(x+250,y+2,"Keyboard2",0,1);
	CenterPrint(x+350,y+2,"Joystick",0,1);
	DrawBox(x+98,y-2,x+198,y+209,16);
	DrawBox(x+198,y-2,x+298,y+209,16);
	DrawBox(x+298,y-2,x+398,y+209,16);

	for(i=0;i<7;i++)
	{
		if(controlY==i && controlX<3)
		{
			if(optMode==1)
				DrawFillBox(x+99+100*controlX,y+20+1+i*27,x+198+100*controlX,y+20+26+i*27,20);
			else
			{
				DrawFillBox(x+99+100*controlX,y+20+1+i*27,x+198+100*controlX,y+20+26+i*27,31);
				CenterPrint(x+150+controlX*100,y+27+i*27,"???",0,1);
			}
		}
		DrawFillBox(x,y+20+1+i*27,x+98,y+20+26+i*27,10);
		DrawBox(x,y+20+i*27,x+398,y+20+27+i*27,16);

		CenterPrint(x+50,y+27+i*27,dirName[i],0,1);
		if(optMode==1 || controlX!=0 || controlY!=i)
			CenterPrint(x+150,y+27+i*27,ScanCodeText(i >= 6 ? opt.moreControl[0][i-6] : opt.control[0][i]),0,1);
		if(optMode==1 || controlX!=1 || controlY!=i)
			CenterPrint(x+250,y+27+i*27,ScanCodeText(i >= 6 ? opt.moreControl[1][i-6] : opt.control[1][i]),0,1);

		if(i>3)
		{
			if(optMode==1 || controlX!=2 || controlY!=i)
			{
				sprintf(btnTxt,"Button %d", (i >= 6 ? opt.moreJoyCtrl[i-6] : opt.joyCtrl[i-4]) + 1);
				CenterPrint(x+350,y+27+i*27,btnTxt,0,1);
			}
		}
		else
		{
			CenterPrint(x+350,y+27+i*27,dirName[i],16,1);
		}
	}
	if(optMode==0)
	{
		if (ShowGamepadText())
		{
			CenterPrintGlow(320,425,"Move with left stick, Fire to select",0,0);
			CenterPrintGlow(320,450,"Back or Start to return to main menu",0,0);
		}
		else
		{
			CenterPrintGlow(320,425,"Move with arrow keys, ENTER to select",0,0);
			CenterPrintGlow(320,450,"ESC to return to main menu",0,0);
		}
	}
	else if(optMode==1)
	{
		if (ShowGamepadText())
		{
			CenterPrintGlow(320,425,"Select with left stick, Fire to set new control",0,0);
			CenterPrintGlow(320,450,"Back or Start to return to options",0,0);
		}
		else
		{
			CenterPrintGlow(320,425,"Select with arrow keys, ENTER to set new control",0,0);
			CenterPrintGlow(320,450,"ESC to return to options",0,0);
		}
	}
	else if(optMode==2)
	{
		sprintf(btnTxt,"Press a key for %s",dirName[controlY]);
		CenterPrintGlow(320,425,btnTxt,0,0);
		CenterPrintGlow(320,450, ShowGamepadText() ? "Back or Start to cancel" : "ESC to cancel",0,0);
	}
	else if(optMode==3)
	{
		sprintf(btnTxt,"Press a joystick button for %s",dirName[controlY]);
		CenterPrintGlow(320,425,btnTxt,0,0);
		CenterPrintGlow(320,450, ShowGamepadText() ? "Back or Start to cancel" : "ESC to cancel",0,0);
	}
}

void RenderOptionsMenu(MGLDraw *mgl)
{
	static const char onoff[3][8]={"Off","On"};
	char buf[32];

	int wid;
	byte* pos;
	int i;

	wid=mgl->GetWidth();
	pos=mgl->GetScreen()+40*wid;
	for(i=40;i<480-40;i++)
	{
		memset(pos,7*32+2,640);
		pos+=wid;
	}

	RenderPlasma(mgl,1);
	RenderPlasma(mgl,0);

	CenterPrintGlow(320,2,"Game Options",0,2);

	PrintColor(240,50,"Sound",7,-10,0);
	snprintf(buf, 32, "%d%%", opt.sound * 100 / 255);
	PrintColor(360,50,buf,7,-10,0);
	if(cursor==0)
	{
		PrintColor(239,49,"Sound",0,0,0);
		PrintColor(359,49,buf,0,0,0);
	}

	PrintColor(240,80,"Help Screen",7,-10,0);
	PrintColor(360,80,onoff[opt.helpOn],7,-10,0);
	if(cursor==1)
	{
		PrintColor(239,79,"Help Screen",0,0,0);
		PrintColor(359,79,onoff[opt.helpOn],0,0,0);
	}

	PrintColor(240,110,"Difficulty",7,-10,0);
	PrintColor(360,110,DifficultyName(opt.difficulty),7,-10,0);
	if(cursor==2)
	{
		PrintColor(239,109,"Difficulty",0,0,0);
		PrintColor(359,109,DifficultyName(opt.difficulty),0,0,0);
	}

	PrintColor(240,140,"Configure Controls",7,-10,0);
	if(cursor==3)
		PrintColor(239,139,"Configure Controls",0,0,0);

	PrintColor(240,170,"Exit To Main Menu",7,-10,0);
	if(cursor==4)
		PrintColor(239,169,"Exit To Main Menu",0,0,0);

	RenderControls(120,210);
}

//----------------

void LoadOptions(void)
{
	int i;

	auto f = AppdataOpen("loony.cfg");
	if(!f)
	{
		opt.sound = DEFAULT_VOLUME;

		opt.control[0][0]=SDL_SCANCODE_UP;	// up
		opt.control[0][1]=SDL_SCANCODE_DOWN;	// down
		opt.control[0][2]=SDL_SCANCODE_LEFT;	// left
		opt.control[0][3]=SDL_SCANCODE_RIGHT;	// right
		opt.control[0][4]=SDL_SCANCODE_LCTRL;	// hammer: CTRL
		opt.control[0][5]=SDL_SCANCODE_LSHIFT;	// special: shift
		opt.control[1][0]=0;	// up
		opt.control[1][1]=0;	// down
		opt.control[1][2]=0;	// left
		opt.control[1][3]=0;	// right
		opt.control[1][4]=SDL_SCANCODE_Z;	// hammer: Z
		opt.control[1][5]=SDL_SCANCODE_X;	// special: X

		opt.joyCtrl[0]=0;
		opt.joyCtrl[1]=1;

		opt.difficulty=0;		// default to Beginner
		opt.helpOn=1;

		for(i=0;i<40;i++)
			opt.meritBadge[i] = MERIT_NO;
		for(i=0;i<20;i++)
			opt.cheats[i]=0;
		for(i=0;i<10;i++)
			opt.bossDead[i]=0;
		opt.remixMode=0;
		opt.moreControl[0][0]=SDL_SCANCODE_TAB;  // wpnlock
		opt.moreControl[0][1]=0;  // reserved
		opt.moreControl[1][0]=0;  // wpnlock
		opt.moreControl[1][1]=0;  // reserved
		opt.moreJoyCtrl[0]=2;
		opt.moreJoyCtrl[1]=3;
		for(i=0;i<std::size(opt.expando);i++)
			opt.expando[i]=0;

		ResetHighScores();
	}
	else
	{
		SDL_RWread(f,&opt,1,sizeof(options_t));
		f.reset();

		if (opt.sound == 1)
		{
			// old setting for sound=on, make that volume=50%
			opt.sound = DEFAULT_VOLUME;
		}

		if (opt.version == 0)
		{
			opt.version = 1;
			opt.moreControl[0][0]=SDL_SCANCODE_TAB;  // wpnlock
			opt.moreControl[0][1]=0;  // reserved
			opt.moreControl[1][0]=0;  // wpnlock
			opt.moreControl[1][1]=0;  // reserved
			opt.moreJoyCtrl[0]=2;
			opt.moreJoyCtrl[1]=3;
		}
	}
	ApplyControlSettings();
	JamulSoundVolume(opt.sound);
	SetMusicVolume(opt.sound);
	Steam()->ProfileReady();
}

void SaveOptions(void)
{
	auto f = AppdataOpen_Write("loony.cfg");
	SDL_RWwrite(f,&opt,sizeof(options_t),1);
	f.reset();
	AppdataSync();
}

TASK(void) OptionsMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	InitOptionsMenu();
	lastTime=1;
	oldPad = ~0;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateOptionsMenu(&lastTime,mgl);
		RenderOptionsMenu(mgl);
		AWAIT mgl->Flip();
		EndClock();

		if(!mgl->Process())
			done=1;

	}

	ExitOptionsMenu();
}

void KilledBoss(byte boss)
{
	opt.bossDead[boss]=1;
	SaveOptions();
}

static const char difficultyName[][18] = {
	"Beginner",
	"Normal",
	"Challenge",
	"Mad",
	"Loony",
	"Hard",
};
static_assert(std::size(difficultyName) == NUM_DIFFICULTY);

const char* DifficultyName(byte difficulty)
{
	if (difficulty < std::size(difficultyName))
	{
		return difficultyName[difficulty];
	}
	return "???";
}

static const byte nextDifficulty[] = { DIFF_NORMAL, DIFF_HARD, DIFF_MAD, DIFF_LOONY, DIFF_BEGINNER, DIFF_CHALLENGE };
static const byte prevDifficulty[] = { DIFF_LOONY, DIFF_BEGINNER, DIFF_HARD, DIFF_CHALLENGE, DIFF_MAD, DIFF_NORMAL };
static_assert(std::size(nextDifficulty) == NUM_DIFFICULTY);
static_assert(std::size(prevDifficulty) == NUM_DIFFICULTY);
byte NextDifficulty(byte difficulty)
{
	if (difficulty < std::size(nextDifficulty))
		return nextDifficulty[difficulty];
	return DIFF_BEGINNER;
}
byte PrevDifficulty(byte difficulty)
{
	if (difficulty < std::size(prevDifficulty))
		return prevDifficulty[difficulty];
	return DIFF_BEGINNER;
}

static const char playerCharacterName[][11] = {
	"Loony",
	"Bonkula",
	"Toad",
	"Swampdog",
	"Witch",
	"Werewolf",
	"Summony",
	"Ninja Girl",
};
static_assert(std::size(playerCharacterName) == PC_MAX);

const char* PlayerCharacterName(PlayerCharacterType character)
{
	if (character >= 0 && character < std::size(playerCharacterName))
	{
		return playerCharacterName[character];
	}
	return "???";
}

static const std::map<PlayerCharacterType, int> characterCheat = {
	{PC_Bonkula,	CH_BONKULA},
	{PC_Toad,		CH_TOAD},
	{PC_Swampdog,	CH_SWAMPDOG},
	{PC_Witch,		CH_WITCH},
	{PC_Werewolf,	CH_WEREWOLF},
	{PC_Summon,		CH_SUMMON},
	{PC_Thief,		CH_THIEF},
};

static const std::map<PlayerCharacterType, PlayerCharacterType> nextCharacter = {
	{PC_Loony, PC_Bonkula},
	{PC_Bonkula, PC_Toad},
	{PC_Toad, PC_Swampdog},
	{PC_Swampdog, PC_Witch},
	{PC_Witch, PC_Werewolf},
	{PC_Werewolf, PC_Summon},
	{PC_Summon, PC_Thief},
	{PC_Thief, PC_Loony}
};
static const std::map<PlayerCharacterType, PlayerCharacterType> prevCharacter = {
	{PC_Loony, PC_Thief},
	{PC_Bonkula, PC_Loony},
	{PC_Toad, PC_Bonkula},
	{PC_Swampdog, PC_Toad},
	{PC_Witch, PC_Swampdog},
	{PC_Werewolf, PC_Witch},
	{PC_Summon, PC_Werewolf},
	{PC_Thief, PC_Summon}
};

PlayerCharacterType GetCurrentPC()
{
	if (opt.cheats[CH_BONKULA])
		return PC_Bonkula;
	if (opt.cheats[CH_TOAD])
		return PC_Toad;
	if (opt.cheats[CH_SWAMPDOG])
		return PC_Swampdog;
	if (opt.cheats[CH_WITCH])
		return PC_Witch;
	if (opt.cheats[CH_WEREWOLF])
		return PC_Werewolf;
	if (opt.cheats[CH_SUMMON])
		return PC_Summon;
	if (opt.cheats[CH_THIEF])
		return PC_Thief;

	return PC_Loony;
}

bool IsCharacterUnlocked(PlayerCharacterType pc)
{
	switch (pc)
	{
		case PC_Loony: return true;
		case PC_Bonkula: return opt.meritBadge[BADGE_BONKULA];
		case PC_Toad: return opt.meritBadge[BADGE_ANNOY];
		case PC_Swampdog: return opt.meritBadge[BADGE_SNEAK];
		case PC_Witch: return opt.meritBadge[BADGE_WITCH];
		case PC_Werewolf: return opt.meritBadge[BADGE_WOLFDEN];
		case PC_Summon: return opt.meritBadge[BADGE_REMIX];
		case PC_Thief: return opt.meritBadge[BADGE_WITCHCRAFT];
		default: return false;
	}
}

void SetCharacter(PlayerCharacterType pc)
{
	ResetCharacterCheats();
	if (pc != PC_Loony)
	{
		opt.cheats[characterCheat.at(pc)] = 1;
	}
}

bool IsAnyCharacterUnlocked()
{
	return opt.meritBadge[BADGE_BONKULA] ||
		opt.meritBadge[BADGE_ANNOY] ||
		opt.meritBadge[BADGE_SNEAK] ||
		opt.meritBadge[BADGE_WITCH] ||
		opt.meritBadge[BADGE_WOLFDEN] ||
		opt.meritBadge[BADGE_REMIX] ||
		opt.meritBadge[BADGE_WITCHCRAFT];
}

void NextCharacter()
{
	PlayerCharacterType pc = GetCurrentPC();
	do
	{
		pc = nextCharacter.at(pc);
	} while (!IsCharacterUnlocked(pc));

	SetCharacter(pc);
}

void PrevCharacter()
{
	PlayerCharacterType pc = GetCurrentPC();
	do
	{
		pc = prevCharacter.at(pc);
	} while (!IsCharacterUnlocked(pc));

	SetCharacter(pc);
}

void ResetCharacterCheats()
{
	opt.cheats[CH_BONKULA] = 0;
	opt.cheats[CH_TOAD] = 0;
	opt.cheats[CH_WITCH] = 0;
	opt.cheats[CH_SWAMPDOG] = 0;
	opt.cheats[CH_WEREWOLF] = 0;
	opt.cheats[CH_SUMMON] = 0;
	opt.cheats[CH_THIEF] = 0;

	//invalidate randomizer seed
	ClearSeed();
}
