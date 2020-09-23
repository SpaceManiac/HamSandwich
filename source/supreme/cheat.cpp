#include "winpch.h"
#include "cheat.h"
#include "shop.h"
#include "gallery.h"
#include "editor.h"
#include "guy.h"
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
	"All Candles",
	"Super Steelskin",
	"Quick Fix",
	"Confusion",
	"Presto Change-O",
	"Black Hole"
};

#define NUM_TYPE_CHEATS	4

char cheatCode[NUM_TYPE_CHEATS][12]={
#ifdef IGF
	"igfsupreme",	// get everything
	"igfcheese",	// get 100,000 coins and 50 keys
#else
	"scummypunk",	// get all cheats
	"leartiste",	// get the editor
	"cheeter",	// get everything
	"rosebud",	// get 1,000 coins and a key
#endif
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
	int i,j;

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
#ifdef IGF
			switch(i)
			{
				case 0:	// everything
					for(j=0;j<256;j++)
					{
						profile.progress.purchase[j]|=SIF_BOUGHT;
						profile.progress.purchase[j]&=(~SIF_AVAILABLE);
					}
					profile.progress.purchase[ShopItemNumber(SHOP_DONATION,0)]=7;
					for(j=0;j<NUM_MONSTERS;j++)
						profile.progress.scanned[j]=1;
					for(j=0;j<120;j++)
						profile.progress.goal[j]=1;
					for(j=0;j<20;j++)
						profile.progress.movie[j]=1;
					NewBigMessage("IGF SUPREME!",60);
					SetupShops(curMap);
					SetupGalPix(curMap);
					break;
				case 1:	// get 100,000 coins & 50 loonykeys
					profile.progress.totalCoins+=100000;
					profile.progress.loonyKeys+=50;
					NewBigMessage("IGF CHEESE!",60);
					break;
			}
#else
			switch(i)
			{
				case 0:	// cheats
					profile.progress.purchase[ShopItemNumber(SHOP_MAJOR,MAJOR_CHEATMENU)]|=SIF_BOUGHT;
					profile.progress.purchase[ShopItemNumber(SHOP_MAJOR,MAJOR_CHEATMENU)]&=(~SIF_AVAILABLE);
					for(j=0;j<NUM_CHEATS;j++)
					{
						profile.progress.purchase[ShopItemNumber(SHOP_CHEAT,j)]|=SIF_BOUGHT;
						profile.progress.purchase[ShopItemNumber(SHOP_CHEAT,j)]&=(~SIF_AVAILABLE);
					}
					NewBigMessage("YOU ARE DIRTY SCUM!!",60);
					SetupShops(curMap);
					SetupGalPix(curMap);
					break;
				case 1:	// editor
					profile.progress.purchase[ShopItemNumber(SHOP_MAJOR,MAJOR_EDITOR)]|=SIF_BOUGHT;
					profile.progress.purchase[ShopItemNumber(SHOP_MAJOR,MAJOR_EDITOR)]&=(~SIF_AVAILABLE);
					NewBigMessage("THE EDITOR IS YOURS!",60);
					break;
				case 2:	// ALL
					for(j=0;j<256;j++)
					{
						profile.progress.purchase[j]|=SIF_BOUGHT;
						profile.progress.purchase[j]&=(~SIF_AVAILABLE);
					}
					NewBigMessage("NO SWEARING!",60);
					SetupShops(curMap);
					SetupGalPix(curMap);
					break;
				case 3:	// editor
					profile.progress.totalCoins+=1000;
					profile.progress.loonyKeys+=1;
					NewBigMessage("DIRTY CHEATER.",60);
					break;
			}
#endif

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
	if(!editing)
		profile.progress.cheats++;
	switch(w)
	{
		case CHEAT_WINLEVEL:
			MakeNormalSound(SND_CHEATWIN);
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
			MakeNormalSound(SND_HEALTHCHEAT);
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
				FireBullet(0,(cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
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
			player.keys[4]=1;
			player.keys[5]=1;
			MakeNormalSound(SND_GETKEY);
			NewMessage("I am the keymaster!",30,0);
			break;
		case CHEAT_SCANNER:
			player.weapon=WPN_SCANNER;
			player.ammo=5;
			NewMessage("Scanner!",30,0);
			break;
		case CHEAT_AMMO:
			if(player.weapon==0 || player.ammo==0)
				player.weapon=WPN_AK8087;
			player.ammoCrate=255;
			NewMessage("Ammo Crate!",30,0);
			break;
		case CHEAT_LIGHT:
			player.cheatrFlags^=HMR_LIGHT;
			MakeNormalSound(SND_LIGHTSON);
			break;
		case CHEAT_WATER:
			ToggleWaterwalk();
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHEAT_OXYGEN:
			player.cheatrFlags^=HMR_OXYGEN;
			player.oxygen=127*256;
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHEAT_NOSKID:
			player.cheatrFlags^=HMR_NOSKID;
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHEAT_SPEED:
			player.cheatrFlags^=HMR_SPEED;
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHEAT_RAGE:
			player.rage=127*256;
			MakeNormalSound(SND_RAGE);
			break;
		case CHEAT_CANDLES:
			player.candles=curMap->numCandles;
			SetPlayerGlow(128);
			MakeNormalSound(SND_ALLCANDLE);
			break;
		case CHEAT_STRENGTH:
			player.cheatrFlags^=HMR_STRENGTH;
			MakeNormalSound(SND_CHEATWIN);
			NewMessage("I've Got Nerves of Steel!",30,0);
			break;
		case CHEAT_QUICKFIX:
			goodguy->weak = 0;
			goodguy->ignited = 0;
			goodguy->poison = 0;
			goodguy->frozen = 0;
			MakeNormalSound(SND_HEALTHCHEAT);
			break;
		case CHEAT_CONFUSE:
			for(i=0;i<GetMaxGuys();i++)
			{
				if(guys[i]->aiType!=1 && !(MonsterFlags(guys[i]->type,guys[i]->aiType)&(MF_NOHIT|MF_INVINCIBLE)))
				{
					guys[i]->confuse=255;
					TeamChangeRing(6,guys[i]->x,guys[i]->y,0,16,4);
				}
			}
			MakeNormalSound(SND_WIZHIT);
			NewMessage("Abracadabra!",30,0);
			break;
		case CHEAT_PRESTO:
			player.playAs = Random(9);
			TeamChangeRing(6,goodguy->x,goodguy->y,0,16,4);
			MakeNormalSound(SND_WIZHIT);
			NewMessage("Alakazam!",30,0);
			break;
		case CHEAT_BLACKHOLE:
			player.weapon=WPN_BLACKHOLE;
			player.ammo=5;
			NewMessage("Abyssinator!",30,0);
			break;
	}
}

char *CheatName(byte w)
{
	return cheatName[w];
}
