#ifndef MGLDRAW_H
#define MGLDRAW_H

#include <SDL2/SDL.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "jamultypes.h"
#include "jamulsound.h"
#include "control.h"
#include <stdio.h>
#include <memory>

// For appdata storage of stuff
FILE* AppdataOpen(const char* filename, const char* mode);

// Replacement for missing MGL functions
int MGL_random(int max);
void MGL_srand(int seed);
long MGL_randoml(long max);
void MGL_fatalError(const char* txt);

// -- SDL adapters
struct RGB
{
	unsigned char r, g, b, a;
};

#define PAL_SIZE	256

typedef RGB PALETTE[PAL_SIZE];

extern int KEY_MAX;
extern const Uint8* key;
// -- End SDL adapters

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
	void StartFlip(void);
	void FinishFlip(void);
	void Flip();
	void Quit();

	bool LoadPalette(const char *name);
	void SetPalette(const RGB *pal2);

	bool LoadBMP(const char *name);
	bool LoadBMP(const char *name, PALETTE pal);

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

	int FormatPixel(int x,int y);
	void PseudoCopy(int x,int y,byte* data,int len);

	int xRes,yRes,bpp,pitch;
	byte windowed;
	bool readyToQuit;
	byte tapTrack;

	int mouse_x, mouse_y, mouse_z, mouse_b;

protected:

	byte* scrn;
	PALETTE pal;
	dword elapsedTime;
	dword now;
	int numFrames;
	char lastKeyPressed;
	int lastRawCode;
	byte mouseDown,rMouseDown;

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;

	int *buffer;
};

#endif
