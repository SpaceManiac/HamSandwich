#ifndef TALENTS_H
#define TALENTS_H

#include "winpch.h"

enum talentList {
	// combat
	TLT_CHOPPING,
	TLT_TOSSING,
	TLT_STONESKIN,
	TLT_CRITICISM,
	TLT_VIVACITY,
	TLT_SNEAKATTACK,
	TLT_KNOCKOUT,
	TLT_FINESSE,
	TLT_SCAVENGING,
	TLT_GLADIATOR,
	// magic
	TLT_FOCUS,
	TLT_COMBUSTION,
	TLT_FREEZING,
	TLT_ARSENIC,
	TLT_ELECTROCUTION,
	TLT_MEDITATION,
	TLT_SPOOKYBONES,
	TLT_GREENTHUMB,
	TLT_SCORCHING,
	TLT_WINDBAG,
	// craft
	TLT_MOBILITY,
	TLT_ALCHEMY,
	TLT_JUNKSMITH,
	TLT_CLOCKWORK,
	TLT_MINING,
	TLT_HAGGLING,
	TLT_MISER,
	TLT_MIXOLOGY,
	TLT_PACKRAT,
	TLT_HERBALISM,
};

#define MAX_TALENTS	(30)

#pragma pack(push, 1)
typedef struct talent_t
{
	char name[16];
	char desc[4][64];
	float powerPerLev;
	word ptsNeeded[10];
} talent_t;
#pragma pack(pop)

void InitialTalents(void);
char *TalentName(byte n);
talent_t *GetTalent(byte n);
word TalentPointsNeeded(byte n);
void LearnTalent(byte s);
void TalentPoint(byte which,word amt);
float TalentBonus(byte which);
void TalentDecrease(void);

#endif
