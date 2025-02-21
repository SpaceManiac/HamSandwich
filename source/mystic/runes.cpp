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
		"I dunno",
		{"didn't make this yet",
		 ""},
		{10,25,50},
		"Huh",
		SD_PERCENT,
	},
	{
		"I dunno",
		{"didn't make this yet",
		 ""},
		{10,25,50},
		"Huh",
		SD_PERCENT,
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
		"Siko Wiko",
		{"I dunno yet",
		 ""},
		{10,25,50},
		"Weaken",
		SD_PERCENT,
	},
	{
		"Isus Coldus",
		{"didn't make this yet",
		 ""},
		{10,25,50},
		"Huh",
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
		{"Each Inferno bomb has a chance to",
		 "drop a mine that arms in 2s."},
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
		"Armor-Up",
		{"didn't make this yet",
		 ""},
		{10,25,50},
		"Huh",
		SD_PERCENT,
	},
	{
		"Armor-Up2",
		{"didn't make this yet",
		 ""},
		{10,25,50},
		"Huh",
		SD_PERCENT,
	},
	{
		"Berz-Up",
		{"didn't make this yet",
		 ""},
		{10,25,50},
		"Huh",
		SD_PERCENT,
	},
	{
		"Berz-Up2",
		{"didn't make this yet",
		 ""},
		{10,25,50},
		"Huh",
		SD_PERCENT,
	},
	{
		"Heal-Up",
		{"didn't make this yet",
		 ""},
		{10,25,50},
		"Huh",
		SD_PERCENT,
	},
	{
		"Heal-Up2",
		{"didn't make this yet",
		 ""},
		{10,25,50},
		"Huh",
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

		float total = runeDef[(int)r].value[RuneLevel(r) - RUNE_RANK1];
		if (runeDef[(int)r].numberType == SD_METERS)
			total = total / 32.0f;
		char symbol = '0';

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
		PrintBrightGlow(x, y + 20 + 12*2, txt, 0, 1);

		if (RuneLevel(r) == RUNE_RANK1)
			PrintBrightGlow(x, y + 20 + 12 * 4, "Upgrade for 50 Runestones.", 0, 1);
		else if (RuneLevel(r) == RUNE_RANK2)
			PrintBrightGlow(x, y + 20 + 12 * 4, "Upgrade for 100 Runestones.", 0, 1);
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
