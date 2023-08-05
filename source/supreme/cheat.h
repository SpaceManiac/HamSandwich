#ifndef CHEAT_H
#define CHEAT_H

#include "sound.h"
#include "display.h"
#include "player.h"
#include "message.h"
#include "bullet.h"

enum : byte
{
	CHEAT_WINLEVEL,
	CHEAT_HAMMERUP,
	CHEAT_LIFE,
	CHEAT_SHIELD,
	CHEAT_BOOM,
	CHEAT_FREEZE,
	CHEAT_BRAINS,
	CHEAT_KEYS,
	CHEAT_SCANNER,
	CHEAT_AMMO,
	CHEAT_LIGHT,
	CHEAT_WATER,
	CHEAT_OXYGEN,
	CHEAT_NOSKID,
	CHEAT_SPEED,
	CHEAT_RAGE,

	NUM_CHEATS
};

void InitCheater(void);
void CheatKey(char c);
void DoCheat(byte w);
const char *CheatName(byte w);

#endif
