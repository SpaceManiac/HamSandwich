#ifndef GAME_H
#define GAME_H

#include "jamultypes.h"

enum class GameMode
{
	None,
	HappyFields = 1,
	DankDungeons = 2,
	SiblingRivalry = 3,
};

extern GameMode g_GameMode;
extern byte g_NumPlayers;
extern int g_RecordTimeFrames;
extern bool g_IsTimeAttack;
extern byte g_MapNum;

void GameInit(GameMode gameMode, bool isTimeAttack, byte mapNum, byte numPlayers);

#endif // GAME_H
