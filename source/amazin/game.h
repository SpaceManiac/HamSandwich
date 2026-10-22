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

extern byte g_EndAnim[2], g_EndAnimPumpkins;
extern GameMode g_GameMode;
extern byte g_NumPlayers;
extern int g_RecordTimeFrames;
extern bool g_IsTimeAttack;
extern byte g_MapNum;
extern byte g_LevelWon;

void GameInit(GameMode gameMode, bool isTimeAttack, byte mapNum, byte numPlayers);

#endif // GAME_H
