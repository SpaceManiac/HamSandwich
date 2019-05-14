#include "cheat.h"
#include "quest.h"
#include "options.h"
#include "title.h"
#include "shop.h"
#include "map.h"
#include "game.h"

#define NUM_CHEATS 4

char cheatCode[NUM_CHEATS][16]={
	"controls",		// kablooie
	"goodbye",		// 100% health
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
	switch(w)
	{
		case 0:	// explosion
			memcpy(&player.var[VAR_CONTROLS],opt.control,sizeof(byte)*2*8);
			memcpy(&player.var[VAR_CONTROLS+2*8],opt.joyCtrl,sizeof(byte)*4);	// copy in the controls
			ApplyControlSettingsMem(&player.var[VAR_CONTROLS]);
			break;
		case 1:	// suicide
			meleeAttacker=NULL;
			goodguy->GetShotReal(0,0,9999,curMap,&curWorld);
			break;
		case 2:	// temporary invulnerability
			break;
		case 3:	// super speed
			break;
	}
}
