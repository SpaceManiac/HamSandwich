#include "guy.h"
#include "jamulspr.h"

sprite_set_t* g_GuySprites[8];

void GuysInitGfx()
{
	g_GuySprites[0] = nullptr;
	g_GuySprites[1] = new sprite_set_t("graphics/bouapha.jsp");
	g_GuySprites[2] = new sprite_set_t("graphics/bphetta.jsp");
	g_GuySprites[3] = new sprite_set_t("graphics/smoove.jsp");
	g_GuySprites[4] = new sprite_set_t("graphics/chuckles.jsp");
	g_GuySprites[5] = new sprite_set_t("graphics/helga.jsp");
	g_GuySprites[6] = new sprite_set_t("graphics/pete.jsp");
	g_GuySprites[7] = new sprite_set_t("graphics/buttons.jsp");
}

void GuysExitGfx()
{
	delete g_GuySprites[1];
	delete g_GuySprites[2];
	delete g_GuySprites[3];
	delete g_GuySprites[4];
	delete g_GuySprites[5];
	delete g_GuySprites[6];
	delete g_GuySprites[7];
}
