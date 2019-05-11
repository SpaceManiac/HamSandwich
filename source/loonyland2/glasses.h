#ifndef GLASSES_H
#define GLASSES_H

#include "mgldraw.h"

// glass effects
#define GE_STAMINA	(0)
#define GE_LIFE		(1)
#define GE_ARMOR	(2)
#define GE_DAMAGE	(3)
#define GE_SPEED	(4)
#define GE_STAMGEN	(5)	// stamina regen +%
#define GE_MAGICGEN	(6)	// magic regen +%
#define GE_FREEZE	(7)	// freeze duration +%
#define GE_POISON	(8)	// poison duration +%
#define GE_MAGICDAM	(9)	// magic damage +%
#define GE_LIFEGEN	(10)	// life regen every X sec.
#define GE_SPELLCOST (11)	// spells of type are reduced by 1 magic.  0=fire, 1=wind, 2=ice, 3=earth, 4=death, 5=all
#define GE_STUN		(12)
#define GE_KNOCK	(13)
#define GE_LUCK		(14) // X luck bonus
#define GE_TALENT	(15) // +1 to talents, 0=combat, 1=magic, 2=craft, 3=all
#define GE_SKILL	(16)	// +1 to skills, 0=passive, 1=attack, 2=throw, 3=magic, 4=all
#define GE_MAGIC	(17)
#define GE_MONSTER  (18)	// double the monster counts

#define NUM_GLASSES	(78)

typedef struct glassEff_t
{
	byte lens[2];
	byte effect;
	byte amount;
} glassEff_t;

void DescribeGlasses(int x,int y);
char *LensName(byte lens);
byte GlassAmount(void);
byte GlassEffect(void);
void GuyDropLens(byte level,byte *type,int *val1,int *val2);
byte AddLens(byte type);

#endif
