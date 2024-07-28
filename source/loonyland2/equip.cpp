#include "equip.h"
#include "player.h"
#include "skill.h"
#include "items.h"
#include "alchemy.h"
#include "glasses.h"
#include "quest.h"
#include "achieve.h"
#include "gallery.h"
#include "options.h"
#include "appdata.h"
#include "string_extras.h"

char magicNames[MAX_EQMAGIC][16]={
	"Normal",
	// axe
	"Handy",
	"Whistling",
	"Poisoned",
	"Piercing",
	"Flaming",
	"Frosty",
	"Vampiric",
	"Smashing",
	"Thorny",
	"Beefy",
	// parka
	"Healthy",
	"Energetic",
	"Stone",
	"Thorny",
	"Frosty",
	"Swift",
	"Vibrant",
	"Curing",
	"Warm",
	"Lucky",
	// amulet
	"Powerful",
	"Charged",
	"Mighty",
	"Absorbing",
	"Boney",
	"Guardian",
	"Shocking",
	"Safety",
	"Mirrored",
	"Hotfoot",
	// elite axe
	"Ebola",
	"Rending",
	"Scorching",
	"Freezing",
	"Bloodsucker",
	// elite parka
	"Invigorating",
	"Kevlar",
	"Nuclear",
	"Toasty",
	"Electric",
	// elite amulet
	"Magic Eater",
	"Druidic",
	"Holy",
	"Frigid",
	"Energized",
	// cursed axe
	"Defensive",
	"Imbued",
	"Mystical",
	// cursed parka
	"Violent",
	"Sickening",
	"Itchy",
	// cursed amulet
	"Angry",
	"Impervious",
	"Cursed",
	// artifacts
	"Nebro-Gubular",
	"Regeneratorizer",
	"Coolguy",
	"Linkin",
	"Ghostly",
	"Imajica",
	"Nucular",
	"Trainee",
	"Fat Crow",
	"Question",
	"Purplekoo",
	"Death",
	"Justhef",
	"Parallel",
	"Bokbok",
	"Whirling",
	"Dark",
	"Summoner's",
	"Jord",
	"Parachute",
	"Evil Eye",
	"Persuadertron",
	"Old Lace",
	"Sun Angel",
#ifdef DIRECTORS
	"Director's",
	"Omelet",
	"Doom",
	"Banhammer",
	"Sp",
	"Imaginary",
#endif
};

char magicDesc[MAX_EQMAGIC][48]={
	// axe
	"Nothing.",
	"Attacks cost 1 less Stamina.",
	"Flies faster than normal when thrown.",
	"Poisons enemies for 2 seconds.",
	"Ignores 20% of armor.",
	"Shoots out fireballs.",
	"Slows enemies on impact.",
	"5% chance to gain 1 Life per hit.",
	"Knockback and Stun added to hits.",
	"Shoots off thorns on hit.",
	"+5 maximum Stamina.",
	// parka
	"Recover 1 Life every 20 seconds.",
	"Recover 1 Stamina every 5 seconds.",
	"5% chance to be unharmed when hit.",
	"Shoot off thorns when you are hit.",
	"Attackers are frozen for 1 second.",
	"Increased movement speed.",
	"+5 maximum Life.",
	"Poison is half as effective.",
	"Freezing is half as effective.",
	"Find slightly more items and money.",
	// amulet
	"+5 maximum Magic.",
	"Recover 1 Magic every 20 seconds.",
	"Attack spells do 20% more damage.",
	"Gain 1 Magic when hit.",
	"Boneheads shoot fire and regenerate.",
	"Become invincible briefly when hit.",
	"Zap nearby enemies occasionally.",
	"Survive fatal hits unless you have 1 Life.",
	"Restoration spell reflects bullets.",
	"Berserk spell trails flames.",
	// elite axe
	"Poisons enemies for 5 seconds.",
	"Ignores 40% of armor.",
	"Shoots out triple fireballs.",
	"Slows enemies for 2 seconds on hit.",
	"10% chance to gain 1 Life per hit.",
	// elite parka
	"Recover 1 Life every 15 seconds.",
	"10% chance to be unharmed when hit.",
	"Explodes when hit, hurting all nearby.",
	"Grants immunity to freezing.",
	"Shock enemies that hit you.",
	// elite amulet
	"Gain 2 Magic when hit.",
	"Earth spells cost 2 less Magic to cast.",
	"Healing of any kind is 25% more effective.",
	"Ice spells freeze all enemies for 1 second.",
	"Recover 1 Magic every 15 seconds.",
	// cursed axe
	"-3 Damage, but +3 Armor.",
	"-3 Speed, but +10 Magic.",
	"Slower Stamina recovery, faster Magic recovery.",
	// cursed parka
	"-3 Armor, but +3 Damage.",
	"-10 Life, but your poison is twice as deadly.",
	"-3 Armor, but +10 Magic.",
	// cursed amulet
	"-3 Magic, but +2 Damage.",
	"10% chance of Magic failing, but +3 Armor.",
	"Lose 1 Magic when hit, but +2 Armor.",
	// artifacts
	"Inflicts nebro-gubular damage.",
	"Regenerates life for you and all your pals.",
	"Freezing recharges your magic.",
	"Fill with angst sometimes when hit.",
	"Summons spirits with each swing.",
	"150% damage with spells, 50% damage with axe.",
	"Explodes on impact.  Big time.",
	"Earn 50% more experience.",
	"Does 99 damage per second to flying enemies.",
	"Disorients merchants for 10% better prices.",
	"Fabulous and amazing!  Try it on!",
	"Improves Boneheads, and +1 to all Death Magic.",
	"Tells you monster facts on impact.",
	"Sometimes clones enemies when you hit them.",
	"Summons Nuclear Bokboks with each spell cast.",
	"Increases your reach when Whirling.",
	"Exudes a mantle of darkness.",
	"Summoned creatures do +3 damage.",
	"Spells do 1/2 damage, earn double XP.",
	"Prevents falling and drowning injuries.",
	"Burn enemies with a glance!",
	"Persuasion never fails with this handy axe.",
	"Offers +5 Armor when poisoned.",
	"Flames do double damage to enemies.",
#ifdef DIRECTORS
	"Stamina costs are halved.",
	"Magic costs are halved.",
	"Enemies that dare to hit you will suffer.",
	"5% Chance to ban lesser enemies from the game.",
	"Spams fireballs constantly.",
	"This parka doesn't exist.  Do you exist?  Do I?",
#endif
};

typedef struct itemDesc_t
{
	byte type;
	char desc[2][64];
} itemDesc_t;

itemDesc_t itemDesc[]={
	{IT_LOG,
		{"It's big, it's heavy, it's wood.",
		 "",
		},
	},
	{IT_GEAR,
		{"This metal gear is solid.",
		 "",
		},
	},
	{IT_STUFFING,
		{"Looks like you really beat the stuffing out of",
		 "that bear!",
		},
	},
	{IT_CAN,
		{"A hollow cylinder of metal, useful for storing green",
		 "beans.",
		},
	},
	{IT_SOCK,
		{"Unfortunately, it doesn't match any of the others",
		 "you've found.",
		},
	},
	{IT_ICECUBE,
		{"Good thing it's so cold, or this would have melted",
		 "by now.  It doesn't appear to be ordinary ice.",
		},
	},
	{IT_SEED,
		{"The seed of a giant scary plant.  Planting it would",
		 "be unwise.",
		},
	},
	{IT_STONE,
		{"Whoopee, you got a rock.  It's made of stone.",
		 "It's a little like dirt, only more solid.",
		},
	},
	{IT_ECTO,
		{"This is totally disgusting.  It's a blob of slimy",
		 "goop, indicative of paranormal activity.",
		},
	},
	{IT_CLAW,
		{"There's an old saying: a claw in the hand is way better",
		 "than a claw IN the hand.",
		},
	},
	{IT_SILVER,
		{"A big old block of solid silver!  If only precious",
		 "metals were considered valuable in Winter Woods!",
		},
	},
	{IT_GOLD,
		{"Goldish in color.",
		 "",
		},
	},
	{IT_RUBY,
		{"A fabulous shiny ruby.  Very red.",
		 "",
		},
	},
	{IT_EMERALD,
		{"This brings out the green in your eyes.",
		 "",
		},
	},
	{IT_DIAMOND,
		{"The most sparklicious of all gemstones.",
		 "",
		},
	},
	{IT_HERBA,
		{"This herb has a pungent flavor, making it excellent",
		 "in non-edible dishes, especially since it's toxic.",
		},
	},
	{IT_HERBB,
		{"Grows only by rivers.  It excretes a gas into the",
		 "water which causes bubbles, hence the name.",
		},
	},
	{IT_HERBC,
		{"A hardy herb that can grow in any environment,",
		 "though it is not as common as other herbs.",
		},
	},
	{IT_HERBD,
		{"A potent hallucinogenic that grows in dark places.",
		 "Best eaten with a 12-foot tall lizard named Albert.",
		},
	},
	{IT_HERBE,
		{"A mountain herb that grows only on the sides of",
		 "cliffs.  Tastes like chicken!",
		},
	},
	{IT_HERBF,
		{"Villagers often adorn the eaves of their house with",
		 "this, hoping to ward off the coldest winter nights.",
		},
	},
	{IT_POT1,
		{"Drink this to clear your mind.  Then your magic will",
		 "recharge faster for a while!",
		},
	},
	{IT_POT2,
		{"If you can stomach this drink, you'll feel strong",
		 "enough to take on anything.  For a little while.",
		},
	},
	{IT_POT3,
		{"The secret ingredient is rocks.  Makes you take less",
		 "damage for a time.",
		},
	},
	{IT_POT4,
		{"The effervescent crystals within release their healing",
		 "juice slowly over time.",
		},
	},
	{IT_POT5,
		{"High potency energy drink, 99% caffeine.  You'll be",
		 "zooming circles around your foes, until it wears off.",
		},
	},
	{IT_POT6,
		{"This potion flows right to where it's needed most:",
		 "your soul.  It keeps you from dying for a while!",
		},
	},
	{IT_POT7,
		{"Okay, it tastes gross.  But pour it on your skin and",
		 "bullets will bounce right off of you!",
		},
	},
	{IT_POT8,
		{"No aliens were harmed in the making of this beverage.",
		 "But it does endow you with harmful acid blood!",
		},
	},
	{IT_POT9,
		{"Keep monsters at bay with the powerful stench of Fear",
		 "Beer!  Not recommended for date nights.",
		},
	},
	{IT_POT10,
		{"It's like drinking pure lightning!  And once you do,",
		 "your enemies will be zapped with pure lightning.",
		},
	},
	{IT_POT11,
		{"The clarity and focus imparted by this water will",
		 "improve your ability with all skills for a time.",
		},
	},
	{IT_POT12,
		{"It's a taco sauce, and it's a floor polish!  Also,",
		 "drinking it makes you earn more experience.",
		},
	},
	{IT_POT13,
		{"Brewed from the mystical waters of Mount Shamrock,",
		 "this is rumored to increase your luck.",
		},
	},
	{IT_POT14,
		{"Dab this behind your ears, and the monsters will come",
		 "running in no time.",
		},
	},
	{IT_POT15,
		{"For a quick pick-me-up, down a Healenbrau!  It'll",
		 "instantly heal any malady.",
		},
	},
	{IT_POT16,
		{"For the wizard on the go.  When you just can't wait",
		 "for Magic to recover, drink it down in liquid form!",
		},
	},
	{IT_POT17,
		{"Nothing powers you up like Stamino.  One gulp and",
		 "you'll be able to work tirelessly for ... a bit.",
		},
	},
	{IT_POT18,
		{"The after-effects of consuming Rocket Sauce are",
		 "what kept it illegal in 14 countries.  Hot stuff!",
		},
	},
	{IT_POT19,
		{"Do not drink Explosivo!  Pour it on your weapon,",
		 "and let the enemies enjoy its bursting flavor.",
		},
	},
	{IT_POT20,
		{"Best served chilled.  Once you drink it, everyone",
		 "around you will be equally chilled.",
		},
	},
	{IT_PRESENT,
		{"Festivus gifts for the children of Eisburg.  They appear",
		 "to be festive.",
		},
	},
	{IT_UNDIES,
		{"These stylish undergarments are made of delightfully",
		 "itchy (and toxic) asbestos.  They protect you from heat.",
		},
	},
	{IT_KEY1,
		{"If I had to guess, I'd say this key will open things",
		 "with pictures of snowmen on them.",
		},
	},
	{IT_KEY2,
		{"A key with a fresh outdoorsy scent.  You almost feel",
		 "like hanging it on a mirror.",
		},
	},
	{IT_KEY3,
		{"Holding this key feels like shaking hands.",
		 "",
		},
	},
	{IT_KEY4,
		{"That is one blocky snowflake!  I guess it's stylized.",
		 "",
		},
	},
	{IT_KEY5,
		{"Warning: do not try to open bears with this key.",
		 "",
		},
	},
	{IT_TOOLBOX,
		{"A box full of handy tools, each individually etched",
		 "with the name \"Hank\".",
		},
	},
	{IT_SHOVEL,
		{"Compact enough to fit in a toolbox, yet sturdy enough",
		 "for digging people out of avalanches.",
		},
	},
	{IT_SHARPSTONE,
		{"A stone that, in the right hands, can make axes sharper.",
		 "In your hands, it can permanently ruin them.",
		},
	},
	{IT_LETTER,
		{"A letter addressed to Curaga in the Frosty Cliffs.  You",
		 "are much too polite to open it.",
		},
	},
	{IT_SOUNDING,
		{"An extremely fragile crystal with the magical power to",
		 "shatter barriers.  If you get hit, it will break.",
		},
	},
	{IT_BCRYSTAL,	// binding
		{"An oddly sticky crystal.",
		 "",
		},
	},
	{IT_ICESKATE,
		{"With these on, you'll have some measure of control when",
		 "on the ice.",
		},
	},
	{IT_WINGBOOT,
		{"Though winged footwear can't hold you aloft, it provides",
		 "just enough lift to make walking on clouds possible.",
		},
	},
	{IT_PACKAGE,
		{"A box full of macguffins for Curaga.  Apparently, she",
		 "needs them urgently.",
		},
	},
	{IT_CANDY,
		{"From a baby.  For shame!",
		 "",
		},
	},
	{IT_STOLEN,
		{"Don't know what's in it, but The Diabolical Onion wants",
		 "it!",
		},
	},
	{IT_GLASSES,
		{"Handy rose-colored lenses help you to see the brighter",
		 "side of life.  Also, they're magical!",
		},
	},
	{IT_BOOK,
		{"The ocean-going thriller \"Das Book.\"",
		 "",
		},
	},
	{IT_THING,
		{"It's sort of a, uh, you know... thing.",
		 "",
		},
	},
	{IT_THING2,
		{"It's like a kind of, uh, other ... thing.",
		 "",
		},
	},
	{IT_WATCH,
		{"Vaguely watch-like, but it doesn't appear "
		 "to tell time.",
		},
	},
	{IT_TOYCRYSTAL,
		{"It looks like a Binding Crystal, but is subtly different.",
		 "In some way.",
		},
	},
	{IT_ARTBIT,
		{"It's so glowy and pretty!  You want more of these!!",
		 "",
		},
	},
	{255,
		{"DONE!",
		 "",
		},
	},
};

byte bossDrop=0;

void InitialEquip(void)
{
	int i,j;

	for(i=0;i<MAX_EQUIP;i++)
	{
		player.items[i].type=IT_NONE;
		player.items[i].magic=0;
		for(j=0;j<2;j++)
			player.items[i].skill[j]=0;
		player.items[i].count=0;
	}

	player.axe.type=IT_AXE;
	player.axe.magic=0;
	player.axe.value=2;
	player.axe.speed=2;
	player.axe.sharpened=0;
	player.axe.count=1;

	player.parka.type=IT_PARKA;
	player.parka.magic=0;
	player.parka.value=2;
	player.parka.speed=2;
	player.parka.sharpened=0;
	player.parka.count=1;

	player.amulet.type=IT_AMULET;
	player.amulet.magic=0;
	player.amulet.value=2;
	player.amulet.speed=2;
	player.amulet.sharpened=0;
	player.amulet.count=1;
	for(i=0;i<3;i++)
	{
		player.axe.skill[i]=0;
		player.parka.skill[i]=0;
		player.amulet.skill[i]=0;
	}
}

void ListSkillBoosts(int x,int y,equip_t *eq)
{
	byte pts[3];
	char s[64];

	if(eq->type==IT_MAGICAXE || eq->type==IT_MAGICAMULET || eq->type==IT_MAGICPARKA)
	{
		Print(x+208,y+352+24,magicDesc[eq->magic],0,1);
		sprintf(s,"+1 to %s Skill.",GetSkillName(eq->skill[0],1));
		Print(x+208,y+352+36,s,0,1);
	}
	else if(eq->type==IT_GOLDENAXE || eq->type==IT_GOLDENAMULET || eq->type==IT_GOLDENPARKA)
	{
		Print(x+208,y+352+24,magicDesc[eq->magic],0,1);
#ifdef DIRECTORS
		if(eq->magic==EM_DIRECTORS || eq->magic==EM_OMELET || eq->magic==EM_DOOM)
		{
			Print(x+208,y+352+36,"+1 to all skills!",0,1);
		}
		else
		{
#endif
		pts[0]=pts[1]=pts[2]=1;
		if(eq->skill[1]==eq->skill[0])
		{
			pts[1]=0;
			pts[0]++;
		}
		if(eq->skill[2]==eq->skill[0])
		{
			pts[2]=0;
			pts[0]++;
		}
		if(eq->skill[2]==eq->skill[1] && eq->skill[2]!=eq->skill[0])
		{
			pts[2]=0;
			pts[1]++;
		}
		if(eq->skill[0]!=0)
		{
			sprintf(s,"+%d to %s Skill.",pts[0],GetSkillName(eq->skill[0],1));
			Print(x+208,y+352+36,s,0,1);
			if(pts[1]!=0)
			{
				sprintf(s,"+%d to %s Skill.",pts[1],GetSkillName(eq->skill[1],1));
				Print(x+208,y+352+48,s,0,1);
			}
			if(pts[2]!=0)
			{
				sprintf(s,"+%d to %s Skill.",pts[2],GetSkillName(eq->skill[2],1));
				Print(x+208,y+352+60,s,0,1);
			}
		}
#ifdef DIRECTORS
		}
#endif
	}
	else
	{
		// not magic, sucker
	}
}

void DescribeItem(int x,int y,equip_t *eq)
{
	char s[64];
	int i,j;

	if(eq->type==IT_NONE)
		return;
#ifdef DIRECTORS
	if(eq->magic==EM_DIRECTORS || eq->magic==EM_OMELET || eq->magic==EM_DOOM)
		PrintColor(x+208,y+352,EquipName(eq),1,-12,1);
	else
		Print(x+208,y+352,EquipName(eq),0,1);
#else
	Print(x+208,y+352,EquipName(eq),0,1);
#endif
	switch(eq->type)
	{
		case IT_AXE:
		case IT_MAGICAXE:
		case IT_GOLDENAXE:
			if(eq->sharpened)
				sprintf(s,"Damage: %d+1 (Sharpened)",eq->value-1);
			else
				sprintf(s,"Damage: %d",eq->value);
			Print(x+208,y+352+12,s,0,1);
			sprintf(s,"Speed: %d",eq->speed);
			Print(x+408,y+352+12,s,0,1);
			ListSkillBoosts(x,y,eq);
			break;
		case IT_AMULET:
		case IT_MAGICAMULET:
		case IT_GOLDENAMULET:
			sprintf(s,"Magic: %d",eq->value);
			Print(x+208,y+352+12,s,0,1);
			sprintf(s,"Spell Dmg +%d%%",eq->speed*5);
			Print(x+408,y+352+12,s,0,1);
			ListSkillBoosts(x,y,eq);
			break;
		case IT_PARKA:
		case IT_MAGICPARKA:
		case IT_GOLDENPARKA:
			sprintf(s,"Defense: %d",eq->value);
			Print(x+208,y+352+12,s,0,1);
			sprintf(s,"Stamina: %d",eq->speed);
			Print(x+408,y+352+12,s,0,1);
			ListSkillBoosts(x,y,eq);
			break;
		default:
			if(eq->type>=IT_LENSA && eq->type<=IT_LENSM)
			{
				sprintf(s,"A glass lens inscribed with the word \"%s\".",LensName(eq->type-IT_LENSA));
				Print(x+208,y+352+12,s,0,1);
			}
			else
			{
				i=0;
				while(itemDesc[i].type!=255)
				{
					if(itemDesc[i].type==eq->type)
					{
						for(j=0;j<2;j++)
							Print(x+208,y+352+24+j*12,itemDesc[i].desc[j],0,1);
					}
					i++;
				}
			}
			break;
	}
}

char potionPrefix[10][16]={
	"Expired",
	"Half-Empty",
	"Half-Full",
	"Diet",
	"Generic",
	"Normal",
	"Fresh",
	"Fizzy",
	"Extra Fizzy",
	"Supreme",
};

char *PotionPrefix(equip_t *eq)
{
	return potionPrefix[eq->value];
}

byte HaveArtifact(byte magic)
{
	int i,j;

	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].magic==magic)
			return 1;
	}
	// also can't find one that's in one of the shops
	for(i=0;i<SHOP_SIZE;i++)
		for(j=0;j<SHOP_MAX;j++)
			if(player.shopInv[j][i].type!=IT_NONE && player.shopInv[j][i].magic==magic)
				return 1;

	if(player.axe.magic==magic)
		return 1;
	if(player.amulet.magic==magic)
		return 1;
	if(player.parka.magic==magic)
		return 1;
	return 0;
}

char *EquipName(equip_t *eq)
{
	// axe prefix is defined by (speed/2)*5+(damage/2)
	char axePrefix[25][16]={
		"Ruined",	// speed 1-2, damage 1-2
		"Rusted",	// speed 1-2, damage 3-4
		"Plodding",	// speed 1-2, damage 5-6
		"Hefty",	// speed 1-2, damage 7-8
		"Crushing",	// speed 1-2, damage 9-10
		"Cruddy",	// speed 3-4, damage 1-2
		"Poor",		// speed 3-4, damage 3-4
		"Big",		// speed 3-4, damage 5-6
		"Mighty",	// speed 3-4, damage 7-8
		"Enormous",	// speed 3-4, damage 9-10
		"Dull",		// speed 5-6, damage 1-2
		"Average",	// speed 5-6, damage 3-4
		"Decent",	// speed 5-6, damage 5-6
		"Great",	// speed 5-6, damage 7-8
		"Devastating",	// speed 5-6, damage 9-10
		"Weak",		// speed 7-8, damage 1-2
		"Swift",	// speed 7-8, damage 3-4
		"Incredible",	// speed 7-8, damage 5-6
		"Demolishing",	// speed 7-8, damage 7-8
		"Supreme",	// speed 7-8, damage 9-10
		"Paper",	// speed 9-10, damage 1-2
		"Flimsy",	// speed 9-10, damage 3-4
		"Quick",	// speed 9-10, damage 5-6
		"Lightning",	// speed 9-10, damage 7-8
		"Ultimate",	// speed 9-10, damage 9-10
	};
	// parka is (defense/2)*5+(stamina/2)
	char parkaPrefix[25][16]={
		"Decrepit", // defense 1-2, stamina 1-2
		"Torn", // defense 1-2, stamina 3-4
		"Ripped", // defense 1-2, stamina 5-6
		"Lightweight", // defense 1-2, stamina 7-8
		"Relaxed Fit", // defense 1-2, stamina 9-10
		"Dumpy", // defense 3-4, stamina 1-2
		"Flimsy", // defense 3-4, stamina 3-4
		"Hoodie", // defense 3-4, stamina 5-6
		"Silky", // defense 3-4, stamina 7-8
		"Comfortable", // defense 3-4, stamina 9-10
		"Uncomfortable", // defense 5-6, stamina 1-2
		"Decent", // defense 5-6, stamina 3-4
		"Woolen", // defense 5-6, stamina 5-6
		"Leather", // defense 5-6, stamina 7-8
		"Sports", // defense 5-6, stamina 9-10
		"Heavy", // defense 7-8, stamina 1-2
		"Hefty", // defense 7-8, stamina 3-4
		"Strong", // defense 7-8, stamina 5-6
		"Hardcore", // defense 7-8, stamina 7-8
		"Ubercomfy", // defense 7-8, stamina 9-10
		"Chainmail", // defense 9-10, stamina 1-2
		"Unbreakable", // defense 9-10, stamina 3-4
		"Premium", // defense 9-10, stamina 5-6
		"Outrageous", // defense 9-10, stamina 7-8
		"Inconceivable", // defense 9-10, stamina 9-10
	};
	// amulet is (magic/2)*5+(power boost/2)
	char amuletPrefix[25][16]={
		"Rusty", // magic 1-2, power 1-2
		"Plain", // magic 1-2, power 3-4
		"Zirconium", // magic 1-2, power 5-6
		"Focused", // magic 1-2, power 7-8
		"Blasting", // magic 1-2, power 9-10
		"Corrupted", // magic 3-4, power 1-2
		"Stylish", // magic 3-4, power 3-4
		"Gold", // magic 3-4, power 5-6
		"Sunstone", // magic 3-4, power 7-8
		"Singing", // magic 3-4, power 9-10
		"Enchanted", // magic 5-6, power 1-2
		"Jade", // magic 5-6, power 3-4
		"Ruby", // magic 5-6, power 5-6
		"Emerald", // magic 5-6, power 7-8
		"Platinum", // magic 5-6, power 9-10
		"Sparkling", // magic 7-8, power 1-2
		"Shimmering", // magic 7-8, power 3-4
		"Diamond", // magic 7-8, power 5-6
		"Ancient", // magic 7-8, power 7-8
		"Devastating", // magic 7-8, power 9-10
		"Elder", // magic 9-10, power 1-2
		"Infused", // magic 9-10, power 3-4
		"Bloodstone", // magic 9-10, power 5-6
		"Fantabulous", // magic 9-10, power 7-8
		"Supertastic", // magic 9-10, power 9-10
	};

	static char result[48];
	byte v;

	result[0]='\0';
	switch(eq->type)
	{
		case IT_AXE:
		case IT_MAGICAXE:
		case IT_GOLDENAXE:
			if(eq->value==0)
				v=0;
			else
			{
				if(eq->value<10)
					v=eq->value-1;
				else
					v=9;
			}

			v/=2;
			if(eq->speed>0)
			{
				if(eq->speed<10)
					v=v+((eq->speed-1)/2)*5;
				else
					v=v+((10-1)/2)*5;
			}

			if(eq->type==IT_MAGICAXE || eq->type==IT_GOLDENAXE)
				strcpy(result,magicNames[eq->magic]);
			else
				strcpy(result,axePrefix[v]);

			if(eq->magic>=EM_ARTSTART && eq->magic<=EM_ARTEND)
			{
				sprintf(result,"The %s Axe",magicNames[eq->magic]);
			}
			else
			{
				if(eq->type==IT_GOLDENAXE)
					strcat(result," Golden");
				strcat(result," Axe");
				if(eq->type==IT_MAGICAXE)	// magic items name their skill, maybe
				{
					strcat(result," Of ");
					strcat(result,GetSkillName(eq->skill[0],0));
				}
			}
			break;
		case IT_AMULET:
		case IT_MAGICAMULET:
		case IT_GOLDENAMULET:
			if(eq->speed==0)
				v=0;
			else
			{
				if(eq->speed<10)
					v=eq->speed-1;
				else
					v=9;
			}

			v/=2;
			if(eq->value>0)
			{
				if(eq->value<10)
					v=v+((eq->value-1)/2)*5;
				else
					v=v+((10-1)/2)*5;
			}

			if(eq->type==IT_MAGICAMULET || eq->type==IT_GOLDENAMULET)
				strcpy(result,magicNames[eq->magic]);
			else
				strcpy(result,amuletPrefix[v]);

			if(eq->magic>=EM_ARTSTART && eq->magic<=EM_ARTEND)
			{
				sprintf(result,"The %s Amulet",magicNames[eq->magic]);
			}
			else
			{
				if(eq->type==IT_GOLDENAMULET)
					strcat(result," Golden");
				strcat(result," Amulet");
				if(eq->type==IT_MAGICAMULET)	// magic items name their skill, maybe
				{
					strcat(result," Of ");
					strcat(result,GetSkillName(eq->skill[0],0));
				}
			}
			break;
		case IT_PARKA:
		case IT_MAGICPARKA:
		case IT_GOLDENPARKA:
			if(eq->speed==0)
				v=0;
			else
			{
				if(eq->speed<10)
					v=eq->speed-1;
				else
					v=9;
			}

			v/=2;
			if(eq->value>0)
			{
				if(eq->value<10)
					v=v+((eq->value-1)/2)*5;
				else
					v=v+((10-1)/2)*5;
			}


			if(eq->type==IT_MAGICPARKA || eq->type==IT_GOLDENPARKA)
				strcpy(result,magicNames[eq->magic]);
			else
				strcpy(result,parkaPrefix[v]);

			if(eq->magic>=EM_ARTSTART && eq->magic<=EM_ARTEND)
			{
				sprintf(result,"The %s Parka",magicNames[eq->magic]);
			}
			else
			{
				if(eq->type==IT_GOLDENPARKA)
					strcat(result," Golden");
				strcat(result," Parka");
				if(eq->type==IT_MAGICPARKA)	// magic items name their skill, maybe
				{
					strcat(result," Of ");
					strcat(result,GetSkillName(eq->skill[0],0));
				}
			}
			break;
		default:
			if(eq->type>=IT_POT1 && eq->type<=IT_POT20)
			{
				sprintf(result,"%s %s",potionPrefix[eq->value],ItemName(eq->type));
			}
			else if(eq->type>=IT_LENSA && eq->type<=IT_LENSM)
			{
				sprintf(result,"%s Lens",LensName(eq->type-IT_LENSA));
			}
			else
				strcpy(result,ItemName(eq->type));
			break;
	}
	return result;
}

byte JunkCount(void)
{
	int i;
	byte c;

	c=0;
	for(i=0;i<MAX_EQUIP;i++)
		if(player.items[i].type==IT_LOG || player.items[i].type==IT_GEAR || player.items[i].type==IT_CAN ||
			player.items[i].type==IT_STUFFING || player.items[i].type==IT_SOCK || player.items[i].type==IT_ICECUBE ||
			player.items[i].type==IT_SEED || player.items[i].type==IT_STONE || player.items[i].type==IT_ECTO || player.items[i].type==IT_CLAW)
		{
			if(c+player.items[i].count<=255)
				c+=player.items[i].count;
			else
				return 255;
		}
	return c;
}

byte EquipCount(byte item)
{
	int i;
	byte c;

	c=0;
	for(i=0;i<MAX_EQUIP;i++)
		if(player.items[i].type==item)
		{
			if(c+player.items[i].count<=255)
				c+=player.items[i].count;
			else
				return 255;
		}
	return c;
}

// same as equipcount, but also checks your stash.  Use equipcount to see if you can DO things, GotItem to see if
// you should be given quest items (i.e. you definitely have never been offered them)
byte GotItem(byte item)
{
	int i;
	byte c;

	c=0;
	for(i=0;i<MAX_EQUIP;i++)
		if(player.items[i].type==item)
		{
			if(c+player.items[i].count<=255)
				c+=player.items[i].count;
			else
				c=255;
		}

	for(i=0;i<SHOP_SIZE;i++)
	{
		if(player.shopInv[SHOP_STASH][i].type==item)
		{
			if(c+player.shopInv[SHOP_STASH][i].count<=255)
				c+=player.shopInv[SHOP_STASH][i].count;
			else
				c=255;
		}
	}
	return c;
}


byte EquipAdd(byte item,int val1,int val2)
{
	int i;
	equip_t eq;
	char s[64];

	eq.type=item;
	eq.value=(val1)%256;
	eq.speed=((val1)/256)%256;
	eq.magic=(val1)/65536;
	eq.sharpened=0;
	eq.count=1;
	switch(eq.type)
	{
		case IT_MAGICAXE:
		case IT_MAGICAMULET:
		case IT_MAGICPARKA:
			eq.skill[0]=(val2)%256;
			eq.skill[1]=0;
			eq.skill[2]=0;
			break;
		case IT_GOLDENAXE:
		case IT_GOLDENAMULET:
		case IT_GOLDENPARKA:
			eq.skill[0]=(val2)%256;
			eq.skill[1]=((val2)/256)%256;
			eq.skill[2]=((val2)/65536)%256;
			break;
		default:
			eq.skill[0]=0;
			eq.skill[1]=0;
			eq.skill[2]=0;
			break;
	}

	sprintf(s,"Got %s.",EquipName(&eq));

	// see if it matches your inventory
	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].type==eq.type && player.items[i].sharpened==eq.sharpened && player.items[i].value==eq.value && player.items[i].speed==eq.speed &&
			player.items[i].skill[0]==eq.skill[0] && player.items[i].skill[1]==eq.skill[1] && player.items[i].skill[2]==eq.skill[2] && player.items[i].magic==eq.magic)
		{
			// it found a friend!
			if(player.items[i].count<255)
				player.items[i].count++;

			NewMessage(s,90);
			QuestReportEvent(QE_GET,eq.type);
			return 1;
		}
	}
	// nope, how about any open slots?
	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].type==IT_NONE)
		{
			memcpy(&player.items[i],&eq,sizeof(equip_t));
			NewMessage(s,90);
			SortEquipment();
			QuestReportEvent(QE_GET,eq.type);
			return 1;
		}
	}
	// pack is full
	sprintf(s,"No room for %s!",EquipName(&eq));
	NoRepeatNewMessage(s,90);
	return 0;
}

byte StashTake(equip_t *e)
{
	int i;

	// see if it matches your inventory
	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].type==e->type && player.items[i].sharpened==e->sharpened && player.items[i].value==e->value && player.items[i].speed==e->speed &&
			player.items[i].skill[0]==e->skill[0] && player.items[i].skill[1]==e->skill[1] && player.items[i].skill[2]==e->skill[2] && player.items[i].magic==e->magic)
		{
			// it found a friend!
			if(player.items[i].count<255)
				player.items[i].count++;;
			return 1;
		}
	}
	// nope, how about any open slots?
	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].type==IT_NONE)
		{
			memcpy(&player.items[i],e,sizeof(equip_t));
			player.items[i].count=1;
			SortEquipment();
			return 1;
		}
	}
	// pack is full
	return 0;
}

byte StashGive(int i)
{
	int j;

	// see if it matches your inventory
	for(j=0;j<SHOP_SIZE;j++)
	{
		if(player.items[i].type==player.shopInv[SHOP_STASH][j].type &&
			player.items[i].sharpened==player.shopInv[SHOP_STASH][j].sharpened &&
			player.items[i].value==player.shopInv[SHOP_STASH][j].value &&
			player.items[i].speed==player.shopInv[SHOP_STASH][j].speed &&
			player.items[i].skill[0]==player.shopInv[SHOP_STASH][j].skill[0] &&
			player.items[i].skill[1]==player.shopInv[SHOP_STASH][j].skill[1] &&
			player.items[i].skill[2]==player.shopInv[SHOP_STASH][j].skill[2] &&
			player.items[i].magic==player.shopInv[SHOP_STASH][j].magic)
		{
			// it found a friend!
			if(player.shopInv[SHOP_STASH][j].count<255)
				player.shopInv[SHOP_STASH][j].count++;
			SortShop(SHOP_STASH);
			return 1;
		}
	}
	// nope, how about any open slots?
	for(j=0;j<SHOP_SIZE;j++)
	{
		if(player.shopInv[SHOP_STASH][j].type==IT_NONE)
		{
			memcpy(&player.shopInv[SHOP_STASH][j],&player.items[i],sizeof(equip_t));
			player.shopInv[SHOP_STASH][j].count=1;
			SortShop(SHOP_STASH);
			return 1;
		}
	}
	// pack is full
	return 0;
}

byte SimpleEquipAdd(equip_t *e)
{
	int i;

	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].type==e->type && player.items[i].sharpened==e->sharpened && player.items[i].value==e->value && player.items[i].speed==e->speed &&
		player.items[i].skill[0]==e->skill[0] && player.items[i].skill[1]==e->skill[1] && player.items[i].skill[2]==e->skill[2] && player.items[i].magic==e->magic)
		{
			player.items[i].count++;
			return 1;
		}
	}
	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].type==IT_NONE)
		{
			memcpy(&player.items[i],e,sizeof(equip_t));
			return 1;
		}
	}
	return 0;	// no open slots!
}

void EquipCreateItem(byte level,byte *type,int *val1,int *val2)
{
	byte val,spd,sk[3],magic;
	byte upAmt;

	val=rand()%13;
	if(ModifierOn(MOD_LIQUID))
	{
		if(rand()%2)
			val=9;	// 50% chance to guarantee it's a potion
	}
	if(!bossDrop)
	{
		switch(val)
		{
			case 0:
			case 1:
			case 2:
				(*type)=IT_AXE;
				break;
			case 3:
			case 4:
			case 5:
				(*type)=IT_AMULET;
				break;
			case 6:
			case 7:
			case 8:
				(*type)=IT_PARKA;
				break;
			case 9:
			case 10:
				GuyDropPotion(level,type,val1,val2);
				return;
				break;
			case 11:
			case 12:
				GuyDropLens(level,type,val1,val2);
				return;
				break;
		}
	}
	else
	{
		switch(rand()%3)
		{
			case 0:
				(*type)=IT_AXE;
				break;
			case 1:
				(*type)=IT_AMULET;
				break;
			case 2:
				(*type)=IT_PARKA;
				break;
		}
	}

	magic=EM_NONE;
	spd=0;

	val=(level/6)+1;	// start at level/6 base value
	if(val>10)
		val=10;

	upAmt=rand()%100;
	while(1)
	{
		if(upAmt<30)
			break;	// 30% chance that this is quite the power level we want
		else if(upAmt<65)
		{
			if(val>1)
				val--;	// 35% chance that it will go down one
		}
		else
		{
			if(val<10)
				val++;	// 35% chance of up
		}

		upAmt=rand()%100;
	}

	sk[0]=val*2;
	val=rand()%(sk[0]);
	if(val==0)
		val=1;
	if(val>10)
		val=10;
	spd=sk[0]-val;
	while(spd>10)
	{
		spd--;
		val++;
	}

	if(spd>10)
		spd=10;
	if(val>10)
		val=10;
	if((rand()%(10-2*(player.var[VAR_MADCAP]!=0)))==0 || bossDrop)	// 1 in 10 chance of getting magic instead of normal, magic also adds +1,+1
	{
		bossDrop=0;
		(*type)++;
		spd++;
		val++;
		magic=1+(rand()%10)+((*type)==IT_MAGICAMULET)*20+((*type)==IT_MAGICPARKA)*10;

		// 1 in 30 chance of it being cursed instead
		if((rand()%30)==0)
		{
			// 1 in 10 chance of being elite instead of that
			if((rand()%10)==0)
			{
				magic=EM_POISON2+(rand()%5)+((*type)==IT_MAGICAMULET)*10+((*type)==IT_MAGICPARKA)*5;
			}
			else
				magic=EM_DEFEND+(rand()%3)+((*type)==IT_MAGICAMULET)*6+((*type)==IT_MAGICPARKA)*3;
		}

		if((rand()%5)==0)	// 1 in 5 chance of it being golden (1 in 50 chance of any item being golden), golden adds another +1,+1
		{
			(*type)++;
			spd++;
			val++;
		}
		if(spd>10)
			spd=10;
		if(val>10)
			val=10;
	}

	sk[0]=sk[1]=sk[2]=0;
	if((*type)==IT_AXE || (*type)==IT_AMULET || (*type)==IT_PARKA)
	{
		// done
	}
	else
	{
		sk[0]=PickSkillToBoost((*type));
		if((*type)==IT_GOLDENAXE || (*type)==IT_GOLDENAMULET || (*type)==IT_GOLDENPARKA)
		{
			sk[1]=PickSkillToBoost(0);
			sk[2]=PickSkillToBoost(0);
			// if golden, 2% chance to be artifact
			if((rand()%100)<2)
			{
				byte m;

				m=EM_ARTSTART+(rand()%(EM_ARTEND-EM_ARTSTART+1));
				if(!HaveArtifact(m))
				{
					magic=m;
					switch(magic)
					{
						case EM_NEBROGUB:
							(*type)=IT_GOLDENAXE;
							spd=7;
							val=12;
							sk[0]=SKILL_AXEMASTERY;
							sk[1]=SKILL_THROWING;
							sk[2]=SKILL_STRENGTH;
							break;
						case EM_REGEN:
							(*type)=IT_GOLDENAMULET;
							spd=5;
							val=7;
							sk[0]=SKILL_SURVIVAL;
							sk[1]=SKILL_RESTORE;
							sk[2]=SKILL_LIFEBERRY;
							break;
						case EM_COOLGUY:
							(*type)=IT_GOLDENPARKA;
							spd=7;
							val=9;
							sk[0]=SKILL_ICECUBE;
							sk[1]=SKILL_COLDWAVE;
							sk[2]=SKILL_CRYOZOID;
							break;
						case EM_LINKIN:
							(*type)=IT_GOLDENPARKA;
							spd=10;
							val=7;
							sk[0]=SKILL_TOUGHNESS;
							sk[1]=SKILL_DEFENSE;
							sk[2]=SKILL_ENERGY;
							break;
						case EM_GHOSTLY:
							(*type)=IT_GOLDENAXE;
							spd=3;
							val=7;
							sk[0]=SKILL_BONEHEAD;
							sk[1]=SKILL_VAMPIRE;
							sk[2]=SKILL_BONEBOMB;
							break;
						case EM_IMAJICA:
							(*type)=IT_GOLDENAMULET;
							spd=10;
							val=10;
							sk[0]=SKILL_ENERGY;
							sk[1]=SKILL_ENERGY;
							sk[2]=SKILL_TRAINING;
							break;
						case EM_NUCULAR:
							(*type)=IT_GOLDENAXE;
							spd=1;
							val=10;
							sk[0]=SKILL_AIRSTRIKE;
							sk[1]=SKILL_FLAME;
							sk[2]=SKILL_BURNINATE;
							break;
						case EM_TRAINEE:
							(*type)=IT_GOLDENPARKA;
							spd=2;
							val=2;
							sk[0]=SKILL_TRAINING;
							sk[1]=SKILL_TRAINING;
							sk[2]=SKILL_TRAINING;
							break;
						case EM_FATCROW:
							(*type)=IT_GOLDENAMULET;
							spd=10;
							val=10;
							sk[0]=0;
							sk[1]=0;
							sk[2]=0;
							break;
						case EM_QUESTION:
							(*type)=IT_GOLDENAXE;
							spd=10;
							val=10;
							sk[0]=Random(MAX_SKILLS)+1;
							sk[1]=Random(MAX_SKILLS)+1;
							sk[2]=Random(MAX_SKILLS)+1;
							break;
						case EM_PURPLEKOO:
							(*type)=IT_GOLDENPARKA;
							spd=1;
							val=15;
							sk[0]=SKILL_TOUGHNESS;
							sk[1]=SKILL_PARRY;
							sk[2]=SKILL_DODGE;
							break;
						case EM_DEATH:
							(*type)=IT_GOLDENAMULET;
							spd=10;
							val=10;
							sk[0]=SKILL_BONEHEAD;
							sk[1]=SKILL_BONEBOMB;
							sk[2]=SKILL_DRAIN;
							break;
						case EM_JUSTHEF:
							(*type)=IT_GOLDENAXE;
							spd=5;
							val=5;
							sk[0]=SKILL_TRAINING;
							sk[1]=SKILL_TRACKING;
							sk[2]=SKILL_CRITICAL;
							break;
						case EM_PARALLEL:
							(*type)=IT_GOLDENPARKA;
							spd=10;
							val=6;
							sk[0]=SKILL_LUCK;
							sk[1]=SKILL_STRENGTH;
							sk[2]=SKILL_SURVIVAL;
							break;
						case EM_BOKBOK:
							(*type)=IT_GOLDENAMULET;
							spd=8;
							val=8;
							sk[0]=SKILL_LIFEBERRY;
							sk[1]=SKILL_CRYOZOID;
							sk[2]=SKILL_SHROOM;
							break;
						case EM_WHIRLING:
							(*type)=IT_GOLDENAXE;
							spd=4;
							val=8;
							sk[0]=SKILL_WHIRL;
							sk[1]=SKILL_WHIRL;
							sk[2]=SKILL_WHIRL;
							break;
						case EM_DARK:
							(*type)=IT_GOLDENPARKA;
							spd=12;
							val=12;
							sk[0]=SKILL_TRACKING;
							sk[1]=SKILL_TRACKING;
							sk[2]=SKILL_TRACKING;
							break;
						case EM_SUMMONERS:
							(*type)=IT_GOLDENAMULET;
							spd=8;
							val=9;
							sk[0]=SKILL_TOASTIES;
							sk[1]=SKILL_CRYOZOID;
							sk[2]=SKILL_BONEHEAD;
							break;
						case EM_JORD:
							(*type)=IT_GOLDENAXE;
							spd=1;
							val=1;
							sk[0]=SKILL_TRAINING;
							sk[1]=SKILL_TRAINING;
							sk[2]=SKILL_TRAINING;
							break;
						case EM_PARACHUTE:
							(*type)=IT_GOLDENPARKA;
							spd=10;
							val=10;
							sk[0]=SKILL_DODGE;
							sk[1]=SKILL_TORNADO;
							sk[2]=SKILL_GALE;
							break;
						case EM_EVILEYE:
							(*type)=IT_GOLDENAMULET;
							spd=5;
							val=12;
							sk[0]=SKILL_TOASTIES;
							sk[1]=SKILL_BURNINATE;
							sk[2]=SKILL_IGNITE;
							break;
						case EM_PERSUADER:
							(*type)=IT_GOLDENAXE;
							spd=4;
							val=3;
							sk[0]=SKILL_PERSUADE;
							sk[1]=SKILL_PERSUADE;
							sk[2]=SKILL_PERSUADE;
							break;
						case EM_OLDLACE:
							(*type)=IT_GOLDENPARKA;
							spd=10;
							val=10;
							sk[0]=SKILL_PLAGUE;
							sk[1]=SKILL_POISON;
							sk[2]=SKILL_PLAGUE;
							break;
						case EM_SUNANGEL:
							(*type)=IT_GOLDENAMULET;
							spd=9;
							val=9;
							sk[0]=SKILL_IGNITE;
							sk[1]=SKILL_FLAME;
							sk[2]=SKILL_BURNINATE;
							break;
#ifdef DIRECTORS
						case EM_DIRECTORS:
							(*type)=IT_GOLDENAXE;
							spd=20;
							val=20;
							sk[0]=0;
							sk[1]=0;
							sk[2]=0;
							break;
						case EM_DOOM:
							(*type)=IT_GOLDENPARKA;
							spd=20;
							val=20;
							sk[0]=0;
							sk[1]=0;
							sk[2]=0;
							break;
						case EM_OMELET:
							(*type)=IT_GOLDENAMULET;
							spd=20;
							val=20;
							sk[0]=0;
							sk[1]=0;
							sk[2]=0;
							break;
						case EM_BANHAMMER:
							(*type)=IT_GOLDENAXE;
							spd=1;
							val=15;
							sk[0]=SKILL_STRENGTH;
							sk[1]=SKILL_BURNINATE;
							sk[2]=SKILL_BASH;
							break;
						case EM_IMAGINARY:
							(*type)=IT_GOLDENPARKA;
							spd=0;
							val=0;
							sk[0]=0;
							sk[1]=0;
							sk[2]=0;
							break;
						case EM_SP:
							(*type)=IT_GOLDENAMULET;
							spd=10;
							val=10;
							sk[0]=SKILL_TOASTIES;
							sk[1]=SKILL_AIRSTRIKE;
							sk[2]=SKILL_FLAME;
							break;
#endif
					}
				}
			}
		}
	}

	(*val1)=magic*65536+spd*256+val;
	(*val2)=sk[2]*65536+sk[1]*256+sk[0];
}

void EquipTest(void)
{
	int i;
	int val1,val2;
	byte type;
	int lensResults[13];
	int potResults[10];

	for(i=0;i<13;i++)
		lensResults[i]=0;
	for(i=0;i<10;i++)
		potResults[i]=0;

	auto f = AppdataOpen_Write("text.txt");
	for(i=0;i<1000;i++)
	{
 		EquipCreateItem((i%60)+1,&type,&val1,&val2);
		PlayerGetItem(type,val1,val2);
		if(type>=IT_LENSA && type<=IT_LENSM)
			lensResults[type-IT_LENSA]++;
		SDL_RWprintf(f.get(), "%02d: %s (%d/%d, %s, %s, %s)\n",(i%60)+1,EquipName(&player.items[0]),player.items[0].value,player.items[0].speed,
				GetSkill(player.items[0].skill[0])->name,GetSkill(player.items[0].skill[1])->name,GetSkill(player.items[0].skill[2])->name);

		player.items[0].type=IT_NONE;
	}
	for(i=0;i<1000;i++)
	{
		type=0;
 		while(type<IT_POT1 || type>IT_POT20)
			EquipCreateItem(20,&type,&val1,&val2);
		PlayerGetItem(type,val1,val2);
		potResults[val1]++;
		SDL_RWprintf(f.get(), "%02d: %s (%d/%d, %s, %s, %s)\n",(i%60)+1,EquipName(&player.items[0]),player.items[0].value,player.items[0].speed,
				GetSkill(player.items[0].skill[0])->name,GetSkill(player.items[0].skill[1])->name,GetSkill(player.items[0].skill[2])->name);

		player.items[0].type=IT_NONE;
	}
	for(i=0;i<13;i++)
	{
		SDL_RWprintf(f.get(), "LENS%d: %03d\n",i,lensResults[i]);
	}
	for(i=0;i<10;i++)
	{
		SDL_RWprintf(f.get(), "POT%d: %03d\n",i,potResults[i]);
	}

	f.reset();
	AppdataSync();
}

byte EquipSkillBoost(byte skill)
{
	int i,j;
	byte bonus;

	bonus=0;
	if(player.axe.type!=IT_NONE)
	{
		for(j=0;j<3;j++)
			if(player.axe.skill[j]==skill)
				bonus++;
	}
	if(player.amulet.type!=IT_NONE)
	{
		for(j=0;j<3;j++)
			if(player.amulet.skill[j]==skill)
				bonus++;
	}
	if(player.parka.type!=IT_NONE)
	{
		for(j=0;j<3;j++)
			if(player.parka.skill[j]==skill)
				bonus++;
	}
#ifdef DIRECTORS
	if(player.axe.magic==EM_DIRECTORS)
		bonus++;
	if(player.amulet.magic==EM_OMELET)
		bonus++;
	if(player.parka.magic==EM_DOOM)
		bonus++;
#endif

	if(GlassEffect()==GE_SKILL)
	{
		if(GlassAmount()==4)
			bonus++;
		else if(GlassAmount()==0 && skill>=SKILL_DEFENSE && skill<=SKILL_LUCK)
			bonus++;
		else if(GlassAmount()==1 && skill>=SKILL_AXEMASTERY && skill<=SKILL_WHIRL)
			bonus++;
		else if(GlassAmount()==2 && skill>=SKILL_THROWING && skill<=SKILL_TURRET)
			bonus++;
		else if(GlassAmount()==3 && skill>=SKILL_SHROOM && skill<=SKILL_PLAGUE)
			bonus++;
	}

	if(skill>=SKILL_DRAIN && skill<=SKILL_PLAGUE && player.amulet.magic==EM_DEATH)
		bonus++;

	j=0;
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]==skill)
		{
			j=1;
			break;
		}
	if(!j)
		bonus=0;	// can't gain bonuses to skills you don't have

	return bonus;
}

void SortEquipment(void)
{
	equip_t tmp,*e;
	int i;
	byte swap;

	e=player.items;
	swap=1;
	while(swap)
	{
		swap=0;
		for(i=0;i<MAX_EQUIP-1;i++)
		{
			if((e[i].type>e[i+1].type && e[i+1].type!=IT_NONE) || (e[i].type==IT_NONE && e[i+1].type!=IT_NONE))	// sort by item type order
			{
				tmp=e[i];
				e[i]=e[i+1];
				e[i+1]=tmp;
				swap=1;
			}
			else if(e[i].type==e[i+1].type && e[i].type!=IT_NONE)	// same item type, so...
			{
				if(e[i].value<e[i+1].value)	// sort by value
				{
					tmp=e[i];
					e[i]=e[i+1];
					e[i+1]=tmp;
					swap=1;
				}
				else if(e[i].value==e[i+1].value && e[i].speed<e[i+1].speed)	// if same, then finally sort by speed
				{
					tmp=e[i];
					e[i]=e[i+1];
					e[i+1]=tmp;
					swap=1;
				}
			}
		}
	}
}

byte DropItemOfType(byte type,byte count)
{
	int i;

	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].type==type)
		{
			if(player.items[i].count>=count)
			{
				player.items[i].count-=count;
				if(player.items[i].count==0)
					player.items[i].type=IT_NONE;
				SortEquipment();
				return 1;	// dropped them!
			}
			else
			{
				// can't drop them all from here, but maybe some
				count-=player.items[i].count;
				player.items[i].count=0;
				player.items[i].type=IT_NONE;
			}
		}
	}
	SortEquipment();
	return 0;	// couldn't drop them all
}

byte AddItemOfType(byte type,byte count)
{
	int i;

	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].type==type)
		{
			if(player.items[i].count<255)
			{
				player.items[i].count+=count;
				return 1;	// got them!
			}
			else
			{
				return 0;	// have too many already
			}
		}
	}
	// if you got here, you didn't have one of those anywhere
	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].type==IT_NONE)
		{
			player.items[i].type=type;
			player.items[i].count=count;
			player.items[i].value=0;
			player.items[i].sharpened=0;
			player.items[i].magic=0;
			player.items[i].speed=0;
			player.items[i].skill[0]=player.items[i].skill[1]=player.items[i].skill[2]=0;
			SortEquipment();
			return 1;
		}
	}

	return 0;	// couldn't add them all
}

void DropItem(byte itm)
{
	player.items[itm].count--;
	if(player.items[itm].count==0)
		player.items[itm].type=IT_NONE;
}

void SmithItems(equip_t *e1,equip_t *e2)
{
	equip_t newitem;
	byte min,max;
	int val1,val2;

	if(e1->type==IT_MAGICAXE)
		newitem.type=IT_GOLDENAXE;
	else if(e1->type==IT_MAGICAMULET)
		newitem.type=IT_GOLDENAMULET;
	else if(e1->type==IT_MAGICPARKA)
		newitem.type=IT_GOLDENPARKA;

	if(rand()%2)
		newitem.magic=e1->magic;
	else
		newitem.magic=e2->magic;

	if(e1->value<e2->value)
	{
		min=e1->value;
		max=e2->value;
	}
	else
	{
		min=e2->value;
		max=e1->value;
	}
	newitem.value=min+(rand()%(max-min+1));
	if(e1->speed<e2->speed)
	{
		min=e1->speed;
		max=e2->speed;
	}
	else
	{
		min=e2->speed;
		max=e1->speed;
	}
	newitem.speed=min+(rand()%(max-min+1));

	newitem.skill[0]=e1->skill[0];
	newitem.skill[1]=e2->skill[0];
	newitem.skill[2]=PickSkillToBoost(0);
	newitem.count=1;
	newitem.sharpened=(byte)(e1->sharpened>0 || e2->sharpened>0);	// can't resharpen if it was already

	val1=(newitem.magic)*65536+(newitem.speed)*256+(newitem.value);
	val2=newitem.skill[2]*65536+newitem.skill[1]*256+newitem.skill[0];

	bullet_t *b;
	b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
	if(b)
	{
		b->speed=0;
		b->target=val1;
		b->owner=val2;
		b->anim=newitem.type;
	}
}

word ItemCost(equip_t *e,byte sell)
{
	word cost;

	if(e->type>=IT_POT1 && e->type<=IT_POT20)
		cost=PotionCost(e);
	else if(e->type>=IT_AXE && e->type<=IT_GOLDENAXE)
	{
		cost=e->value*e->value*e->value*2+e->speed*e->speed*e->speed*2;
		if(e->type==IT_MAGICAXE)
		{
			cost*=2;	// double price for magic!
		}
		if(e->type==IT_GOLDENAXE)
		{
			cost*=4;	// quadruple!
			if(e->magic>=EM_ARTSTART && e->magic<=EM_ARTEND)
				cost=cost*3/2;	// 50% more for artifacts
		}
		if(e->type!=IT_AXE && e->magic>=EM_POISON2 && e->magic<=EM_THINKING)	// elite, 150% cost
			cost=cost*3/2;
		if(e->type!=IT_AXE && e->magic>=EM_DEFEND && e->magic<=EM_CURSED)	// cursed, 125% cost
			cost=cost*5/4;
	}
	else if(e->type>=IT_PARKA && e->type<=IT_GOLDENPARKA)
	{
		cost=e->value*e->value*e->value*3+e->speed*e->speed*e->speed;
		if(e->type==IT_MAGICPARKA)
		{
			cost*=2;	// double price for magic!
		}
		if(e->type==IT_GOLDENPARKA)
		{
			cost*=4;	// quadruple!
			if(e->magic>=EM_ARTSTART && e->magic<=EM_ARTEND)
				cost=cost*3/2;	// 50% more for artifacts
		}
		if(e->type!=IT_PARKA && e->magic>=EM_POISON2 && e->magic<=EM_THINKING)	// elite, 150% cost
			cost=cost*3/2;
		if(e->type!=IT_PARKA && e->magic>=EM_DEFEND && e->magic<=EM_CURSED)	// cursed, 125% cost
			cost=cost*5/4;
	}
	else if(e->type>=IT_AMULET && e->type<=IT_GOLDENAMULET)
	{
		cost=e->value*e->value*e->value*2+e->speed*e->speed*e->speed*2;
		if(e->type==IT_MAGICAMULET)
		{
			cost*=2;	// double price for magic!
		}
		if(e->type==IT_GOLDENAMULET)
		{
			cost*=4;	// quadruple!
			if(e->magic>=EM_ARTSTART && e->magic<=EM_ARTEND)
				cost=cost*3/2;	// 50% more for artifacts
		}

		if(e->type!=IT_AMULET && e->magic>=EM_POISON2 && e->magic<=EM_THINKING)	// elite, 150% cost
			cost=cost*3/2;
		if(e->type!=IT_AMULET && e->magic>=EM_DEFEND && e->magic<=EM_CURSED)	// cursed, 125% cost
			cost=cost*5/4;
	}
	else if(e->type>=IT_LENSA && e->type<=IT_LENSM)
	{
		cost=(e->type-IT_LENSA+1)*(e->type-IT_LENSA+1)*(e->type-IT_LENSA+1)/2;
		if(cost<10)
			cost=10;
	}
	else switch(e->type)
	{
		// junk items
		case IT_LOG:
		case IT_STUFFING:
		case IT_GEAR:
		case IT_CAN:
		case IT_SOCK:
		case IT_SEED:
		case IT_ICECUBE:
		case IT_STONE:
		case IT_ECTO:
		case IT_CLAW:
			cost=4*10;
			break;
		case IT_ROPE:
			cost=50;
			break;
		case IT_HERBA:
		case IT_HERBB:
		case IT_HERBC:
		case IT_HERBD:
		case IT_HERBE:
		case IT_HERBF:
			cost=3*10;
			break;
		case IT_SILVER:
			cost=4*10;
			break;
		case IT_GOLD:
			cost=4*30;
			break;
		case IT_RUBY:
			cost=4*50;
			break;
		case IT_EMERALD:
			cost=4*100;
			break;
		case IT_DIAMOND:
			cost=4*500;
			break;
		case IT_GLASSES:
			cost=5000;
			break;
		case IT_TOYCRYSTAL:
			cost=20;
			break;
		case IT_STOLEN:
			cost=1;
			break;
		case IT_ARTBIT:
			cost=20000;
			break;
		// quest items and anything not listed here, unsellable
		default:
			cost=0;
			return cost;
			break;
	}

	if(sell)
	{
		if(cost>0)
		{
			cost/=10;
			// miser talent here
			cost=(word)((int)cost*(100+(int)TalentBonus(TLT_MISER)+10*(player.axe.magic==EM_QUESTION))/100);
			if(cost<1)
				cost=1;
		}
	}
	else
	{
		// haggling talent here
		cost=(word)((int)cost*(100-(int)TalentBonus(TLT_HAGGLING)-10*(player.axe.magic==EM_QUESTION))/100);
	}
	return cost;
}

void ConvertEquipmentForShow(byte on)
{
	int i;

	if(on)
	{
		if(player.axe.magic>=EM_ARTSTART && player.axe.magic<=EM_ARTEND)
			player.axe.type=IT_ARTAXE;
		if(player.amulet.magic>=EM_ARTSTART && player.amulet.magic<=EM_ARTEND)
			player.amulet.type=IT_ARTAMMY;
		if(player.parka.magic>=EM_ARTSTART && player.parka.magic<=EM_ARTEND)
			player.parka.type=IT_ARTPARKA;
	}
	else
	{
		if(player.axe.magic>=EM_ARTSTART && player.axe.magic<=EM_ARTEND)
			player.axe.type=IT_GOLDENAXE;
		if(player.amulet.magic>=EM_ARTSTART && player.amulet.magic<=EM_ARTEND)
			player.amulet.type=IT_GOLDENAMULET;
		if(player.parka.magic>=EM_ARTSTART && player.parka.magic<=EM_ARTEND)
			player.parka.type=IT_GOLDENPARKA;
	}

	for(i=0;i<MAX_EQUIP;i++)
	{
		if((player.items[i].type==IT_GOLDENAXE || player.items[i].type==IT_ARTAXE) && player.items[i].magic>=EM_ARTSTART && player.items[i].magic<=EM_ARTEND)
		{
			if(on)
				player.items[i].type=IT_ARTAXE;
			else
				player.items[i].type=IT_GOLDENAXE;
		}
		if((player.items[i].type==IT_GOLDENAMULET || player.items[i].type==IT_ARTAMMY) && player.items[i].magic>=EM_ARTSTART && player.items[i].magic<=EM_ARTEND)
		{
			if(on)
				player.items[i].type=IT_ARTAMMY;
			else
				player.items[i].type=IT_GOLDENAMULET;
		}
		if((player.items[i].type==IT_GOLDENPARKA || player.items[i].type==IT_ARTPARKA) && player.items[i].magic>=EM_ARTSTART && player.items[i].magic<=EM_ARTEND)
		{
			if(on)
				player.items[i].type=IT_ARTPARKA;
			else
				player.items[i].type=IT_GOLDENPARKA;
		}
	}
}


void ConvertShopForShow(byte on,byte shop)
{
	int i;

	for(i=0;i<SHOP_SIZE;i++)
	{
		if((player.shopInv[shop][i].type==IT_GOLDENAXE || player.shopInv[shop][i].type==IT_ARTAXE) && player.shopInv[shop][i].magic>=EM_ARTSTART && player.shopInv[shop][i].magic<=EM_ARTEND)
		{
			if(on)
				player.shopInv[shop][i].type=IT_ARTAXE;
			else
				player.shopInv[shop][i].type=IT_GOLDENAXE;
		}
		if((player.shopInv[shop][i].type==IT_GOLDENAMULET || player.shopInv[shop][i].type==IT_ARTAMMY) && player.shopInv[shop][i].magic>=EM_ARTSTART && player.shopInv[shop][i].magic<=EM_ARTEND)
		{
			if(on)
				player.shopInv[shop][i].type=IT_ARTAMMY;
			else
				player.shopInv[shop][i].type=IT_GOLDENAMULET;
		}
		if((player.shopInv[shop][i].type==IT_GOLDENPARKA || player.shopInv[shop][i].type==IT_ARTPARKA) && player.shopInv[shop][i].magic>=EM_ARTSTART && player.shopInv[shop][i].magic<=EM_ARTEND)
		{
			if(on)
				player.shopInv[shop][i].type=IT_ARTPARKA;
			else
				player.shopInv[shop][i].type=IT_GOLDENPARKA;
		}
	}
}

byte AllPotionCheck(void)
{
#ifdef DIRECTORS
	int i;
	byte have[20],b;

	for(i=0;i<20;i++)
		have[i]=0;

	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].type>=IT_POT1 && player.items[i].type<=IT_POT20)
		{
			have[player.items[i].type-IT_POT1]=1;
		}
	}
	b=0;
	for(i=0;i<20;i++)
		if(have[i])
			b++;
	picToDrop=7;
	if(b==20 && opt.galPix[picToDrop-1]==0 && player.var[VAR_TEMP+15]==0)
	{
		bullet_t *b;

		b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,64,BLT_ITEM,0);
		if(b)
		{
			player.var[VAR_TEMP+15]=1;
			b->anim=IT_PICTURE;
			b->target=picToDrop;
			NewMessage("You have one of every potion!",0);
		}
	}
	picToDrop=0;
#endif
	return 1;
}

byte GenerateArtifact(void)
{
	byte m;
	byte count;
	byte type;
	int val1,val2;
	bullet_t *b;
	int tries;

	count=0;

	for(m=EM_ARTSTART;m<=EM_ARTEND;m++)
		if(HaveArtifact(m))
			count++;

	if(count==EM_ARTEND-EM_ARTSTART+1)
	{
		NewMessage("There are no more artifacts in the universe!",0);
		return 0;
	}

	tries=50000;
	while((tries--)>0)
	{
		EquipCreateItem(50,&type,&val1,&val2);
		if((type==IT_GOLDENAMULET || type==IT_GOLDENAXE || type==IT_GOLDENPARKA) &&
			(val1/65536>=EM_ARTSTART && val1/65536<=EM_ARTEND))
		{
			b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,64,BLT_ITEM,0);
			if(b)
			{
				b->anim=type;
				b->target=val1;
				b->owner=val2;
			}
			return 1;
		}
	}

	NewMessage("Couldn't quite figure it out.",0);
	return 0;
}
