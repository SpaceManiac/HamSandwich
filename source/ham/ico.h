#ifndef ICO_H
#define ICO_H

#include <vector>
#include <memory>
#include "owned_sdl.h"

// Get the largest PNG icon at or above the target size, or if not found, the highest bit-depth icon that SDL_image understands.
owned::SDL_Surface ReadIcoFile(SDL_RWops* rw, int targetSize);

inline owned::SDL_Surface ReadIcoFile(const owned::SDL_RWops& rw, int targetSize)
{
	return ReadIcoFile(rw.get(), targetSize);
}

#endif // ICO_H
