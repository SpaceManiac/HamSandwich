#include "skills.h"
#include "display.h"
#include "player.h"
#include "hammusic.h"

skill_t skillList[] = {
	{"Kid Beefy","Increase your maximum Life by 10."},
	{"Kid Magicky","I feel like this could've been named better somehow. Increase maximum Mana by 10."},
	{"Critique","When you critically hit, deal 25% extra damage (a critical hit normally deals 50% extra damage)."},
	{"Flaming Heap","Fireballs gain +5% chance to critically hit."},

};

void DescribeSkill(byte skill,int x,int y)
{
	char txt[64];
	switch (skill)
	{
		case SKILL_LIFE:
			PrintBrightGlow(x, y, "Kid Beefy", 0, 2);
			PrintBrightGlow(x, y + 20, "Increase maximum Life", 0, 1);
			PrintBrightGlow(x, y + 32, "by 10 per point.", 0, 1);
			if (player.skill[skill] > 0)
			{
				sprintf(txt, "[Current bonus: %d]", player.skill[skill] * 10);
				PrintBrightGlow(x, y + 44, txt, 0, 1);
			}
			break;
	}
}
