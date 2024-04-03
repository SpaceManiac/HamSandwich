#include "vfs_inno.h"
#include <string.h>
#include <vector>
#include <zlib.h>
#include <7z.h>
#include <7zCrc.h>
#include <SDL_rwops.h>
#include "base_archive.h"
#include "lzma_helpers.h"
#include "vec_rw.h"

// ----------------------------------------------------------------------------
// Inno VFS implementation

class InnoVfs : public vanilla::Vfs
{
	struct DataEntry
	{
		uint32_t chunk_offset;
		uint64_t file_size;
		uint64_t chunk_size;
	};

	vanilla::Archive archive;
	std::vector<DataEntry> data_entries;
	std::vector<uint8_t> setup_1_bin;

public:
	explicit InnoVfs(SDL_RWops* rw);

	bool is_ok()
	{
		return setup_1_bin.size();
	}

	owned::SDL_RWops open_sdl(const char* filename) override;
	bool list_dir(const char* directory, std::set<std::string, vanilla::CaseInsensitive>& output) override;
};

std::unique_ptr<vanilla::Vfs> vanilla::open_inno(SDL_RWops* rw)
{
	return std::make_unique<InnoVfs>(rw);
}

bool InnoVfs::list_dir(const char* directory, std::set<std::string, vanilla::CaseInsensitive>& output)
{
	return archive.list_dir(directory, output);
}

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
// 7z-compatible stream from SDL_RWops*

struct SeekInStream_RW
{
	ISeekInStream base;
	SDL_RWops *rw;
};

static SRes SeekInStream_RW_Read(const ISeekInStream *p, void *buf, size_t *size)
{
	const SeekInStream_RW *self = (const SeekInStream_RW *) p;
	*size = SDL_RWread(self->rw, buf, 1, *size);
	return SZ_OK;
}

// Skip writing a switch by directly mapping the seek types.
static_assert(SZ_SEEK_SET == RW_SEEK_SET);
static_assert(SZ_SEEK_CUR == RW_SEEK_CUR);
static_assert(SZ_SEEK_END == RW_SEEK_END);
static SRes SeekInStream_RW_Seek(const ISeekInStream *p, Int64 *pos, ESzSeek origin)
{
	const SeekInStream_RW *self = (const SeekInStream_RW *) p;
	int ret = SDL_RWseek(self->rw, *pos, origin);
	if (ret < 0)
		return SZ_ERROR_READ;
	*pos = ret;
	return SZ_OK;
}

// ----------------------------------------------------------------------------
// Inno setup data types

static owned::SDL_RWops zlib_crc_block_reader(uint8_t** input_buffer)
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
			if (int r = inflateInit(&zip); r != Z_OK)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "zlib_crc_block_reader: bad inflateInit: %d: %s", r, zip.msg);
				return nullptr;
			}
			first = false;
		}
		if (int r = inflate(&zip, Z_NO_FLUSH); r != Z_OK && r != Z_STREAM_END)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "zlib_crc_block_reader: bad inflate: %d: %s", r, zip.msg);
			return nullptr;
		}

		compressed_size -= to_read;
		*input_buffer += to_read;
	}
	if (int r = inflate(&zip, Z_FINISH); r != Z_STREAM_END)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "zlib_crc_block_reader: bad finish: %d: %s", r, zip.msg);
		return nullptr;
	}
	if (int r = inflateEnd(&zip); r != Z_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "zlib_crc_block_reader: bad inflateEnd: %d: %s", r, zip.msg);
		return nullptr;
	}

	return vanilla::create_vec_rwops(std::move(uncompressed));
}

static void binary_string(SDL_RWops* rw, std::string* dest = nullptr)
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
// RAII units

struct OwnedCSzArEx : CSzArEx
{
	OwnedCSzArEx() { SzArEx_Init(this); }
	~OwnedCSzArEx() { SzArEx_Free(this, vanilla::LZMA_ALLOCATOR); }
	OwnedCSzArEx(const OwnedCSzArEx&) = delete;
	OwnedCSzArEx(OwnedCSzArEx&&) = delete;
	OwnedCSzArEx& operator=(const OwnedCSzArEx&) = delete;
	OwnedCSzArEx& operator=(OwnedCSzArEx&&) = delete;
};

struct LzmaOutBuffer
{
	uint8_t* raw;
	LzmaOutBuffer() : raw(nullptr) {}
	~LzmaOutBuffer() { vanilla::LZMA_ALLOCATOR->Free(vanilla::LZMA_ALLOCATOR, raw); }
	LzmaOutBuffer(const LzmaOutBuffer&) = delete;
	LzmaOutBuffer(LzmaOutBuffer&&) = delete;
	LzmaOutBuffer& operator=(const LzmaOutBuffer&) = delete;
	LzmaOutBuffer& operator=(LzmaOutBuffer&&) = delete;
};

// ----------------------------------------------------------------------------
// Decoding the file list

static bool crc_table_generated = false;

InnoVfs::InnoVfs(SDL_RWops* rw)
{
	if (SDL_RWseek(rw, OFFSET_OF_7Z_IN_EXE, RW_SEEK_CUR) < 0)
		return;

	uint8_t signature[k7zSignatureSize];
	if (!SDL_RWread(rw, signature, k7zSignatureSize, 1))
		return;
	if (SDL_RWseek(rw, -k7zSignatureSize, RW_SEEK_CUR) < 0)
		return;

	if (memcmp(signature, k7zSignature, k7zSignatureSize))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InnoVfs: 7z signature missing at offset 0x%x", (unsigned int)OFFSET_OF_7Z_IN_EXE);
		return;
	}

	// Prepare 7z input stream
	SeekInStream_RW seekStream =
	{
		{
			SeekInStream_RW_Read,
			SeekInStream_RW_Seek,
		},
		rw
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

	OwnedCSzArEx zip;
	SRes res = SzArEx_Open(&zip, stream, vanilla::LZMA_ALLOCATOR, vanilla::LZMA_ALLOCATOR);
	if (res != SZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InnoVfs: SzArEx_Open failed: %d", res);
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
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InnoVfs: archive is missing setup.0 or setup-1.bin");
		return;
	}

	// Extract setup.0 into memory.
	uint32_t blockIndex = 0;
	LzmaOutBuffer outBuffer;
	size_t outBufferSize = 0;
	size_t offset;
	size_t outSizeProcessed;
	res = SzArEx_Extract(
		&zip,
		stream,
		fileIndex_setup_0,
		&blockIndex,
		&outBuffer.raw,
		&outBufferSize,
		&offset,
		&outSizeProcessed,
		vanilla::LZMA_ALLOCATOR,
		vanilla::LZMA_ALLOCATOR);
	if (res != SZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InnoVfs: SzArEx_Extract setup.0 failed: %d", res);
		return;
	}

	// Parse it.
	uint8_t *position = &outBuffer.raw[offset], *end = &outBuffer.raw[offset + outSizeProcessed];
	if (memcmp(position, INNO_VERSION, sizeof(INNO_VERSION)))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InnoVfs: bad inno version %s", position);
		return;
	}
	position += 64;

	owned::SDL_RWops headers2 = zlib_crc_block_reader(&position);
	SDL_RWops* headers = headers2.get();
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
	SDL_RWseek(headers, 4 * 1, RW_SEEK_CUR);  // directory_count
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
			vanilla::Archive::Directory* current = &archive.root;
			if (const char* last_component = vanilla::Archive::navigate(destination.c_str() + APP_PREFIX_SZ, current))
			{
				current->files.insert(make_pair(std::string(last_component), location));
			}
		}
	}
	headers2.reset();

	// Parse the second zlib'd block containing the data entries.
	owned::SDL_RWops datas = zlib_crc_block_reader(&position);
	if (position != end)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InnoVfs: extra data at end of setup.0");
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
	datas.reset();

	// Extract setup-1.bin into memory.
	res = SzArEx_Extract(
		&zip,
		stream,
		fileIndex_setup_1_bin,
		&blockIndex,
		&outBuffer.raw,
		&outBufferSize,
		&offset,
		&outSizeProcessed,
		vanilla::LZMA_ALLOCATOR,
		vanilla::LZMA_ALLOCATOR);
	if (res != SZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InnoVfs: SzArEx_Extract setup-1.bin failed: %d", res);
		return;
	}

	setup_1_bin.assign(outBuffer.raw + offset, outBuffer.raw + offset + outSizeProcessed);
}

owned::SDL_RWops InnoVfs::open_sdl(const char* path)
{
	size_t data_entry_idx = archive.get_file(path);
	if (data_entry_idx == SIZE_MAX)
		return nullptr;

	DataEntry entry = data_entries.at(data_entry_idx);

	if (entry.chunk_size + 11 == entry.file_size)
	{
		// Skip decompression if contents are nothing but
		// [magic number:4][zlib header][file]
		return owned::SDL_RWFromConstMem(&setup_1_bin[entry.chunk_offset + 11], entry.file_size);
	}

	std::vector<uint8_t> uncompressed(entry.file_size);
	z_stream zip {};
	zip.next_out = uncompressed.data();
	zip.avail_out = uncompressed.size();
	// Skip 4 bytes of "zlb\x1a" magic number
	zip.next_in = &setup_1_bin[entry.chunk_offset + 4];
	zip.avail_in = entry.chunk_size - 4;

	if (int r = inflateInit(&zip); r != Z_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InnoVfs::open_sdl(%s): bad inflateInit: %d: %s", path, r, zip.msg);
		return nullptr;
	}
	if (int r = inflate(&zip, Z_NO_FLUSH); r != Z_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InnoVfs::open_sdl(%s): bad inflate: %d: %s", path, r, zip.msg);
		return nullptr;
	}
	if (int r = inflateEnd(&zip); r != Z_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InnoVfs::open_sdl(%s): bad inflateEnd: %d: %s", path, r, zip.msg);
		return nullptr;
	}

	return vanilla::create_vec_rwops(std::move(uncompressed));
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
