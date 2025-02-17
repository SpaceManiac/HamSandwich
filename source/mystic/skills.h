#ifndef SKILLS_H
#define SKILLS_H

#include "jamulsound.h"
#include "jamultypes.h"

// row 1
#define SKILL_LIFE (0)
#define SKILL_MANA (1)
#define SKILL_FIREBALL_CRIT (2)
#define SKILL_CRITDMG (3)
#define SKILL_GREED (4)
#define SKILL_POTIONS (5)
//row 2
#define SKILL_ENERGYRATE (6)	// energy storm upgrades
#define SKILL_ENERGYMANA (7)
#define SKILL_ENERGYBARRIER (8)
#define SKILL_FLAMEON (9)
#define SKILL_MELTARMOR (10)
#define SKILL_BACKDRAFT (11)
//row 3
#define SKILL_SEEKER (12)
#define SKILL_SEEKBOUNCE (13)
#define SKILL_SEEKBOOM (14)
#define SKILL_ICEPOWER (15)
#define SKILL_ICECRIT	(16)
#define SKILL_FREEZEMONEY 	(17)
//row 4
#define SKILL_INFERNO (18)
#define SKILL_MAYHEM  (19)
#define SKILL_SHOCKWAVE (20)
#define SKILL_SUMMON (21)
#define SKILL_HEALSUMMONS (22)
#define SKILL_DISTRACTION (23)
//row 5
#define SKILL_ARMOR (24)
#define SKILL_PARRY (25)
#define SKILL_REFLECT (26)
#define SKILL_BERSERK (27)
#define SKILL_BLOODLUST (28)
#define SKILL_BERSERKCRIT (29)
//row 6
#define SKILL_HEAL (30)
#define SKILL_RESTORATION (31)
#define SKILL_RESURRECT (32)
#define SKILL_FIMBULWINTER (33)
#define SKILL_MANAGETTIN (34)
#define SKILL_MURDALIZE (35)

#define MAX_SKILLS (36)

#define MAX_SKILL_LVL (5)

// skill display types
#define SD_NUMBER (0)
#define SD_PERCENT (1)
#define SD_SECONDS (2)
#define SD_METERS	(3)

#define MURDALIZE_RANGE (32*4)

typedef struct skill_t
{
	public:
		char name[32];
		char desc[4][64];
		float baseVal;
		float amtPerPoint;
		char statName[32];
		byte displayType;
		byte spellReq;
} skill_t;

void DescribeSkill(byte skill, int x, int y);
float SkillValue(byte skill);
extern skill_t skillList[];

#endif
