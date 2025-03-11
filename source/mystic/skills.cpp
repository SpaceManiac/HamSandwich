#include "skills.h"
#include <math.h>
#include "display.h"
#include "player.h"
#include "hammusic.h"
#include "spell.h"
#include "fairy.h"

skill_t skillList[] = {
	{"Kid Beefy",
		"Increase maximum Life",
		"with your beefy muscles.",
		"",
		"",
		14,10,"Life",SD_NUMBER,255},
	{"Kid Magicky",
		"I feel like this could've",
		"been named better somehow.",
		"Maximum Mana increased.",
		"",
		14,10,"Mana",SD_NUMBER,255},
	{"Kid Fiery",
		"Fireballs gain a chance to",
		"Critically Hit per point.",
		"",
		"",
		0,5,"Chance",SD_PERCENT,255},
	{"Kid Critic",
		"Boost Critical Damage by",
		"25% per point.",
		"",
		"",
		150,25,"Damage",SD_PERCENT,255},
	{"Kid Greedy",
		"Chance to get Big Coins",
		"and Runestones increased.",
		"",
		"",
		1,1,"Chance",SD_PERCENT,255},
	{"Kid Thirsty",
		"Both Life and Mana Potions",
		"give you bonus mana.",
		"Beware: This lets you drink",
		"Life potions at full Life!",
		0,5,"Mana",SD_NUMBER,255},
	// row 2
	{"Energetic Casting",
		"Energy spells cast much much",
		"faster.",
		"",
		"",
		0,30,"Bonus",SD_PERCENT,SPL_ENERGY},
	{"Energy Management",
		"Energy spells have a chance",
		"to cast for free.",
		"",
		"",
		0,10,"Chance",SD_PERCENT,SPL_ENERGY},
	{"Energy Barrier",
		"When you stop casting Energy",
		"spells, you are protected for",
		"a short time per 20 bolts fired.",
		"(1/4 time for Energy Storm)",
		0,0.1f,"Time",SD_SECONDS,SPL_ENERGY},
	{"Flame On!",
		"Flame spells are much more",
		"potent. Damage-wise and",
		"whatnot.",
		"",
		0,1,"Potency",SD_NUMBER,SPL_FLAME},
	{"Unforging",
		"Flame spells melt enemy armor,",
		"increasing damage taken from",
		"all attacks. Hit with more",
		"flames to extend duration.",
		0,7,"Damage",SD_PERCENT,SPL_FLAME},
	{"Backdraft",
		"Casting a Flame spell melts",
		"all nearby enemy bullets.",
		"",
		"",
		0,16,"Radius",SD_METERS,SPL_FLAME},
	// row 3
	{"Seek Mystique",
		"Seekers deal more damage and",
		"fire faster.",
		"",
		"",
		0,1,"Potency",SD_NUMBER,SPL_SEEKER},
	{"Seek n' Seek",
		"Seekers have a chance to",
		"bounce off of walls.",
		"",
		"",
		0,20,"Chance",SD_PERCENT,SPL_SEEKER},
	{"Seeksplosion",
		"Seekers make enemies explode",
		"on death over a large radius.",
		"",
		"",
		0,2,"Damage",SD_NUMBER,SPL_SEEKER},
	{"Ice Is Nice",
		"Ice spells freeze enemies",
		"longer by some amount that",
		"is very hard to quantify. It",
		"relates to the enemy's life.",
		100,180,"Freeze",SD_PERCENT,SPL_ICE},
	{"Shatter",
		"When hitting frozen enemies,",
		"you have increased Critical",
		"Chance with all attacks.",
		"",
		0,10,"Chance",SD_PERCENT,SPL_ICE},
	{"Frozen Assets",
		"Killing a frozen enemy makes",
		"the Coins it drops more likely",
		"to be Big Coins. Runestone",
		"chance is equally increased.",
		0,5,"Chance",SD_PERCENT,SPL_ICE},
	// row 4
	{"Explodomancy",
		"Inferno explodes more. Lots",
		"more.",
		"",
		"",
		0,1,"Exploditude",SD_NUMBER,SPL_INFERNO},
	{"Mayhem",
		"Every Inferno explosion that",
		"occurs has a chance to make",
		"a new explosion in a wildly",
		"random location.",
		0,10,"Chance",SD_PERCENT,SPL_INFERNO},
	{"Shockwave",
		"All enemies onscreen are",
		"stunned when an Inferno is",
		"cast, even if they don't get",
		"hit. It's just stunning!",
		0,0.5f,"Stun",SD_SECONDS,SPL_INFERNO},
	{"Pteromancy",
		"Summon spells summon stronger",
		"minions. Golems gain more life",
		"and damage, Pteros gain more",
		"damage, because they're tiny.",
		0,1,"Might",SD_NUMBER,SPL_SUMMON},
	{"Magic is Friendship",
		"Your Healing and Armor spells",
		"affect your summons. Summons",
		"also gain damage for 5s",
		"after this.",
		0,10,"Damage",SD_PERCENT,SPL_SUMMON},
	{"Distraction",
		"For a short time after you",
		"summon a minion, enemies will",
		"ignore you and attack them.",
		"",
		0,1,"Time",SD_SECONDS,SPL_SUMMON},
	// row 5
	{ "Warding",
		"Armor spells last much longer.",
		"",
		"",
		"",
		100,100,"Duration",SD_PERCENT,SPL_ARMOR},
	{"Perfect Peridot",
		"For 0.5s after casting an",
		"Armor spell, you are immune,",
		"and if hit during this time,",
		"some casting cost is refunded.",
		0, 25,"Refund",SD_PERCENT,SPL_ARMOR},
	{"Diamondskin",
		"When Armored, you shoot out",
		"fireballs in random directions",
		"when hit.",
		"",
		0,1,"Fireballs",SD_NUMBER,SPL_ARMOR},
	{ "Zerkomancy",
		"Berserk spells last longer.",
		"",
		"",
		"",
		2,0.5f,"Duration",SD_SECONDS,SPL_BERSERK},
	{ "Bloodlust",
		"While Berserking, you heal",
		"with each kill. You have a",
		"serious problem and need",
		"some therapy.",
		0,2,"Heal",SD_NUMBER,SPL_BERSERK},
	{ "Critical Velocity",
		"All your attacks gain Critical",
		"Chance while Berserking.",
		"",
		"",
		0,5,"Chance",SD_PERCENT,SPL_BERSERK},
	// row 6
	{ "Attunement",
		"Healing spells heal more.",
		"",
		"",
		"",
		100, 20,"Heal",SD_PERCENT,SPL_HEAL},
	{ "Restoration",
		"You always heal for 1 Life",
		"every 3 seconds, but your",
		"Healing spells now also heal",
		"over time.",
		0,5,"Max Heal/s",SD_NUMBER,SPL_HEAL},
	{ "Lifesaver",
		"Once per level, you will return",
		"from death. You come back",
		"with a portion of your full Life",
		"and Mana.",
		0,15,"Life/Mana",SD_PERCENT,SPL_HEAL},
	{ "Fimbulwinter",
		"Armageddon comets no longer",
		"deal damage, they freeze",
		"enemies.",
		"",
		0,180,"Chilliness",SD_PERCENT,SPL_ARMAGEDDON},
	{ "Mana Gettin'",
		"Armageddon drops fewer",
		"comets, but it grants you",
		"unlimited mana for its duration.",
		"No recasting, smartypants!",
		0,10,"Comets",SD_PERCENT,SPL_ARMAGEDDON},
	{ "Murdalize",
		"When a close enemy is at low",
		"Life, press [Next+Previous]",
		"to instantly destroy it!",
		"",
		0,3,"Life",SD_PERCENT,SPL_ARMAGEDDON},
};

void DescribeSkill(byte skill,int x,int y)
{
	char txt[64];

	if (skillList[skill].spellReq != 255 && player.spell[skillList[skill].spellReq] == 0)
	{
		PrintBrightGlow(x, y, "Unknown!", 10, 2);
		PrintBrightGlow(x, y + 20, "You need to find a new", 0, 1);
		PrintBrightGlow(x, y + 20+12, "spell to learn this skill.", 0, 1);
		return;
	}
	PrintBrightGlow(x, y, skillList[skill].name, 10, 2);
	int lines;
	for (lines = 0; lines < 4; lines++)
		PrintBrightGlow(x, y + 20 + 12 * lines, skillList[skill].desc[lines], 0, 1);
	float total = SkillValue(skill);
	if (skillList[skill].displayType == SD_METERS)
		total = total / 32.0f;

	if(total-floorf(total)<0.1f)	// if it's nearly an integer, let's just show an integer
		sprintf(txt, "[Rank %d/%d, %s: %d", player.skill[skill], MAX_SKILL_LVL, skillList[skill].statName, (int)total);
	else
		sprintf(txt, "[Rank %d/%d, %s: %0.1f", player.skill[skill], MAX_SKILL_LVL, skillList[skill].statName, total);
	if (skillList[skill].displayType == SD_PERCENT)
		strcat(txt, "%");
	else if (skillList[skill].displayType == SD_SECONDS)
		strcat(txt, "s");
	else if (skillList[skill].displayType == SD_METERS)
		strcat(txt, "m");

	if (player.skill[skill] < MAX_SKILL_LVL)
	{
		char txt2[30];
		player.skill[skill]++;
		float total = SkillValue(skill);
		player.skill[skill]--;
		if (skillList[skill].displayType == SD_METERS)
			total = total / 32.0f;
		char symbol = '0';

		if (total - floorf(total) < 0.1f)	// if it's nearly an integer, let's just show an integer
			sprintf(txt2, " -> %d", (int)total);
		else
			sprintf(txt2, " -> %0.1f", total);
		if (skillList[skill].displayType == SD_PERCENT)
			strcat(txt2, "%");
		else if (skillList[skill].displayType == SD_SECONDS)
			strcat(txt2, "s");
		else if (skillList[skill].displayType == SD_METERS)
			strcat(txt2, "m");
		strcat(txt, txt2);
	}
	strcat(txt, "]");
	PrintBrightGlow(x, y + 20+12*4, txt, 0, 1);
}

float SkillValue(byte skill)
{
	if (player.fairyOn == FAIRY_CASTY &&
		(skill == SKILL_ENERGYRATE || skill == SKILL_FLAMEON || skill == SKILL_SEEKER ||
			skill == SKILL_INFERNO || skill == SKILL_ICEPOWER || skill == SKILL_BERSERK ||
			skill == SKILL_SUMMON || skill==SKILL_ARMOR || skill==SKILL_HEAL))	// 20% boost to skill value, for the skills that are equivalent to spell levels
	{
		return (float)player.skill[skill] * (skillList[skill].amtPerPoint*1.2f) + skillList[skill].baseVal;
	}
	else
		return (float)player.skill[skill] * skillList[skill].amtPerPoint + skillList[skill].baseVal;
}

void ResetSkills(void)
{
	for (int i = 0; i < 36; i++)
	{
		player.skillPts += player.skill[i];
		player.skill[i] = 0;
	}
}
