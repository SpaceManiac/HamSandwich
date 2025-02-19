#include "skills.h"
#include "display.h"
#include "player.h"
#include "hammusic.h"
#include "spell.h"

char runeName[(int)Rune::NUM_RUNES][32] = {
	"Protecto",
	"Pyrus Ballio",
	"Lyfus Uppus",
	"Magikus Uppus",
	"NEW1",
	"NEW2",

	"Energo Zappo",
	"Energo Cheepus",

	"Scorchus Burnio",
	"Trio Inflammo",

	"Seekius",
	"Siko Neato",

	"Isus Coldus",
	"Icio Argento",

	"Infernus Critikus",
	"Inferneri Jumbo",

	"sum1",
	"sum2",

	"armor1",
	"armro2",

	"berz1",
	"berz2",

	"heal1",
	"heal2"
};
/*
RUNE_DEFENSE,	// damage reduction 10/25/50%
RUNE_FIREBALLS,	// fireballs 10/25/50% damage
RUNE_RECOVER,	// heal 1 life per 5/3/1 second
RUNE_MANA,		// recover 1 mana per 5/3/1s 
RUNE_NEW1,
RUNE_NEW2,

RUNE_ENERGY,	// energy spells 5/15/25% crit chance
RUNE_ENERGY2,	// energy spell 10/25/50% chance of free cost

RUNE_FLAMEDMG,	// flames 5/15/25% crit chance
RUNE_FLAME3,	// flames go out sideways too

RUNE_SEEKER,	// seekers 5/15/25% crit chance
RUNE_SEEKER2,

RUNE_ICECOST,	// ice spells cost 10/25/50% less
RUNE_ICE2,

RUNE_INFERNO,	// inferno 5/15/25% crit chance
RUNE_INFERNO2,

RUNE_SUMMON,	// summoned guys have 5/15/25% crit chance
RUNE_PALS,		// 25/50/100% chance to summon a ptero along with what you intend to summon

RUNE_ARMOR,		// armor spells cost 10/25/50% less
RUNE_ARMOR2,

RUNE_BERSERK,	// berserk bonus
RUNE_BERSERK2,	// other

RUNE_HEAL,		// casting heal buffs your all damage by 5/15/25% for 10s
RUNE_HEAL2,
*/


void DescribeRune(Rune r,int x,int y)
{
	char txt[64];

/*	if (skillList[skill].spellReq != 255 && player.spell[skillList[skill].spellReq] == 0)
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
	char symbol='0';
	
	if(total-floorf(total)<0.1f)	// if it's nearly an integer, let's just show an integer
		sprintf(txt, "[Rank %d/%d, %s: %d", player.skill[skill], MAX_SKILL_LVL, skillList[skill].statName, (int)total);
	else
		sprintf(txt, "[Rank %d/%d, %s: %0.1f", player.skill[skill], MAX_SKILL_LVL, skillList[skill].statName, total);
	if (skillList[skill].displayType == SD_PERCENT)
		strcat(txt, "%]");
	else if (skillList[skill].displayType == SD_SECONDS)
		strcat(txt, "s]");
	else if (skillList[skill].displayType == SD_METERS)
		strcat(txt, "m]");
	else
		strcat(txt, "]");

	PrintBrightGlow(x, y + 20+12*4, txt, 0, 1);*/
}
