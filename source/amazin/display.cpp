#include "display.h"
#include "jamulfont.h"
#include "mgldraw.h"

MGLDraw *mgl;

mfont_t g_FontBronco;
// TODO DisplayObj g_DisplayList[512];
byte *g_Background;
uint32_t g_DisplayListLen = 0;
uint32_t g_DisplayListHead = 0xffffffff;

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
