#ifndef HAM_EXTERN_H
#define HAM_EXTERN_H

#include "jamultypes.h"
#include "owned_sdl.h"

// Callbacks that games may provide to Ham.
extern HAM_API struct HamExtern
{
	void (*AfterFlip)();

	// ---- hammusic ----
	// Should call PlaySongFile or StopSong, or the current song will loop.
	void (*ChooseNextSong)();

	// ---- jamulsound ----
	owned::SDL_IOStream (*SoundLoadOverride)(int);
} g_HamExtern;

#endif  // HAM_EXTERN_H
