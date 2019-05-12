#include "help.h"
#include "options.h"

typedef struct help_t
{
	char name[32];
	char txt[10][128];
} help_t;

help_t help[]={
	{"Adventure Mode",
	 "Bump into the villagers to talk to them. They will give you",
	 "quests you need to fulfill. Also look around carefully for",
	 "powerups to boost your abilities! There are many items to find",
	 "which will allow you to go to new places and to open areas you",
	 "couldn't before. Press the Fire button to shoot your slingshot",
	 "and press the Weapon button to use any special weapons you find.",
	 "Remember to save your game often by bumping into Save Crystals",
	 "like the one in the middle of Luniton Village. When you are",
	 "defeated, you'll restart from the last place you saved!",
	 "",
	},
	{"Survival Mode",
	 "25 rounds of mayhem and pain. Survive them all and you win.",
	 "The controls are the same as in Adventure Mode.  After each",
	 "round, you earn two random powerups.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	},
	{"Slingshot Mode",
	 "No help.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	},
	{"Loonyball Mode",
	 "Attempt to score 3 goals in 2 minutes.  There are five games",
	 "total, get through them all to win.  The final game does not",
	 "have a 3-goal limit, so score as many times as you can to boost",
	 "your final ranking.  Bump into the ball with your body to dribble",
	 "it down the field.  The Fire button is used as normal, and you",
	 "can shoot enemies to put them in the penalty box.  But you",
	 "can't use special weapons - instead, hold down the Weapon",
	 "button as you bump into the ball to kick it a long way. Losing",
	 "all your hearts won't end your game, but will cause you to",
	 "suffer a 10 second penalty.",
	},
	{"Bowling Mode",
	 "Bowl for pumpkins! Grab a ball from the ball dispenser on the",
	 "right side, then line yourself up with the lane and press Fire",
	 "to throw the ball. You can bank the ball off of the walls for",
	 "tough shots, and can also get a running start to make it move",
	 "faster.  The most important technique, however, is that, just",
	 "like in real life, you can influence the motion of the ball",
	 "after you've thrown it by moving left or right. There are 5",
	 "rounds to play, but if you score a strike, you get to play a",
	 "bonus round to help you earn even more points.",
	 "",
	},
	{"Boss Bash Mode",
	 "Select a power level first. This determines how many power-ups",
	 "you begin the battle with. Your final score is lowered for",
	 "choosing a higher power level. Then you select your victim!",
	 "You can choose from any of the bosses in the Adventure mode,",
	 "or for more fun, there are several 'scenarios' you can play",
	 "instead. Gameplay works just like Adventure mode, and you win",
	 "when you defeat every monster.",
	 "",
	 "",
	 "",
	},
	{"Infinite Survival",
	 "A never-ending series of little battles against increasingly",
	 "more deadly enemies.  You receive one power-up at random each",
	 "time you pass a level, until you've gotten them all.  After",
	 "that, you're on your own!",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	},
	{"Remix Mode",
	 "For advanced players, here's a chance to go through the",
	 "Adventure again, but this time, you'll encounter much tougher",
	 "enemies, some changed puzzles, and relocated power-ups.",
	 "The actual quests are the same, but you might encounter some",
	 "surprises trying to accomplish them!",
	 "",
	 "",
	 "",
	 "",
	 "",
	},
};

static byte oldc;

void HelpDisplay(MGLDraw *mgl)
{
	int i,p;
	byte colTab[]={0,32,0,96,128,192,32,224};

	p=player.worldNum;

	if(player.worldNum==WORLD_SURVIVAL && opt.cheats[CH_RNDSURVIVAL])
		p=6;

	for(i=0;i<480;i++)
		memset(mgl->GetScreen()+mgl->GetWidth()*i,colTab[p]+(i*16)/480,640);

	CenterPrintGlow(320,6,help[p].name,0,2);

	for(i=0;i<10;i++)
		CenterPrintGlow(320,100+i*25,help[p].txt[i],0,0);
}

byte HelpUpdate(MGLDraw *mgl)
{
	byte c;

	c=GetControls()|GetArrows();

	if((c&(CONTROL_B1|CONTROL_B2)) && (!(oldc&(CONTROL_B1|CONTROL_B2))))
	{
		return 1;
	}
	oldc=c;

	c=mgl->LastKeyPressed();
	if(c)
		return 1;

	return 0;
}

void Help(MGLDraw *mgl)
{
	byte done;

	if(!opt.helpOn)
		return;

	oldc=255;

	mgl->LastKeyPressed();

	oldc=CONTROL_B1|CONTROL_B2;

	done=0;
	while(done==0)
	{
		done=HelpUpdate(mgl);
		HelpDisplay(mgl);
		mgl->Flip();
		if(!mgl->Process())
		{
			return;
		}
	}
}
