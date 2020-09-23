#ifndef CHEAT_H
#define CHEAT_H

#include "sound.h"
#include "display.h"
#include "player.h"
#include "message.h"
#include "bullet.h"

#define CHEAT_WINLEVEL	0
#define CHEAT_HAMMERUP	1
#define CHEAT_LIFE		2
#define CHEAT_SHIELD	3
#define CHEAT_BOOM		4
#define CHEAT_FREEZE	5
#define CHEAT_BRAINS	6
#define CHEAT_KEYS		7
#define CHEAT_SCANNER	8
#define CHEAT_AMMO		9
#define CHEAT_LIGHT		10
#define CHEAT_WATER		11
#define CHEAT_OXYGEN	12
#define CHEAT_NOSKID	13
#define CHEAT_SPEED		14
#define CHEAT_RAGE		15
#define CHEAT_CANDLES	16
#define CHEAT_STRENGTH	17
#define CHEAT_QUICKFIX	18
#define CHEAT_CONFUSE	19
#define CHEAT_PRESTO	20
#define CHEAT_BLACKHOLE	21

#define NUM_CHEATS		22

void InitCheater(void);
void CheatKey(char c);
void DoCheat(byte w);
char *CheatName(byte w);

#endif
