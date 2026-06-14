#ifndef OWNED_MIXER_H
#define OWNED_MIXER_H

#include <memory>
#include <SDL3_mixer/SDL_mixer.h>
#include "owned_sdl.h"

// ----------------------------------------------------------------------------
// MIX_Mixer

namespace owned
{
	namespace _deleter
	{
		struct MIX_Mixer
		{
			void operator()(::MIX_Mixer* ptr) { return MIX_DestroyMixer(ptr); }
		};
	}

	typedef std::unique_ptr<::MIX_Mixer, _deleter::MIX_Mixer> MIX_Mixer;

	inline MIX_Mixer MIX_CreateMixerDevice(SDL_AudioDeviceID devid, const SDL_AudioSpec *spec)
	{
		return MIX_Mixer { ::MIX_CreateMixerDevice(devid, spec) };
	}
}

// ----------------------------------------------------------------------------
// MIX_Audio

namespace owned
{
	namespace _deleter
	{
		struct MIX_Audio
		{
			void operator()(::MIX_Audio* ptr) { return MIX_DestroyAudio(ptr); }
		};
	}

	typedef std::unique_ptr<::MIX_Audio, _deleter::MIX_Audio> MIX_Audio;

	inline MIX_Audio MIX_LoadAudio_IO(::MIX_Mixer* mixer, ::SDL_IOStream* io, bool predecode)
	{
		return MIX_Audio { ::MIX_LoadAudio_IO(mixer, io, predecode, false) };
	}

	inline MIX_Audio MIX_LoadAudio_IO(::MIX_Mixer* mixer, owned::SDL_IOStream io, bool predecode)
	{
		return MIX_Audio { ::MIX_LoadAudio_IO(mixer, io.release(), predecode, true) };
	}
}

// ----------------------------------------------------------------------------
// MIX_Track

namespace owned
{
	namespace _deleter
	{
		struct MIX_Track
		{
			void operator()(::MIX_Track* ptr) { return MIX_DestroyTrack(ptr); }
		};
	}

	typedef std::unique_ptr<::MIX_Track, _deleter::MIX_Track> MIX_Track;

	inline MIX_Track MIX_CreateTrack(::MIX_Mixer* mixer)
	{
		return MIX_Track { ::MIX_CreateTrack(mixer) };
	}
}

// ----------------------------------------------------------------------------
// MIX_AudioDecoder

namespace owned
{
	namespace _deleter
	{
		struct MIX_AudioDecoder
		{
			void operator()(::MIX_AudioDecoder* ptr) { return MIX_DestroyAudioDecoder(ptr); }
		};
	}

	typedef std::unique_ptr<::MIX_AudioDecoder, _deleter::MIX_AudioDecoder> MIX_AudioDecoder;

	inline MIX_AudioDecoder MIX_CreateAudioDecoder_IO(::SDL_IOStream *io, SDL_PropertiesID props)
	{
		return MIX_AudioDecoder { ::MIX_CreateAudioDecoder_IO(io, false, props) };
	}

	inline MIX_AudioDecoder MIX_CreateAudioDecoder_IO(owned::SDL_IOStream io, SDL_PropertiesID props)
	{
		return MIX_AudioDecoder { ::MIX_CreateAudioDecoder_IO(io.release(), true, props) };
	}
}

#endif
