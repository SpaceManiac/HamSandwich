#include "lzma_helpers.h"
#include <LzmaDec.h>
#include <stdlib.h>
#include <stdio.h>

// ----------------------------------------------------------------------------
// LZMA decompression utils

static void* lz_alloc(ISzAllocPtr p, size_t sz)
{
	(void) p;
	return malloc(sz);
}

static void lz_free(ISzAllocPtr p, void *address)
{
	(void) p;
	return free(address);
}

static const ISzAlloc alloc = { lz_alloc, lz_free };
extern const ISzAllocPtr vanilla::LZMA_ALLOCATOR = &alloc;

const size_t BUFSIZE_IN = 16 * 1024;
const size_t BUFSIZE_OUT = 2 * BUFSIZE_IN;

bool vanilla::lzma_decompress(std::vector<uint8_t>& dest, uint8_t* src, size_t srclen, size_t propsize)
{
	CLzmaDec decoder;
	LzmaDec_Construct(&decoder);
	if (LzmaDec_AllocateProbs(&decoder, src, propsize, &alloc) != SZ_OK)
	{
		fprintf(stderr, "LzmaDec_AllocateProbs failed\n");
		return false;
	}
	src += propsize;
	srclen -= propsize;

	dest.resize(BUFSIZE_OUT);
	decoder.dic = dest.data();
	decoder.dicBufSize = dest.size();
	LzmaDec_Init(&decoder);

	ELzmaStatus status;
	size_t read = srclen;
	int res = LzmaDec_DecodeToDic(&decoder, decoder.dicBufSize, src, &read, ELzmaFinishMode::LZMA_FINISH_ANY, &status);
	while (res == SZ_OK && status != LZMA_STATUS_FINISHED_WITH_MARK)
	{
		src += read;
		srclen -= read;
		read = srclen;
		if (!read)
			return false;

		dest.resize(dest.size() + BUFSIZE_OUT);
		decoder.dic = dest.data();
		decoder.dicBufSize = dest.size();

		res = LzmaDec_DecodeToDic(&decoder, decoder.dicBufSize, src, &read, ELzmaFinishMode::LZMA_FINISH_ANY, &status);
	}
	dest.resize(decoder.dicPos);
	LzmaDec_FreeProbs(&decoder, &alloc);
	return res == SZ_OK;
}
