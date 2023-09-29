#include "player.h"
#include "options.h"
#include "sound.h"
#include "game.h"

Player g_Player[2];

void PlayerInit()
{
	g_Player[0]._0 = 0;
	g_Player[0].lives = ConfigGetLives();
	g_Player[0].candles = 0;
	g_Player[0].score = 0;
	g_Player[1]._0 = 0;
	g_Player[1].lives = ConfigGetLives();
	g_Player[1].candles = 0;
	g_Player[1].score = 0;
}

void PlayerGainLife(byte player)
{
	PlaySound(0xf, 2000);
	if (g_Player[player].lives < 99)
	{
		g_Player[player].lives = g_Player[player].lives + 1;
	}
}

bool PlayerLoseLife(byte player)
{
	g_Player[player].lives = g_Player[player].lives - 1;
	return g_Player[player].lives == 0;
}

void PlayerGetPoints(byte player, int points)
{
	if (g_GameMode != GameMode::SiblingRivalry)
	{
		dword oldScore = g_Player[player].score;
		g_Player[player].score = g_Player[player].score + points;
		if (oldScore / 5000 < g_Player[player].score / 5000)
		{
			PlayerGainLife(0);
		}
	}
}
