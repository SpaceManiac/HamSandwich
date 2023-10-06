// LunaticPal v2, 2022
// Based on LunaticPal v1 released on Wombat in 2013.
// Converts any image SDL_image can read into a .bmp in the Dr. L palette.

#include <vector>
#include <fstream>
#include <zlib.h>
#include <stdint.h>

const uint8_t palette[256][3] =
{
	#include "lunaticpal_values.inl"
};

uint8_t convert_color(uint8_t r1, uint8_t g1, uint8_t b1)
{
	uint8_t best = 0;
	/*
	LunaticPal used to have a bug where `best_dist` was an int instead of
	a float or double. Fixing this changed 4130 mappings (0.025%), of which
	only 520 were changed by more than one palette entry.
	*/
	float best_dist = 0;
	for (int i = 1; i < 256; ++i)
	{
		uint8_t r2 = palette[i][0], g2 = palette[i][1], b2 = palette[i][2];

		/*
		Needlessly micro-optimized by using floats instead of doubles and carefully
		choosing the order of operations. Tested not to change the output on my machine.
		*/
		float rmean = (r1 + r2) / 2.0f;
		int r = r1 - r2;
		int g = g1 - g2;
		int b = b1 - b2;
		float weightR = 2 + rmean / 256.0f;
		int weightG = 4;
		float weightB = 2 + (255 - rmean) / 256.0f;
		float dist = r * r * weightR + g * g * weightG + b * b * weightB;

		if (dist < best_dist || best == 0)
		{
			best = i;
			best_dist = dist;
		}
		if (dist == 0)
			break;
	}

	return best;
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "usage: lunaticpal_precompute <output-path>\n");
		return 1;
	}
	const char* out_fname = argv[1];

	// ------------------------------------------------------------------------
	// Compute the table.

	std::vector<uint8_t> table(256 * 256 * 256);
	uint8_t* p = table.data();
	for (int r = 0; r < 256; ++r)
	{
		for (int g = 0; g < 256; ++g)
		{
			for (int b = 0; b < 256; ++b)
			{
				*p++ = convert_color(r, g, b);
			}
		}
	}

	// ------------------------------------------------------------------------
	// Print the CRC32.
	const uint32_t expected_crc = 0xcd310532;
	uint32_t crc = crc32(crc32(0L, Z_NULL, 0), table.data(), table.size());
	if (crc != expected_crc)
	{
		fprintf(stderr, "lunaticpal_precompute: expected crc32=%08x, got crc32=%08x\n", expected_crc, crc);
		return 1;
	}

	// ------------------------------------------------------------------------
	// Compress it with zlib deflate.

	// Allocate a big-enough buffer that we can definitely deflate in one shot.
	std::vector<uint8_t> compressed(table.size());
	z_stream zip {};
	zip.next_out = compressed.data();
	zip.avail_out = compressed.size();
	zip.next_in = table.data();
	zip.avail_in = table.size();

	if (int r = deflateInit(&zip, Z_DEFAULT_COMPRESSION); r != Z_OK)
	{
		fprintf(stderr, "lunaticpal_precompute: deflateInit failed: %d: %s\n", r, zip.msg);
		return 1;
	}
	if (int r = deflate(&zip, Z_FINISH); r != Z_STREAM_END)
	{
		fprintf(stderr, "lunaticpal_precompute: bad deflate: %d: %s\n", r, zip.msg);
		return 1;
	}
	size_t compressed_written = compressed.size() - zip.avail_out;
	if (int r = deflateEnd(&zip); r != Z_OK)
	{
		fprintf(stderr, "lunaticpal_precompute: bad deflateEnd: %d: %s\n", r, zip.msg);
		return 1;
	}

	// ------------------------------------------------------------------------
	// Write it to the output file.

	std::ofstream out(out_fname, std::ios_base::binary);
	out.write((const char*)compressed.data(), compressed_written);
#if 0
	// Debugging variant: write uncompressed.
	std::string alt = out_fname;
	alt.append(".uncompressed");
	std::ofstream out2(alt, std::ios_base::binary);
	out2.write((const char*)table.data(), table.size());
#endif

	return 0;
}
