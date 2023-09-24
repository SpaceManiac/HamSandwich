#include "help.h"
#include "display.h"
#include "title.h"
#include "loop.h"

enum class HelpType
{
	None,
	Text,
	Img,
	Btn,
	IBtn,
	_,
	Box,
	CText,
	RText,
};

struct Help
{
	HelpType type;
	int x, y;
	char text[256];
	// much undefined...
	int spriteNum;
	// much undefined...
};

byte g_HelpPage;
byte g_HelpCursor;
byte g_HelpLen;
Help *g_Help;
int g_HelpMouseX, g_HelpMouseY;
byte g_HelpOldHeld;
byte g_CurHelp;
int g_HelpHovered;

static bool HelpUpdate();
static void HelpRender();
static void LoadHelpDocument(byte page);
static void HelpClear();

void HelpInit(byte page)
{
	g_HelpLen = 0;
	g_Help = (Help *)calloc(sizeof(Help), 32);
	SetUpdate(HelpUpdate);
	SetRender(HelpRender);
	LoadHelpDocument(page);
	g_HelpPage = page;
	mgl->GetMouse(&g_HelpMouseX, &g_HelpMouseY);
}

void HelpExit()
{
	free(g_Help);
}

static bool LoadHelpHeader(char *line)
{
	// TODO
	return false;
}

static void LoadHelpPage(SDL_RWops *f)
{
	// TODO
}

static void LoadHelpDocument(byte page)
{
	// TODO
}

static void HelpClear()
{
	for (int i = 0; i < g_HelpLen; i = i + 1)
	{
		g_Help[i].type = HelpType::None;
		g_Help[i].text[0] = '\0';
	}
	g_HelpLen = 0;
}

bool HelpIsHovered(Help *me)
{
	int x = me->x;
	int y = me->y;
	int w = GetStrLength(me->text);
	if ((((g_HelpMouseX < x) || (g_HelpMouseY < y)) || (w + 4 + x < g_HelpMouseX)) ||
	    (y + 30 < g_HelpMouseY))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool HelpButtonIsHovered(Help *me)
{
	const sprite_t *spr = g_ButtonsJsp->GetSprite(me->spriteNum + 1);
	int x = me->x + (int)spr->ofsx;
	int y = me->y + (int)spr->ofsy;
	if ((((g_HelpMouseX < x) || (g_HelpMouseY < y)) || ((int)(x + (uint)spr->width) < g_HelpMouseX)) ||
	    ((int)(y + (uint)spr->height) < g_HelpMouseY))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool HelpUpdate()
{
	// TODO
	return false;
}

void HelpRender()
{
	// TODO
}
