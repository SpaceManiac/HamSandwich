#include "audiofx.h"
#include <stdlib.h>
#include <memory.h>
#include <utility>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "jamultypes.h"

#if 0 // SDL3 TODO

static int SampleSize(uint16_t format)
{
	return SDL_AUDIO_BITSIZE(format) / 8;
}

owned::MIX_Audio FxBackwards(MIX_Audio* sample)
{
	int channels;
	uint16_t format;
	Mix_QuerySpec(nullptr, &format, &channels);

	byte* buf = (byte*) SDL_malloc(sample->alen);

	byte *from = &sample->abuf[sample->alen], *to = buf;
	int each_len = SampleSize(format) * channels;
	int num_samples = sample->alen / each_len;
	for (int i = 0; i < num_samples; ++i)
	{
		from -= each_len;
		memcpy(to, from, each_len);
		to += each_len;
	}

	owned::MIX_Audio output { Mix_QuickLoad_RAW(buf, sample->alen) };
	output->allocated = true;
	return output;
}

#endif
