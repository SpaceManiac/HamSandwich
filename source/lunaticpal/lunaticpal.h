#ifndef LUNATICPAL_H

#include <stdint.h>
#include <SDL_pixels.h>

struct SDL_Surface;

namespace lunaticpal
{

	SDL_Color GetColor(uint8_t index);
	uint8_t GetNearest(SDL_Color color);

	bool ReduceImage(SDL_Surface *image);

}

#endif  // LUNATICPAL_H
