#include "cheat.h"
#include "quest.h"
#include "options.h"
#include "ch_swampdog.h"
#include "badge.h"
#include "title.h"

#define NUM_CHEATS 4

char cheatCode[NUM_CHEATS][16]={
	"superboom",	// kablooie
	"getalife",		// 100% health
	"bangzoom",		// invincibility for a while
	"gogogo",		// super speed
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
	int i;

	switch(w)
	{
		case 0:	// explosion
			for(i=0;i<50;i++)
			{
				FireBullet(goodguy->x-320*FIXAMT+Random(640*FIXAMT),
						   goodguy->y-240*FIXAMT+Random(480*FIXAMT),0,BLT_GOODBOOM2);
			}
			break;
		case 1:	// 100% health
			player.hearts=player.maxHearts;
			goodguy->hp=player.hearts;
			break;
		case 2:	// temporary invulnerability
			player.invinc=30*30;
			break;
		case 3:	// super speed
			player.speed=255;
			break;
	}
}
