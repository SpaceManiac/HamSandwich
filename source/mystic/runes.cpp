#include "skills.h"
#include "display.h"
#include "player.h"
#include "hammusic.h"
#include "spell.h"

RuneDef runeDef[] = {
	{
		"Protecto",
		{"A chance of resisting any",
		 "damage taken."},
		{10,20,40},
		"Block Chance",
		SD_PERCENT,
	},
	{
		"Pyrus Ballio",
		{"Fireballs deal more damage.",
		 ""},
		{10,25,50},
		"Damage",
		SD_PERCENT,
	},
	{
		"Lyfus Uppus",
		{"Heal 1 Life every few seconds.",
		 ""},
		{5,3,1},
		"Delay",
		SD_SECONDS,
	},
	{
		"Magikus Uppus",
		{"Recover 1 Mana every few",
		 "seconds."},
		{5,3,1},
		"Delay",
		SD_SECONDS,
	},
	{
		"Shottus Gunis",
		{"Fireball fire rate, damage,",
		 "and range reduced, but fire",
		 "a lot more fireballs."},
		{3,6,10},
		"Fireballs",
		SD_NUMBER,
	},
	{
		"Manos Mortis",
		{"Killing enemies grants mana.",
		 ""},
		{1,2,4},
		"Mana",
		SD_NUMBER,
	},
	{
		"Energo Zappo",
		{"Energy spells gain Critical",
		 "Chance."},
		{5,15,25},
		"Chance",
		SD_PERCENT,
	},
	{
		"Energo Cheepus",
		{"Energy spells have a chance",
		 "to cast for free."},
		{10,25,50},
		"Chance",
		SD_PERCENT,
	},
	/*{
		"Energo Cheepus",
		{"Energy spells deal more",
		 "damage by consuming your",
		 "Shield or Barrier."},
		{200,300,500},
		"Damage",
		SD_PERCENT,
	},*/
	{
		"Scorchus Burnio",
		{"Flame spells gain Critical",
		 "Chance."},
		{5,15,25},
		"Chance",
		SD_PERCENT,
	},
	{
		"Trio Inflammo",
		{"After a number of Flame Spells,",
		 "the next one fires 3 ways."},
		{5,3,1},
		"Casts",
		SD_NUMBER,
	},
	{
		"Seekius",
		{"Seeker spells gain Critical",
		 "Chance."},
		{5,15,25},
		"Chance",
		SD_PERCENT,
	},
	{
		"Otto Siko",
		{"Casting non-seeker spells",
		 "stores mana. Store enough",
		 "and a Seeker casts itself."},
		{10,25,50},
		"Mana Stored",
		SD_PERCENT,
	},
	{
		"Isus Coldus",
		{"Thawing an enemy spews ice",
		 "shards for a portion of the",
		 "damage taken while frozen."},
		{25,50,100},
		"Damage",
		SD_PERCENT,
	},
	{
		"Icio Chompus",
		{"Frozen enemies suffer frostbite",
		 "damage each second."},
		{2,4,6},
		"Damage",
		SD_NUMBER,
	},
	{
		"Infernus Critikus",
		{"Inferno spells gain Critical",
		 "Chance."},
		{5,15,25},
		"Chance",
		SD_PERCENT,
	},
	{
		"Inferneri Minus",
		{"Each Inferno bomb has a",
		 "chance to drop a mine that",
		 "arms in 2s."},
		{20,40,60},
		"Chance",
		SD_PERCENT,
	},
	{
		"Pterus Golemus",
		{"Summoned minions gain Critical",
		 "Chance."},
		{5,15,25},
		"Chance",
		SD_PERCENT,
	},
	{
		"Palio Dactyli",
		{"Chance to summon a free Ptero",
		 "anytime you summon something."},
		{25,50,100},
		"Chance",
		SD_PERCENT,
	},
	{
		"Armis Harmus",
		{"Boost your damage dealt when",
		 "protected by an Armor spell."},
		{5,15,25},
		"Damage",
		SD_PERCENT,
	},
	{
		"Claustro Philis",
		{"Armor spells also grant you",
		 "a Barrier. Steelskin grants",
		 "twice this amount."},
		{5,15,25},
		"Barrier",
		SD_NUMBER,
	},
	{
		"Zerko Killus",
		{"Kills while Berserk refill",
		 "its duration, up to",
		 "double normal duration."},
		{0.25f,0.5f,1},
		"Gain",
		SD_SECONDS,
	},
	{
		"Burnus Longus",
		{"Insane Rage's flames last",
		 "longer.",
		 ""},
		{150,200,300},
		"Duration",
		SD_PERCENT,
	},
	{
		"Helius Bario",
		{"Heal spells that overheal",
		 "grant you a barrier, up to",
		 "a limit."},
		{20,50,100},
		"Limit",
		SD_NUMBER,
	},
	{
		"Freebius Lyfus",
		{"Casting a non-Heal spell lowers",
		 "the cost of the next Heal spell",
		 "by a % of the mana spent."},
		{10,25,50},
		"Mana Stored",
		SD_PERCENT,
	},
};

const char* RuneName(Rune r)
{
	if (r < Rune::NUM_RUNES)
		return runeDef[(int)r].name;
	else
		return "????";
}

int RuneValue(Rune r)
{
	if (!RuneEquipped(r))
		return 0;
	return runeDef[(int)r].value[RuneLevel(r) - RUNE_RANK1];
}

void DescribeRune(Rune r,int x,int y)
{
	char txt[64];

	if (RuneLevel(r) == RUNE_EMPTY)
	{
		PrintBrightGlow(x, y, "Unknown!", 10, 2);
		PrintBrightGlow(x, y + 20, "You haven't found this", 0, 1);
		PrintBrightGlow(x, y + 20 + 12, "Rune yet! Keep looking.", 0, 1);
	}
	else if (RuneLevel(r) == RUNE_ASLEEP)
	{
		sprintf(txt, "Sleeping %s", RuneName(r));
		PrintBrightGlow(x, y, txt, 10, 2);
		PrintBrightGlow(x, y + 20, "You must awaken this rune", 0, 1);
		PrintBrightGlow(x, y + 20 + 12, "with Runestones to see", 0, 1);
		PrintBrightGlow(x, y + 20 + 12*2, "its power.", 0, 1);
		PrintBrightGlow(x, y + 20 + 12 * 4, "Awaken for 10 Runestones.", 0, 1);
	}
	else
	{
		if (RuneLevel(r) == RUNE_RANK1)
			sprintf(txt, "%s", RuneName(r));
		else if (RuneLevel(r) == RUNE_RANK2)
			sprintf(txt, "Gilded %s", RuneName(r));
		else if (RuneLevel(r) == RUNE_RANK3)
			sprintf(txt, "Wild %s", RuneName(r));
		PrintBrightGlow(x, y, txt, 10, 2);

		PrintBrightGlow(x, y + 20, runeDef[(int)r].desc[0], 0, 1);
		PrintBrightGlow(x, y + 20 + 12, runeDef[(int)r].desc[1], 0, 1);
		PrintBrightGlow(x, y + 20 + 12*2, runeDef[(int)r].desc[2], 0, 1);

		float total = runeDef[(int)r].value[RuneLevel(r) - RUNE_RANK1];
		if (runeDef[(int)r].numberType == SD_METERS)
			total = total / 32.0f;
		
		if (RuneLevel(r) < RUNE_RANK3)	// gotta show what's next
		{
			if (total - floorf(total) < 0.1f)	// if it's nearly an integer, let's just show an integer
				sprintf(txt, "[%s: %d", runeDef[(int)r].numDesc, (int)total);
			else
				sprintf(txt, "[%s: %0.1f", runeDef[(int)r].numDesc, total);
			if (runeDef[(int)r].numberType == SD_PERCENT)
				strcat(txt, "%");
			else if (runeDef[(int)r].numberType == SD_SECONDS)
				strcat(txt, "s");
			else if (runeDef[(int)r].numberType == SD_METERS)
				strcat(txt, "m");

			float total = runeDef[(int)r].value[RuneLevel(r)+1 - RUNE_RANK1];
			if (runeDef[(int)r].numberType == SD_METERS)
				total = total / 32.0f;
			strcat(txt, " -> ");
			char txt2[36];
			if (total - floorf(total) < 0.1f)	// if it's nearly an integer, let's just show an integer
				sprintf(txt2, "%d", (int)total);
			else
				sprintf(txt2, "%0.1f", total);
			if (runeDef[(int)r].numberType == SD_PERCENT)
				strcat(txt2, "%]");
			else if (runeDef[(int)r].numberType == SD_SECONDS)
				strcat(txt2, "s]");
			else if (runeDef[(int)r].numberType == SD_METERS)
				strcat(txt2, "m]");
			else
				strcat(txt2, "]");
			strcat(txt, txt2);
			PrintBrightGlow(x, y + 20 + 12 * 3, txt, 0, 1);
		}
		else
		{
			if (total - floorf(total) < 0.1f)	// if it's nearly an integer, let's just show an integer
				sprintf(txt, "[%s: %d", runeDef[(int)r].numDesc, (int)total);
			else
				sprintf(txt, "[%s: %0.1f", runeDef[(int)r].numDesc, total);
			if (runeDef[(int)r].numberType == SD_PERCENT)
				strcat(txt, "%]");
			else if (runeDef[(int)r].numberType == SD_SECONDS)
				strcat(txt, "s]");
			else if (runeDef[(int)r].numberType == SD_METERS)
				strcat(txt, "m]");
			else
				strcat(txt, "]");
			PrintBrightGlow(x, y + 20 + 12 * 3, txt, 0, 1);
		}

		if (RuneLevel(r) == RUNE_RANK1)
			PrintBrightGlow(x, y + 20 + 12 * 4, "Upgrade for 20 Runestones.", 0, 1);
		else if (RuneLevel(r) == RUNE_RANK2)
			PrintBrightGlow(x, y + 20 + 12 * 4, "Upgrade for 30 Runestones.", 0, 1);
		// rank 3, there's nothing more to do
	}
}

byte RuneLevel(Rune r)
{
	return (player.runes[(int)r] & RUNE_MASK);
}

bool RuneEquipped(Rune r)
{
	return (player.runes[(int)r] & RUNE_EQUIPPED);
}

byte RunesEquipped(void)
{
	byte total = 0;
	for (int i = 0; i < (int)Rune::NUM_RUNES; i++)
		if (RuneEquipped((Rune)i))
			total++;
	return total;
}
