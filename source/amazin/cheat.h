#ifndef CHEAT_H
#define CHEAT_H

#include "jamultypes.h"

enum class Cheat : byte
{
	None,
	Skippy,
	Umlaut,
	Yippee,
};

void CheatInit();
Cheat CheatKeyPressed(char ch);

#endif  // CHEAT_H
