#include "jamulfmv.h"
#include <allegro.h>
#include <winalleg.h>
#include <allegavi.h>
#include "display.h"

byte ShouldExitFlic(MGLDraw *mgl)
{
	if(mgl->MouseTap())
		return 1;
	if(mgl->LastKeyPressed())
		return 1;
	if(GetTaps())
		return 1;

	return 0;
}

void FLIToScreen(void)
{
	if(fli_pal_dirty_from<=fli_pal_dirty_to)
	{
		// palette has changed
		set_palette(fli_palette);
	}

	acquire_screen();

	if(fli_bitmap->w==640 && fli_bitmap->h==480)	// fits the screen size, no need to stretch
	{
		blit(fli_bitmap,screen,0,0,0,0,640,480);
	}
	else
	{
		stretch_blit(fli_bitmap,screen,0,0,fli_bitmap->w,fli_bitmap->h,0,0,640,480);
	}

	release_screen();

	reset_fli_variables();
}

void FLI_play(char *name,byte loop,word wait,MGLDraw *mgl)
{
	int err;
	dword start,now;

	start=timeGetTime();
	ShouldExitFlic(mgl);	// check the keys, to clear previous presses

	err=open_fli(name);
	if(err!=FLI_OK)
		return;	// can't play it, load it, something

	mgl->ClearScreen();
	mgl->Flip();

	while(1)
	{
		now=timeGetTime();
		while(fli_timer==0)
		{
			FLIToScreen();
			if(ShouldExitFlic(mgl) && (now-start)>1000)	// can only quit out after 1 second has passed
				break;
		}
		err=next_fli_frame(0);

		if(err==FLI_EOF)
			break;	// done
	}

	close_fli();

	mgl->ClearScreen();
	mgl->Flip();
	mgl->RealizePalette();
}

/*
int open_fli(const char *filename);
int open_memory_fli(const void *fli_data);
Open FLI files ready for playing, reading the data from disk or memory respectively.
Return FLI_OK on success. Information about the current FLI is held in global variables,
so you can only have one animation open at a time.

void close_fli();
Closes an FLI file when you have finished reading from it.

int next_fli_frame(int loop);
Reads the next frame of the current animation file.
If loop is set the player will cycle when it reaches the end of the file, otherwise it will
 return FLI_EOF. Returns FLI_OK on success, FLI_ERROR or FLI_NOT_OPEN on error, and FLI_EOF on
 reaching the end of the file. The frame is read into the global variables fli_bitmap and fli_palette.

extern BITMAP *fli_bitmap;
Contains the current frame of the FLI/FLC animation.

extern PALETTE fli_palette;
Contains the current FLI palette.

extern int fli_bmp_dirty_from;
extern int fli_bmp_dirty_to;
These variables are set by next_fli_frame() to indicate which part of the
fli_bitmap has changed since the last call to reset_fli_variables(). If fli_bmp_dirty_from
 is greater than fli_bmp_dirty_to, the bitmap has not changed, otherwise lines fli_bmp_dirty_from
 to fli_bmp_dirty_to (inclusive) have altered. You can use these when copying the fli_bitmap onto
 the screen, to avoid moving data unnecessarily.

extern int fli_pal_dirty_from;
extern int fli_pal_dirty_to;
These variables are set by next_fli_frame() to indicate which part of the fli_palette
has changed since the last call to reset_fli_variables(). If fli_pal_dirty_from is greater than
fli_pal_dirty_to, the palette has not changed, otherwise colors fli_pal_dirty_from to
fli_pal_dirty_to (inclusive) have altered. You can use these when updating the hardware palette,
to avoid unnecessary calls to set_palette().

void reset_fli_variables();
Once you have done whatever you are going to do with the fli_bitmap and fli_palette, call this function
 to reset the fli_bmp_dirty_* and fli_pal_dirty_* variables.

extern int fli_frame;
Global variable containing the current frame number in the FLI file. This is useful for synchronising other events with the animation, for instance you could check it in a play_fli() callback function and use it to trigger a sample at a particular point.

extern volatile int fli_timer;
Global variable for timing FLI playback. When you open an FLI file, a timer interrupt is installed
which increments this variable every time a new frame should be displayed. Calling next_fli_frame()
decrements it, so you can test it and know that it is time to display a new frame if it is greater than zero.

*/
