#ifndef NEXT_SOUNDFILE_H
#define NEXT_SOUNDFILE_H

#include "owned_mixer.h"

// Load a NeXT/Sun soundfile.
owned::Mix_Chunk LoadNextSoundfile(SDL_RWops *rw);

#endif  // NEXT_SOUNDFILE_H
