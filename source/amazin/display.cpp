#include "display.h"
#include "jamulfont.h"
#include "mgldraw.h"

MGLDraw* g_MGL;

mfont_t g_FontBronco;

uint32_t g_DisplayListLen = 0;
uint32_t g_DisplayListHead = 0xffffffff;
byte *g_Background;

void DisplayInit()
{
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
	FontFree(&g_FontBronco);
	free(g_Background);
}
