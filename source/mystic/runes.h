#ifndef RUNES_H
#define RUNES_H

#include "jamulsound.h"
#include "jamultypes.h"

enum class Rune :byte
{
	RUNE_DEFENSE,	// damage reduction 10/25/50%
	RUNE_FIREBALLS,	// fireballs 10/25/50% damage
	RUNE_RECOVER,	// heal 1 life per 5/3/1 second
	RUNE_MANA,		// regain 1 mana per 5/3/1s
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

	NUM_RUNES,
};

void DescribeRune(Rune r, int x, int y);

#endif
