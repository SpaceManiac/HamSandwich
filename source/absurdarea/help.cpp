#include "help.h"
#include "options.h"

typedef struct help_t
{
	char name[32];
	char text[4][64];
} help_t;

help_t tips[]={
	// freebie!
	{"Hamumu Games Rock!",
		{"You should buy every Hamumu game there is, because otherwise",
		 "your friends will all laugh at you.",
		 "",
		 ""},
	},
	// 1 TIP_BEGIN
	{"Playing The Game",
		{"Use the arrow keys or gamepad to move.  Z, CTRL or gamepad",
		"button 1 is Fire.  Use that to exit this tip and conversations.",
		"X or SHIFT or button 2 is Jump.  ESC is Pause. Holding down A",
		"or button 3 brings up the Spell Menu.  Play to find out more!"},
	},
	// 2 TIP_QUEST
	{"Quests",
		{"You just got your first Quest!  Go do what you've been told,",
		"then come back and collect the reward!  If you press ESC, you",
		"can visit the Pause Menu to look at the details of the Quest.",
		"You can do this any time during play."},
	},
	// 3 TIP_GO (went to a new map)
	{"Badguys!",
		{"Look out, there are badguys here!  Press CTRL or Z to attack.",
		 "Your attacks will be slow until you get a better axe.  Press X",
		 "or SHIFT to jump.  A jumping attack does more damage.  Try",
		 "not to get hit.  Don't be scared - dying has little penalty."},
	},
	// 4 TIP_SKILL
	{"Skills",
		{"You just got your first Skill!  When you gain levels, you will",
		 "earn Skill Points that you can spend in the Pause Menu to",
		 "upgrade your Skills.  Skills have no effect until they have",
		 "at least one point invested."},
	},
	// 5 TIP_DIE
	{"The Agony Of Defeat",
		{"You were squished into mush. But don't fear - you're just sent",
		 "back to the last town you were in, and you lose a bit of",
		 "progress on all your Talents.  You never lose any items, money",
		 "or experience.  So you shouldn't fear the reaper.  Much."},
	},
	// 6 TIP_THROWING
	{"Throwing Axes",
		{"To switch between swinging your axe and throwing it, hold down",
		 "A to open the Spell Menu.  Use Left & Right to choose the Axe",
		 "icon, and release A to switch.  You can also just tap A to",
		 "repeat the last thing you did in the Spell Menu."},
	},
	// 7 TIP_STAMINA
	{"Stamina",
		{"You're running low on Stamina!  See the grey meter under Life",
		 "in the upper left?  Attacking uses it up, but it recharges",
		 "quickly.  The more damage you do, the more it costs.  Getting",
		 "hit also lowers your Stamina, so don't get hit!"},
	},
	// 8 TIP_EQUIP
	{"Equipment",
		{"You got a new piece of gear!  Press ESC to visit the Pause",
		 "Menu, and select Items.  From there you can swap the new item",
		 "in.  But don't do that unless it's better than the one you",
		 "already have!  Just a little common sense tip for you."},
	},
	// 9 TIP_POTION
	{"Potions",
		{"Your first Potion!  Hold down A to access the Spell Menu. Tap",
		 "Left or Right to select the Potion.  If you have more Potions,",
		 "tapping Up or Down will flip through them. Release A to drink!",
		 "Better quality potions last longer and have bigger effects."},
	},
	// 10 TIP_QUIT (paused the game)
	{"Saving Your Game",
		{"If you suddenly lose your mind and decide to quit playing",
		"briefly, choose 'Save & Quit' in the Pause Menu here.",
		"It will save everything *exactly* how it was, so you can come",
		"back to where you were when you return."},
	},
	// 11 TIP_GO2 (went into town for the first time)
	{"Starting Out",
		{"Welcome to the town of Eisburg!  Be sure to talk to everyone",
		 "to see what you need to do.  You can complete several quests",
		 "and gain a bunch of levels, Skills, and Talents without even",
		 "leaving town.  So make sure you check out all the cabins!"},
	},
	// 12 TIP_MAGIC
	{"Magic Items",
		{"Your first Magic item!  These have bonus powers, and improve",
		 "one of your Skills.  If it says \"of ????\", that means you",
		 "don't have the Skill that it improves yet.  Magic items are",
		 "usually better than normal, and Golden items are the best!"},
	},
	// 13 TIP_SHOVEL
	{"Shoveling",
		{"You got a Shovel!  To use it, just jump.  This will attempt",
		 "to dig where you land.  If nothing happens, there's nothing",
		 "there.  If there is something to be dug up, you'll dig it up!",
		 "Cracks in the ground and lumps of snow mark digging spots."},
	},
	// 14 TIP_GLASSES
	{"Magic Glasses",
		{"In the Items screen of the Pause menu, you can now equip",
		 "Inscribed Lenses.  Your glasses will give a bonus depending on",
		 "which 2 you have in.  Sadly, they can't be removed.  To change",
		 "the lenses, you'll have to break the existing ones."},
	},
	// 15 TIP_LEVELUP
	{"Level Up!",
		{"You just gained a level!  This gets you 2 Life, 1 Stamina, and",
		 "1 Magic.  You also earn Skill Points with each level (more at",
		 "higher levels).  Spend them in the Pause Menu.  The more points",
		 "you invest in one skill, the more effective it gets."},
	},
	// 16 TIP_SPELL
	{"Magic Spells",
		{"To cast your spell, hold down A to open the Spell Menu.  Press",
		 "the arrows to choose the spell, then release A. You can just",
		 "tap A to repeat the last spell you used.  The blue Magic Meter",
		 "shows how much power you have left to cast spells."},
	},
	// 17 TIP_CLOCKWORK
	{"Clockwork",
		{"To build a Clockwork Robot, just bump into a Clock.  If you",
		 "already have one, bumping a Clock will repair it.  The type",
		 "of junk you build the Robot from determines what abilities",
		 "it has.  If it's destroyed, you must rebuild from scratch!"},
	},
	// 18 TIP_JUNKSMITH
	{"Junksmithing",
		{"Bump into an Oven to begin Junksmithing.  Place junk items",
		 "in the Oven, and it will produce a piece of equipment.  The",
		 "same recipe always makes the same item.  There is a chance",
		 "of failure, decreasing as the Talent improves."
		 },
	},
	// 19 TIP_SYNERGY
	{"Spell Synergy",
		{"You now have two spells of the same Spell School.  Every time",
		 "you add points to a Spell, all Spells of the same School gain",
		 "1% more damage or healing.  Focusing on one School will",
		 "maximize your magic power."
		 },
	},
	// 20 TIP_ENDING
	{"No Turning Back!",
		{"Watch out!  If you still have things you want to do, go back",
		 "down the cliff and do them.  If you win the battle that lies",
		 "beyond the cave you are entering, you will win the game and",
		 "start over in Madcap Mode.",
		},
	},
};

static byte btnwait;
static byte help=0;

void RenderHelp(MGLDraw *mgl)
{
	char s[32];

	if(help==0)
		return;

	// 486 wide
	PauseBox(77,250,563,340,200);

	strcpy(s,"Handy Tip!");
	CenterPrintColor(320-1,220-1,s,0,-32,0);
	CenterPrintColor(320+1,220-1,s,0,-32,0);
	CenterPrintColor(320-1,220+1,s,0,-32,0);
	CenterPrintColor(320+1,220+1,s,0,-32,0);
	CenterPrintGlow(320,220,s,0,0);

	CenterPrintGlow(320,254,tips[help].name,0,0);

	Print(82,284,tips[help].text[0],0,1);
	Print(82,284+12,tips[help].text[1],0,1);
	Print(82,284+24,tips[help].text[2],0,1);
	Print(82,284+36,tips[help].text[3],0,1);
}

void InitHelp(byte sk)
{
	help=sk;

	btnwait=30;
	MakeNormalSound(SND_NEWSKILL);
}

byte UpdateHelp(MGLDraw *mgl)
{
	byte c;

	if(help==0)
		return 0;	// not in the help now

	if(btnwait)
	{
		btnwait--;
		c=GetTaps();
		mgl->LastKeyPressed();
		return 1;
	}

	c=GetTaps();

	if((c&(CONTROL_B1|CONTROL_B2|CONTROL_B3)))	// pressed a button
	{
		help=0;
		return 0;
	}

	if(mgl->LastKeyPressed()==27)	// ESC
	{
		help=0;
		return 0;
	}

	return 1;
}

void TipEvent(byte eventType)
{
	if(player.var[VAR_TIP+eventType-1]==0)
	{
		player.var[VAR_TIP+eventType-1]=1;
		InitHelp(eventType);
	}
}

char *PauseTipName(byte tip)
{
	return tips[tip].name;
}

void PauseShowTip(byte help,int x,int y)
{
	PrintGlow(x,y-2,tips[help].name,0,0);

	Print(x,y+24,tips[help].text[0],0,1);
	Print(x,y+24+12,tips[help].text[1],0,1);
	Print(x,y+24+24,tips[help].text[2],0,1);
	Print(x,y+24+36,tips[help].text[3],0,1);
}
