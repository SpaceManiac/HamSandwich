#include <stdlib.h>
#include "lzma1900/LzmaDec.h"
#include "lzma_helpers.h"

namespace lzma_helpers {

void* lz_alloc(ISzAllocPtr p, size_t sz) {
	return malloc(sz);
}

void lz_free(ISzAllocPtr p, void *address) {
	return free(address);
}

const ISzAlloc alloc = { lz_alloc, lz_free };

bool decompress_all(std::vector<uint8_t>& dest, uint8_t* src, size_t srclen, size_t propsize) {
	const size_t BLOCKSIZE = 16 * 1024;

	CLzmaDec decoder;
	LzmaDec_Construct(&decoder);
	if (LzmaDec_AllocateProbs(&decoder, src, propsize, &alloc) != SZ_OK)
		return false;
	src += propsize;
	srclen -= propsize;

	dest.resize(BLOCKSIZE);
	decoder.dic = dest.data();
	decoder.dicBufSize = dest.size();
	LzmaDec_Init(&decoder);

	ELzmaStatus status;
	size_t read = srclen;
	int res = LzmaDec_DecodeToDic(&decoder, decoder.dicBufSize, src, &read, ELzmaFinishMode::LZMA_FINISH_ANY, &status);
	while (res == SZ_OK && status != LZMA_STATUS_FINISHED_WITH_MARK) {
		src += read;
		srclen -= read;
		read = srclen;
		if (!srclen)
			return false;

		dest.resize(dest.size() + BLOCKSIZE);
		decoder.dic = dest.data();
		decoder.dicBufSize = dest.size();

		res = LzmaDec_DecodeToDic(&decoder, decoder.dicBufSize, src, &read, ELzmaFinishMode::LZMA_FINISH_ANY, &status);
	}
	dest.resize(decoder.dicPos);
	LzmaDec_FreeProbs(&decoder, &alloc);
	return res == SZ_OK;
}

}  // namespace lzma_helpers
