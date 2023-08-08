#ifndef HAM_EXTERN_H
#define HAM_EXTERN_H

#include "owned_sdl.h"

// Callbacks that games may provide to Ham.
extern struct HamExtern
{
	// ---- hammusic ----
	// Should call PlaySongFile or StopSong, or the current song will loop.
	void (*ChooseNextSong)();

	// ---- jamulsound ----
	owned::SDL_RWops (*SoundLoadOverride)(int);
} g_HamExtern;

#endif  // HAM_EXTERN_H
