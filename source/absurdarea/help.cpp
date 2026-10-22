#include "help.h"
#include "options.h"

typedef struct help_t
{
	char name[32];
	char text[4][64];
} help_t;

help_t tips[]={
	// freebie!
	{"Frogdude is Awesome!",
		{"He spent a lot of time making this work, and you should be",
		 "very grateful.  What are you waiting for? Be grateful already!",
		 "Also, this game has secret walls that you can break through.",
		 "If you see something suspicious, slash it or shoot it."},
	},
	// 1 TIP_BEGIN
	{"Playing The Game",
		{"Use the arrow keys or gamepad to move.  Z, CTRL or gamepad",
		"button 1 is Fire.  Use that to exit this tip and conversations.",
		"X or SHIFT or button 2 is Jump.  ESC is Pause. Holding A, Tab,",
		"or button 3 brings up the Spell Menu.  Play to find out more!"},
	},
	// 2 TIP_QUEST
	{"Quests",
		{"You just got your first Quest!  Go do what you've been told,",
		"then come back and collect the reward!  If you've played the",
		"original, you will notice that some quests are different.",
		"Check Quests from the ESC Pause menu if you need a reminder."},
	},
	// 3 TIP_GO (went to a new map)
	{"Upgrades",
		{"In this expansion, you don't find Skills and Talents just lying",
		 "around.  You must earn Objective Points, and then return to the",
		 "Halls of Mastery at any glowing pole.  To see the entire",
		 "upgrade tree, press \"Spell\" on the Pause screen."},
	},
	// 4 TIP_SKILL
	{"Skills",
		{"You just got your first Skill!  When you gain levels, you will",
		 "earn Skill Points that you can spend in the Pause Menu to",
		 "upgrade your Skills.  Skills have no effect until they have",
		 "at least one point invested.  Most skills were unchanged."},
	},
	// 5 TIP_DIE
	{"You Have Died",
		{"You are DEAD!  But don't worry, you will just be sent back to",
		 "the last town with less Talent XP.  Sometimes it's best to",
		 "play it safe - and there are several minibosses that may be",
		 "too hard when you first meet them.  Don't worry, just run away!"},
	},
	// 6 TIP_THROWING
	{"Throwing Axes",
		{"To switch between swinging your axe and throwing it, hold down",
		 "A to open the Spell Menu.  Use Left & Right to choose the Axe",
		 "icon, and release A to switch.  You can also just tap A to",
		 "repeat the last thing you did in the Spell Menu."},
	},
	// 7 TIP_STAMINA
	{"Running On Empty",
		{"You're running low on Stamina! This happens all the time",
		 "even in normal Loonyland 2,  but in Absurd Area, skills",
		 "are not in the usual place.  If you need the Endurance",
		 "skill, you must purchase the \"Gladiator 3\" upgrade."},
	},
	// 8 TIP_EQUIP
	{"Special Equipment",
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
	{"Getting Started",
		{"Welcome to the town of Einsburg!  Be sure to talk to everyone",
		 "and explore everywhere.  You can complete several quests and",
		 "get some money, levels, and an Upgrade Point without even",
		 "leaving town.  So make sure you check out all the cabins!"},
	},
	// 12 TIP_MAGIC
	{"Magical Loot",
		{"Your first Magic item!  These have bonus powers, and also,",
		 "improve one of your Skills, if you have the Skill.  There are",
		 "also several unique items lying around.  Some will be quest",
		 "rewards, but others will be hidden - poke your nose everywhere!"},
	},
	// 13 TIP_SHOVEL
	{"Shoveling",
		{"You got a Shovel!  To use it, just jump to dig where you land.",
		 "Cracks and lumps of snow mark digging spots, but there is also",
		 "one place you need a Shovel to move debris.  If you find it,",
		 "all you need to do is just walk up to the blockage."},
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
		{"Absurd Area starts you off with a free levelup.  Every time you",
		 "level up, you get 2 Life, 1 Stamina, and 1 Magic.  You earn one",
		 "Skill Point per level, and even more at higher levels.  To buy",
		 "Skills, go to the Halls of Mastery at any glowing pole."},
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
		{"Bump into a Clock to build a friendly Clockbot from Junk items,",
		 "or repair it.  Each Junk Item will give it a different power.",
		 "Also, when you meet the Clockmaker, he can upgrade your bot",
		 "if it was made with one of every junk item.  Experiment!"},
	},
	// 18 TIP_JUNKSMITH
	{"Junksmithing",
		{"Bump into an Oven to Junksmith.  This works the same as in the", // CHANGE THIS if you change anything
		"original, but you get double XP.  Put junk in the Oven and it",
		 "will produce a piece of equipment.  The same recipe always",
		 "makes the same item, so look for something that sells well!"
		 },
	},
	// 19 TIP_SYNERGY
	{"Spell Synergy",
		{"You now have two spells of the same Spell School.  Every time",
		 "you add points to a Spell, all Spells of the same School gain",
		 "1% more damage or healing.  Consult the Upgrade Tree to plan",
		 "your build if you want more power!"
		 },
	},
	// 20 TIP_ENDING
	{"The End Is Near!",
		{"Watch out!  If you still have things you want to do, go back",
		 "down and do them.  There are 30 Upgrades to purchase, do you",
		 "have all you want?  Found enough secret areas?",
		 "If you win, you will start over in Madcap Mode.",
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
