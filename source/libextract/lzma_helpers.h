#ifndef LIBEXTRACT_LZMA_HELPERS_H
#define LIBEXTRACT_LZMA_HELPERS_H

#include <vector>
#include <stdint.h>

namespace lzma_helpers {

bool decompress_all(std::vector<uint8_t>& dest, uint8_t* src, size_t srclen, size_t propsize);

}  // namespace lzma_helpers

#endif  // LIBEXTRACT_LZMA_HELPERS_H
