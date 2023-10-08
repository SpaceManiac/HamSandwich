#include "status.h"
#include "game.h"
#include "jamulspr.h"
#include "jamultypes.h"
#include "options.h"
#include "display.h"
#include "player.h"

sprite_set_t *g_StatusJsp;

void StatusInit()
{
	g_StatusJsp = new sprite_set_t("graphics/status.jsp");
}

void StatusExit()
{
	delete g_StatusJsp;
}

void StatusRender()
{
	bool bVar1;
	byte i;
	sprite_t *spr;
	char buf[16];
	int x;
	int y;

	g_StatusJsp->GetSprite(ConfigGetBouaphettaMode())->Draw(0x22, 0x24, mgl);
	if (g_NumPlayers == 2)
	{
		g_StatusJsp->GetSprite(!ConfigGetBouaphettaMode())->Draw(0x25e, 0x24, mgl);
	}
	if (g_GameMode == GameMode::SiblingRivalry)
	{
		sprintf(buf, "%03d", (unsigned int)g_Player[0].candles);
		Print(0x3c, 4, buf);
		sprintf(buf, "%03d", (unsigned int)g_Player[1].candles);
		Print(0x21c, 4, buf);
		for (i = 0; i < g_Player[0].score; i = i + 1)
		{
			g_StatusJsp->GetSprite(ConfigGetBouaphettaMode())->Draw((unsigned int)i * 0x14 + 0x96, 0x24, mgl);
		}
		for (i = 0; i < g_Player[1].score; i = i + 1)
		{
			g_StatusJsp->GetSprite(!ConfigGetBouaphettaMode())->Draw(0x280 - ((unsigned int)i * 0x14 + 0x96), 0x24, mgl);
		}
	}
	else
	{
		g_StatusJsp->GetSprite(2)->Draw(0x3c, 0x1f, mgl);
		sprintf(buf, "%09d", g_Player[0].score);
		Print(0x3c, 2, buf);
		if (g_NumPlayers == 2)
		{
			g_StatusJsp->GetSprite(2)->Draw(0x21c, 0x1f, mgl);
			sprintf(buf, "%09d", g_Player[1].score);
			RightPrint(0x244, 2, buf);
		}
		if (g_IsTimeAttack == false)
		{
			g_StatusJsp->GetSprite(3)->Draw(300, 0x1f, mgl);
			sprintf(buf, "%02d", (unsigned int)g_Player[0].lives);
			Print(300, 2, buf);
		}
	}
	if (g_IsTimeAttack != false)
	{
		sprintf(buf, "%02d:%02d", g_RecordTimeFrames / 0x708, g_RecordTimeFrames / 0x1e + (g_RecordTimeFrames / 0x708) * -0x3c);
		Print(0x230, 2, buf);
	}
}
