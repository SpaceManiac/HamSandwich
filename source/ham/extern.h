#ifndef HAM_EXTERN_H
#define HAM_EXTERN_H

#include "jamultypes.h"
#include "owned_sdl.h"

// Callbacks that games may provide to Ham.
extern struct HamExtern
{
	void (*AfterFlip)();

	// ---- hammusic ----
	// Should call PlaySongFile or StopSong, or the current song will loop.
	void (*ChooseNextSong)();

	// ---- jamulsound ----
	owned::SDL_IOStream (*SoundLoadOverride)(int);
} HAM_API g_HamExtern;

#endif  // HAM_EXTERN_H
