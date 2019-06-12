/* MGLDraw

Once a wrapper around SciTech Multi-platform Graphics Library;
then Allegro;
briefly PixelToaster;
now SDL2.

*/

#ifndef HAM_MGLDRAW_H
#define HAM_MGLDRAW_H

#ifdef SDL_UNPREFIXED
	#include <SDL.h>
#else  // SDL_UNPREFIXED
	#include <SDL2/SDL.h>
#endif  // SDL_UNPREFIXED

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif  // _WIN32

#include "jamultypes.h"

struct RGB {
	byte b, g, r, a;
};
typedef RGB PALETTE[256];

class SoftJoystick;

class MGLDraw
{
public:
	MGLDraw(const char *name, int xRes, int yRes, bool window);
	~MGLDraw();

	int GetWidth();
	int GetHeight();
	// Get a pointer to the screen memory.
	byte *GetScreen();
	void ClearScreen();

	// Resize the SCREEN BUFFER - does not resize the window.
	void ResizeBuffer(int w, int h);

	// Perform any necessary per-frame handling. Returns false if quit.
	bool Process();
	void Quit();

	// Display the buffer to the screen.
	void Flip();
	void WaterFlip(int v);
	void TeensyFlip();
	void TeensyWaterFlip(int v);
	void RasterFlip();
	void RasterWaterFlip(int v);

	// Return a pointer to the primary palette.
	const RGB *GetPalette();
	// Set the primary palette.
	void SetPalette(PALETTE newpal);
	// Activate the primary palette.
	void RealizePalette();
	// Set and activate the secondary palette.
	void SetSecondaryPalette(PALETTE newpal);

	// Load an image and store its palette to the primary palette.
	bool LoadBMP(const char *name);
	// Load an image and store its palette to `pal`.
	bool LoadBMP(const char *name, PALETTE pal);
	// Save an image with the current
	bool SaveBMP(const char *name);

	// Get and clear the last pressed key.
	// Based on SDL_Keycode and includes only keys representable as characters.
	char LastKeyPressed();
	// Get without clearing the last pressed key.
	char LastKeyPeek();
	// Clear any buffered keys.
	void ClearKeys();

	// handy little drawing routines
	void Box(int x, int y, int x2, int y2, byte c);
	void FillBox(int x, int y, int x2, int y2, byte c);
	void SelectLineH(int x, int x2, int y, byte ofs);
	void SelectLineV(int x, int y, int y2, byte ofs);
	void BrightBox(int x, int y, int x2, int y2, byte amt);
	void DarkBox(int x, int y, int x2, int y2, byte amt);

	// mouse functions
	void GetMouse(int *x, int *y);
	void SetMouse(int x, int y);
	bool MouseDown();
	bool RMouseDown();
	bool MouseTap();
	bool RMouseTap();
	bool MouseDown3();
	bool MouseTap3();

	int mouse_x, mouse_y, mouse_z, mouse_b;

#ifdef _WIN32
	HWND GetHWnd(void);
#endif

protected:
	void putpixel(int x, int y, RGB value);
	RGB FormatPixel(int x, int y);
	void PseudoCopy(int x, int y, byte* data, int len);

	void StartFlip(void);
	void FinishFlip(void);

	bool windowed, readyToQuit, idle;

	int xRes, yRes, pitch, winWidth, winHeight;
	byte *scrn;
	PALETTE pal, pal2;
	RGB *thePal;

	byte tapTrack;
	char lastKeyPressed;
	int lastRawCode;

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	RGB *buffer;

	friend class SoftJoystick;
	SoftJoystick *softJoystick;
};

void FatalError(const char *msg);

void SeedRNG(void);
dword Random(dword range);
int MGL_random(int range);
long MGL_randoml(long range);
void MGL_srand(int seed);

#endif
