#include <string.h>
#include <vector>
#include "lzma_helpers.h"
#include "lzma1900/7z.h"
#include "lzma1900/7zCrc.h"
#include "inno.h"

namespace inno {

// ----------------------------------------------------------------------------
// Constants

const size_t OFFSET_OF_7Z_IN_EXE = 120908;

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

	CrcGenerateTable();

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

	CSzArEx zip;
	SzArEx_Init(&zip);
	if (int res = SzArEx_Open(&zip, stream, lzma_helpers::allocator, lzma_helpers::allocator); res != SZ_OK)
	{
		fprintf(stderr, "inno::Archive: SzArEx_Open failed: %d\n", res);
		return;
	}

	for (uint32_t fileIndex = 0; fileIndex < zip.NumFiles; ++fileIndex)
	{
		const size_t BUFFER_SZ = 64;
		uint16_t filename_utf16[BUFFER_SZ];
		size_t size = SzArEx_GetFileNameUtf16(&zip, fileIndex, nullptr);
		if (size > BUFFER_SZ)
			continue;
		SzArEx_GetFileNameUtf16(&zip, fileIndex, filename_utf16);

		uint8_t filename_ascii[BUFFER_SZ + 1];
		for (size_t i = 0; i < size; ++i)
		{
			filename_ascii[i] = filename_utf16[i] & 0x7f;
		}
		filename_ascii[size] = 0;

		printf("%s\n", filename_ascii);
	}

	printf("cool\n");
}

}  // namespace inno
