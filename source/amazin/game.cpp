#include "game.h"
#include "title.h"

byte g_RivalryRound;
GameMode g_GameMode;
byte g_NumPlayers;
byte g_MapNum;
bool g_RivalryHasWinner;
byte g_SongTimer;
int g_RecordTimeFrames;
bool g_IsTimeAttack;

void GameInit(GameMode gameMode, bool isTimeAttack, byte mapNum, byte numPlayers)
{
	// TODO
	TitleInit();
}
