#include "pause.h"
#include "player.h"
#include "progress.h"
#include "editor.h"
#include "intface.h"
#include "shop.h"
#include "dialogbits.h"
#include "music.h"

#define PE_CONTINUE	0	// back to gameplay
#define PE_RETRY	1	// retry this level
#define PE_GIVEUP	2	// give up, go back to hub
#define PE_WORLDSEL 3	// world select screen
#define PE_EXIT		4	// exit entirely, to main menu or editor, wherever you came from
#define PE_SNDVOL	5	// sound volume adjust
#define PE_MUSVOL	6	// music volume adjust
#define PE_MUSIC	7	// music options menu
#define PE_CHEAT	8	// go to the cheat menu
#define PE_DONE		9	// a marker to indicate the list is done
#define PE_MORECHEATS 10	// next page of cheats
#define PE_MUSICMODE 11	// random/sequential/repeat
#define PE_SONG		12	// which song is playing
#define PE_MUSICTYPE 13 // music type adjust (which playlist, or official music)
#define PE_PREVCHEATS 14
#define PE_BZZT		15		// an invalid option
#define PE_SHOP		16	// go shopping, go back to playing
#define PE_WPNLOCK	17	// weapon lock
#define PE_MORECHEATS2 18	// next page of cheats(2)
#define PE_PREVCHEATS2 19

#define PE_CHEATS	50

typedef struct pauseItem_t
{
	byte effect;
	char text[16];
} pauseItem_t;

static pauseItem_t gamePause[]={
	{PE_CONTINUE,"Continue"},
	{PE_RETRY,"Retry"},
	{PE_GIVEUP,"Give Up"},
	{PE_WORLDSEL,"World Select"},
	{PE_SNDVOL,""},
	{PE_MUSIC,"Music Options"},
	{PE_WPNLOCK,"Weapon Lock"},
	{PE_SHOP,"Quit & Shop"},
	{PE_EXIT,"Exit Game"},
	{PE_DONE,""}
};

static pauseItem_t shopPause[]={
	{PE_CONTINUE,"Continue"},
	{PE_SNDVOL,""},
	{PE_MUSIC,"Music Options"},
	{PE_SHOP,"Quit & Play"},
	{PE_EXIT,"Main Menu"},
	{PE_DONE,""}
};

static pauseItem_t editPause[]={
	{PE_CONTINUE,"Continue"},
	{PE_RETRY,"Retry"},
	{PE_GIVEUP,"Give Up"},
	{PE_SNDVOL,""},
	{PE_MUSIC,"Music Options"},
	{PE_CHEAT,"Cheats!!"},
	{PE_WPNLOCK,"Weapon Lock"},
	{PE_EXIT,"Editor"},
	{PE_DONE,""}
};

static pauseItem_t gameCheatPause[]={
	{PE_CONTINUE,"Continue"},
	{PE_RETRY,"Retry"},
	{PE_GIVEUP,"Give Up"},
	{PE_WORLDSEL,"World Select"},
	{PE_SNDVOL,""},
	{PE_MUSIC,"Music Options"},
	{PE_CHEAT,"Cheats!!"},
	{PE_WPNLOCK,"Weapon Lock"},
	{PE_SHOP,"Quit & Shop"},
	{PE_EXIT,"Exit Game"},
	{PE_DONE,""}
};

static pauseItem_t cheatPause[]={
	{PE_CHEAT,"Stop Cheating!"},
	{PE_CHEATS+CHEAT_WINLEVEL,"Win Level"},
	{PE_CHEATS+CHEAT_HAMMERUP,"Max. Firepower"},
	{PE_CHEATS+CHEAT_LIFE,"Max Health"},
	{PE_CHEATS+CHEAT_RAGE,"Max Rage"},
	{PE_CHEATS+CHEAT_SHIELD,"Energy Barrier"},
	{PE_CHEATS+CHEAT_BOOM,"KABLOOIE!"},
	{PE_CHEATS+CHEAT_FREEZE,"Freeze Enemies"},
	{PE_CHEATS+CHEAT_CANDLES,"All Candles"},
	{PE_CHEATS+CHEAT_BRAINS,"All Brains"},
	{PE_MORECHEATS,"More Cheats"},
	{PE_DONE,""},
};

static pauseItem_t cheatPause2[]={
	{PE_PREVCHEATS,"Prev. Cheats"},
	{PE_CHEATS+CHEAT_KEYS,"All Keys"},
	{PE_CHEATS+CHEAT_WATER,"Walk On Water"},
	{PE_CHEATS+CHEAT_LIGHT,"Lights On"},
	{PE_CHEATS+CHEAT_SPEED,"Super Speed"},
	{PE_CHEATS+CHEAT_SCANNER,"Free Scanner"},
	{PE_CHEATS+CHEAT_AMMO,"Ammo Crate"},
	{PE_CHEATS+CHEAT_OXYGEN,"Infinite Oxygen"},
	{PE_CHEATS+CHEAT_NOSKID,"No-Skid Boots"},
	{PE_MORECHEATS2,"More Cheats"},
	{PE_CHEAT,"Stop Cheating!"},
	{PE_DONE,""},
};

static pauseItem_t cheatPause3[]={
	{PE_PREVCHEATS2,"Prev. Cheats"},
	{PE_CHEATS+CHEAT_STRENGTH,"Super Steelskin"},
	{PE_CHEATS+CHEAT_QUICKFIX,"Quick Fix"},
	{PE_CHEATS+CHEAT_CONFUSE,"Confusion"},
	{PE_CHEATS+CHEAT_PRESTO,"Presto Change-O"},
	{PE_CHEATS+CHEAT_BLACKHOLE,"Black Hole"},
	{PE_CHEAT,"Stop Cheating!"},
	{PE_DONE,""},
};

static pauseItem_t musicPause[]={
	{PE_MUSVOL,""},
	{PE_MUSICTYPE,""},
	{PE_MUSICMODE,""},
	{PE_SONG,""},
	{PE_MUSIC,"Back To Menu"},
	{PE_DONE,""},
};

byte cursor=0;
static char lastKey=0;
static byte menuMode;
static sprite_set_t *pauseSpr=NULL;
static int pauseX=640,pauseY=480;
static byte numItems;
static pauseItem_t menu[15];
static int msx,msy;
static char msBright,msDBright;
static byte oldc=255;

byte volumeSpot[]={0,26,51,77,102,128,153,179,204,230,255};

void RenderPauseButton(byte b,int x,int y,int wid,char *txt,MGLDraw *mgl)
{
	if(cursor==b)
	{
		mgl->Box(x,y,x+wid,y+19,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+19-1,32+8);
	}
	else
		mgl->Box(x,y,x+wid,y+19,32+16);

	PrintGlowLimited(x+2,y+2,x+wid-1,txt,0,2);
}

void RenderUnpaused(void)
{
	if(pauseX<640)
		RenderPauseMenu();
}

void RenderPauseMenu(void)
{
	int i;
	int msx2,msy2,cx,cy,cx2,cy2;

	if(pauseSpr==NULL)
	{
		pauseX=640;
		pauseY=480;
		pauseSpr=new sprite_set_t("graphics/pause.jsp");
	}

	pauseSpr->GetSprite(3)->Draw(pauseX,pauseY,GetDisplayMGL());

	for(i=0;i<numItems;i++)
	{
		RenderPauseButton(i,pauseX+20,pauseY+15+22*i,125,menu[i].text,GetDisplayMGL());
	}

	RenderCollectedStuff(pauseX,pauseY,GetDisplayMGL());

	// mouse cursor
	cx=pauseX+13;
	cy=pauseY+13;
	cx2=pauseX+201;
	cy2=pauseY+261;
	if(cx>638)
		cx=638;
	if(cx2>639)
		cx2=639;
	if(cy>478)
		cy=478;
	if(cy2>479)
		cy2=479;
	if(msx<cx)
	{
		GetDisplayMGL()->SetMouse(cx,msy);
		GetDisplayMGL()->GetMouse(&msx,&msy);
	}
	if(msx>cx2)
	{
		GetDisplayMGL()->SetMouse(cx2,msy);
		GetDisplayMGL()->GetMouse(&msx,&msy);
	}
	if(msy<cy)
	{
		GetDisplayMGL()->SetMouse(msx,cy);
		GetDisplayMGL()->GetMouse(&msx,&msy);
	}
	if(msy>cy2)
	{
		GetDisplayMGL()->SetMouse(msx,cy2);
		GetDisplayMGL()->GetMouse(&msx,&msy);
	}
	SetSpriteConstraints(cx,cy,cx2,cy2);
	msx2=msx;
	msy2=msy;
	if(msx2<pauseX+13)
		msx2=pauseX+13;
	if(msy2<pauseY+13)
		msy2=pauseY+13;
	if(msx2>pauseX+200)
		msx2=pauseX+200;
	if(msy2>pauseY+260)
		msy2=pauseY+260;
	pauseSpr->GetSprite(0)->DrawBright(msx2,msy2,GetDisplayMGL(),msBright/2);
	SetSpriteConstraints(0,0,639,479);
}

void HandlePauseKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

void SetupSoundItems(void)
{
	int i;

	for(i=0;i<numItems;i++)
	{
		switch(menu[i].effect)
		{
			case PE_SNDVOL:
				sprintf(menu[i].text,"Sound: %d%%",(profile.sound*100/255));
				break;
			case PE_MUSVOL:
				sprintf(menu[i].text,"Volume: %d%%",(profile.music*100/255));
				break;
			case PE_MUSICTYPE:
				switch(profile.musicMode&7)
				{
					case 0:
						strcpy(menu[i].text,"Official Music");
						break;
					default:
						sprintf(menu[i].text,"Playlist %d",(profile.musicMode&7));
						break;
				}
				break;
			case PE_MUSICMODE:
				if((profile.musicMode&MUSIC_PLAYMODE)==MUSIC_RANDOM)
					strcpy(menu[i].text,"Random Play");
				else if((profile.musicMode&MUSIC_PLAYMODE)==MUSIC_SEQUENTIAL)
					strcpy(menu[i].text,"Sequential");
				else
					strcpy(menu[i].text,"Repeat");
				break;
			case PE_SONG:
				strncpy(menu[i].text,CurSongTitle(),15);
				menu[i].text[15]='\0';
				break;
		}
	}
}

void FillPauseMenu(pauseItem_t *src)
{
	int i;

	for(i=0;i<15;i++)
	{
		if(src[i].effect==PE_DONE)
		{
			numItems=i;
			break;
		}
		else
		{
			memcpy(&menu[i],&src[i],sizeof(pauseItem_t));
			if(menu[i].effect>=PE_CHEATS)
			{
				// squigglize cheats that aren't owned
				if(!ItemPurchased(SHOP_CHEAT,menu[i].effect-PE_CHEATS) && !editing)
				{
					strcpy(menu[i].text,"????????");
					menu[i].effect=PE_BZZT;
				}
			}
		}
		if(src[i].effect==PE_WPNLOCK)
		{
			if(profile.progress.wpnLock)
				strcpy(menu[i].text,"Wpn Lock: On");
			else
				strcpy(menu[i].text,"Wpn Lock: Off");
		}
	}
	SetupSoundItems();
}

void InitPauseMenu(void)
{
	lastKey=0;
	cursor=0;
	oldc=255;

	if(!editing)
	{
		if(shopping)
			FillPauseMenu(shopPause);
		else if(ItemPurchased(SHOP_MAJOR,MAJOR_CHEATMENU))
			FillPauseMenu(gameCheatPause);
		else
			FillPauseMenu(gamePause);

	}
	else
	{
		FillPauseMenu(editPause);
	}

	MakeNormalSound(SND_PAUSE);
	if(pauseSpr==NULL)
		pauseSpr=new sprite_set_t("graphics/pause.jsp");
	pauseX=640;
	pauseY=480;
	menuMode=0;
	msBright=0;
	msDBright=1;
	GetDisplayMGL()->MouseTap();
}

void ExitPauseMenu(void)
{
	if(pauseSpr)
		delete pauseSpr;
	pauseSpr=NULL;
}

void UpdateUnpaused(void)
{
	if(pauseX<640)
		pauseX+=15;
	if(pauseY<480)
		pauseY+=20;
}

byte NextVolumeSpot(byte v)
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
byte PrevVolumeSpot(byte v)
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

byte UpdatePauseMenu(MGLDraw *mgl)
{
	byte c;
	int i;
	static byte reptCounter=0;

	mgl->GetMouse(&msx,&msy);

	msBright+=msDBright;
	if(msBright>10)
		msDBright=-1;
	if(msBright<-2)
		msDBright=1;

	if(pauseX>427)
	{
		pauseX-=15;
		mgl->SetMouse(msx-15,msy);
		mgl->GetMouse(&msx,&msy);
		if(pauseX<427)
			pauseX=427;
	}
	if(pauseY>206)
	{
		pauseY-=20;
		mgl->SetMouse(msx,msy-20);
		mgl->GetMouse(&msx,&msy);
		if(pauseY<206)
			pauseY=206;
	}


	for(i=0;i<numItems;i++)
	{
		if(PointInRect(msx,msy,pauseX+20,pauseY+15+22*i,pauseX+20+125,pauseY+15+22*i+19))
			cursor=i;
	}

	c=GetControls()|GetArrows();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if((c&CONTROL_UP) && (!reptCounter))
	{
		cursor--;
		if(cursor==255)
			cursor=numItems-1;

		MakeNormalSound(SND_MENUCLICK);
	}
	if((c&CONTROL_DN) && (!reptCounter))
	{
		cursor++;
		if(cursor==numItems)
			cursor=0;

		MakeNormalSound(SND_MENUCLICK);
	}

	if((c&CONTROL_LF) && (!reptCounter))
	{
		switch(menu[cursor].effect)
		{
			case PE_SNDVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.sound=PrevVolumeSpot(profile.sound);
				JamulSoundVolume(profile.sound);
				break;
			case PE_MUSVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.music=PrevVolumeSpot(profile.music);
				SetMusicVolume(profile.music);
				break;
			case PE_SONG:
				PlayPrevSong();
				break;
		}
		SetupSoundItems();
	}
	if((c&CONTROL_RT) && (!reptCounter))
	{
		switch(menu[cursor].effect)
		{
			case PE_SNDVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.sound=NextVolumeSpot(profile.sound);
				JamulSoundVolume(profile.sound);
				break;
			case PE_MUSVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.music=NextVolumeSpot(profile.music);
				SetMusicVolume(profile.music);
				break;
			case PE_SONG:
				PlayNextSong();
				break;
		}
		SetupSoundItems();
	}
	if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
	   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))) ||
	   mgl->MouseTap())
	{
		MakeNormalSound(SND_MENUSELECT);
		i=0;
		switch(menu[cursor].effect)
		{
			case PE_CONTINUE:
				return PAUSE_CONTINUE;
			case PE_RETRY:
				return PAUSE_RETRY;
			case PE_GIVEUP:
				return PAUSE_GIVEUP;
			case PE_WORLDSEL:
				strcpy(profile.lastWorld,player.worldName);
				player.worldProg->levelOn=player.levelNum;
				SaveProfile();
				return PAUSE_WORLDSEL;
			case PE_SNDVOL:
				profile.sound=NextVolumeSpot(profile.sound);
				JamulSoundVolume(profile.sound);
				break;
			case PE_MUSVOL:
				profile.music=NextVolumeSpot(profile.music);
				SetMusicVolume(profile.music);
				break;
			case PE_MUSICMODE:
				if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
					profile.musicMode=MUSIC_OFFICIAL|MUSIC_REPEAT;
				else
				{
					i=profile.musicMode&MUSIC_LISTBITS;
					profile.musicMode=(profile.musicMode&MUSIC_PLAYMODE)+8;
					if(profile.musicMode>16)
						profile.musicMode=0;
					profile.musicMode+=i;
					PlaySong(curMap->song);
				}
				break;
			case PE_MUSICTYPE:
				i=profile.musicMode&MUSIC_PLAYMODE;
				profile.musicMode=(profile.musicMode&MUSIC_LISTBITS)+1;
				if(profile.musicMode>4)
					profile.musicMode=0;
				profile.musicMode+=i;
				if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
					profile.musicMode=MUSIC_OFFICIAL|MUSIC_REPEAT;
				PlaySong(curMap->song);
				break;
			case PE_SONG:
				ChooseNextSong();
				break;
			case PE_MUSIC:
				cursor=0;
				menuMode=3-menuMode;
				if(menuMode==3)
				{
					FillPauseMenu(musicPause);
				}
				else
				{
					if(!shopping && !editing)
					{
						// if cheats are available, use cheat game list instead
						if(ItemPurchased(SHOP_MAJOR,MAJOR_CHEATMENU))
							FillPauseMenu(gameCheatPause);
						else
							FillPauseMenu(gamePause);

					}
					else if(!shopping)
					{
						FillPauseMenu(editPause);
					}
					else
						FillPauseMenu(shopPause);
				}
				i=100;
				break;
			case PE_EXIT:
				if(!shopping && !editing)
				{
					strcpy(profile.lastWorld,player.worldName);
					player.worldProg->levelOn=player.levelNum;
				}
				else if (editing)
				{
					PlaySongForce("");
				}
				SaveProfile();
				return PAUSE_EXIT;
			case PE_SHOP:
				if(!shopping)
				{
					strcpy(profile.lastWorld,player.worldName);
					player.worldProg->levelOn=player.levelNum;
				}
				SaveProfile();
				return PAUSE_SHOP;
			case PE_CHEAT:
				cursor=0;
				menuMode=1-menuMode;
				if(menuMode==1)
				{
					FillPauseMenu(cheatPause);
				}
				else
				{
					if(!editing)
					{
						// if cheats are available, use cheat game list instead
						if(ItemPurchased(SHOP_MAJOR,MAJOR_CHEATMENU))
							FillPauseMenu(gameCheatPause);
						else
							FillPauseMenu(gamePause);

					}
					else
					{
						FillPauseMenu(editPause);
					}
				}
				i=100;
				break;
			case PE_MORECHEATS:
				cursor=0;
				FillPauseMenu(cheatPause2);
				i=100;
				break;
			case PE_PREVCHEATS:
				cursor=0;
				FillPauseMenu(cheatPause);
				i=100;
				break;
			case PE_MORECHEATS2:
				cursor=0;
				FillPauseMenu(cheatPause3);
				i=100;
				break;
			case PE_PREVCHEATS2:
				cursor=0;
				FillPauseMenu(cheatPause2);
				i=100;
				break;
			case PE_BZZT:
				MakeNormalSound(SND_TURRETBZZT);
				break;
			case PE_WPNLOCK:
				if(profile.progress.wpnLock)
					profile.progress.wpnLock=0;
				else
					profile.progress.wpnLock=1;
				if(menuMode==1)
				{
					FillPauseMenu(cheatPause);
				}
				else
				{
					if(!editing)
					{
						// if cheats are available, use cheat game list instead
						if(ItemPurchased(SHOP_MAJOR,MAJOR_CHEATMENU))
							FillPauseMenu(gameCheatPause);
						else
							FillPauseMenu(gamePause);

					}
					else
					{
						FillPauseMenu(editPause);
					}
				}

				break;
		}
		if(i==0 && menu[cursor].effect>=PE_CHEATS)
		{
			DoCheat(menu[cursor].effect-PE_CHEATS);
			return PAUSE_CONTINUE;
		}
		SetupSoundItems();
	}


	HandlePauseKeyPresses(mgl);
	if(lastKey==27 || ((c&CONTROL_B3) && !(oldc&CONTROL_B3)))	// hit ESC to exit pause menu
	{
		MakeNormalSound(SND_MENUSELECT);
		lastKey=0;
		return PAUSE_CONTINUE;
	}

	oldc=c;

	return PAUSE_PAUSED;
}
