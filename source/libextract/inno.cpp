#include <algorithm>
#include <string.h>
#include <vector>
#include <zlib.h>
#include "common.h"
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
const size_t APP_PREFIX_SZ = 6;
const char APP_PREFIX[APP_PREFIX_SZ + 1] = "{app}\\";

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
// Inno setup data types

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

void binary_string(SDL_RWops* rw, std::string* dest = nullptr)
{
	uint32_t size;
	SDL_RWread(rw, &size, 4, 1);
	if (dest)
	{
		dest->resize(size);
		SDL_RWread(rw, dest->data(), size, 1);
	}
	else
	{
		SDL_RWseek(rw, size, RW_SEEK_CUR);
	}
}

// ----------------------------------------------------------------------------
// Decoding the file list

static bool crc_table_generated = false;

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
	SRes res = SzArEx_Open(&zip, stream, sauce::allocator, sauce::allocator);
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
		sauce::allocator,
		sauce::allocator);
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
	binary_string(headers);  // app_name
	binary_string(headers);  // app_versioned_name
	binary_string(headers);  // app_id
	binary_string(headers);  // app_copyright
	binary_string(headers);  // app_publisher
	binary_string(headers);  // app_publisher_url
	binary_string(headers);  // app_support_url
	binary_string(headers);  // app_updates_url
	binary_string(headers);  // app_version
	binary_string(headers);  // default_dir_name
	binary_string(headers);  // default_group_name
	binary_string(headers);  // base_filename
	binary_string(headers);  // license_text
	binary_string(headers);  // info_before
	binary_string(headers);  // info_after
	binary_string(headers);  // uninstall_files_dir
	binary_string(headers);  // uninstall_name
	binary_string(headers);  // uninstall_icon
	binary_string(headers);  // app_mutex
	binary_string(headers);  // default_user_name
	binary_string(headers);  // default_user_organisation
	binary_string(headers);  // default_serial
	binary_string(headers);  // compiled_code
	SDL_RWseek(headers, 32, RW_SEEK_CUR);  // lead_bytes

	uint32_t language_count, task_count, file_count, data_entry_count;
	SDL_RWread(headers, &language_count, 4, 1);
	SDL_RWseek(headers, 4 * 2, RW_SEEK_CUR);  // type_count, component_count
	SDL_RWread(headers, &task_count, 4, 1);
	SDL_RWseek(headers, 4 * 1, RW_SEEK_CUR);  // task_count
	SDL_RWread(headers, &file_count, 4, 1);
	SDL_RWread(headers, &data_entry_count, 4, 1);
	SDL_RWseek(headers, 91, RW_SEEK_CUR);

	// skip languages
	for (uint32_t i = 0; i < language_count; ++i)
	{
		binary_string(headers);  // name
		binary_string(headers);  // language_name
		binary_string(headers);  // dialog_font
		binary_string(headers);  // title_font
		binary_string(headers);  // welcome_font
		binary_string(headers);  // copyright_font
		binary_string(headers);  // data
		binary_string(headers);  // license_text
		binary_string(headers);  // info_before
		binary_string(headers);  // info_after
		SDL_RWseek(headers, 4 * 6, RW_SEEK_CUR);
	}
	// assume 0 messages, permissions, types, components
	// skip tasks
	for (uint32_t i = 0; i < task_count; ++i)
	{
		binary_string(headers);  // name
		binary_string(headers);  // description
		binary_string(headers);  // group_description
		binary_string(headers);  // components
		binary_string(headers);  // languages
		binary_string(headers);  // check
		SDL_RWseek(headers, 4 + 1 + (2 * 4 + 2) + 1, RW_SEEK_CUR);
	}
	// assume 0 directories
	SDL_RWseek(headers, 79, RW_SEEK_CUR);

	// read file entries
	for (uint32_t i = 0; i < file_count; ++i)
	{
		std::string source, destination;
		binary_string(headers, &source);
		binary_string(headers, &destination);
		binary_string(headers);  // install_font_name
		binary_string(headers);  // condition.components
		binary_string(headers);  // condition.tasks
		binary_string(headers);  // condition.languages
		binary_string(headers);  // condition.check
		SDL_RWseek(headers, 2 * (2 * 4 + 2), RW_SEEK_CUR);  // windows_version_range
		uint32_t location;
		SDL_RWread(headers, &location, 4, 1);
		SDL_RWseek(headers, 17, RW_SEEK_CUR);

		if (source.empty() && !memcmp(destination.data(), APP_PREFIX, APP_PREFIX_SZ))
		{
			Directory* current = &root;
			if (const char* last_component = navigate(destination.c_str() + APP_PREFIX_SZ, current))
			{
				current->files.insert(make_pair(std::string(last_component), location));
			}
		}
	}
	SDL_RWclose(headers);

	// Parse the second zlib'd block containing the data entries.
	SDL_RWops* datas = zlib_crc_block_reader(&position);
	if (position != end)
	{
		fprintf(stderr, "inno::Archive: extra data at end of setup.0\n");
		SDL_RWclose(datas);
		return;
	}

	data_entries.resize(data_entry_count);
	for (uint32_t i = 0; i < data_entry_count; ++i)
	{
		SDL_RWseek(datas, 4 * 2, RW_SEEK_CUR);  // first_slice, last_slice
		SDL_RWread(datas, &data_entries[i].chunk_offset, 4, 1);
		SDL_RWseek(datas, 8, RW_SEEK_CUR);  // file_offset
		SDL_RWread(datas, &data_entries[i].file_size, 8, 1);
		SDL_RWread(datas, &data_entries[i].chunk_size, 8, 1);
		SDL_RWseek(datas, 4 + 8 + 8 + 1, RW_SEEK_CUR);
	}
	SDL_RWclose(datas);

	// Extract setup-1.bin into memory.
	res = SzArEx_Extract(
		&zip,
		stream,
		fileIndex_setup_1_bin,
		&blockIndex,
		&outBuffer,
		&outBufferSize,
		&offset,
		&outSizeProcessed,
		sauce::allocator,
		sauce::allocator);
	if (res != SZ_OK)
	{
		fprintf(stderr, "inno::Archive: SzArEx_Extract setup-1.bin failed: %d\n", res);
		return;
	}

	setup_1_bin.assign(outBuffer + offset, outBuffer + offset + outSizeProcessed);

	// Clean up.
	sauce::allocator->Free(sauce::allocator, outBuffer);
	SzArEx_Free(&zip, sauce::allocator);
}

SDL_RWops* Archive::open_file(const char* path)
{
	size_t data_entry_idx = get_file(path);
	if (data_entry_idx == SIZE_MAX)
		return nullptr;

	DataEntry entry = data_entries.at(data_entry_idx);

	if (entry.chunk_size + 11 == entry.file_size)
	{
		// Skip decompression if contents are nothing but
		// [magic number:4][zlib header][file]
		return SDL_RWFromConstMem(&setup_1_bin[entry.chunk_offset + 11], entry.file_size);
	}

	std::vector<uint8_t> uncompressed(entry.file_size);
	z_stream zip {};
	zip.next_out = uncompressed.data();
	zip.avail_out = uncompressed.size();
	// Skip 4 bytes of "zlb\x1a" magic number
	zip.next_in = &setup_1_bin[entry.chunk_offset + 4];
	zip.avail_in = entry.chunk_size - 4;

	if (inflateInit(&zip) != Z_OK)
	{
		fprintf(stderr, "inno::Archive::open_file: bad inflateInit\n");
		return nullptr;
	}
	if (int r = inflate(&zip, Z_NO_FLUSH); r != Z_OK && r != Z_STREAM_END)
	{
		fprintf(stderr, "inno::Archive::open_file: bad inflate: %d\n", r);
		return nullptr;
	}
	if (inflateEnd(&zip) != Z_OK)
	{
		fprintf(stderr, "inno::Archive::open_file: bad inflateEnd\n");
		return nullptr;
	}

	return create_vec_rwops(std::move(uncompressed));
}

/*
Some of the above is derived in part from Innoextract, under the zlib/libpng
license. <https://github.com/dscharrer/innoextract>

Copyright (C) 2011-2020 Daniel Scharrer <daniel@constexpr.org>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the author(s) be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

}  // namespace inno
