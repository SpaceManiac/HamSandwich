#include "ico.h"
#include <SDL3_image/SDL_image.h>

owned::SDL_Surface ReadIcoFile(SDL_IOStream* src, int targetSize)
{
	owned::SDL_Surface surface { IMG_Load_IO(src, false) };
	if (!surface)
	{
		return nullptr;
	}
	int count;
	owned::SdlMem<SDL_Surface*[]> alternates { SDL_GetSurfaceImages(surface.get(), &count) };
	if (!alternates || count < 1)
	{
		return surface;
	}
	SDL_Surface* largest = alternates[count - 1];
	if (targetSize > 0)
	{
		for (int i = 0; i < count; ++i)
		{
			if (alternates[i]->w >= targetSize)
			{
				largest = alternates[i];
				break;
			}
		}
	}
	return owned::SDL_Surface { SDL_DuplicateSurface(largest) };
}
