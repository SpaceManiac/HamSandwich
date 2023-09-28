#ifndef GAME_H
#define GAME_H

#include "jamultypes.h"

enum class GameMode
{
	HappyFields = 1,
	DankDungeons = 2,
	SiblingRivalry = 3,
};

void GameInit(GameMode gameMode, bool isTimeAttack, byte mapNum, byte numPlayers);

#endif // GAME_H
