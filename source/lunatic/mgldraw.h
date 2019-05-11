#ifndef MGLDRAW_H
#define MGLDRAW_H

#include "allegro.h"
#include <winalleg.h>
#include "winpch.h"
#include "jamulsound.h"
#include "control.h"
#include <stdio.h>
#include <memory>

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
	~MGLDraw();

	bool Process(); // handle windows messages and such

	HWND GetHWnd();
	byte *GetScreen(); // get a pointer to the screen memory
	int GetWidth();
	int GetHeight();
	void ClearScreen();
	void Flip();
	void Quit();

	bool LoadPalette(const char *name);
	void SetPalette(const palette_t *pal2);

	bool LoadBMP(const char *name);

	char LastKeyPressed();
	char LastKeyPeek();
	void SetLastKey(char c);

	void GammaCorrect(byte gamma);

	// handy little drawing routines
	void Box(int x, int y, int x2, int y2, byte c);
	void FillBox(int x, int y, int x2, int y2, byte c);

	// mouse functions
	byte MouseDown();
	void SetMouseDown(byte w);
	void SetMouse(int x, int y);
	void TeleportMouse(int x, int y);
	void GetMouse(int *x, int *y);

protected:
	struct bitmap_deleter {
		void operator()(BITMAP* buffer) {
			destroy_bitmap(buffer);
		}
	};

	int xRes, yRes, pitch;
	int mousex, mousey;
	std::unique_ptr<byte[]> scrn;
	std::unique_ptr<BITMAP, bitmap_deleter> buffer;
	palette_t pal[256];
	bool readyToQuit;
	char lastKeyPressed;
	byte mouseDown;
};

#endif
