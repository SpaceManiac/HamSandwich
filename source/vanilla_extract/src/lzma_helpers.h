#ifndef VANILLA_EXTRACT_LZMA_HELPERS_H
#define VANILLA_EXTRACT_LZMA_HELPERS_H

#include <vector>
#include <stdint.h>
#include <stddef.h>

struct ISzAlloc;
typedef const ISzAlloc* ISzAllocPtr;

namespace vanilla
{
	extern const ISzAllocPtr LZMA_ALLOCATOR;
	bool lzma_decompress(std::vector<uint8_t>& dest, uint8_t* src, size_t srclen, size_t propsize);
}

#endif  // VANILLA_EXTRACT_LZMA_HELPERS_H
