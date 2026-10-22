#include "audiofx.h"
#include "jamultypes.h"
#include "log.h"
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

owned::MIX_Audio FxBackwards(SDL_IOStream *file, MIX_Mixer *mixer)
{
	// Decode everything
	SDL_AudioSpec spec;
	if (!MIX_GetMixerFormat(mixer, &spec))
	{
		LogError("MIX_GetMixerFormat: %s", SDL_GetError());
		return nullptr;
	}

	owned::MIX_AudioDecoder decoder = owned::MIX_CreateAudioDecoder_IO(file, 0);
	if (!decoder)
	{
		LogError("MIX_CreateAudioDecoder_IO: %s", SDL_GetError());
		return nullptr;
	}

	std::vector<uint8_t> pcm;
	while (true)
	{
		size_t start = pcm.size();
		size_t end = (start + 4096) * 2;
		pcm.resize(end);
		int read = MIX_DecodeAudio(decoder.get(), &pcm[start], end - start, &spec);
		if (read < 0)
		{
			LogError("MIX_DecodeAudio: %s", SDL_GetError());
			return nullptr;
		}
		pcm.resize(start + read);
		if (read == 0)
		{
			break;
		}
	}

	// Reverse it
	int each_len = SDL_AUDIO_BYTESIZE(spec.format);
	int num_samples = pcm.size() / each_len;
	for (int i = 0; i < num_samples / 2; ++i)
	{
		std::swap_ranges(&pcm[i * each_len], &pcm[(i + 1) * each_len], &pcm[(num_samples - 1 - i) * each_len]);
	}

	// Load it back
	return owned::MIX_Audio { MIX_LoadRawAudio(mixer, &pcm[0], pcm.size(), &spec) };
}
