#include "vfs_inno3.h"
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

class Inno3Vfs : public vanilla::Vfs
{
	struct DataEntry
	{
		uint32_t chunk_offset;
		uint32_t file_size;
		uint32_t chunk_size;
	};

	vanilla::Archive archive;
	std::vector<DataEntry> data_entries;
	owned::SDL_RWops file;
	size_t net_data_offset;

public:
	explicit Inno3Vfs(owned::SDL_RWops rw);

	bool is_ok()
	{
		return file != nullptr;
	}

	owned::SDL_RWops open_sdl(const char* filename) override;
	bool list_dir(const char* directory, std::set<std::string, vanilla::CaseInsensitive>& output) override;
};

std::unique_ptr<vanilla::Vfs> vanilla::open_inno3(owned::SDL_RWops rw)
{
	return std::make_unique<Inno3Vfs>(std::move(rw));
}

bool Inno3Vfs::list_dir(const char* directory, std::set<std::string, vanilla::CaseInsensitive>& output)
{
	return archive.list_dir(directory, output);
}

// ----------------------------------------------------------------------------
// Inno setup data types

static owned::SDL_RWops zlib_crc_block_reader(SDL_RWops* input)
{
	uint32_t header_crc32, compressed_size, uncompressed_size;
	SDL_RWread(input, &header_crc32, 4, 1);
	SDL_RWread(input, &compressed_size, 4, 1);
	SDL_RWread(input, &uncompressed_size, 4, 1);

	std::vector<uint8_t> uncompressed(uncompressed_size);
	z_stream zip {};
	zip.next_out = uncompressed.data();
	zip.avail_out = uncompressed.size();

	bool first = true;
	while (compressed_size > 0)
	{
		uint32_t block_crc32;
		SDL_RWread(input, &block_crc32, 4, 1);

		uint8_t buf[4096];
		uint32_t to_read = std::min(compressed_size, uint32_t(sizeof(buf)));
		SDL_RWread(input, buf, to_read, 1);
		zip.next_in = buf;
		zip.avail_in = to_read;

		if (first)
		{
			if (int r = inflateInit(&zip); r != Z_OK)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "zlib_crc_block_reader: bad inflateInit: %d", r);
				return nullptr;
			}
			first = false;
		}
		if (int r = inflate(&zip, Z_NO_FLUSH); r != Z_OK && r != Z_STREAM_END)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "zlib_crc_block_reader: bad inflate: %d", r);
			return nullptr;
		}

		compressed_size -= to_read;
	}
	if (int r = inflate(&zip, Z_FINISH); r != Z_STREAM_END)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "zlib_crc_block_reader: bad flush: %d", r);
		return nullptr;
	}
	if (int r = inflateEnd(&zip); r != Z_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "zlib_crc_block_reader: bad inflateInit: %d", r);
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
// Constants

const size_t OFFSET_OF_INNO_IN_EXE = 0x30;
const size_t INNO_MAGIC_SZ = 4;
const char INNO_MAGIC[INNO_MAGIC_SZ + 1] = "Inno";

const size_t LOADER_HEADER_MAGIC_SZ = 12;
const unsigned char LOADER_HEADER_MAGIC[LOADER_HEADER_MAGIC_SZ] = { 'r', 'D', 'l', 'P', 't', 'S', '0', '2', 0x87, 'e', 'V', 'x' };  // 1.2.10

const char INNO_VERSION[] = "Inno Setup Setup Data (3.0.3)";
const size_t APP_PREFIX_SZ = 6;
const char APP_PREFIX[APP_PREFIX_SZ + 1] = "{app}\\";

// ----------------------------------------------------------------------------
// Decoding the file list

Inno3Vfs::Inno3Vfs(owned::SDL_RWops rw)
{
	int64_t start = SDL_RWtell(rw);
	if (SDL_RWseek(rw, OFFSET_OF_INNO_IN_EXE, RW_SEEK_CUR) < 0)
		return;

	uint8_t signature[INNO_MAGIC_SZ];
	if (!SDL_RWread(rw, signature, INNO_MAGIC_SZ, 1))
		return;

	if (memcmp(signature, INNO_MAGIC, INNO_MAGIC_SZ))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Inno3Vfs: Inno signature missing at offset 0x%zx", OFFSET_OF_INNO_IN_EXE);
		return;
	}

	uint32_t offset, notOffset;
	SDL_RWread(rw, &offset, 4, 1);
	SDL_RWread(rw, &notOffset, 4, 1);
	if (offset != ~notOffset)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Inno3Vfs: apparent offset 0x%x != ~%x", offset, notOffset);
		return;
	}

	SDL_RWseek(rw, start + offset, RW_SEEK_SET);
	uint8_t signature2[LOADER_HEADER_MAGIC_SZ];
	SDL_RWread(rw, signature2, LOADER_HEADER_MAGIC_SZ, 1);
	if (memcmp(signature2, LOADER_HEADER_MAGIC, LOADER_HEADER_MAGIC_SZ))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Inno3Vfs: loader signature missing at offset 0x%x", offset);
		return;
	}

	SDL_RWseek(rw, 24, RW_SEEK_CUR);  // various offsets we don't care about
	uint32_t header_offset, data_offset;
	SDL_RWread(rw, &header_offset, 4, 1);
	SDL_RWread(rw, &data_offset, 4, 1);

	SDL_RWseek(rw, start + header_offset, RW_SEEK_SET);
	uint8_t signature3[sizeof(INNO_VERSION)];
	SDL_RWread(rw, signature3, sizeof(INNO_VERSION), 1);
	if (memcmp(signature3, INNO_VERSION, sizeof(INNO_VERSION)))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Inno3Vfs: bad inno version %s", signature3);
		return;
	}

	SDL_RWseek(rw, start + header_offset + 64, RW_SEEK_SET);

	owned::SDL_RWops headers2 = zlib_crc_block_reader(rw.get());
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
	SDL_RWseek(headers, 32, RW_SEEK_CUR);  // lead_bytes

	uint32_t language_count, task_count, file_count, data_entry_count, directory_count;
	language_count = 1;
	SDL_RWseek(headers, 4 * 2, RW_SEEK_CUR);  // type_count, component_count
	SDL_RWread(headers, &task_count, 4, 1);
	SDL_RWread(headers, &directory_count, 4, 1);
	SDL_RWread(headers, &file_count, 4, 1);
	SDL_RWread(headers, &data_entry_count, 4, 1);
	// 4 icon count
	// 4 ini_entry_count
	// 4 registry_entry_count
	// 4 delete_entry_count
	// 4 uninstall_delete_entry_count
	// 4 run_entry_count
	// 4 uninstall_run_entry_count
	// 2 * (2 * 4 + 2) winver range
	// 4 back_color
	// 4 back_color2
	// 4 image_back_color
	// 4 small_image_back_color
	// 4 password.crc32
	// 4 extra_disk_space_required
	// 1 install_mode
	// 1 uninstall_log_mode
	// 1 uninstall_style
	// 1 dir_exists_warning
	// 5 bytes of flags
	SDL_RWseek(headers, 82, RW_SEEK_CUR);

	// skip languages
	for (uint32_t i = 0; i < language_count; ++i)
	{
		binary_string(headers);  // language_name
		binary_string(headers);  // dialog_font
		binary_string(headers);  // title_font
		binary_string(headers);  // welcome_font
		binary_string(headers);  // copyright_font
		// 4 language_id
		// 4 dialog_font_size
		// 4 dialog_font_standard_height
		// 4 title_font_size
		// 4 welcome_font_size
		// 4 copyright_font_size
		SDL_RWseek(headers, 4 * 6, RW_SEEK_CUR);
	}
	// assume 0 messages, permissions, types, components, tasks
	binary_string(headers);  // ???
	binary_string(headers);  // ???
	for (uint32_t i = 0; i < directory_count; ++i)
	{
		binary_string(headers);  // name
		binary_string(headers);  // condition.components
		binary_string(headers);  // condition.tasks
		// 4 attributes
		// 2 * (2 * 4 + 2) windows_version_range
		// 16 flags
		SDL_RWseek(headers, 25, RW_SEEK_CUR);
	}

	// read file entries
	for (uint32_t i = 0; i < file_count; ++i)
	{
		std::string source, destination;
		binary_string(headers, &source);
		binary_string(headers, &destination);
		binary_string(headers);  // install_font_name
		binary_string(headers);  // condition.components
		binary_string(headers);  // condition.tasks
		SDL_RWseek(headers, 2 * (2 * 4 + 2), RW_SEEK_CUR);  // windows_version_range
		uint32_t location;
		SDL_RWread(headers, &location, 4, 1);
		SDL_RWseek(headers, 12, RW_SEEK_CUR);

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
	owned::SDL_RWops datas = zlib_crc_block_reader(rw.get());
	/*if (position != end)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Inno3Vfs: extra data at end of setup.0");
		return;
	}*/

	data_entries.resize(data_entry_count);
	for (uint32_t i = 0; i < data_entry_count; ++i)
	{
		SDL_RWseek(datas, 4 * 2, RW_SEEK_CUR);  // first_slice, last_slice
		SDL_RWread(datas, &data_entries[i].chunk_offset, 4, 1);
		SDL_RWread(datas, &data_entries[i].file_size, 4, 1);
		SDL_RWread(datas, &data_entries[i].chunk_size, 4, 1);
		SDL_RWseek(datas, 21, RW_SEEK_CUR);
	}
	datas.reset();

	net_data_offset = start + data_offset;
	file = std::move(rw);
}

owned::SDL_RWops Inno3Vfs::open_sdl(const char* path)
{
	size_t data_entry_idx = archive.get_file(path);
	if (data_entry_idx == SIZE_MAX)
		return nullptr;

	DataEntry entry = data_entries.at(data_entry_idx);

	std::vector<uint8_t> compressed(entry.chunk_size);
	SDL_RWseek(file, net_data_offset + entry.chunk_offset, RW_SEEK_SET);
	SDL_RWread(file, compressed.data(), entry.chunk_size, 1);

	if (entry.chunk_size + 11 == entry.file_size)
	{
		// Skip decompression if contents are nothing but
		// [magic number:4][zlib header][file]
		compressed.erase(compressed.begin(), compressed.begin() + 11);
		return vanilla::create_vec_rwops(std::move(compressed));
	}

	std::vector<uint8_t> uncompressed(entry.file_size);
	z_stream zip {};
	zip.next_out = uncompressed.data();
	zip.avail_out = uncompressed.size();
	// Skip 4 bytes of "zlb\x1a" magic number
	zip.next_in = &compressed[4];
	zip.avail_in = entry.chunk_size - 4;

	if (int r = inflateInit(&zip); r != Z_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Inno3Vfs::open_sdl(%s): bad inflateInit: %d", path, r);
		return nullptr;
	}
	if (int r = inflate(&zip, Z_NO_FLUSH); r != Z_OK && r != Z_STREAM_END)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Inno3Vfs::open_sdl(%s): bad inflate: %d", path, r);
		return nullptr;
	}
	if (int r = inflateEnd(&zip); r != Z_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Inno3Vfs::open_sdl(%s): bad inflateEnd: %d", path, r);
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
