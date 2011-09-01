/* MGLDraw

   A moderately cute little wrapper around MGL.

   For quick reference, to use MGL in a program, you must:

   link in gm.lib and mglfx.lib as the first two things in the list of things to link in.
   also add in the project options thing under that: /nodefaultlib:libc.lib
     to avoid a dumb warning.

*/

#ifndef MGLDRAW_H
#define MGLDRAW_H

#include "gm\gm.h"
#include "winpch.h"
#include "jamulsound.h"
#include "control.h"

class MGLDraw
{
	public:
		MGLDraw(char *name,int xRes,int yRes,int bpp,bool window,HINSTANCE hInst);
		~MGLDraw(void);

		bool Process(void);	// handle windows messages and such

		HWND GetHWnd(void);
		MGLDC *GetDC(void);
		byte *GetScreen(void); // get a pointer to the screen memory
		int GetWidth(void);
		int GetHeight(void);
		void ClearScreen(void);
		void Flip(void);
		void Quit(void);

		bool LoadPalette(char *name);
		void SetPalette(palette_t *pal2);
		void RealizePalette(void);
		void DarkPalette(void);

		bool LoadBMP(char *name);

		char LastKeyPressed(void);
		char LastKeyPeek(void);
		void SetLastKey(char c);

		void GammaCorrect(byte gamma);

		// handy little drawing routines
		void Box(int x,int y,int x2,int y2,byte c);
		void FillBox(int x,int y,int x2,int y2,byte c);

		// functions to measure frame rate
		void ResetTimer(void);
		void TickTimer(void);
		float FrameRate(void);

		// mouse functions
		byte MouseDown(void);
		void SetMouseDown(byte w);

	protected:
		int xRes,yRes,bpp,pitch;
		byte *scrn;
		GMDC *gm;
		palette_t pal[256];
		bool readyToQuit;
		dword elapsedTime;
		dword now;
		int numFrames;
		char lastKeyPressed;
		byte mouseDown;
};

// ignore these functions.  They are only outside of the protected portion of the MGLDraw
// class because I don't know how to make a function pointer point to a member function.
int ASMAPI MGLDraw_Suspend(MGLDC *dc,int flags);
void MGLDraw_Activate(bool active);
void MGLDraw_SwitchModes(GM_modeInfo *mode,bool windowed);
long FAR PASCAL MGLDraw_EventHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND MGLGetHWnd(void);	// augh.

#endif
