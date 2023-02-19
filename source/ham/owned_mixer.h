#ifndef OWNED_MIXER_H
#define OWNED_MIXER_H

#include <memory>
#include <SDL_mixer.h>

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
}

#endif
