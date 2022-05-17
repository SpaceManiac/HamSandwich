#include "cheat.h"
#include "options.h"
#include "title.h"

#define NUM_CHEATS 11

char cheatCode[NUM_CHEATS][16]={
	"stocking",		// 0:level skip
	"starrynight",	// 1:all stars
	"iamscum",		// 2:get points
	"hammersplease", // 3:dr.lunatic cheat
	"hello",			// 4:say hi
	"hi",				// 5:say hi
	"howareyou",		// 6:how are you
	"fine",				// 7:response to it
	"good",				// 8:also response to it
	"bad",				// 9:more response
	"sad",				// 10:more response
	};

char lastKeys[17];	// the last 16 letter keys pressed
byte askedFeelings;

void InitCheater(void)
{
	int i;

	for(i=0;i<16;i++)
		lastKeys[i]='\0';

	lastKeys[16]='\0';	// a padding 0, so strcmp can be used
	askedFeelings=0;
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
		case 0:	// level skip
			SendMessageToGame(MSG_WINLEVEL,0);
			break;
		case 1:	// get all stars
#ifdef BETA
			profile.stars=999;
			profile.starsLeft=999;
			SaveProfile(opt.curProfile);
#else
			NewBigMessage("You typed \"STARRYNIGHT\"! Good typing!",90);
#endif
			break;
		case 2:	// get points
			player.score+=10000;
			break;
		case 3:	// dr. lunatic cheat
			NewBigMessage("This isn't Dr. Lunatic!",90);
			break;
		case 4:
		case 5:	// say hi
			NewBigMessage("Hi!!",90);
			break;
		case 6: // how are you
			NewBigMessage("Why I'm doing great. How are YOU?",90);
			askedFeelings=1;
			break;
		case 7:
		case 8:	// fine/good
			if(askedFeelings)
			{
				NewBigMessage("Well, that's just dandy!",90);
				askedFeelings=0;
			}
			else
				NewBigMessage("Huh??",90);
			break;
		case 9:	// bad
		case 10: // sad
			if(askedFeelings)
			{
				NewBigMessage("Oh, I'm sorry!",90);
				askedFeelings=0;
			}
			else
				NewBigMessage("You seem to have emotional problems.",90);
			break;

	}
}
