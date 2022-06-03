#include "cheat.h"

#define NUM_CHEATS 7

char cheatCode[NUM_CHEATS][16]={
	"zappo",			// auto-win the level
	"hammersplease",	// max hammer up
	"kablooie",			// super nuke attack
	"zool",				// all keys
	"medic",			// full health
	"barrier",			// shield
	"agua"				// waterwalk
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
				PlayerGetItem(ITM_HAMMERUP);
			for(i=0;i<4;i++)
				PlayerGetItem(ITM_PANTS);
			PlayerGetItem(ITM_REVERSE);
			PlayerGetItem(ITM_REFLECT);
			NewMessage("ULTRA HAMMER UP!!",30);
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
				PlayerGetItem(ITM_KEY);
			PlayerGetItem(ITM_KEYR);
			PlayerGetItem(ITM_KEYG);
			PlayerGetItem(ITM_KEYB);
			NewMessage("I am the keymaster!",30);
			break;
		case 4:	// restore health
			PlayerHeal(128);
			NewMessage("Aaaaah",30);
			MakeNormalSound(SND_HEALTHCHEAT);
			break;
		case 5:	// shield
			PlayerGetItem(ITM_SHIELD);
			break;
		case 6:	// water-walk
			ToggleWaterwalk();
			MakeNormalSound(SND_CHEATWIN);
			break;
	}
}
