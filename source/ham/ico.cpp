#include "ico.h"
#include <utility>
#include <SDL_image.h>

struct IconDirEntry
{
	int width, height, colorCount;
	Uint16 planes, bitCount;
	Uint32 size, offset;
};

owned::SDL_Surface ReadIcoFile(SDL_RWops* src, int targetSize)
{
	Sint64 fp_offset = SDL_RWtell(src);
	SDL_ClearError();

	Uint16 bfReserved = SDL_ReadLE16(src);
	Uint16 bfType = SDL_ReadLE16(src);
	Uint16 bfCount = SDL_ReadLE16(src);
    if ((bfReserved != 0) || (bfType != 1 && bfType != 2) || (bfCount == 0))
	{
        SDL_SetError("File is not a Windows ICO or CUR file");
		SDL_RWseek(src, fp_offset, RW_SEEK_SET);
		return {};
    }

	std::vector<IconDirEntry> entries;
	entries.reserve(bfCount);

	for (int i = 0; i < bfCount; ++i)
	{
		IconDirEntry entry;
		entry.width = SDL_ReadU8(src);
		entry.height = SDL_ReadU8(src);
		entry.colorCount = SDL_ReadU8(src);
        /* reserved = */ SDL_ReadU8(src);
        entry.planes = SDL_ReadLE16(src);
        entry.bitCount = SDL_ReadLE16(src);
        entry.size = SDL_ReadLE32(src);
        entry.offset = SDL_ReadLE32(src);

        if (!entry.width)
            entry.width = 256;
        if (!entry.height)
            entry.height = 256;
        if (!entry.colorCount)
            entry.colorCount = 256;

		entries.push_back(entry);
	}

	std::vector<owned::SDL_Surface> surfaces;
	surfaces.reserve(bfCount);

	for (int i = 0; i < bfCount; ++i)
	{
		SDL_RWseek(src, entries[i].offset, RW_SEEK_SET);
		// Try to load it as a PNG with IMG_Load_RW. If it fails, too bad.
		owned::SDL_Surface surface { IMG_Load_RW(src, SDL_FALSE) };
		if (surface)
			surfaces.push_back(std::move(surface));
	}

	if (surfaces.empty())
	{
		// If we failed to load any PNGs, fall back to SDL_image's ico.
		// Can't load all compressions, but it's something.
		SDL_RWseek(src, fp_offset, RW_SEEK_SET);
		owned::SDL_Surface surface { IMG_LoadICO_RW(src) };
		if (surface)
			surfaces.push_back(std::move(surface));
	}

	if (targetSize > 0)
	{
		for (auto& surface : surfaces)
		{
			if (surface->w >= targetSize)
			{
				return std::move(surface);
			}
		}
	}
	return surfaces.empty() ? nullptr : std::move(surfaces.back());
}

// Parts of this file are based on the ICO reader from SDL2_image/IMG_bmp.c:
/*
  SDL_image:  An example image loading library for use with SDL
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
