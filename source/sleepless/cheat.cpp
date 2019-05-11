#include "cheat.h"
#include "shop.h"
#include "gallery.h"
#include "editor.h"
#include <ctype.h>

char cheatName[NUM_CHEATS][16]={
	"Win Level",
	"Ultra Hammer-Up",
	"Max. Health",
	"Energy Barrier",
	"KABLOOIE!",
	"Freeze Enemies",
	"All Brains",
	"All Keys",
	"Free Scanner",
	"Ammo Crate",
	"Lights On",
	"Walk On Water",
	"Infinite Oxygen",
	"No-Skid Boots",
	"Super Speed",
	"Max. Rage",
};

#define NUM_TYPE_CHEATS	2

char cheatCode[NUM_TYPE_CHEATS][12]={
	"idspispopd",	// walk through walls
	"iddqd",	// invincible
};

char lastKeys[17];	// the last 16 letter keys pressed

void InitCheater(void)
{
	int i;

	for(i=0;i<16;i++)
		lastKeys[i]='\0';

	lastKeys[16]='\0';	// a padding 0, so strcmp can be used
}

void CheatKey(char c)
{
	int i;

	// scoot the existing letters over 1
	for(i=0;i<15;i++)
		lastKeys[i]=lastKeys[i+1];
	// and stick the new one on the end
	lastKeys[15]=tolower(c);

	for(i=0;i<NUM_TYPE_CHEATS;i++)
	{
		if(!strcmp(cheatCode[i],&lastKeys[16-strlen(cheatCode[i])]))
		{
			// cheat was triggered
			switch(i)
			{
				case 0:	// wall walk
					cheatsOn^=1;
					NewMessage("Walk through walls!",90,1);
					break;
				case 1:	// invincible
					cheatsOn^=2;
					NewMessage("Invincibility!",90,1);
					break;
			}

			lastKeys[15]='\0';	// scrap the highest letter, effectively clearing the buffer
		}
	}
}

void DoCheat(byte w)
{
	int cx,cy,i;

	if(w==CHEAT_WINLEVEL && curMap->flags&MAP_HUB)
	{
		MakeNormalSound(SND_TURRETBZZT);
		return;
	}

	player.cheated=1;
	if(!editing && verified)
		profile.progress.cheats++;
	switch(w)
	{
		case CHEAT_WINLEVEL:
			NewMessage("Lemme out!",30,0);
			SendMessageToGame(MSG_WINLEVEL,0);
			break;
		case CHEAT_HAMMERUP:
			player.hammers=5;
			player.hamSpeed=0;
			player.cheesePower=255;
			player.hammerFlags|=HMR_REVERSE|HMR_REFLECT;
			MakeNormalSound(SND_HAMMERUP);
			NewMessage("ULTRA HAMMER UP!!",30,0);
			break;
		case CHEAT_LIFE:
			PlayerHeal(128);
			NewMessage("Aaaaah",30,0);
			break;
		case CHEAT_SHIELD:
			player.shield=255;
			MakeNormalSound(SND_SHIELD);
			break;
		case CHEAT_BOOM:
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			for(i=0;i<60;i++)
			{
				FireBullet((cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
							0,BLT_BOOM,1);
			}
			ShakeScreen(10);	// make the screen shake!
			break;
		case CHEAT_FREEZE:
			player.timeStop=255;
			break;
		case CHEAT_BRAINS:
			player.brains=curMap->numBrains;
			SetPlayerGlow(128);
			MakeNormalSound(SND_KOOLKAT);
			break;
		case CHEAT_KEYS:
			player.keys[0]=3;
			player.keys[1]=1;
			player.keys[2]=1;
			player.keys[3]=1;
			MakeNormalSound(SND_GETKEY);
			NewMessage("I am the keymaster!",30,0);
			break;
		case CHEAT_SCANNER:
			player.ammo=5;
			NewMessage("Scanner!",30,0);
			break;
		case CHEAT_AMMO:
			player.ammoCrate=255;
			NewMessage("Ammo Crate!",30,0);
			break;
		case CHEAT_LIGHT:
			player.hammerFlags^=HMR_LIGHT;
			MakeNormalSound(SND_LIGHTSON);
			break;
		case CHEAT_WATER:
			ToggleWaterwalk();
			break;
		case CHEAT_OXYGEN:
			break;
		case CHEAT_NOSKID:
			break;
		case CHEAT_SPEED:
			player.hammerFlags^=HMR_SPEED;
			break;
		case CHEAT_RAGE:
			player.rage=127*256;
			MakeNormalSound(SND_RAGE);
			break;
	}
}

char *CheatName(byte w)
{
	return cheatName[w];
}
