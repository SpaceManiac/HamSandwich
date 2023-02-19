#ifndef VANILLA_EXTRACT_VEC_RW_H
#define VANILLA_EXTRACT_VEC_RW_H

#include <vector>
#include <stdint.h>
#include "owned_sdl.h"

namespace vanilla
{

	// Create an SDL_RWops from an owned byte buffer. It will be freed when closed.
	owned::SDL_RWops create_vec_rwops(std::vector<uint8_t>&& buffer);

}  // namespace vanilla

#endif  // VANILLA_EXTRACT_VEC_RW_H
