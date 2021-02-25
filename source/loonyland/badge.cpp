#include "badge.h"
#include "options.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "items.h"
#include "quest.h"
#include "fireworks.h"
#include "title.h"
#include "palettes.h"

static byte cursor;
static byte oldc;
static byte viewing;
static byte cantearn;
static char badgeKeys[17];

badge_t badge[NUM_BADGES]={
	{"Evil Smashin'",
	 "Earned for defeating The Evilizer in three",
	 "minutes.",
	 CH_QUICK,
	 "Quick Mode",
	 "With this cheat on, you do everything at double",
	 "speed, much to the chagrin of the monsters.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Bowlin'",
	 "Earned for getting best rank in Bowling mode.",
	 "",
	 CH_DISCO,
	 "Disco Mode",
	 "'Enjoy' fabulous disco effects on the graphics.",
	 "Not recommended for people susceptible to",
	 "seizures!",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Vandalizin'",
	 "Earned for smashing all the graves in Zombiton.",
	 "",
	 CH_VINTAGE,
	 "Vintage Mode",
	 "Play the game as though it were a classic silent",
	 "horror movie.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Greed",
	 "Earned for collecting 5000 gems.",
	 "",
	 CH_GEMS,
	 "Infinite Gems",
	 "With this cheat, you can never run out of gems,",
	 "and will always be at maximum.  Very handy for",
	 "firing special weapons with reckless abandon.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Monster Slayin'",
	 "Earned for obtaining 30,000 Monster Points.",
	 "",
	 CH_HARDCORE,
	 "Hardcore Mode",
	 "Make the game much more challenging - you can",
	 "no longer save your progress, but when you",
	 "choose to quit, it saves automatically. If you",
	 "die, your saved game is DELETED. No kidding.",
	 "",
	 "",
	 "",
	 "",
	 RULE_NEWGAME},
	{"Monster Poundin'",
	 "Earned for obtaining 10,000 Monster Points.",
	 "",
	 CH_SIDEKICK,
	 "Sidekick",
	 "With this cheat, Farley will always remain by",
	 "your side, from the beginning of the game.",
	 "If you are injured, it weakens him, but never",
	 "removes him from play.",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Ghostbustin'",
	 "Earned for beating Polterguy in thirty seconds.",
	 "",
	 CH_HOMING,
	 "Homing Shots",
	 "With this cheat, your slingshot bullets will",
	 "home in on nearby enemies.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Lookin'",
	 "Earned for getting the Arrow in Zizwalda's house",
	 "without the help of a torch.",
	 CH_LIGHT,
	 "20/20 Vision",
	 "With this cheat on, you will see through all",
	 "darkness just as clear as day. You will even see",
	 "many areas that aren't a part of gameplay.",
	 "Ignore them!",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Ultimate Victory",
	 "Earned for best rank in Adventure Mode.",
	 "",
	 CH_MAXPOWER,
	 "Ultimate Firepower",
	 "Begin the game with absolutely maximum",
	 "firepower!",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 RULE_NEWGAME},
	{"Kickin'",
	 "Earned for best rank in Loonyball Mode.",
	 "",
	 CH_SURPRISE,
	 "Surprise Mode",
	 "Makes the game much more fun - enemies are",
	 "all replaced at random, with new ones of",
	 "approximately the same difficulty. Some big",
	 "monsters will sometimes be replaced with",
	 "hordes of smaller ones! Important monsters",
	 "like the bosses are unaffected.",
	 "",
	 "",
	 0},
	{"Boss Bashin'",
	 "Earned for getting the best rank in Boss Bash",
	 "mode.",
	 CH_CORPSE,
	 "Combo-Bombo",
	 "When a monster is beaten, it will make a huge",
	 "explosion.  Time it right, and you can make a",
	 "chain reaction that wipes them all out!",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Vampire Slayin'",
	 "Earned for beating Bonkula in one minute.",
	 "",
	 CH_BONKULA,
	 "Play As Bonkula",
	 "Loses life over time, must beat enemies to",
	 "recover. Fights with his big hammer.",
	 "Lightning boosts damage.",
	 "Pants speed up swing speed.",
	 "Arrows increase radius of smash attack.",
	 "Floats around at extreme speeds!",
	 "Uses special weapons the same way as Loony.",
	 "",
	 RULE_NEWGAME|RULE_PLAYAS},
	{"Sneakin'",
	 "Earned for going through Haunted Tower Floor 1",
	 "without defeating any Ghosts.",
	 CH_SWAMPDOG,
	 "Play As Swampdog",
	 "Unlike anyone else, the Swampdog earns",
	 "experience by beating monsters.  As he gains",
	 "levels, his powers will increase. The power-ups",
	 "merely heal him and fill up his gem supply.",
	 "Look on the Pause screen to see what level he",
	 "is at, and how many Monster Points you need to",
	 "advance.",
	 "",
	 RULE_NEWGAME|RULE_PLAYAS},
	{"Witch Hunt",
	 "Earned for beating Happy Stick Witch in one",
	 "minute.",
	 CH_WITCH,
	 "Play As Witch",
	 "The Happy Stick Witch learns spells rather than",
	 "using weapons. Press the Special Weapon button",
	 "to cycle through your spells, and the Fire button",
	 "to cast them. Use spells successfully or grab",
	 "weapon powerups to increase the spell level.",
	 "Lightning increases the rate of magic recovery.",
	 "Arrows increase your maximum magic.",
	 "Pants also increase the rate of magic recovery.",
	 RULE_NEWGAME|RULE_PLAYAS},
	{"Animal Control",
	 "Earned for defeating every werewolf in the Wolf",
	 "Den.",
	 CH_WEREWOLF,
	 "Play As Werewolf",
	 "Oh no, Loony got bit! Play as normal, except",
	 "without the use of Special Weapons. Instead,",
	 "pressing the Special Weapon button uses up 30",
	 "Gems to turn you into a wolf for 10 seconds.",
	 "The wolf has a pounce attack and a shooting",
	 "attack. Hold the weapon button down and push",
	 "left or right to spin like a turret.",
	 "",
	 RULE_NEWGAME|RULE_PLAYAS},
	{"Annoyance",
	 "Earned for annoying Zizwalda The Witch.",
	 "",
	 CH_TOAD,
	 "Play As Toad",
	 "The Toad moves faster than Loony, but is limited",
	 "to a powerful tongue attack and special weapons.",
	 "Lightning boosts damage.",
	 "Pants increase the speed of the tongue.",
	 "Arrows increase the knockback effect of your",
	 "attack.",
	 "",
	 "",
	 RULE_NEWGAME|RULE_PLAYAS},
	{"Civic Duty",
	 "Earned for clearing out the Gloomy Cave before",
	 "talking to the scared girl.",
	 CH_RESPAWN,
	 "Enemy Overload",
	 "As you play, more monsters continually arrive in",
	 "the game, adding to the challenge.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 RULE_NEWGAME},
	{"Winnin'",
	 "Earned for winning Adventure Mode.",
	 "",
	 CH_CRYSTAL,
	 "Crystal Ball",
	 "Use this cheat to find missing power-ups!  The",
	 "name of the area you are in will be red if there",
	 "are no more power-ups. It will glow brighter and",
	 "brighter blue the closer you get to a power-up.",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Combo Plate",
	 "Earned for getting a 50 Combo.",
	 "",
	 CH_RADAR,
	 "Radar",
	 "Spot enemies everywhere in the world. This",
	 "shows a radar display in the lower right of the",
	 "screen with friendly creatures in white and",
	 "enemies in red.",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Cleanin' Up",
	 "Earned for smashing all the rocks that can be",
	 "smashed in The Rocky Cliffs.",
	 CH_HEAVYHIT,
	 "Heavy Hitter",
	 "This makes Arrows and Pants boost your damage, just",
	 "like Lightning does, rather than increasing your",
	 "range and rate of fire. You remain at the slowest",
	 "rate of fire, but have a medium range.",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Findin'",
	 "Earned for finding it hidden in Adventure Mode.",
	 "",
	 CH_RAPIDFIRE,
	 "Rapid Fire",
	 "Begin the game with all 10 Pants, so you have",
	 "the highest rate of fire possible from the",
	 "beginning",
	 "",
	 "",
	 "",
	 "",
	 "",
	 RULE_NEWGAME},
	{"Extreme Survivin'",
	 "Earned for finishing Survival in Terror Mode.",
	 "",
	 CH_DEATH,
	 "Touch Of Death",
	 "Kill anything with one hit.  ANYTHING.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Huntin'",
	 "Earned for beating every enemy in the overworld",
	 "of Adventure Mode.",
	 CH_ALLACCESS,
	 "All Access Pass",
	 "Begin the game with all the 'access' items, like",
	 "the special keys, fertilizer, boots, mystery orbs,",
	 "and so on. All the passages that can be opened,",
	 "like the Rocky Cliffs bridge, are already opened.",
	 "",
	 "",
	 "",
	 "",
	 RULE_NEWGAME},
	{"Advanced Winnin'",
	 "Earned for getting 100% complete in Adventure",
	 "Mode.",
	 CH_TERROR,
	 "Terror Mode",
	 "Enter a new world of supreme agony.  In Terror",
	 "Mode you begin with maximum firepower, and can",
	 "earn double your starting life.  So why is it",
	 "hard?  Try it.",
	 "",
	 "",
	 "",
	 "",
	 RULE_NEWGAME},
	{"Survivin'",
	 "Earned for best rank in Survival Mode.",
	 "",
	 CH_SAVEANY,
	 "Save Anywhere",
	 "Adds a new option to the Pause Menu to save",
	 "your game any time you like.  No need to find",
	 "Save Crystals!",
	 "",
	 "(Does not work during final boss battle! That",
	 "would be too easy!)",
	 "",
	 "",
	 0},
	{"Puppy Poundin'",
	 "Earned for beating Larry in one minute.",
	 "",
	 CH_GUIDED,
	 "Guided Shots",
	 "Useless but fun, your shots have extremely long",
	 "range, and will follow the movements of your",
	 "controller. Try sneaking around walls to hit",
	 "things!",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Mad Skeelz",
	 "Earned for finishing (did you cheat!?) Adventure",
	 "Mode with Terror Mode activated.",
	 CH_KICKCAT,
	 "Kick The Cat!",
	 "In Loonyball Mode and Bowling Mode, replace the",
	 "ball with an innocent kitten.  Enjoy!!",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Frankenfryin'",
	 "Earned for beating Frankenjulie in thirty seconds.",
	 "",
	 CH_SLOMO,
	 "Slo-Mo",
	 "All monsters and enemy bullets move in slow",
	 "motion. Take advantage of them! So to speak.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Combo Magic",
	 "Earned for getting a 10 Combo in Survival Mode.",
	 "",
	 CH_RNDSURVIVAL,
	 "Infinite Survival",
	 "Switch this on to experience an entirely new",
	 "form of Survival Mode. Instead of 25 pre-set",
	 "levels, it goes on forever with random",
	 "enemies, getting harder and harder. Survive",
	 "as long as you can! This is a very tough",
	 "mode! High scores for Infinite Survival are",
	 "kept separately from normal Survival scores.",
	 "",
	 0},
	{"Hedge Clippin'",
	 "Earned for destroying all cat-tails in Slurpy",
	 "Swamp.",
	 CH_FROGWPN,
	 "Frog-o-rama",
	 "Lots of frogs in lots of places. You will enjoy.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"R-R-R-Remix!",
	 "Earned for completing Remix mode 100%.",
	 "",
	 CH_SUMMON,
	 "Play As Summony",
	 "Summony's abilities never upgrade, so he must",
	 "rely on summoning monsters that do upgrade.",
	 "Each Special Weapon gives a different summon.",
	 "Summoned monsters lose life over time.",
	 "Hearts make them live longer.",
	 "Lightning makes them do more damage.",
	 "Pants make them move and attack faster.",
	 "Arrows let you summon more monsters.",
	 RULE_NEWGAME|RULE_PLAYAS},
	{"Witchcraft",
	 "Earned for getting Happy Stick Witch's spells",
	 "all to level 9.",
	 CH_THIEF,
	 "Play As Ninja Girl",
	 "Ninja Girl fires throwing knives rapidly.",
	 "She has a limited supply and must reload when",
	 "she runs out, or when she gets hit.  While the",
	 "fire button is held down, she stays facing the",
	 "same direction, allowing you to strafe enemies.",
	 "Each Special Weapon grants her a power-up she",
	 "can add to her knives for a limited time for 20",
	 "gems each. She can combine power-ups too!",
	 RULE_NEWGAME|RULE_PLAYAS},
	{"Ninjitsu",
	 "Earned for having all 7 Ninja Girl Power-Ups",
	 "active at once.",
	 CH_WALLWALK,
	 "Molecular Dispersion",
	 "No longer will solid objects keep you down!",
	 "Walk through walls and obstacles!",
	 "Beware: this can 'break' lots of puzzles and",
	 "things in the game, and let you go places that",
	 "you're not supposed to be!  Take a peek",
	 "behind the scenes!",
	 "",
	 "",
	 0},
	{"Stayin' Alive",
	 "Earned for getting to Wave 100 of Infinite",
	 "Survival mode.",
	 CH_SUPERSURV,
	 "Guaranteed Survival",
	 "With this turned on, you are invincible.",
	 "Surviving should prove quite easy now!",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Hooliganin'",
	 "Earned by causing a riot in Loonyball.",
	 "",
	 CH_BEND,
	 "Bend It Like Bouapha",
	 "Gives you the ability to steer the ball in",
	 "Loonyball for a short time after you kick it.",
	 "It also improves your mental control over the",
	 "ball in Bowling mode.",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Swampdoggin'",
	 "Earned by leveling up the Swampdog to level",
	 "50.",
	 CH_STINKY,
	 "Stinky Farley",
	 "Replaces Farley's normal attack with poison",
	 "gas clouds, making him generally more powerful",
	 "than normal.  You'll still have to earn him",
	 "or use a cheat to obtain him, though.",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Scorin'",
	 "Earned for scoring over 10 million points in",
	 "any game mode.",
	 CH_ULTRAWEAPON,
	 "Ultra Weapons Are Go!",
	 "Replaces the Level III version of each",
	 "Special Weapon with a super duper version,",
	 "that costs more coins to fire.  These are",
	 "really overpowered!",
	 "Only useful to characters that use Special",
	 "Weapons.",
	 "",
	 "",
	 0},
	{"Brawlin'",
	 "Earned for beating everyone in the bar using",
	 "the Touch Of Death cheat.",
	 CH_THORNS,
	 "Thorns Aura",
	 "Anytime you are hit by any attack (or just",
	 "stub your toe on water!), you spew out cactus",
	 "needles in every direction.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Minin' For Treasure",
	 "Earned for finding the hidden badge in Remix",
	 "mode.",
	 CH_REGEN,
	 "Monster Regeneration",
	 "All monsters regenerate their life rapidly,",
	 "making it very difficult to beat them!",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
	{"Digitally Masterin'",
	 "Earned for earning the other 39 badges!  Way to",
	 "go!",
	 CH_NOFARLEY,
	 "Farley Free Zone",
	 "The greatest badge of all time!  Turning this on",
	 "simply disables Farley completely.",
	 "",
	 "",
	 "",
	 "",
	 "",
	 "",
	 0},
};

int waitCtr;
byte progress=0;

void InitBadgeMenu(void)
{
	oldc=255;
	cursor=0;
}

void ExitBadgeMenu(void)
{
	SaveOptions();
}


void InitBadgeCheat(void)
{
	int i;

	for(i=0;i<16;i++)
		badgeKeys[i]='\0';

	badgeKeys[16]='\0';	// a padding 0, so strcmp can be used
}

void BadgeCheatKey(char c)
{
	int i,b;

	if(c>=1 && c<=26)
		c=c+'a'-1;	// remove CTRL

	// scoot the existing letters over 1
	for(i=0;i<15;i++)
		badgeKeys[i]=badgeKeys[i+1];
	// and stick the new one on the end
	badgeKeys[15]=tolower(c);

	if(!strcmp("gimme",&badgeKeys[16-strlen("gimme")]))
	{
		opt.meritBadge[cursor]=1-opt.meritBadge[cursor];
		if(opt.cheats[badge[cursor].cheatNum])
			opt.cheats[badge[cursor].cheatNum]=0;

		if(opt.meritBadge[cursor])
		{
			b=0;
			for(i=0;i<NUM_BADGES;i++)
				if(opt.meritBadge[i])
					b++;

			if(b>=5)
				opt.modes[0]=1;
			if(b>=10)
				opt.modes[1]=1;
			if(b>=15)
				opt.modes[2]=1;
			if(b>=20)
				opt.modes[3]=1;
			if(b>0)
				opt.modes[4]=1;
		}
		else
		{
			b=0;
			for(i=0;i<NUM_BADGES;i++)
				if(opt.meritBadge[i])
					b++;

			if(b<5)
				opt.modes[0]=0;
			if(b<10)
				opt.modes[1]=0;
			if(b<15)
				opt.modes[2]=0;
			if(b<20)
				opt.modes[3]=0;
			if(b==0)
				opt.modes[4]=0;
		}

		MakeSuperLoony();
		SaveOptions();
		MakeNormalSound(SND_ENTERMAP);
	}
	if(!strcmp("clear",&badgeKeys[16-strlen("clear")]))
	{
		for(i=0;i<NUM_BADGES;i++)
		{
			opt.meritBadge[i]=0;
			opt.cheats[i]=0;
		}
		for(i=0;i<10;i++)
			opt.bossDead[i]=0;
		for(i=0;i<5;i++)
			opt.modes[i]=0;

		MakeSuperLoony();
		SaveOptions();
		MakeNormalSound(SND_POWERUP);
	}
	if(!strcmp("giveall",&badgeKeys[16-strlen("giveall")]))
	{
		for(i=0;i<NUM_BADGES;i++)
		{
			opt.meritBadge[i]=1;
		}
		for(i=0;i<5;i++)
			opt.modes[i]=1;

		MakeSuperLoony();
		SaveOptions();
		MakeNormalSound(SND_POWERUP);
	}
	if(!strcmp("bossbash",&badgeKeys[16-strlen("bossbash")]))
	{
		for(i=0;i<10;i++)
			opt.bossDead[i]=1;
		SaveOptions();
		MakeNormalSound(SND_POWERUP);
	}
}

byte UpdateBadgeMenu(MGLDraw *mgl)
{
	char c;
	byte c2;

	c=mgl->LastKeyPressed();
	c2=GetControls()|GetArrows();

	if(c==27)
	{
		return 1;
	}

	if((c2&CONTROL_B1) && (c2&CONTROL_B2) && c!=0)
		BadgeCheatKey(c);

	if((c2&CONTROL_UP) && (!(oldc&CONTROL_UP)))
	{
		if(cursor%10==0)
		{
			cursor+=9;
		}
		else
			cursor--;
		MakeNormalSound(SND_MENUCLICK);
	}
	if((c2&CONTROL_DN) && (!(oldc&CONTROL_DN)))
	{
		if(cursor%10==9)
		{
			cursor-=9;
		}
		else
			cursor++;
		MakeNormalSound(SND_MENUCLICK);
	}
	if((c2&CONTROL_LF) && (!(oldc&CONTROL_LF)))
	{
		if(cursor/10==0)
		{
			cursor+=30;
		}
		else
			cursor-=10;
		MakeNormalSound(SND_MENUCLICK);
	}
	if((c2&CONTROL_RT) && (!(oldc&CONTROL_RT)))
	{
		if(cursor/10==3)
		{
			cursor-=30;
		}
		else
			cursor+=10;
		MakeNormalSound(SND_MENUCLICK);
	}
	if((c2&(CONTROL_B1|CONTROL_B2|CONTROL_B3)) && (!(oldc&(CONTROL_B1|CONTROL_B2|CONTROL_B3))))
	{
		if(opt.meritBadge[cursor])
		{
			// toggle the cheat
			opt.cheats[badge[cursor].cheatNum]=1-opt.cheats[badge[cursor].cheatNum];
			// if it's a play as cheat, shut off all other playases
			if(opt.cheats[badge[cursor].cheatNum]==1)
			{
				switch(badge[cursor].cheatNum)
				{
					case CH_BONKULA:
					case CH_TOAD:
					case CH_WITCH:
					case CH_SWAMPDOG:
					case CH_WEREWOLF:
					case CH_SUMMON:
					case CH_THIEF:
						opt.cheats[CH_BONKULA]=0;
						opt.cheats[CH_TOAD]=0;
						opt.cheats[CH_WITCH]=0;
						opt.cheats[CH_SWAMPDOG]=0;
						opt.cheats[CH_WEREWOLF]=0;
						opt.cheats[CH_SUMMON]=0;
						opt.cheats[CH_THIEF]=0;
						opt.cheats[badge[cursor].cheatNum]=1;
						break;
				}
			}
			// if it's vintage mode grey or colorize the screen
			if(badge[cursor].cheatNum==CH_VINTAGE)
			{
				if(opt.cheats[badge[cursor].cheatNum]==1)
				{
					GreyPalette(GetDisplayMGL());
					JamulSoundPurge();
				}
				else
				{
					GetDisplayMGL()->LoadBMP("graphics/title.bmp");
					GetDisplayMGL()->RealizePalette();
					LoopingSound(SND_HAMUMU);
				}
			}
			MakeNormalSound(SND_MENUSELECT);
		}
		else
			MakeNormalSound(SND_MENUCANCEL);
	}

	oldc=c2;

	return 0;
}

void RenderBadgeMenu(MGLDraw *mgl)
{
	int i,x,y;
	char b;

	mgl->ClearScreen();

	CenterPrint(450,2,"Merit Badges",0,2);
	DrawBox(270,55,639,56,31);
	DrawBox(270,0,271,479,31);
	x=2;
	y=7;
	for(i=0;i<NUM_BADGES;i++)
	{
		b=0;
		if(cursor==i)
			b=16;
		if(opt.meritBadge[i])
		{
			RenderIntfaceSprite(x,y,26+i,b,mgl);
			if(opt.cheats[badge[i].cheatNum])
				RenderIntfaceSprite(x,y,25,b,mgl);
		}
		else
			RenderIntfaceSprite(x,y,24,b,mgl);
		y+=46;
		if(y>480-46)
		{
			x+=66;
			y=7;
		}
	}

	// display info on current badge
	if(opt.meritBadge[cursor])
	{
		RenderIntfaceSprite(274,60,26+cursor,0,mgl);
		Print(350,70,badge[cursor].name,0,0);
		if(opt.cheats[badge[cursor].cheatNum])
			RenderIntfaceSprite(274,60,25,b,mgl);

		DrawBox(270,108,639,108,31);

		for(i=0;i<2;i++)
		{
			Print(274,112+i*16,badge[cursor].howGet[i],0,1);
		}
		DrawBox(270,164,639,164,31);
		Print(274,168,"For getting this badge, you are awarded:",0,1);
		Print(274,190,badge[cursor].cheatName,0,0);
		for(i=0;i<8;i++)
		{
			Print(274,220+i*16,badge[cursor].cheatDesc[i],0,1);
		}

		y=370;
		if(badge[cursor].rules&RULE_NEWGAME)
		{
			Print(274,y,"* Takes effect when you start a new game",0,1);
			y+=20;
		}
		if(badge[cursor].rules&RULE_PLAYAS)
		{
			Print(274,y,"* Only one 'Play As' cheat works at a time",0,1);
			y+=20;
		}
		if(badge[cursor].rules&RULE_ADVENTURE)
		{
			Print(274,y,"* Only affects adventure mode",0,1);
			y+=20;
		}
	}
	else	// badge is unknown
	{
		RenderIntfaceSprite(274,60,24,0,mgl);
		Print(350,70,"Unknown!",0,0);
	}

	if(viewing==0)
	{
		Print(275,440,"Press Fire to toggle cheat On/Off",0,1);
		Print(274,440,"Press Fire to toggle cheat On/Off",0,1);
		Print(275,460,"Press ESC to exit",0,1);
		Print(274,460,"Press ESC to exit",0,1);
	}
	else
	{
		Print(275,460,"Press any button to exit",0,1);
		Print(274,460,"Press any button to exit",0,1);
	}
}

TASK(void) BadgeMenu(MGLDraw *mgl)
{
	byte done=0;

	viewing=0;
	InitBadgeMenu();

	while(!done)
	{
		done=UpdateBadgeMenu(mgl);
		RenderBadgeMenu(mgl);
		AWAIT mgl->Flip();

		if(mgl->LastKeyPeek()==27)
			done=1;

		if(!mgl->Process())
			done=1;

	}

	ExitBadgeMenu();
}

static TASK(void) EarnBadgeTask(byte b)
{
	int i,c;

	c=0;
	for(i=0;i<NUM_BADGES;i++)
		if(opt.meritBadge[i])
			c++;

	if(c>=25 && opt.expando[0]==0)
	{
		opt.expando[0]=1;
		AWAIT ShowGameMode(4,25);
		SaveOptions();
	}

	if(opt.meritBadge[b])
		CO_RETURN;

	opt.modes[4]=1;	// this just indicates that badge listings should be available

	MakeNormalSound(SND_BADGEGET);
	opt.meritBadge[b]=1;
	AWAIT ShowBadge(b);

	if(c>=5 && opt.modes[0]==0)
	{
		opt.modes[0]=1;
		AWAIT ShowGameMode(0,5);
	}
	if(c>=10 && opt.modes[1]==0)
	{
		opt.modes[1]=1;
		AWAIT ShowGameMode(1,10);
	}
	if(c>=15 && opt.modes[2]==0)
	{
		opt.modes[2]=1;
		AWAIT ShowGameMode(2,15);
	}
	if(c>=20 && opt.modes[3]==0)
	{
		opt.modes[3]=1;
		AWAIT ShowGameMode(3,20);
	}

	SaveOptions();
	if(c==40)
	{
		LoopingSound(SND_ENDSONG);
		AWAIT CheatText(GetDisplayMGL(),1);
		KillSong();
	}
}

void EarnBadge(byte b)
{
	coro::launch(std::bind(EarnBadgeTask, b));
}

void RenderGameMode(MGLDraw *mgl,byte progress,byte mode,byte numBadges)
{
	char s[32];
	char modeName[5][32]={"Survival","Boss Bash","Loonyball","Bowling","Remix"};

	mgl->ClearScreen();
	RenderFireworks(mgl);

	if(progress>10)
		CenterPrint(320,2,"CONGRATULATIONS!",0,2);

	sprintf(s,"For earning %d Merit Badges,",numBadges);
	if(progress>60)
		CenterPrint(320,150,s,0,2);

	sprintf(s,"you've unlocked %s Mode!",modeName[mode]);
	if(progress>90)
		CenterPrint(320,200,s,0,2);
	if(progress>120)
		CenterPrint(320,420,"Select it from the main menu!",0,2);
}

byte UpdateGameMode(MGLDraw *mgl,int *lastTime)
{
	static byte fwClock=0;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		UpdateFireworks();
		waitCtr++;
		if(progress<150)
			progress++;

		if(fwClock>0)
		{
			fwClock--;
			if(fwClock==0)
			{
				if(Random(100)<30)
					LaunchShell(20);
			}
		}
		else
			fwClock=10;
		if(waitCtr>30*5)
		{
			if(mgl->LastKeyPressed()==27)
				return 1;
			if(GetTaps()&(CONTROL_B1|CONTROL_B2))
				return 1;
		}
		*lastTime-=TIME_PER_FRAME;
	}
	return 0;
}

TASK(void) ShowGameMode(byte mode,byte numBadges)
{
	/*byte done=0;
	MGLDraw *mgl=GetDisplayMGL();
	int lastTime;
	dword hangon;

	EndClock();
	hangon=timeGetTime();

	InitFireworks();

	GetTaps();
	MakeNormalSound(SND_MODEGET);
	lastTime=1;
	progress=0;
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateGameMode(mgl,&lastTime);
		RenderGameMode(mgl,progress,mode,numBadges);
		AWAIT mgl->Flip();
		if(!mgl->Process())
			done=1;

		EndClock();

	}
	ExitFireworks();
	ResetClock(hangon);*/
}

TASK(void) ShowBadge(byte b)
{
	byte done=0;
	dword wait;
	MGLDraw *mgl=GetDisplayMGL();

	viewing=1;
	wait=timeGetTime();
	InitBadgeMenu();

	GetTaps();
	cursor=b;

	while(!done)
	{
		RenderBadgeMenu(mgl);
		AWAIT mgl->Flip();

		// can't do anything for 3 seconds
		if(timeGetTime()-wait>1000*3)
		{
			if(mgl->LastKeyPressed()==27)
				done=1;
			if(GetTaps()&(CONTROL_B1|CONTROL_B2))
				done=1;
		}
		if(!mgl->Process())
			done=1;
	}

	ExitBadgeMenu();
}

void BadgeCheck(byte event,int value,Map *map)
{
	dword now,then;
	int i,have;

	now=timeGetTime();
	if(event==BE_KILL)
	{
		// killed a monster
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && value==MONS_FRANKENJ && player.timeLimit<30*30)
		{
			// killed frank in 2 minutes
			EarnBadge(BADGE_FRANKENJ);
		}
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && value==MONS_BIGGHOST && player.timeLimit<30*30)
		{
			// killed polterguy in 2 minutes
			EarnBadge(BADGE_POLTERGUY);
		}
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && (value==MONS_HUMANLARRY || value==MONS_LARRY) && player.timeLimit<30*60)
		{
			// killed larry in 1 minute
			EarnBadge(BADGE_LARRY);
		}
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && value==MONS_BONKULA && player.timeLimit<2*60*30)
		{
			// killed bonkula in 2 minutes
			EarnBadge(BADGE_BONKULA);
		}
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && value==MONS_EVILIZER && player.timeLimit<3*60*30)
		{
			// killed evilizer in 3 minutes
			EarnBadge(BADGE_EVILIZER);
		}
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && value==MONS_STICKWITCH && player.timeLimit<30*60)
		{
			// killed stick witch in 1 minute
			EarnBadge(BADGE_WITCH);
		}
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && player.levelNum==0 && !AnyMonsterExists())
		{
			// killed everyone in overworld
			EarnBadge(BADGE_KILLALL);
		}
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && player.var[VAR_QUESTDONE+QUEST_ZOMBIES] && player.levelNum==9 && !MonsterExists(MONS_VILLAGER) &&
			!MonsterExists(MONS_VILLAGER2) && !MonsterExists(MONS_VILLAGER3) && !MonsterExists(MONS_VILLAGER4) && !MonsterExists(MONS_VILLAGER5) &&
			!MonsterExists(MONS_VILLAGER6))
		{
			// killed everyone in the bar
			EarnBadge(BADGE_BRAWL);
		}

		if(player.monsterPoints>30000)
			EarnBadge(BADGE_MONSTERPTS);
		if(player.monsterPoints>10000)
			EarnBadge(BADGE_MONSTERPTS2);

		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && player.levelNum==24 && !AnyMonsterExists() &&
			player.var[250]==0)
		{
			// killed everyone in rescue cave before talking to chick
			EarnBadge(BADGE_RESCUE);
		}
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && player.levelNum==26 && !AnyMonsterExists() &&
			player.var[250]==0)
		{
			// killed all the wolves in the den
			EarnBadge(BADGE_WOLFDEN);
		}
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && player.levelNum==17 && value==MONS_GHOST)
			cantearn=1;	// no longer possible to get sneakin' badge
	}
	else if(event==BE_GETITEM)
	{
		// got an item
		if(value==ITM_RANGEUP && player.worldNum==WORLD_NORMAL && player.levelNum==2 &&
			player.var[VAR_TORCH]==0)
		{
			// got the arrow in the witch's cabin without a torch
			EarnBadge(BADGE_DARKNESS);
		}
	}
	else if(event==BE_GEMSUP)
	{
		if(player.gemsGotten>=5000)
		{
			EarnBadge(BADGE_GEMS);
		}
	}
	else if(event==BE_DESTROY)
	{
		// smashed an item
		if((value==ITM_GRAVE || value==ITM_GRAVE2) && (player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && player.levelNum==0)
		{
			// smashed a grave in zombiton... is it the last?
			for(i=0;i<map->width*map->height;i++)
				if(map->map[i].item==ITM_GRAVE || map->map[i].item==ITM_GRAVE2)
					return;	// no badge for you!
			EarnBadge(BADGE_GRAVES);
		}
		if(value==ITM_CATTAIL && (player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && player.levelNum==0)
		{
			// smashed a cattail
			for(i=0;i<map->width*map->height;i++)
				if(map->map[i].item==ITM_CATTAIL)
					return;	// no badge for you!
			EarnBadge(BADGE_CATTAILS);
		}
		if(value==ITM_BOOMROCK && (player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && player.levelNum==0)
		{
			// blew up a rock in the overworld
			for(i=0;i<map->width*map->height;i++)
				if(map->map[i].item==ITM_BOOMROCK)
					return;	// no badge for you!
			EarnBadge(BADGE_ROCKS);
		}
	}
	else if(event==BE_VARSET && (player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX))
	{
		if(CalcPercent(&player)>=100.0)
		{
			if(player.worldNum==WORLD_NORMAL)
				EarnBadge(BADGE_HUNDREDPCT);
			else
				EarnBadge(BADGE_REMIX);
		}
	}
	else if(event==BE_ENDGAME)
	{
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX))
		{
			if(player.var[VAR_QUESTDONE+QUEST_HILL])	// if this is a win
				EarnBadge(BADGE_WINGAME);
			if(player.rank==20)
				EarnBadge(BADGE_BESTRANK);
		}
		if(player.worldNum==WORLD_BOWLING && player.rank==20)
		{
			EarnBadge(BADGE_BOWLING);
		}
		if(player.worldNum==WORLD_LOONYBALL && player.rank==20)
		{
			EarnBadge(BADGE_LOONYBALL);
		}
		if(player.worldNum==WORLD_SURVIVAL && player.rank==20)
		{
			EarnBadge(BADGE_SURVIVAL);
		}
		if(player.worldNum==WORLD_SURVIVAL && (player.cheatsOn&PC_TERROR) && player.numSaves==25)
		{
			EarnBadge(BADGE_TERRORSRV);
		}
		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX) && (player.cheatsOn&PC_TERROR))
		{
			if(player.var[VAR_QUESTDONE+QUEST_HILL])	// if this is a win
				EarnBadge(BADGE_TERROR);
		}
		if(player.worldNum==WORLD_BOSSBASH && player.rank==20)
		{
			EarnBadge(BADGE_BOSSBASH);
		}
	}
	else if(event==BE_COMBO)
	{
		if(value==50)
			EarnBadge(BADGE_COMBO);
		if(player.worldNum==WORLD_SURVIVAL && value==10)
			EarnBadge(BADGE_SURVCOMBO);
	}
	else if(event==BE_GOTOMAP)
	{
		if(player.levelNum==0 && value==17)
			cantearn=0;	// just entered haunted tower floor 1, clear this
		if(player.levelNum==17 && value==18 && !cantearn)
			EarnBadge(BADGE_SNEAK);

	}

	have=0;
	for(i=0;i<NUM_BADGES;i++)
	{
		if(opt.meritBadge[i])
			have++;
	}
	if(have==39 && !opt.meritBadge[BADGE_MASTER])
		EarnBadge(BADGE_MASTER);

	then=timeGetTime();

	//AddGarbageTime(then-now);
	ResetClock(0);

}
