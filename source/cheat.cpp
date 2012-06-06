#include "cheat.h"
#include "items.h"
#include <ctype.h> // for tolower

const int NUM_CHEATS = 13;

char cheatCode[NUM_CHEATS][16] = {
	"zappo", // auto-win the level
	"hammersplease", // max hammer up
	"kablooie", // super nuke attack
	"zool", // all keys
	"medic", // full health
	"barrier", // shield
	"agua", // waterwalk
	"stampy", // keychains!
	"brainz", // get all brains
	"itembright", // light up items
	"whereami", // cloak d'invisible
	"conwiz", // conspiracies!
	"arglebargle", // fill with rage
};

char lastKeys[17]; // the last 16 letter keys pressed

void InitCheater(void)
{
	int i;

	for (i = 0; i < 16; i++)
		lastKeys[i] = '\0';

	lastKeys[16] = '\0'; // a padding 0, so strcmp can be used
}

void CheatKey(char c)
{
	int i;

	// scoot the existing letters over 1
	for (i = 0; i < 15; i++)
		lastKeys[i] = lastKeys[i + 1];
	// and stick the new one on the end
	lastKeys[15] = c;

	lastKeys[15] = tolower(lastKeys[15]);

	for (i = 0; i < NUM_CHEATS; i++)
	{
		if (!strcmp(cheatCode[i], &lastKeys[16 - strlen(cheatCode[i])]))
		{
			// cheat was triggered
			DoCheat(i);
			lastKeys[15] = '\0'; // scrap the highest letter, effectively clearing the buffer
		}
	}
}

void DoCheat(byte w)
{
	int cx, cy, i;

	switch (w) {
		case 0: // auto-win level
			MakeNormalSound(SND_CHEATWIN);
			NewMessage("Lemme out!", 30, 0);
			SendMessageToGame(MSG_WINLEVEL, 0);
			break;
		case 1: // max hammer up
			for (i = 0; i < 5; i++)
				PlayerGetItem(ITM_HAMMERUP, 0, 0);
			for (i = 0; i < 4; i++)
				PlayerGetItem(ITM_PANTS, 0, 0);
			PlayerGetItem(ITM_REVERSE, 0, 0);
			PlayerGetItem(ITM_REFLECT, 0, 0);
			NewMessage("ULTRA HAMMER UP!!", 30, 0);
			break;
		case 2: // meganuke
			GetCamera(&cx, &cy);
			cx -= 320;
			cy -= 240;
			for (i = 0; i < 60; i++)
			{
				FireBullet((cx + MGL_random(640)) << FIXSHIFT, (cy + MGL_random(480)) << FIXSHIFT,
						0, BLT_BOOM, 1);
			}
			ShakeScreen(10); // make the screen shake!
			break;
		case 3: // all keys
			for (i = 0; i < 3; i++)
				PlayerGetItem(ITM_KEY, 0, 0);
			PlayerGetItem(ITM_KEYR, 0, 0);
			PlayerGetItem(ITM_KEYG, 0, 0);
			PlayerGetItem(ITM_KEYB, 0, 0);
			NewMessage("I am the keymaster!", 30, 0);
			break;
		case 4: // restore health
			PlayerHeal(128);
			NewMessage("Aaaaah", 30, 0);
			MakeNormalSound(SND_HEALTHCHEAT);
			break;
		case 5: // shield
			PlayerGetItem(ITM_SHIELD, 0, 0);
			break;
		case 6: // water-walk
			ToggleWaterwalk();
			MakeNormalSound(SND_CHEATWIN);
			break;
		case 7: // keychains
			PlayerGetItem(ITM_KEYCH1, 0, 0);
			PlayerGetItem(ITM_KEYCH2, 0, 0);
			PlayerGetItem(ITM_KEYCH3, 0, 0);
			PlayerGetItem(ITM_KEYCH4, 0, 0);
			break;
		case 8: // brains
			for (i = 0; i < 20; i++)
				PlayerGetItem(ITM_BRAIN, 0, 0);
			break;
		case 9: // itemlight
			ItemLightUp();
			break;
		case 10: // clock invisible
			PlayerGetItem(ITM_INVIS, 0, 0);
			break;
		case 11: // conwiz!
			NewBigMessage("Conspiracies", 30);
			break;
		case 12: // fill with rage
			NewMessage("Filling... with... RAGE!!", 75, 0);
			player.rage = 127 * 256;
			break;
	}
}
