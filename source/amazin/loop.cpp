#include "loop.h"
#include "display.h"
#include "clock.h"
#include "jamulsound.h"

updateFunc g_Update = nullptr;
renderFunc g_Render = nullptr;
bool g_WantsRender;
int g_Runs;

dword g_Accumulator;
dword g_Time;
dword g_MsPerFrame = 1000 / 30;

void SetUpdate(updateFunc update)
{
	g_Update = update;
}

void SetRender(renderFunc render)
{
	g_Render = render;
}

void ClockReset()
{
	g_Accumulator = 0;
	g_Time = timeGetTime();
}

void ClockStart()
{
	g_Time = timeGetTime();
}

void ClockUpdate()
{
	dword now = timeGetTime();
	g_Accumulator += now - g_Time;
	g_Time = now;
}

void MglUpdate()
{
	g_WantsRender = false;
	ClockUpdate();
	g_Runs = 0;
	while (true)
	{
		if (g_Accumulator < g_MsPerFrame)
		{
			return;
		}
		JamulSoundUpdate();
		if (g_Update && !g_Update())
		{
			mgl->Quit();
			g_WantsRender = false;
			return;
		}
		g_Accumulator = g_Accumulator - g_MsPerFrame;
		g_Runs = g_Runs + 1;
		g_WantsRender = true;
	}
}

TASK(void) MglRender()
{
	if (g_Render)
	{
		g_Render();
	}
	AWAIT mgl->Flip();
}

TASK(void) LoopMain()
{
	ClockReset();
	do
	{
		MglUpdate();
		if (g_WantsRender)
		{
			AWAIT MglRender();
		}
	} while (mgl->Process());
}
