/* MGLDraw

  hacked all to hell to be in allegro instead.

*/

#ifndef MGLDRAW_H
#define MGLDRAW_H

#include <allegro.h>
#include <winalleg.h>
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include "jamultypes.h"
#include "jamulsound.h"
#include "control.h"

#define SCRWID	640
#define SCRHEI  480

// if I'm not mistaken, this number is NOT used by windows as a window event
#define INTERNET_EVENT	(WM_USER+1)

#define MAX_RUNS	(3)

class MGLDraw
{
	public:
        MGLDraw(char *name,int xRes,int yRes,int bpp,bool window);
		~MGLDraw(void);

		bool Process(void);	// handle windows messages and such

		HWND GetHWnd(void);
		byte *GetScreen(void); // get a pointer to the screen memory
		int GetWidth(void);
		int GetHeight(void);
		void ClearScreen(void);
		void Quit(void);

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

		bool LoadBMP(char *name);
		bool LoadBMPNoPalette(char *name);

		int  LastRawCode(void);
		char LastKeyPressed(void);
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

		int xRes,yRes,bpp,pitch;
        byte windowed;
		bool readyToQuit;
		byte tapTrack;
	protected:
		int windowX,windowY;

		BITMAP* buffer;
		byte *scrn;
		PALETTE pal, pal2, *thePal;
		dword elapsedTime;
		dword now;
		int numFrames;
		char lastKeyPressed;
		int lastRawCode;
		byte mouseDown,rMouseDown;
};

extern MGLDraw *_globalMGLDraw;
HWND MGLGetHWnd(void);	// augh.

void SeedRNG(void);
dword Random(dword range);
void FatalError(char *msg);

long FAR PASCAL MyWindowsEventHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void FatalErrorQuit(void);

#endif
