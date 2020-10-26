#ifndef LIBEXTRACT_LZMA_HELPERS_H
#define LIBEXTRACT_LZMA_HELPERS_H

#include <vector>
#include <stdint.h>

typedef struct ISzAlloc ISzAlloc;
typedef const ISzAlloc * ISzAllocPtr;

namespace lzma_helpers {

extern const ISzAllocPtr allocator;

bool decompress_all(std::vector<uint8_t>& dest, uint8_t* src, size_t srclen, size_t propsize);

}  // namespace lzma_helpers

#endif  // LIBEXTRACT_LZMA_HELPERS_H
