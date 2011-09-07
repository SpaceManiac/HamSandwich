/* MGLDraw

   A moderately cute little wrapper around MGL.

   -- No longer actually an MGL wrapper! --
   -- Currently using PixelToaster --

   For quick reference, to use MGL in a program, you must:

   link in gm.lib and mglfx.lib as the first two things in the list of things to link in.
   also add in the project options thing under that: /nodefaultlib:libc.lib
     to avoid a dumb warning.
*/

#ifndef MGLDRAW_H
#define MGLDRAW_H

#include "PixelToaster.h"
#include "winpch.h"
#include "jamulsound.h"
#include "control.h"

// Replacement for missing palette_t
typedef struct palette_t {
    byte alpha, red, green, blue;
} palette_t;

// Replacement for missing MGL functions
int MGL_random(int max);
long MGL_randoml(long max);
void MGL_fatalError(const char* txt);

class PtListener;

class MGLDraw
{
    friend class PtListener;
	public:
		MGLDraw(const char *name,int xRes,int yRes,int bpp,bool window,HINSTANCE hInst);
		~MGLDraw(void);

        void ReopenWindow();

		void FatalError(char *msg);

		bool Process(void);	// handle windows messages and such

		HWND GetHWnd(void);
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

		bool LoadBMP(const char *name);

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
        void SetMouse(int x, int y);
        void TeleportMouse(int x, int y);
        void GetMouse(int *x, int *y);

	protected:
		int xRes,yRes,bpp,pitch;
        int mousex,mousey;
		byte *scrn;
        PixelToaster::Display ptDisplay;
        PtListener* ptListener;
        PixelToaster::TrueColorPixel *ptBuffer;
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

long FAR PASCAL MGLDraw_EventHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam); // needed by jamulsound
HWND MGLGetHWnd(void);	// needed by music ("augh.")

#endif
