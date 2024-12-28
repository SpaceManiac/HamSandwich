#include "pause.h"
#include "player.h"
#include "shop.h"
#include "options.h"
#include "challenge.h"
#include "appdata.h"
#include "archipelago.h"

#define SUBMODE_NONE	 0
#define SUBMODE_SLOTPICK 1

byte cursor=0;
static byte subcursor=0;
static char lastKey=0;
byte subMode;
static byte giveUp=0;	// which text should be shown for "Give Up"
int pauseX=-250,subX=-250;
byte saveLevel[5],saveNightmare[5];
byte saveChapter[5];
byte saveHour[5],saveMin[5];
static byte armaBrt=0;

void ResetPauseMenu(void)
{
	pauseX=-250;
	subX=-250;
	cursor=0;
}

void SetSubCursor(byte s)
{
	subcursor=s;
}

void RenderPauseMenu(void)
{
	if(pauseX==-250)
		return;

	RenderPlayerGear(440-pauseX*2,armaBrt);

	GetShopSpr(34)->Draw(pauseX,230,GetDisplayMGL());
	PrintBrightGlow(pauseX+10,240,"Cancel",-16+(cursor==0)*16,0);
	if(!Challenging() && !Archipelago())
	{
		PrintBrightGlow(pauseX+10,280,"Load",-16+(cursor==1)*16,0);
	}
	if(giveUp==0)
	{
		if (!Archipelago())
		{
			PrintBrightGlow(pauseX+10,320,"Save",-16+(cursor==2)*16,0);
		}
	}
	else if(giveUp==2)	// random battle
		PrintBrightGlow(pauseX+10,320,"Run Away",-16+(cursor==2)*16,0);
	else 	// regular level
		PrintBrightGlow(pauseX+10,320,"Give Up",-16+(cursor==2)*16,0);

	PrintBrightGlow(pauseX+10,360,"Music:",-16+(cursor==4)*16,0);
	if(opt.musicVol==0)
		PrintBrightGlow(pauseX+10+160,360,"Off",-16+(cursor==4)*16,0);
	else if(opt.musicVol==1)
		PrintBrightGlow(pauseX+10+160,360,"I",-16+(cursor==4)*16,0);
	else if(opt.musicVol==2)
		PrintBrightGlow(pauseX+10+160,360,"II",-16+(cursor==4)*16,0);
	else if(opt.musicVol==3)
		PrintBrightGlow(pauseX+10+160,360,"III",-16+(cursor==4)*16,0);
	PrintBrightGlow(pauseX+10,400,"Quit Game",-16+(cursor==5)*16,0);
	RenderSlotPickMenu();

	if (auto ap = Archipelago())
	{
		std::string s = "AP: ";
		s += ap->Status();
		Print(pauseX+10,SCRHEI-26+2,s,1,2);
		Print(pauseX+10,SCRHEI-26,s,0,2);
	}
}

void RenderSlotPickMenu(void)
{
	int i;
	char s[32];

	GetShopSpr(34)->Draw(subX,40,GetDisplayMGL());

	for(i=0;i<5;i++)
	{
		s[0]='1'+i;
		s[1]='\0';
		if(subcursor==i)
		{
			PrintBrightGlow(subX+10,50+i*40,s,0,0);
			if(saveChapter[i]==0 || saveLevel[i]==0)
			{
				PrintBrightGlow(subX+40,50+i*40,"Empty Slot",0,2);
			}
			else
			{
				if(!saveNightmare[i])
					sprintf(s,"Chapter %d",saveChapter[i]);
				else
					sprintf(s,"Chapter %d!!!",saveChapter[i]);
				PrintBrightGlow(subX+40,50+i*40,s,0,2);
				sprintf(s,"%02d:%02d  Lvl: %02d",saveHour[i],saveMin[i],saveLevel[i]);
				PrintBrightGlow(subX+40,50+i*40+20,s,0,2);
			}
		}
		else
		{
			PrintBright(subX+10,50+i*40,s,-32,0);
			if(saveChapter[i]==0 || saveLevel[i]==0)
			{
				PrintBright(subX+40,50+i*40,"Empty Slot",-32,2);
			}
			else
			{
				if(!saveNightmare[i])
					sprintf(s,"Chapter %d",saveChapter[i]);
				else
					sprintf(s,"Chapter %d!!!",saveChapter[i]);
				PrintBright(subX+40,50+i*40,s,-32,2);
				sprintf(s,"%02d:%02d  Lvl: %02d",saveHour[i],saveMin[i],saveLevel[i]);
				PrintBright(subX+40,50+i*40+20,s,-32,2);
			}
		}
	}
}

void HandlePauseKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

float CalcTotalPercent(player_t *p)
{
	int i,amt,total;

	amt=0;
	total=0;
	for(i=0;i<1;i++)
	{
		total+=p->totalCompletion[i];
		amt+=p->complete[i];
	}
	return (float)amt/(float)total;
}

void SetGiveUpText(byte gu)
{
	giveUp=gu;
}

void InitPauseMenu(void)
{
	player_t p;
	int i;

	lastKey=0;
	subMode=0;

	auto f = AppdataOpen("mystic.sav");
	if(!f)
	{
		for(i=0;i<5;i++)
		{
			saveLevel[i]=0;
			saveChapter[i]=0;
			saveHour[i]=0;
			saveMin[i]=0;
		}
	}
	else
	{
		for(i=0;i<5;i++)
		{
			SDL_RWread(f,&p,sizeof(player_t),1);
			saveLevel[i]=p.level;
			saveChapter[i]=p.worldNum+1;
			saveHour[i]=(byte)(p.gameClock/(30*60*60));
			saveMin[i]=(byte)((p.gameClock/(30*60))%60);
			if(p.nightmare)
				saveNightmare[i]=1;
			else
				saveNightmare[i]=0;
		}

		f.reset();
	}
	MakeNormalSound(SND_PAUSE);

	if((Challenging() || Archipelago()) && cursor==1)
		cursor=0;
	if(Archipelago() && cursor==2 && giveUp==0)
		cursor=0;

	pauseX=-250;
	subX=-250;
}

void UpdateUnPausedMenu(void)
{
	if(pauseX>-250)
		pauseX-=25;
	if(subX>-250)
		subX-=25;
}

byte UpdatePauseMenu(MGLDraw *mgl)
{
	byte c;
	static byte oldc=0;
	static byte reptCounter=0;

	armaBrt++;
	if(pauseX<0)
		pauseX+=25;

	if(subMode!=SUBMODE_NONE)
	{
		if(subX<0)
			subX+=25;
	}
	else
	{
		if(subX>-250)
			subX-=25;
	}

	c=GetControls();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if(subMode==SUBMODE_NONE)	// not in any submenu
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			cursor--;
			if(cursor==255)
				cursor=5;
			if(cursor==3)
				cursor=2;
			if(Archipelago() && cursor==2 && giveUp==0)
				cursor=1;
			if((Challenging() || Archipelago()) && cursor==1)
				cursor=0;

			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			cursor++;
			if(cursor==6)
				cursor=0;
			if((Challenging() || Archipelago()) && cursor==1)
				cursor=2;
			if(Archipelago() && cursor==2 && giveUp==0)
				cursor=3;
			if(cursor==3)
				cursor=4;

			MakeNormalSound(SND_MENUCLICK);
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
		   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			MakeNormalSound(SND_MENUSELECT);
			switch(cursor)
			{
				case 0: // cancel
					return 0;
				case 1:	// Load
					subMode=SUBMODE_SLOTPICK;
					break;
				case 2: // Save
					if(giveUp)
						return 2;
					else
						subMode=SUBMODE_SLOTPICK;
					break;
				case 4:	// music
					opt.musicVol++;
					if(opt.musicVol>3)
						opt.musicVol=0;
					if(opt.musicVol==0)
						StopSong();
					else
						VolumeSong(opt.musicVol);
					if(opt.musicVol==1)
						ReplaySong();
					break;
				case 5:	// quit game
					return 3;
			}
		}
	}
	else if(subMode==SUBMODE_SLOTPICK)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor--;
			if(subcursor==255)
				subcursor=4;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			MakeNormalSound(SND_MENUCLICK);
			subcursor++;
			if(subcursor==5)
				subcursor=0;
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
		   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			if(cursor==1)	// Load
			{
				if(saveLevel[subcursor]==0 || saveChapter[subcursor]==0)
				{
					MakeNormalSound(SND_ACIDSPLAT);
				}
				else
				{
					SendMessageToGame(MSG_LOADGAME,0);
					PlayerLoadGame(subcursor);
					MakeNormalSound(SND_LOADGAME);
					subMode=SUBMODE_NONE;
					return 0;
				}
			}
			else if(cursor==2)	// Save
			{
				PlayerSaveGame(subcursor);
				MakeNormalSound(SND_SAVEGAME);
				subMode=SUBMODE_NONE;
				return 0;
			}
		}
	}
	oldc=c;

	HandlePauseKeyPresses(mgl);
	if(lastKey==27)	// hit ESC to exit pause menu
	{
		MakeNormalSound(SND_MENUSELECT);
		if(subMode==SUBMODE_NONE)
			return 0;
		else
			subMode=SUBMODE_NONE;
		lastKey=0;
	}
	return 1;
}
