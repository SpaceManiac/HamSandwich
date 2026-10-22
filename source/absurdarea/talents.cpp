#include "talents.h"
#include "player.h"
#include "quest.h"
#include "glasses.h"
#include "help.h"
#include "achieve.h"

talent_t talents[MAX_TALENTS]={
	// combat talents
	{"Chopping",
		{"Learn to strike enemies in the less protected parts.  Your",
		 "swinging attacks penetrate 3% of enemy armor per level.",
		 "",
		 ""},
		 3.0f,
		{100,200,300,400,500,600,700,800,900,2000},	// enemies killed by melee axe
	},
	{"Tossing",
		{"Build up your throwing arm!  As this talent grows, you'll",
		 "knock enemies back further with thrown axes.",
		 "",
		 ""},
		 1.5f,
		{100,200,300,400,500,600,700,800,900,2000},	// enemies killed by throwing axe
	},
	{"Stoneskin",
		{"Enemies beating on you?  Meh, you get used to it.",
		 "Increase your Defense by 3% per level.",
		 "",
		 ""},
		 3.0f,
		{100,200,300,400,500,600,700,800,900,2000},	// damage taken
	},
	{"Criticism",
		{"Improve your critical strikes - they penetrate 5% of",
		 "enemy armor per level.",
		 "",
		 ""},
		 5.0f,
		{50,60,70,80,90,100,110,120,130,200},	// critical hits obtained
	},
	{"Vivacity",
		{"Reduce the Stamina costs of all actions by 3% per level.",
		 "Can't beat that with a stick!  But if you try, it won't be",
		 "as tiring as it was.",
		 ""},
		 3.0f,
		{1000,1200,1400,1600,1800,2000,2200,2400,2600,4000},	// stamina used
	},
	{"Sneak Attack",
		{"Smash unsuspecting foes.  They're monsters, they deserve",
		 "it.  Adds 1 Damage per level to attacks against uninjured",
		 "monsters.",
		 ""},
		 1.0f,
		{100,200,300,400,500,600,700,800,900,2000}, // enemies killed in one axe hit
	},
	{"Knockout",
		{"Find just the right soft spot on a monster's head, and he",
		 "goes down like a sack of potatoes.  Stun enemies 3%",
		 "longer per level.",
		 ""},
		 3.0f,
		{30*30,90*30,150*30,210*30,270*30,330*30,390*30,450*30,510*30,700*30}, // (time enemies have spent stunned)
	},
	{"Finesse",
		{"You have a knack for not getting hit that simply",
		 "confounds enemies.  Stun enemies for 1/8 second per",
		 "level each time you parry, dodge, or deflect one of their",
		 "melee attacks."},
		 30.0f/8.0f,
		{50,60,70,80,90,100,110,120,130,200},	// hits parried, dodged, or deflected (Stone Parka)
	},
	{"Scavenging",
		{"Let's not discuss how you are gaining Life from each",
		 "monster you slay.  Maybe they're carrying crackers in",
		 "their pockets.  Could be!  10% chance per level to",
		 "recover 1 Life when you beat a monster."},
		 10.0f,
		{50,100,150,200,250,300,350,400,450,1000},	// items dropped by enemies
	},
	{"Gladiator",
		{"Prove your skills at fightin' round the world!  This talent",
		 "has one effect: it shows Tina at the battle arena just",
		 "how tough you are, so she'll let you enter harder",
		 "matches."},
		 1.0f,
		{5,10,15,20,25,30,35,40,45,50},	// battles won in the arena (N points per battle, N=rank)
	},
	// magic talents
	{"Mental Focus",
		{"As you cast spells, you obtain a clarity of mind that",
		 "greatly improves your ability to blow things up.  Adds 3%",
		 "damage per level to offensive spells.",
		 ""},
		 3.0f,
		{100,200,300,400,500,600,700,800,900,2000},	// kills by magic
	},
	{"Combustion",
		{"Boy, you really like fire, don't you?  This talent slightly",
		 "increases the rate at which fire does damage to enemies.",
		 "",
		 ""},
		 1.0f,
		{300,600,900,1200,1500,1800,2100,2400,2700,5000},	// damage done via fire or explosions
	},
	{"Freezing",
		{"Icy glare, cold shoulder, assorted other ice puns. Frozen",
		 "enemies take damage over time, more rapidly with more",
		 "levels.",
		 ""},
		 8.0f,
		{100*30,200*30,300*30,400*30,500*30,600*30,700*30,800*30,900*30,2000*30},	// seconds enemies have spent frozen
	},
	{"Arsenic",
		{"Over time, you have developed a discerning eye for the",
		 "best toxins.  Decreases the armor of poisoned enemies by",
		 "5% per level.",
		 ""},
		 5.0f,
		{100*30,200*30,300*30,400*30,500*30,600*30,700*30,800*30,900*30,2000*30},	// seconds enemies spent poisoned
	},
	{"Electrocution",
		{"Lightning never strikes twice in the same place, but with",
		 "this talent, it will come closer to doing so.  Increases the",
		 "rate at which all Lightning abilities zap, by 5% per level.",
		 ""},
		 5.0f,
		{300,600,900,1200,1500,1800,2100,2400,2700,5000},	// damage done by lightning
	},
	{"Meditation",
		{"You are able to maintain your calm even when Ice Mice",
		 "are trying to drive you insane.  Regenerate Magic faster",
		 "when standing still.",
		 ""},
		 2.0f,
		{100,200,300,400,500,600,700,800,900,2000},	// magic used
	},
	{"Spookiness",
		{"Be careful with this dark magic stuff, it can be",
		 "dangerous! Enemies slain by Death Magic create Ghosts",
		 "that live for 2 seconds per level, attacking enemies.",
		 ""},
		 2.0f,
		{2000,3000,4000,5000,6000,7000,8000,9000,10000,20000},	// kills by death magic (1 point per level of killed guy)
	},
	{"Green Thumb",
		{"Get in touch with nature.  Heal 1 Life for every 2 levels,",
		 "each time you cast a Nature spell.",
		 "",
		 ""},
		 0.5f,
		{50,70,90,110,130,150,170,190,210,400},	// nature spells cast
	},
	{"Scorching",
		{"Casting too much fire magic seems to build up an internal",
		 "fire of your own.  Each time you cast a Fire spell, 1",
		 "fireball per level will shoot off in a random direction.",
		 ""},
		 1.0f,
		{100,200,300,400,500,600,700,800,900,2000},	// kills by fire magic
	},
	{"Windbag",
		{"It's not a flattering name for a talent, but it's handy",
		 "anyway.  Wind spells cost 3% less Magic per level.",
		 "",
		 ""},
		 3.0f,
		{50,70,90,110,130,150,170,190,210,400},	// wind spells cast
	},
	// craft talents
	{"Mobility",
		{"Go outside!  Exercise your leg muscles!  This will gradually",
		 "increase your movement speed.",
		 "",
		 ""},
		 (FIXAMT*3.0f)/10.0f,
		{4000,6000,8000,10000,12000,14000,16000,18000,20000,32000},	// tiles traversed
	},
	{"Alchemy",
		{"Use this talent to mix your own potions from herbs you",
		 "find!  Higher level improves your odds of success, and",
		 "results in higher quality potions.  You'll need a Cauldron",
		 "to do your mixing in."},
		 1.0f,
		{10,15,20,25,30,35,40,45,50,100},	// potions brewed
	},
	{"Junksmith",
		{"There was a famed Scottish inventor named MacGiver who",
		 "was known for his ability to take seemingly random junk",
		 "and make it into useful items.  Just wanted to share",
		 "that story.  Requires an Oven to work at."},
		 2.0f,
		{10,15,20,25,30,35,40,45,50,100},	// items crafted
	},
	{"Clockwork",
		{"It gets lonely out in the snow.  Build yourself a friend",
		 "out of the junk you find!  The friend can be made from",
		 "5 items, plus 1 per level.  Different items have different",
		 "effects.  Can only be done at a Clock."},
		 1.5f,
		{10,20,30,40,50,60,70,80,90,200},	// kills by clockwork buddy
	},
	{"Mining",
		{"There's good stuff beneath the ground, and you seem to",
		 "be naturally attracted to it!  Odds of finding things when",
		 "digging increase with each level.",
		 ""},
		 2.0f,
		{10,13,16,19,22,25,28,31,34,50},	// items dug up
	},
	{"Haggling",
		{"You are frugal to your very bones.  You pinch pennies so",
		 "hard, you can sell them as souvenirs (and you do!).  Buy",
		 "things 3% cheaper per level.",
		 ""},
		 3.0f,
		{2000,4000,6000,8000,10000,12000,14000,16000,18000,30000},	// money spent shopping
	},
	{"Miser",
		{"You are unwilling to part with even the most crass of",
		 "garbage... except for a good price.  Earn 3% more per",
		 "level when you sell items.",
		 ""},
		 3.0f,
		{100,200,300,400,500,600,700,800,900,2000},	// coins collected off the ground
	},
	{"Mixology",
		{"Soula Cola with a twist of lemon?  A Brain Juice and",
		 "Tonic?  Shaken, not stirred.  Potions you drink have a 3%",
		 "bigger effect per level.",
		 ""},
		 3.0f,
		{30,50,70,90,110,130,150,170,190,300},	// potions drank
	},
	{"Packrat",
		{"There's no junk you will pass over in your collecting",
		 "frenzy.  Increases odds of finding junk items by 2% per",
		 "level.",
		 ""},
		 2.0f,
		{50,70,90,110,130,150,170,190,210,500},	// junk collected
	},
	{"Herbalism",
		{"In your travels, you've collected so many herbs that you",
		 "are able to spot even the scraggly ones at a glance.",
		 "Increases odds of finding herbs.",
		 ""},
		 0.25f,
		{50,60,70,80,90,100,110,120,130,300},	// herbs collected
	},
};

void InitialTalents(void)
{
	int i;

	for(i=0;i<MAX_TALENTS;i++)
	{
		player.talentPts[i]=0;	// no points earned yet (and when it is 0 and level is 255, you don't know the skill at all)
		player.talentLevel[i]=255;	// and no level
		if(ModifierOn(MOD_EXPERT))
			player.talentLevel[i]=0;
		if(ModifierOn(MOD_TICKING) && i==TLT_CLOCKWORK)
			player.talentLevel[i]=0;
	}
}

char *TalentName(byte n)
{
	static char huh[]="????";

	if(n>MAX_TALENTS)
		return huh;
	if(player.talentLevel[n]!=255)
		return talents[n].name;

	return huh;
}

talent_t *GetTalent(byte n)
{
	if(n>MAX_TALENTS)
		return NULL;
	return &talents[n];
}

word TalentPointsNeeded(byte n)
{
	if(player.talentLevel[n]==255) // don't have this talent
		return 60000;
	if(player.talentLevel[n]==10)
		return 60000;
	return talents[n].ptsNeeded[player.talentLevel[n]];
}

void LearnTalent(byte s)
{
	char txt[64];

	if(player.talentLevel[s]==255)
	{
		player.talentLevel[s]=0;
		player.talentPts[s]=0;
		sprintf(txt,"Learned %s Talent!",TalentName(s));
		NewMessage(txt,0);
		EnterNewSkill(s+100);
		QuestReportEvent(QE_TALENT,s);
		if(s==TLT_JUNKSMITH)
			TipEvent(TIP_JUNKSMITH);
		if(s==TLT_CLOCKWORK)
			TipEvent(TIP_CLOCKWORK);
		return;
	}
}

void TalentPoint(byte which,word amt)
{
	char txt[64];
	byte lv;

	if(player.talentLevel[which]>=0 && player.talentLevel[which]<10)
	{
		player.talentPts[which]+=amt;
		while(player.talentPts[which]>=talents[which].ptsNeeded[player.talentLevel[which]] && player.talentLevel[which]<10)
		{
			player.talentPts[which]-=talents[which].ptsNeeded[player.talentLevel[which]];
			player.talentLevel[which]++;
			lv=player.talentLevel[which];
			if(GlassEffect()==GE_TALENT)
			{
				if(GlassAmount()==3)
					lv++;
				else if(GlassAmount()==0 && which>=TLT_CHOPPING && which<=TLT_GLADIATOR)
					lv++;
				else if(GlassAmount()==1 && which>=TLT_FOCUS && which<=TLT_WINDBAG)
					lv++;
				else if(GlassAmount()==2 && which>=TLT_MOBILITY && which<=TLT_HERBALISM)
					lv++;
			}
			if(lv>10)
				lv=10;
			sprintf(txt,"%s Talent is now level %d!",TalentName(which),lv);
			NewMessage(txt,0);
			MakeNormalSound(SND_TALENTUP);
			if(goodguy)
			{
				FXRing(6,goodguy->x,goodguy->y,goodguy->z,10,1);
				FXRing(6,goodguy->x,goodguy->y,goodguy->z,10,2);
			}
		}
	}
}

float TalentBonus(byte which)
{
	float lv;

	if(player.talentLevel[which]==255)
		return 0;

	lv=(float)player.talentLevel[which];
	if(GlassEffect()==GE_TALENT)
	{
		if(GlassAmount()==3)
			lv+=1.0f;
		else if(GlassAmount()==0 && which>=TLT_CHOPPING && which<=TLT_GLADIATOR)
			lv+=1.0f;
		else if(GlassAmount()==1 && which>=TLT_FOCUS && which<=TLT_WINDBAG)
			lv+=1.0f;
		else if(GlassAmount()==2 && which>=TLT_MOBILITY && which<=TLT_HERBALISM)
			lv+=1.0f;
	}
	if(lv>10.0f)
		lv=10.0f;

	return (talents[which].powerPerLev*lv);
}

// when you die, all talents lose 10% of their current level
void TalentDecrease(void)
{
	int i;

	for(i=0;i<MAX_TALENTS;i++)
	{
		if(player.talentLevel[i]!=255)
		{
			if(player.talentPts[i]>=(talents[i].ptsNeeded[player.talentLevel[i]]/10))
				player.talentPts[i]-=talents[i].ptsNeeded[player.talentLevel[i]]/10;
			else
				player.talentPts[i]=0;
		}
	}
}
