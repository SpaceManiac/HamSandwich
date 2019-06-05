#ifndef ACHIEVE_H
#define ACHIEVE_H

#include "mgldraw.h"

#define ACH_SPECIAL	(0)	// an achievement attained in some special way
#define ACH_SKILL	(1)	// get a specific skill to 15
#define ACH_TALENT	(2)	// specific talent to 10

// modifiers
#define MOD_NONE	(0)
#define MOD_EASY	(1)
#define MOD_HARD	(2)
#define MOD_RESPAWN	(3)
#define MOD_EXPERT	(4)
#define MOD_LITERACY (5)
#define MOD_CROWDED	(6)
#define MOD_TOY		(7)
#define MOD_RUSH	(8)
#define MOD_IRONMAN	(9)
#define MOD_DISCO	(10)
#define MOD_VAMPIRE	(11)
#define MOD_LIQUID	(12)
#define MOD_GANGSTA	(13)
#define MOD_ARENA	(14)
#define MOD_KNIGHT	(15)
#define MOD_SKILLBOR (16)
#define MOD_TRAMPLE	(17)
#define MOD_TIRELESS (18)
#define MOD_HEIR	(19)
#define MOD_HAUNTED	(20)
#define MOD_TICKING	(21)
#define MOD_MERCHANT (22)
#define MOD_EVEN	(23)
#define MAX_MODS	(24)

byte ModifierOn(byte mod);
const char *ModifierDesc(byte m);
const char *ModifierName(byte m);
void AdjustModifier(byte *me,byte w,char dir,byte *list);

void InitAchieve(byte ach,byte mod1,byte mod2);
byte UpdateAchieve(MGLDraw *mgl);
void RenderAchieve(MGLDraw *mgl);

void AchieveCheck(void);
void AchieveNow(byte n);

void AchieveMenu(MGLDraw *mgl,byte *backScr);

#endif
