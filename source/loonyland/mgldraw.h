/* MGLDraw

  hacked all to hell to be in SDL2 instead.

*/

#ifndef MGLDRAW_H
#define MGLDRAW_H

#include <SDL2/SDL.h>
#ifdef _WIN32
#include "winpch.h"
#endif
#include <stdio.h>
#include "jamultypes.h"
#include "jamulsound.h"
#include "control.h"
#include "debuggo.h"

#define SCRWID	640
#define SCRHEI  480

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

// if I'm not mistaken, this number is NOT used by windows as a window event
#define INTERNET_EVENT	(WM_USER+1)

#define MAX_RUNS	(3)

class MGLDraw
{
	public:
		MGLDraw(char *name,int xRes,int yRes,int bpp,bool window);
		~MGLDraw(void);

		bool Process(void);	// handle windows messages and such

		byte *GetScreen(void); // get a pointer to the screen memory
		int GetWidth(void);
		int GetHeight(void);
		void ClearScreen(void);
		void Quit(void);

		void StartFlip(void);
		void FinishFlip(void);
		void Flip(void);
		void WaterFlip(int v);
		void TeensyFlip(void);
		void TeensyWaterFlip(int v);
		void RasterFlip(void);
		void RasterWaterFlip(int v);

		bool LoadPalette(char *name);
		void SetPalette(PALETTE pal2);
		RGB *GetPalette(void);

		void RealizePalette(void);
		void DarkPalette(void);
		void CyclePalette(void);
		void GreyPalette(void);

		bool LoadBMP(char *name);
		bool LoadBMP(char *name, PALETTE pal);

		int LastKeyPressed(void);
		char LastKeyPeek(void);
		void SetLastKey(char c);
		void ClearKeys(void);

		void WaterPalette(byte b);

		// handy little drawing routines
		void Box(int x,int y,int x2,int y2,byte c);
		void FillBox(int x,int y,int x2,int y2,byte c);
		void SelectLineH(int x,int x2,int y,byte ofs);
		void SelectLineV(int x,int y,int y2,byte ofs);

		// functions to measure frame rate
		void ResetTimer(void);
		void TickTimer(void);
		float FrameRate(void);

		// mouse functions
		byte MouseDown();
		byte RMouseDown();
		byte MouseTap();
		byte RMouseTap();
		void SetMouseDown(byte w);
		void SetRMouseDown(byte w);
		void GetMouse(int *x,int *y);
		void SetMouse(int x,int y);

		int FormatPixel(int x,int y);
		void PseudoCopy(int x,int y,byte* data,int len);

		void ResetGM(void);

#ifdef _WIN32
		HWND GetHWnd(void);
#endif

		int xRes,yRes,bpp,pitch;
		byte windowed;
		bool readyToQuit;
		byte tapTrack;

		int mouse_x, mouse_y, mouse_z, mouse_b;
	protected:

		byte *scrn;
		PALETTE pal, pal2, *thePal;
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

void SeedRNG(void);
dword Random(dword range);
void FatalError(char *msg);

#endif
