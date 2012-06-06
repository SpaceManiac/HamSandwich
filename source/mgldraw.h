#ifndef MGLDRAW_H
#define MGLDRAW_H

#include "allegro.h"
#include <winalleg.h>
#include "winpch.h"
#include "jamulsound.h"
#include "control.h"
#include <stdio.h>

// For appdata storage of stuff
FILE* AppdataOpen(const char* filename, const char* mode);

// Replacement for missing palette_t

struct palette_t
{
	byte alpha, red, green, blue;
};

// Replacement for missing MGL functions
int MGL_random(int max);
void MGL_srand(int seed);
long MGL_randoml(long max);
void MGL_fatalError(const char* txt);

class MGLDraw
{
public:
	MGLDraw(const char *name, int xRes, int yRes, bool window);
	~MGLDraw(void);

	bool Process(void); // handle windows messages and such

	HWND GetHWnd(void);
	byte *GetScreen(void); // get a pointer to the screen memory
	int GetWidth(void);
	int GetHeight(void);
	void ClearScreen(void);
	void Flip(void);
	void Quit(void);

	bool LoadPalette(char *name);
	void SetPalette(palette_t *pal2);

	bool LoadBMP(const char *name);

	char LastKeyPressed(void);
	char LastKeyPeek(void);
	void SetLastKey(char c);

	void GammaCorrect(byte gamma);

	// handy little drawing routines
	void Box(int x, int y, int x2, int y2, byte c);
	void FillBox(int x, int y, int x2, int y2, byte c);

	// functions to measure frame rate
	void ResetTimer(void);
	void TickTimer(void);
	float FrameRate(void);

	// mouse functions
	byte MouseDown(void);
	void SetMouseDown(byte w);
	void SetMouse(int x, int y);
	void TeleportMouse(int x, int y);
	void GetMouse(int *x, int *y);

protected:
	int xRes, yRes, pitch;
	int mousex, mousey;
	byte *scrn;
	BITMAP* buffer;
	palette_t pal[256];
	bool readyToQuit;
	dword elapsedTime;
	dword now;
	int numFrames;
	char lastKeyPressed;
	byte mouseDown;
};

#endif
