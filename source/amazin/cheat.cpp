#include "cheat.h"
#include <string.h>
#include <ctype.h>

const char g_Cheats[3][16] = {
	"skippy",
	"umlaut",
	"yippee",
};

char g_CheatBuffer[17];

void CheatInit()
{
	for (int i = 0; i < 0x10; i = i + 1)
	{
		g_CheatBuffer[i] = 0;
	}
	g_CheatBuffer[16] = 0;
}

Cheat CheatKeyPressed(char ch)
{
	int i;

	for (i = 0; i < 0xf; i = i + 1)
	{
		g_CheatBuffer[i] = g_CheatBuffer[i + 1];
	}
	g_CheatBuffer[15] = tolower(ch);
	for (i = 0; i < 3; i = i + 1)
	{
		if (!strcmp(g_Cheats[i], g_CheatBuffer + (0x10 - strlen(g_Cheats[i]))))
		{
			g_CheatBuffer[15] = 0;
			return Cheat(i + 1);
		}
	}
	return Cheat::None;
}
