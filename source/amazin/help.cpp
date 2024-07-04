#include "help.h"
#include "appdata.h"
#include "options.h"
#include "control.h"
#include "display.h"
#include "guy.h"
#include "loop.h"
#include "register.h"
#include "title.h"
#include "map.h"

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
	byte cursorId;
	byte action;
	int x, y;
	char text[256];
	int navUp, navDown, navLeft, navRight;
	int destPage;
	int spriteNum;
};

byte g_HelpPage;
byte g_HelpCursor;
byte g_HelpLen;
Help *g_Help;
int g_HelpMouseX, g_HelpMouseY;
byte g_HelpOldHeld;
byte g_HelpNext;
Help *g_HelpHovered;

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
	char *tok;
	int num;

	tok = strtok(line, " ,~\n");
	if (!strcmp(tok, "PAGE"))
	{
		return false;
	}
	else if (!strcmp(tok, "TEXT"))
	{
		g_HelpLen = g_HelpLen + 1;
		g_HelpNext = g_HelpNext + 1;
		g_Help[g_HelpNext].type = HelpType::Text;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].x = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].y = num;
	}
	else if (!strcmp(tok, "CTEXT"))
	{
		g_HelpLen = g_HelpLen + 1;
		g_HelpNext = g_HelpNext + 1;
		g_Help[g_HelpNext].type = HelpType::CText;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].x = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].y = num;
	}
	else if (!strcmp(tok, "RTEXT"))
	{
		g_HelpLen = g_HelpLen + 1;
		g_HelpNext = g_HelpNext + 1;
		g_Help[g_HelpNext].type = HelpType::RText;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].x = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].y = num;
	}
	else if (!strcmp(tok, "IMG"))
	{
		g_HelpLen = g_HelpLen + 1;
		g_HelpNext = g_HelpNext + 1;
		g_Help[g_HelpNext].type = HelpType::Img;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].x = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].y = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navUp = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navDown = num;
	}
	else if (!strcmp(tok, "BTN"))
	{
		g_HelpLen = g_HelpLen + 1;
		g_HelpNext = g_HelpNext + 1;
		g_Help[g_HelpNext].type = HelpType::Btn;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].cursorId = (byte)num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].x = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].y = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navUp = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navDown = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navLeft = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navRight = num;
		tok = strtok(nullptr, " ,~\n");
		if (!strcmp(tok, "GO"))
		{
			g_Help[g_HelpNext].action = 1;
		tok = strtok(nullptr, " ,~\n");
			num = atoi(tok);
			g_Help[g_HelpNext].destPage = num;
		}
		else if (!strcmp(tok, "EXIT"))
		{
			g_Help[g_HelpNext].action = 0;
		}
	}
	else if (!strcmp(tok, "IBTN"))
	{
		g_HelpLen = g_HelpLen + 1;
		g_HelpNext = g_HelpNext + 1;
		g_Help[g_HelpNext].type = HelpType::IBtn;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].cursorId = (byte)num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].x = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].y = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navUp = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navDown = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navLeft = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navRight = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].spriteNum = num;
		tok = strtok(nullptr, " ,~\n");
		if (!strcmp(tok, "GO"))
		{
			g_Help[g_HelpNext].action = 1;
			tok = strtok(nullptr, " ,~\n");
			num = atoi(tok);
			g_Help[g_HelpNext].destPage = num;
		}
		else if (!strcmp(tok, "EXIT"))
		{
			g_Help[g_HelpNext].action = 0;
		}
	}
	else if (!strcmp(tok, "BOX"))
	{
		g_HelpLen = g_HelpLen + 1;
		g_HelpNext = g_HelpNext + 1;
		g_Help[g_HelpNext].type = HelpType::Box;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].x = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].y = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navUp = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navDown = num;
		tok = strtok(nullptr, " ,~\n");
		num = atoi(tok);
		g_Help[g_HelpNext].navLeft = num;
	}
	return true;
}

static void LoadHelpPage(FILE *f)
{
	size_t lineLen;
	size_t textLen;
	char line[128];

	while (true)
	{
		while (true)
		{
			if (fscanf(f, "%[^\r\n]\n", line) == -1)
			{
				return;
			}
			lineLen = strlen(line);
			line[lineLen + 1] = '\0';
			line[lineLen] = '\n';
			if (line[0] != '~')
				break;
			if (!LoadHelpHeader(strtok(line + 1, "~\n")))
			{
				return;
			}
		}
		lineLen = strlen(line);
		textLen = strlen(g_Help[g_HelpNext].text);
		if ((254 < lineLen + textLen) || (g_HelpNext == 0xff))
			break;
		strcat(g_Help[g_HelpNext].text, line);
	}
	FatalError("Text too long!");
	exit(1);
}

static void LoadHelpDocument(byte page)
{
	char line[128];

	HelpClear();
	FILE *f = AssetOpen("data/help.jhl");
	if (!f)
	{
		FatalError("Help File Not Found!");
		exit(1);
	}
	bool found = false;
	while (fscanf(f, "%[^\n]\n", line) != -1 && !found)
	{
		if (line[0] == '~')
		{
			char *tok = strtok(line + 1, " ~");
			if (strcmp(tok, "PAGE") == 0)
			{
				tok = strtok(nullptr, " ,~");
				if (atoi(tok) == page)
				{
					g_HelpLen = 0;
					g_HelpNext = 0xff;
					LoadHelpPage(f);
					found = true;
				}
			}
		}
	}
	if (!found)
	{
		FatalError("Help Page Not Found!");
		exit(1);
	}

	fclose(f);
	g_HelpCursor = 0;
	g_HelpOldHeld = 0xff;
	g_HelpHovered = nullptr;
	for (int i = 0; i < g_HelpLen; i = i + 1)
	{
		if ((g_Help[i].cursorId == 0) &&
		    ((g_Help[i].type == HelpType::Btn || (g_Help[i].type == HelpType::IBtn))))
		{
			g_HelpHovered = g_Help + i;
		}
	}
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
	if ((((g_HelpMouseX < x) || (g_HelpMouseY < y)) || ((int)(x + (unsigned int)spr->width) < g_HelpMouseX)) ||
	    ((int)(y + (unsigned int)spr->height) < g_HelpMouseY))
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
	int i;
	char k;
	int oldMouseX;
	int oldMouseY;

	k = mgl->LastKeyPressed();
	oldMouseY = g_HelpMouseY;
	oldMouseX = g_HelpMouseX;
	if (k == '\x1b')
	{
		if (g_HelpPage != 0x18)
		{
			HelpExit();
			TitleInit();
			return true;
		}
		HelpExit();
		return false;
	}
	mgl->GetMouse(&g_HelpMouseX, &g_HelpMouseY);
	if ((oldMouseX != g_HelpMouseX) || (oldMouseY != g_HelpMouseY))
	{
		for (i = 0; i < (int)(unsigned int)g_HelpLen; i = i + 1)
		{
			if (g_Help[i].type == HelpType::Btn)
			{
				if (HelpIsHovered(g_Help + i))
				{
					g_HelpCursor = g_Help[i].cursorId;
					g_HelpHovered = g_Help + i;
				}
			}
			else if ((g_Help[i].type == HelpType::IBtn) && HelpButtonIsHovered(g_Help + i))
			{
				g_HelpCursor = g_Help[i].cursorId;
				g_HelpHovered = g_Help + i;
			}
		}
	}
	if (mgl->MouseTap() && ((g_HelpHovered->type == HelpType::Btn && HelpIsHovered(g_HelpHovered)) ||
	                        (g_HelpHovered->type == HelpType::IBtn && HelpButtonIsHovered(g_HelpHovered))))
	{
		k = '\r';
	}
	byte held = GetPlayerControls(0) | GetPlayerControls(1) | GetArrows();
	if ((((held & CONTROL_UP) != 0) && ((g_HelpOldHeld & CONTROL_UP) == 0)) && g_HelpHovered)
	{
		g_HelpCursor = g_HelpHovered->navUp;
	}
	if ((((held & CONTROL_DN) != 0) && ((g_HelpOldHeld & CONTROL_DN) == 0)) && g_HelpHovered)
	{
		g_HelpCursor = g_HelpHovered->navDown;
	}
	if ((((held & CONTROL_LF) != 0) && ((g_HelpOldHeld & CONTROL_LF) == 0)) && g_HelpHovered)
	{
		g_HelpCursor = g_HelpHovered->navLeft;
	}
	if ((((held & CONTROL_RT) != 0) && ((g_HelpOldHeld & CONTROL_RT) == 0)) && g_HelpHovered)
	{
		g_HelpCursor = g_HelpHovered->navRight;
	}
	if ((((k == '\r') || (k == ' ')) ||
	     (((held & CONTROL_B1) != 0 && ((g_HelpOldHeld & CONTROL_B1) == 0)))) &&
	    g_HelpHovered)
	{
		if (g_HelpHovered->action == 0)
		{
			HelpExit();
			TitleInit();
		}
		else if (g_HelpHovered->action == 1)
		{
			if (g_HelpHovered->destPage < 200)
			{
				g_HelpPage = *(byte *)&g_HelpHovered->destPage;
				LoadHelpDocument(*(byte *)&g_HelpHovered->destPage);
			}
			else if (g_HelpHovered->destPage == 0xff)
			{
				//HelpExit();
				//RegisterInit();
				return true;
			}
			else if (g_HelpHovered->destPage == 0xfe)
			{
				HelpExit();
				g_QuitAction = QuitAction::BuyOnline;
				return false;
			}
			else if (g_HelpHovered->destPage == 0xfd)
			{
				for (i = 0; i < 5; i = i + 1)
				{
					strcpy(g_Config.scoreNames[i], "??????");
					g_Config.scores[i] = 0;
				}
				LoadHelpDocument(0x16);
				g_HelpPage = 0x16;
			}
			else if (g_HelpHovered->destPage == 0xfc)
			{
				HelpExit();
				return false;
			}
			else if (g_HelpHovered->destPage == 0xfb)
			{
				HelpExit();
				g_QuitAction = QuitAction::RegisterTxt;
				return false;
			}
		}
	}
	if (held != 0)
	{
		g_HelpHovered = (Help *)0x0;
		for (i = 0; i < (int)(unsigned int)g_HelpLen; i = i + 1)
		{
			if ((g_Help[i].cursorId == g_HelpCursor) &&
			    ((g_Help[i].type == HelpType::Btn || (g_Help[i].type == HelpType::IBtn))))
			{
				g_HelpHovered = g_Help + i;
			}
		}
	}
	g_HelpOldHeld = held;
	return true;
}

void HelpRender()
{
	int iVar1;
	sprite_t *spr;
	int iVar2;
	int i;
	char buf[32];

	mgl->ClearScreen();
	for (i = 0; i < 9; i = i + 1)
	{
		mgl->FillBox(0, (8 - i) * -8 + 0x1b9, 0x27f, (8 - i) * -8 + 0x1c1, (char)i + 0x60);
		mgl->FillBox(0, i * -2 + 0x10, 0x27f, i * -2 + 0x11, (char)i + 0x60);
	}
	mgl->FillBox(0, 0x1c2, 0x27f, 0x1df, 0x69);
	for (i = 0; i < (int)(unsigned int)g_HelpLen; i = i + 1)
	{
		switch (g_Help[i].type)
		{
		case HelpType::Text:
			PrintMultiline(g_Help[i].x, g_Help[i].y, g_Help[i].text);
			break;
		case HelpType::Img:
			if (g_Help[i].navUp < 6)
			{
				GetGuySprite(g_Help[i].navUp + 1, g_Help[i].navDown)->Draw(g_Help[i].x, g_Help[i].y, mgl);
			}
			else if (g_Help[i].navUp == 6)
			{
				g_ItemsJsp->GetSprite(g_Help[i].navDown)->Draw(g_Help[i].x, g_Help[i].y, mgl);
			}
			else if (g_Help[i].navUp == 7)
			{
				TileRender(g_Help[i].navDown,g_Help[i].x,g_Help[i].y);
			}
			else if (g_Help[i].navUp == 8)
			{
				g_ButtonsJsp->GetSprite(g_Help[i].navDown)->Draw(g_Help[i].x, g_Help[i].y, mgl);
			}
			break;
		case HelpType::Btn:
			iVar1 = GetStrLength(g_Help[i].text);
			iVar1 = iVar1 + 4 + g_Help[i].x;
			iVar2 = g_Help[i].y;
			if (g_Help[i].cursorId == g_HelpCursor)
			{
				mgl->Box(g_Help[i].x, g_Help[i].y, iVar1, iVar2 + 0x1e, 0x1f);
				mgl->FillBox(g_Help[i].x + 1, g_Help[i].y + 1, iVar1 + -1, iVar2 + 0x1d, 4);
				PrintMultiline(g_Help[i].x + 4, g_Help[i].y + 2, g_Help[i].text);
			}
			else
			{
				mgl->Box(g_Help[i].x, g_Help[i].y, iVar1, iVar2 + 0x1e, 0x10);
				mgl->FillBox(g_Help[i].x + 1, g_Help[i].y + 1, iVar1 + -1, iVar2 + 0x1d, 0);
				PrintBright(g_Help[i].x + 4, g_Help[i].y + 2, g_Help[i].text, -0x10);
			}
			break;
		case HelpType::IBtn:
			if (g_Help[i].cursorId == g_HelpCursor)
			{
				g_ButtonsJsp->GetSprite(g_Help[i].spriteNum + 1)->Draw(g_Help[i].x, g_Help[i].y, mgl);
			}
			else
			{
				g_ButtonsJsp->GetSprite(g_Help[i].spriteNum)->Draw(g_Help[i].x, g_Help[i].y, mgl);
			}
			break;
		case HelpType::Box:
			mgl->Box(g_Help[i].x, g_Help[i].y, g_Help[i].navUp, g_Help[i].navDown, g_Help[i].navLeft);
			break;
		case HelpType::CText:
			CenterPrintMultiline(g_Help[i].x, g_Help[i].y, g_Help[i].text);
			break;
		case HelpType::RText:
			RightPrintMultiline(g_Help[i].x, g_Help[i].y, g_Help[i].text);
		}
	}
	if (g_HelpPage == 0x16)
	{
		for (i = 0; i < 5; i = i + 1)
		{
			PrintMultiline(0xb4, i * 0x1e + 0x6e, (char *)g_Config.scoreNames[i]);
			sprintf(buf, "%d", g_Config.scores[i]);
			RightPrintMultiline(0x1cc, i * 0x1e + 0x6e, buf);
		}
	}
	g_ButtonsJsp->GetSprite(26)->Draw(g_HelpMouseX, g_HelpMouseY, mgl);
}
