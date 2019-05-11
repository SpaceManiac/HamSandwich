#include "cheat.h"
#include "challenge.h"

#ifdef CHEAT
#define NUM_CHEATS 17
#else
#define NUM_CHEATS 9
#endif

char cheatCode[NUM_CHEATS][16]={
	"iamawinner",		// auto-win the level
	"plopplopfizfiz",	// max hammer up
	"kaboooom",			// super nuke attack
	"staypuft",			// all keys
	"mashmash",			// full health
	"rocknroll",		// shield
	"liquefaction",		// waterwalk
	"rutabaga",			// gain a level
	"damedinero",		// free money
#ifdef CHEAT
	/* THE REST ARE TEMP CHEATS!! */
	"one",				// all type 1 spells
	"two",				// all type 2 spells
	"dorf",				// get challenge stars
	"xp",				// gain 10 levels
	"gamera",			// get armageddon sword
	"chapterwin",		// skip this chapter
	"boss",				// go to final boss, if in chapter 4
	"secretize",		// get all secrets
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
	int i;

	// scoot the existing letters over 1
	for(i=0;i<15;i++)
		lastKeys[i]=lastKeys[i+1];
	// and stick the new one on the end
	lastKeys[15]=c;

	_strlwr(&lastKeys[15]);

	for(i=0;i<NUM_CHEATS;i++)
	{
		if(!strcmp(cheatCode[i],&lastKeys[16-strlen(cheatCode[i])]))
		{
			// cheat was triggered
			DoCheat(i);
			lastKeys[15]='\0';	// scrap the highest letter, effectively clearing the buffer
		}
	}
}

void DoCheat(byte w)
{
	int cx,cy,i;

	switch(w)
	{
		case 0:	// auto-win level
			MakeNormalSound(SND_CHEATWIN);
			NewMessage("Lemme out!",30);
			SendMessageToGame(MSG_WINLEVEL,0);
			break;
		case 1:	// max hammer up
			for(i=0;i<5;i++)
				PlayerGetItem(ITM_HAMMERUP,0,0);
			for(i=0;i<4;i++)
				PlayerGetItem(ITM_PANTS,0,0);
			PlayerGetItem(ITM_REVERSE,0,0);
			PlayerGetItem(ITM_REFLECT,0,0);
			NewMessage("ULTRA FIRE UP!!",30);
			break;
		case 2:	// meganuke
			GetCamera(&cx,&cy);
			cx-=320;
			cy-=240;
			for(i=0;i<60;i++)
			{
				FireBullet((cx+MGL_random(640))<<FIXSHIFT,(cy+MGL_random(480))<<FIXSHIFT,
							0,BLT_BOOM);
			}
			ShakeScreen(10);	// make the screen shake!
			break;
		case 3:	// all keys
			for(i=0;i<3;i++)
				PlayerGetItem(ITM_KEY,0,0);
			PlayerGetItem(ITM_KEYR,0,0);
			PlayerGetItem(ITM_KEYG,0,0);
			PlayerGetItem(ITM_KEYB,0,0);
			NewMessage("I am the keymaster!",30);
			break;
		case 4:	// restore health
			PlayerHeal(128);
			player.mana=128;
			NewMessage("Aaaaah",30);
			MakeNormalSound(SND_HEALTHCHEAT);
			break;
		case 5:	// shield
			player.stoneskin=30*20;
			break;
		case 6:	// water-walk
			ToggleWaterwalk();
			MakeNormalSound(SND_CHEATWIN);
			break;
		case 7: // gain a level
			if(player.level<50)
				player.level++;
			NewMessage("Level Up!",30);
			break;
		case 8: // money
			player.money+=50000;
			NewMessage("DINERO POR TODOS!",30);
			break;
		// THE REST ARE TEMP CHEATS!
		case 9:
			for(i=0;i<10;i++)
				player.spell[i]=1;
			break;
		case 10:
			for(i=0;i<9;i++)
				player.spell[i]=2;
			break;
		case 11:
			CheatChal();
			NewMessage("Got 10 stars",30);
			break;
		case 12:
			PlayerGetPoints(10000);
			break;
		case 13:
			CheckForAllSecrets();
			break;
		case 14:	// auto-win level
			if(player.worldNum==3)
				break;
			MakeNormalSound(SND_CHEATWIN);
			NewMessage("Lemme out!",30);
			SendMessageToGame(MSG_WINLEVEL,0);
			switch(player.worldNum)
			{
				case 0:
					player.levelNum=14;
					break;
				case 1:
					player.levelNum=12;
					break;
				case 2:
					player.levelNum=15;
					break;
			}
			break;
		case 15:
			if(player.worldNum==3)
				SendMessageToGame(MSG_GOTOMAP,11);
			if(player.worldNum==0)
				SendMessageToGame(MSG_GOTOMAP,14);
			break;
		case 16:
			for(i=0;i<20;i++)
				player.gotSpell[i]=1;
			player.haveFairy=65535;
			player.keychain[0]=1;
			player.keychain[1]=1;
			player.keychain[2]=1;
			player.keychain[3]=1;
			KeyChainAllCheck();
			CheckForAllSecrets();
			break;
	}
}
