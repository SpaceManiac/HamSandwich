#ifndef SPELL_H
#define SPELL_H

#include "jamultypes.h"
#include "guy.h"

static char spellName[20][32]={
	"Energy Barrage",
	"Energy Storm",
	"Dragon's Flame",
	"Liquify",
	"Seeker Bolt",
	"Seeker Swarm",
	"Ice Blast",
	"Ice Beam",
	"Inferno",
	"Hyper Inferno",
	"Summon Ptero",
	"Summon Golem",
	"Stoneskin",
	"Steelskin",
	"Berserk",
	"Insane Rage",
	"Healing",
	"Life",
	"Armageddon",
	"ARMAGEDDON!!"
};

// spells
#define SPL_ENERGY  0	// energy barrage/storm
#define SPL_FLAME	1	// dragon's flame/whatever
#define SPL_SEEKER  2	// seeker bolt
#define SPL_ICE		3	// ice beam/blast
#define SPL_INFERNO 4	// inferno
#define SPL_SUMMON  5	// summon golem
#define SPL_ARMOR	6	// stoneskin/steelskin
#define SPL_BERSERK 7	// berserk
#define SPL_HEAL	8	// healing
#define SPL_ARMAGEDDON 9	// armageddon

void CastSpell(Guy *me);
byte EnoughMana(void);
byte SpellLevel(void);


#endif
