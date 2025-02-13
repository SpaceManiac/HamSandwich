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
//row 2
#define SKILL_ENERGYRATE (5)	// energy storm upgrades
#define SKILL_ENERGYMANA (6)
#define SKILL_ENERGYBARRIER (7)
//row 3
//row 4
//row 5

#define MAX_SKILLS (25)

#define MAX_SKILL_LVL (5)

// skill display types
#define SD_NUMBER (0)
#define SD_PERCENT (1)
#define SD_SECONDS (2)

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
