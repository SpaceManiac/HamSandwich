#ifndef OWNED_MIXER_H
#define OWNED_MIXER_H

#include <memory>
#include <SDL_mixer.h>
#include "owned_sdl.h"

// ----------------------------------------------------------------------------
// Mix_Chunk

namespace owned
{
	namespace _deleter
	{
		struct Mix_Chunk
		{
			void operator()(::Mix_Chunk* ptr) { return Mix_FreeChunk(ptr); }
		};
	}

	typedef std::unique_ptr<::Mix_Chunk, _deleter::Mix_Chunk> Mix_Chunk;

	inline Mix_Chunk Mix_LoadWAV_RW(::SDL_RWops* rw)
	{
		return Mix_Chunk { ::Mix_LoadWAV_RW(rw, SDL_FALSE) };
	}

	inline Mix_Chunk Mix_LoadWAV_RW(owned::SDL_RWops rw)
	{
		return Mix_Chunk { ::Mix_LoadWAV_RW(rw.release(), SDL_TRUE) };
	}
}

// ----------------------------------------------------------------------------
// Mix_Music

namespace owned
{
	namespace _deleter
	{
		struct Mix_Music
		{
			void operator()(::Mix_Music* ptr) { return Mix_FreeMusic(ptr); }
		};
	}

	typedef std::unique_ptr<::Mix_Music, _deleter::Mix_Music> Mix_Music;

	inline Mix_Music Mix_LoadMUS_RW(::SDL_RWops* rw)
	{
		return Mix_Music { ::Mix_LoadMUS_RW(rw, SDL_FALSE) };
	}

	inline Mix_Music Mix_LoadMUS_RW(owned::SDL_RWops rw)
	{
		return Mix_Music { ::Mix_LoadMUS_RW(rw.release(), SDL_TRUE) };
	}

	inline Mix_Music Mix_LoadMUSType_RW(::SDL_RWops* rw, Mix_MusicType type)
	{
		return Mix_Music { ::Mix_LoadMUSType_RW(rw, type, SDL_FALSE) };
	}

	inline Mix_Music Mix_LoadMUSType_RW(owned::SDL_RWops rw, Mix_MusicType type)
	{
		return Mix_Music { ::Mix_LoadMUSType_RW(rw.release(), type, SDL_TRUE) };
	}
}

#endif
