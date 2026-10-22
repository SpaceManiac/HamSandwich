#ifndef PLAYER_H
#define PLAYER_H

#include "jamultypes.h"

struct Player
{
	dword _0;
	byte lives;
	dword score;
	word candles;
};

extern Player g_Player[2];

void PlayerInit();
void PlayerGainLife(byte player);
bool PlayerLoseLife(byte player);
void PlayerGetPoints(byte player, int points);

#endif // PLAYER_H
