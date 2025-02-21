#ifndef RUNES_H
#define RUNES_H

#include "jamulsound.h"
#include "jamultypes.h"

enum class Rune :byte
{
	BLOCK,	// chance to not get hit 10/20/40
	FIREBALLS,	// fireballs 10/25/50% damage
	RECOVER,	// heal 1 life per 5/3/1 second
	MANA,		// regain 1 mana per 5/3/1s
	NEW1,
	NEW2,

	ENERGY,	// energy spells 5/15/25% crit chance
	ENERGY2,	// energy spell 10/25/50% chance of free cost

	FLAMEDMG,	// flames 5/15/25% crit chance
	FLAME3,	// flames go out sideways too

	SEEKER,	// seekers 5/15/25% crit chance
	SEEKER2, //

	ICECOST,	// ice spells cost 10/25/50% less
	FROSTBITE,

	INFERNO,	// inferno 5/15/25% crit chance
	INFERNO2,

	SUMMON,	// summoned guys have 5/15/25% crit chance
	PALS,		// 25/50/100% chance to summon a ptero along with what you intend to summon

	ARMOR,		// armor spells cost 10/25/50% less
	ARMOR2,

	BERSERK,	// berserk bonus
	BERSERK2,	// other

	HEAL,		// casting heal buffs your all damage by 5/15/25% for 10s
	HEAL2,

	NUM_RUNES,
};

typedef struct RuneDef
{
	public:
		char name[32];
		char desc[2][64];
		float value[3];
		char numDesc[32];
		byte numberType;
} RuneDef;

// rune status values
#define RUNE_EMPTY (0)
#define RUNE_ASLEEP (1)
#define RUNE_RANK1 (2)
#define RUNE_RANK2 (3)
#define RUNE_RANK3 (4)
#define RUNE_MASK	(7)
#define RUNE_EQUIPPED (8)

void DescribeRune(Rune r, int x, int y);

const char* RuneName(Rune r);
int RuneValue(Rune r);

byte RuneLevel(Rune r);
bool RuneEquipped(Rune r);
byte RunesEquipped(void);

#endif
