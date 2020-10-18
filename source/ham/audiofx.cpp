#include "audiofx.h"
#include "jamultypes.h"

#ifdef SDL_UNPREFIXED
	#include <SDL.h>
	#include <SDL_mixer.h>
#else  // SDL_UNPREFIXED
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_mixer.h>
#endif  // SDL_UNPREFIXED

static int SampleSize(uint16_t format)
{
	return SDL_AUDIO_BITSIZE(format) / 8;
}

Mix_Chunk* FxRandomPitch(Mix_Chunk* sample)
{
	int freq, channels;
	uint16_t format;
	Mix_QuerySpec(&freq, &format, &channels);

	int new_freq = freq - (freq / 5) + (rand() % ((freq / 5) * 2 + 1));

	SDL_AudioCVT cvt;
	SDL_BuildAudioCVT(&cvt, format, channels, freq, format, channels, new_freq);
	cvt.len = sample->alen;
	cvt.buf = (byte*) SDL_malloc(cvt.len * cvt.len_mult);
	memcpy(cvt.buf, sample->abuf, sample->alen);
	SDL_ConvertAudio(&cvt);

	Mix_Chunk* output = Mix_QuickLoad_RAW(cvt.buf, cvt.len_cvt);
	output->allocated = true;
	return output;
}

Mix_Chunk* FxBackwards(Mix_Chunk* sample)
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

	Mix_Chunk* output = Mix_QuickLoad_RAW(buf, sample->alen);
	output->allocated = true;
	return output;
}

Mix_Chunk* FxDoubleSpeed(Mix_Chunk* sample)
{
	int channels;
	uint16_t format;
	Mix_QuerySpec(nullptr, &format, &channels);

	int new_len = sample->alen / 2;
	byte* buf = (byte*) SDL_malloc(new_len);

	int each_len = SampleSize(format) * channels;
	int num_samples = sample->alen / each_len / 2;
	for (int i = 0; i < num_samples; ++i)
	{
		memcpy(&buf[i * each_len], &sample->abuf[2 * i * each_len], each_len);
	}

	Mix_Chunk* output = Mix_QuickLoad_RAW(buf, new_len);
	output->allocated = true;
	return output;
}
