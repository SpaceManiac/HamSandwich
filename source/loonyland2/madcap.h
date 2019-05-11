#ifndef MADCAP_H
#define MADCAP_H

#include "mgldraw.h"

#define MC_MIGHT	(0)
#define MC_LIFE		(1)
#define MC_SHIELD	(2)
#define MC_CURSE	(3)
#define MC_HEALTH	(4)
#define MC_SHOCK	(5)
#define MC_FIRE		(6)
#define MC_ICE		(7)
#define MC_WIND		(8)
#define MC_NATURE	(9)
#define MC_DEATH	(10)

#define MADCAP_HPBOOST	(30)	// 30x HP
#define MADCAP_DAMBOOST	(10)	// 10x damage
#define MADCAP_ARMBOOST	(5)		// 5x armor
#define MADCAP_LVLBOOST	(50)	// +50 levels
#define MADCAP_CURSEBOOST (10)	// 10x poison and fire damage

#define MADCAP_PRICE	(250)
#define MADCAP_XP		(1000)

void PrintMadcapInfo(int x,int y,byte w);
char *MadcapName(byte w);
word DamageAmt(word dmg,byte team);
word CurseDmg(word dmg,byte team);
word ShockDmg(word dmg,byte team);
word RecoverAmt(word dmg);
byte CrystalCost(byte w);

#endif
