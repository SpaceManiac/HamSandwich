#include <string.h>
#include <vector>
#include <zlib.h>
#include "lzma_helpers.h"
#include "vec_rw.h"
#include "lzma1900/7z.h"
#include "lzma1900/7zCrc.h"
#include "inno.h"

#ifdef SDL_UNPREFIXED
	#include <SDL_rwops.h>
#else  // SDL_UNPREFIXED
	#include <SDL2/SDL_rwops.h>
#endif  // SDL_UNPREFIXED

namespace inno {

// ----------------------------------------------------------------------------
// Constants

const size_t OFFSET_OF_7Z_IN_EXE = 120908;

const size_t SETUP_0_SZ = 8;
const char16_t SETUP_0[SETUP_0_SZ] = u"setup.0";
const size_t SETUP_1_BIN_SZ = 12;
const char16_t SETUP_1_BIN[SETUP_1_BIN_SZ] = u"setup-1.bin";

const char INNO_VERSION[] = "Inno Setup Setup Data (4.0.5)";

// ----------------------------------------------------------------------------
// 7z-compatible stream from FILE*

struct FilePtrStream
{
	ISeekInStream base;
	FILE *fptr;
};

SRes FilePtrStream_Read(const ISeekInStream *p, void *buf, size_t *size)
{
	const FilePtrStream *self = (const FilePtrStream *) p;
	*size = fread(buf, 1, *size, self->fptr);
	return SZ_OK;
}

SRes FilePtrStream_Seek(const ISeekInStream *p, Int64 *pos, ESzSeek origin)
{
	const FilePtrStream *self = (const FilePtrStream *) p;
	if (fseek(self->fptr, *pos, origin))
		return SZ_ERROR_READ;
	*pos = ftell(self->fptr);
	return SZ_OK;
}

// ----------------------------------------------------------------------------
// Decoding the file list

static bool crc_table_generated = false;

static SDL_RWops* zlib_crc_block_reader(uint8_t** input_buffer);

Archive::~Archive()
{
}

Archive::Archive(FILE* fptr)
{
	if (fseek(fptr, OFFSET_OF_7Z_IN_EXE, SEEK_CUR))
		return;

	uint8_t signature[k7zSignatureSize];
	if (!fread(signature, k7zSignatureSize, 1, fptr))
		return;
	if (fseek(fptr, -k7zSignatureSize, SEEK_CUR))
		return;

	if (memcmp(signature, k7zSignature, k7zSignatureSize))
	{
		fprintf(stderr, "inno::Archive: 7z signature missing at offset 0x%zx", OFFSET_OF_7Z_IN_EXE);
		return;
	}

	// Prepare 7z input stream
	FilePtrStream seekStream =
	{
		{
			FilePtrStream_Read,
			FilePtrStream_Seek,
		},
		fptr
	};

	uint8_t stream2buf[16 * 1024];
	CLookToRead2 stream2;
	stream2.realStream = &seekStream.base;
	LookToRead2_CreateVTable(&stream2, 0);
	LookToRead2_Init(&stream2);
	stream2.buf = stream2buf;
	stream2.bufSize = sizeof(stream2buf);
	ILookInStream* stream = &stream2.vt;

	// Read archive header
	if (!crc_table_generated)
	{
		CrcGenerateTable();
		crc_table_generated = true;
	}

	CSzArEx zip;
	SzArEx_Init(&zip);
	SRes res = SzArEx_Open(&zip, stream, lzma_helpers::allocator, lzma_helpers::allocator);
	if (res != SZ_OK)
	{
		fprintf(stderr, "inno::Archive: SzArEx_Open failed: %d\n", res);
		return;
	}

	uint32_t fileIndex_setup_0 = UINT32_MAX;
	uint32_t fileIndex_setup_1_bin = UINT32_MAX;

	// Examine archive file list
	for (uint32_t fileIndex = 0; fileIndex < zip.NumFiles; ++fileIndex)
	{
		const size_t BUFFER_SZ = 32;
		uint16_t filename_utf16[BUFFER_SZ];
		size_t size = SzArEx_GetFileNameUtf16(&zip, fileIndex, nullptr);
		if (size > BUFFER_SZ)
			continue;
		SzArEx_GetFileNameUtf16(&zip, fileIndex, filename_utf16);

		if (size == SETUP_0_SZ && !memcmp(filename_utf16, SETUP_0, 2 * SETUP_0_SZ))
			fileIndex_setup_0 = fileIndex;
		else if (size == SETUP_1_BIN_SZ && !memcmp(filename_utf16, SETUP_1_BIN, 2 * SETUP_1_BIN_SZ))
			fileIndex_setup_1_bin = fileIndex;
	}

	if (fileIndex_setup_0 == UINT32_MAX || fileIndex_setup_1_bin == UINT32_MAX)
	{
		fprintf(stderr, "inno::Archive: archive is missing setup.0 or setup-1.bin\n");
		return;
	}

	// Extract setup.0 into memory.
	uint32_t blockIndex = 0;
	uint8_t* outBuffer = nullptr;
	size_t outBufferSize = 0;
	size_t offset;
	size_t outSizeProcessed;
	res = SzArEx_Extract(
		&zip,
		stream,
		fileIndex_setup_0,
		&blockIndex,
		&outBuffer,
		&outBufferSize,
		&offset,
		&outSizeProcessed,
		lzma_helpers::allocator,
		lzma_helpers::allocator);
	if (res != SZ_OK)
	{
		fprintf(stderr, "inno::Archive: SzArEx_Extract setup.0 failed: %d\n", res);
		return;
	}

	// Parse it.
	uint8_t *position = &outBuffer[offset], *end = &outBuffer[offset + outSizeProcessed];
	if (memcmp(&outBuffer[offset], INNO_VERSION, sizeof(INNO_VERSION)))
	{
		fprintf(stderr, "inno::Archive: bad inno version %s\n", &outBuffer[offset]);
		return;
	}
	position += 64;

	SDL_RWops* headers = zlib_crc_block_reader(&position);

	printf("cool\n");
}

// ----------------------------------------------------------------------------
// Zlib helpers

SDL_RWops* zlib_crc_block_reader(uint8_t** input_buffer)
{
	uint32_t header_crc32, compressed_size, uncompressed_size;
	memcpy(&header_crc32, *input_buffer, 4); *input_buffer += 4;
	memcpy(&compressed_size, *input_buffer, 4); *input_buffer += 4;
	memcpy(&uncompressed_size, *input_buffer, 4); *input_buffer += 4;

	std::vector<uint8_t> uncompressed(uncompressed_size);
	z_stream zip {};
	zip.next_out = uncompressed.data();
	zip.avail_out = uncompressed.size();

	bool first = true;
	while (compressed_size > 0)
	{
		uint32_t block_crc32;
		memcpy(&block_crc32, *input_buffer, 4); *input_buffer += 4;

		uint32_t to_read = std::min(compressed_size, 4096u);
		zip.next_in = *input_buffer;
		zip.avail_in = to_read;

		if (first)
		{
			if (inflateInit(&zip) != Z_OK)
			{
				fprintf(stderr, "zlib_crc_block_reader: bad inflateInit\n");
				return nullptr;
			}
			first = false;
		}
		int r = inflate(&zip, Z_NO_FLUSH);
		if (r != Z_OK && r != Z_STREAM_END)
		{
			fprintf(stderr, "zlib_crc_block_reader: bad inflate: %d\n", r);
			return nullptr;
		}

		compressed_size -= to_read;
		*input_buffer += to_read;
	}
	if (inflate(&zip, Z_FINISH) != Z_STREAM_END)
	{
		fprintf(stderr, "zlib_crc_block_reader: bad flush\n");
		return nullptr;
	}
	if (inflateEnd(&zip) != Z_OK)
	{
		fprintf(stderr, "zlib_crc_block_reader: bad inflateEnd\n");
		return nullptr;
	}

	return create_vec_rwops(std::move(uncompressed));
}

}  // namespace inno
