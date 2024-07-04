/* MGLDraw

Once a wrapper around SciTech Multi-platform Graphics Library;
then Allegro;
briefly PixelToaster;
now SDL2.

*/

#ifndef HAM_MGLDRAW_H
#define HAM_MGLDRAW_H

#include <memory>
#include <SDL.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif  // _WIN32

#include "jamultypes.h"
#include "coro.h"

struct RGB {
	byte b, g, r, a;
};
typedef RGB PALETTE[256];

class SoftJoystick;

class MGLDraw
{
public:
	MGLDraw(const char *name, int xRes, int yRes, bool window);
	virtual ~MGLDraw();

	int GetWidth();
	int GetHeight();
	// Get a pointer to the screen memory.
	byte *GetScreen();
	void ClearScreen();

	// Resize the SCREEN BUFFER - window is only resized if necessary.
	void ResizeBuffer(int w, int h, bool clamp = false);
	bool IsWindowed();
	void SetWindowed(bool windowed);

	// Perform any necessary per-frame handling. Returns false if quit.
	virtual bool Process();
	void Quit();

	// Display the buffer to the screen.
	TASK(void) Flip();
	void BufferFlip();  // Convert 8-bit to 32-bit, but don't present.
	TASK(void) WaterFlip(int v);
	TASK(void) TeensyFlip();
	TASK(void) TeensyWaterFlip(int v);
	TASK(void) RasterFlip();
	TASK(void) RasterWaterFlip(int v);

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
	// Save an image with the current contents of the screen.
	bool SaveBMP(const char *name);
	bool SavePNG(const char *name);

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

	// Like SDL's text input functions but in MGL coordinates.
	void StartTextInput(int x, int y, int x2, int y2);
	static void StopTextInput();

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
	void PseudoCopy(int y, int x, byte* data, int len);

	void StartFlip(void);
	TASK(void) FinishFlip(void);

	bool windowed, readyToQuit, idle;

	int xRes, yRes, pitch, winWidth, winHeight;
	std::unique_ptr<byte[]> scrn;
	PALETTE pal, pal2;
	RGB *thePal;

	byte tapTrack;
	char lastKeyPressed;
	int lastRawCode;

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	std::unique_ptr<RGB[]> buffer;

	friend class SoftJoystick;
	std::unique_ptr<SoftJoystick> softJoystick;
};

void FatalError(const char *msg);

void SeedRNG(void);
dword Random(dword range);
int MGL_random(int range);
long MGL_randoml(long range);
void MGL_srand(int seed);

bool GetGameIdle();

#endif
