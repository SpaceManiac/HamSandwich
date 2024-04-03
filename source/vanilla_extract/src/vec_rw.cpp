#include "vec_rw.h"

#include <SDL_rwops.h>

namespace vanilla
{

	struct Vec_RWops
	{
		SDL_RWops base;
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

	static int delete_vec_close(SDL_RWops *rw)
	{
		delete (Vec_RWops *)rw;
		return 0;
	}

	owned::SDL_RWops create_vec_rwops(std::vector<uint8_t> &&buffer)
	{
		// Use a modified RWFromConstMem so the JS audio code knows how to slurp it.
		SDL_RWops *rw_base = SDL_RWFromConstMem(buffer.data(), buffer.size());
		if (!rw_base)
		{
			// SDL_RWFromConstMem returns null for size 0 buffers. It would make
			// more sense to return an empty stream IMO, but we should at least
			// not crash.
			return nullptr;
		}
		Vec_RWops *rw = new Vec_RWops{*rw_base, std::move(buffer)};
		SDL_FreeRW(rw_base);
		rw->base.close = delete_vec_close;
		return owned::SDL_RWops{&rw->base};
	}

} // namespace vanilla
