#include "vec_rw.h"
#include <utility>
#include <SDL3/SDL_iostream.h>

namespace vanilla
{

	struct Vec_RWops
	{
		SDL_IOStream base;
		std::vector<uint8_t> vec;

		void *operator new(size_t size)
		{
			return SDL_malloc(size);
		}

		void operator delete(void *ptr)
		{
			return SDL_free(ptr);
		}
	};

	static int delete_vec_close(SDL_IOStream *rw)
	{
		delete (Vec_RWops *)rw;
		return 0;
	}

	owned::SDL_IOStream create_vec_rwops(std::vector<uint8_t> &&buffer)
	{
		// Use a modified RWFromConstMem so the JS audio code knows how to slurp it.
		SDL_IOStream *rw_base = SDL_IOFromConstMem(buffer.data(), buffer.size());
		if (!rw_base)
		{
			// SDL_IOFromConstMem returns null for size 0 buffers. It would make
			// more sense to return an empty stream IMO, but we should at least
			// not crash.
			return nullptr;
		}
		Vec_RWops *rw = new Vec_RWops{*rw_base, std::move(buffer)};
		SDL_FreeRW(rw_base);
		rw->base.close = delete_vec_close;
		return owned::SDL_IOStream{&rw->base};
	}

} // namespace vanilla
