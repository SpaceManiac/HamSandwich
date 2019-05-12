#ifndef CH_WITCH_H
#define CH_WITCH_H

#include "player.h"

#define SPL_BLAST	0
#define SPL_KABOOM	1
#define SPL_SHOCK	2
#define SPL_ICE		3
#define SPL_HEALING 4
#define SPL_BUZZSAW	5
#define SPL_PLAGUE	6
#define SPL_SPEED	7

void WitchControlMe(Guy *me,mapTile_t *mapTile,world_t *world);

char *SpellName(byte spl);
byte SpellCost(byte spl);
byte SpellLevel(byte spl);
void PowerupSpell(byte spl,byte amt);
byte SpellPct(byte spl);

#endif
