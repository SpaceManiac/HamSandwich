#include "display.h"
#include "jamulfont.h"
#include "mgldraw.h"
#include "jamulspr.h"

constexpr int DISPLAYLIST_LEN = 512;

struct DisplayObj
{
	const sprite_t *spr;
	int x, y, z;
	int sort;
	char bright;
	DisplayEffect effect;
	int next, prev;
};

mfont_t g_FontBronco;
DisplayObj g_DisplayList[DISPLAYLIST_LEN];
byte *g_Background;
uint32_t g_DisplayListLen;
uint32_t g_DisplayListHead;

void DisplayInit()
{
	FontInit(mgl);

	FontLoad("graphics/bronto.jft", &g_FontBronco);
	g_DisplayListLen = 0;
	g_DisplayListHead = 0xffffffff;
	g_Background = (byte *)malloc(0x4b000);
	if (g_Background == (byte *)0x0)
	{
		FatalError("out of memory!");
		exit(1);
	}
}

void DisplayExit()
{
	free(g_Background);
	FontFree(&g_FontBronco);
	FontExit();
}

void PrintMultiline(int x, int y, std::string_view s)
{
	FontPrintStringMultiline(x, y, s, &g_FontBronco);
}

int GetStrLength(std::string_view s)
{
	return FontStrLen(s, &g_FontBronco);
}

void PrintBright(int x, int y, std::string_view s, char bright)
{
	FontPrintStringBright(x, y, s, &g_FontBronco, bright);
}

void CenterPrintMultiline(int x, int y, std::string_view s)
{
	FontPrintStringMultiline(x - FontStrLen(s, &g_FontBronco) / 2, y, s, &g_FontBronco);
}

void CenterPrint(int x, int y, std::string_view s)
{
	FontPrintString(x - FontStrLen(s, &g_FontBronco) / 2, y, s, &g_FontBronco);
}

void CenterPrintBright(int x, int y, std::string_view s, char bright)
{
	FontPrintStringBright(x - FontStrLen(s, &g_FontBronco) / 2, y, s, &g_FontBronco, bright);
}

void Print(int x, int y, std::string_view s)
{
	FontPrintString(x, y, s, &g_FontBronco);
}

void RightPrint(int x, int y, std::string_view s)
{
	FontPrintString(x - FontStrLen(s, &g_FontBronco), y, s, &g_FontBronco);
}

void RightPrintMultiline(int x, int y, std::string_view s)
{
	FontPrintStringMultiline(x - FontStrLen(s, &g_FontBronco), y, s, &g_FontBronco);
}

void DisplayListClear()
{
	g_DisplayListLen = 0;
	g_DisplayListHead = 0xffffffff;
}

void DisplayListAdd(const sprite_t *spr, int x, int y, int z, int sort, char bright, DisplayEffect effect)
{
	uint32_t i;
	if (g_DisplayListLen < DISPLAYLIST_LEN)
	{
		g_DisplayList[g_DisplayListLen].spr = spr;
		g_DisplayList[g_DisplayListLen].x = x;
		g_DisplayList[g_DisplayListLen].y = y;
		g_DisplayList[g_DisplayListLen].z = z;
		g_DisplayList[g_DisplayListLen].sort = sort;
		g_DisplayList[g_DisplayListLen].bright = bright;
		g_DisplayList[g_DisplayListLen].effect = effect;
		if (g_DisplayListLen == 0)
		{
			g_DisplayList[0].next = -1;
			g_DisplayList[0].prev = -1;
			g_DisplayListHead = 0;
		}
		else
		{
			for (
				i = g_DisplayListHead;
				g_DisplayList[i].sort <= g_DisplayList[g_DisplayListLen].sort;
				i = g_DisplayList[i].next
			)
			{
				if (g_DisplayList[i].next == -1)
				{
					g_DisplayList[i].next = (unsigned int)g_DisplayListLen;
					g_DisplayList[g_DisplayListLen].prev = i;
					g_DisplayList[g_DisplayListLen].next = -1;
					g_DisplayListLen = g_DisplayListLen + 1;
					return;
				}
			}
			g_DisplayList[g_DisplayListLen].prev = g_DisplayList[i].prev;
			g_DisplayList[g_DisplayListLen].next = i;
			if (g_DisplayList[i].prev == -1)
			{
				g_DisplayListHead = (unsigned int)g_DisplayListLen;
			}
			else
			{
				g_DisplayList[g_DisplayList[i].prev].next = (unsigned int)g_DisplayListLen;
			}
			g_DisplayList[i].prev = (unsigned int)g_DisplayListLen;
		}
		g_DisplayListLen = g_DisplayListLen + 1;
	}
}

void DisplayListRender()
{
	int i;

	i = g_DisplayListHead;
	if (g_DisplayListHead != -1)
	{
		while (true)
		{
			switch (g_DisplayList[i].effect)
			{
			case DisplayEffect::Normal:
				g_DisplayList[i].spr->Draw(g_DisplayList[i].x, g_DisplayList[i].y, mgl);
				break;
			case DisplayEffect::Glow:
				g_DisplayList[i].spr->DrawGlow(g_DisplayList[i].x, g_DisplayList[i].y, mgl, g_DisplayList[i].bright);
				break;
			case DisplayEffect::Bright:
				g_DisplayList[i].spr->DrawBright(g_DisplayList[i].x, g_DisplayList[i].y, mgl, g_DisplayList[i].bright);
				break;
			case DisplayEffect::Red:
				g_DisplayList[i].spr->DrawColored(g_DisplayList[i].x, g_DisplayList[i].y, mgl, 4, g_DisplayList[i].bright);
				break;
			case DisplayEffect::Blue:
				g_DisplayList[i].spr->DrawColored(g_DisplayList[i].x, g_DisplayList[i].y, mgl, 3, g_DisplayList[i].bright);
				break;
			case DisplayEffect::Grey:
				g_DisplayList[i].spr->DrawColored(g_DisplayList[i].x, g_DisplayList[i].y, mgl, 0, g_DisplayList[i].bright);
			}
			if (g_DisplayList[i].next == -1)
				break;
			i = g_DisplayList[i].next;
		}
		DisplayListClear();
	}
}
