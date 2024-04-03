#include "lunaticpal.h"
#include <stdio.h>
#include <vector>
#include <utility>
#include <SDL.h>
#define ZLIB_CONST
#include <zlib.h>

extern const unsigned char embed_compressed_lookup_table[];
extern const size_t embed_compressed_lookup_table_size;

namespace
{
	const uint8_t palette[256][3] =
	{
		#include "lunaticpal_values.inl"
	};

	bool lookup_table_loaded = false;
	std::vector<uint8_t> lookup_table;
}

SDL_Color lunaticpal::GetColor(uint8_t index)
{
	if (index == 0)
	{
		return { 0, 0, 0, 0 }; // transparent
	}
	return { palette[index][0], palette[index][1], palette[index][2], 255 };
}

static bool LoadTable()
{
	std::vector<uint8_t> local_table(256 * 256 * 256);

	// Decompress lookup table from embedded deflate blob.
	z_stream zip {};
	zip.next_out = local_table.data();
	zip.avail_out = local_table.size();
#ifdef z_const
	zip.next_in = embed_compressed_lookup_table;
#else
	// Inefficient, but feels less scummy than const_cast.
	std::vector<uint8_t> copy { embed_compressed_lookup_table, embed_compressed_lookup_table + embed_compressed_lookup_table_size };
	zip.next_in = copy.data();
#endif
	zip.avail_in = embed_compressed_lookup_table_size;

	if (int r = inflateInit(&zip); r != Z_OK)
	{
		fprintf(stderr, "lunaticpal::LoadTable: bad inflateInit: %d: %s\n", r, zip.msg);
		return false;
	}
	if (int r = inflate(&zip, Z_FINISH); r != Z_STREAM_END)
	{
		fprintf(stderr, "lunaticpal::LoadTable: bad inflate: %d: %s\n", r, zip.msg);
		return false;
	}
	if (int r = inflateEnd(&zip); r != Z_OK)
	{
		fprintf(stderr, "lunaticpal::LoadTable: bad inflateEnd: %d: %s\n", r, zip.msg);
		return false;
	}

	lookup_table = std::move(local_table);
	return true;
}

uint8_t lunaticpal::GetNearest(SDL_Color color)
{
	if (color.a < 128)
	{
		return 0;  // transparent
	}

	if (!lookup_table_loaded)
	{
		if (!LoadTable())
		{
			abort();
		}
		lookup_table_loaded = true;
	}

	return lookup_table[(color.r << 16) | (color.g << 8) | (color.b)];
}

bool lunaticpal::ReduceImage(SDL_Surface *image)
{
    bool result = false;

    SDL_LockSurface(image);
    SDL_Color *pixels = (SDL_Color *) image->pixels;
    int pitch = image->pitch / sizeof(SDL_Color);

    for (int y = 0; y < image->h; ++y)
	{
        for (int x = 0; x < image->w; ++x)
		{
            SDL_Color original = pixels[y * pitch + x];
            uint8_t index = GetNearest(original);
            SDL_Color actual = GetColor(index);
            if (memcmp(&original, &actual, sizeof(SDL_Color)))
			{
                pixels[y * pitch + x] = actual;
                result = true;
            }
        }
    }

    SDL_UnlockSurface(image);
    return result;
}
