#ifndef SKILLS_H
#define SKILLS_H

#include "jamulsound.h"
#include "jamultypes.h"

#define SKILL_LIFE (0)
#define SKILL_MANA (1)

#define MAX_SKILLS (25)

#define MAX_SKILL_LVL (5)

typedef struct skill_t
{
	public:
		char name[32];
		char desc[128];

} skill_t;

void DescribeSkill(byte skill, int x, int y);

#endif
