#include "vfs_nsis.h"
#include <string.h>
#include <vector>
#include <SDL_rwops.h>
#include <fileform.h>
#include "base_archive.h"
#include "vec_rw.h"
#include "lzma_helpers.h"

// ----------------------------------------------------------------------------
// NSIS VFS implementation

class NsisVfs : public vanilla::Vfs
{
	vanilla::Archive archive;
	owned::SDL_RWops archive_rw;
	size_t datablock_start;

	const char* navigate_nsis(const char* path, vanilla::Archive::Directory*& current);
	bool extract_internal(const char* path, bool compressed, uint32_t size, std::vector<uint8_t>& result);

public:
	explicit NsisVfs(owned::SDL_RWops fp);

	bool is_ok()
	{
		return archive_rw != nullptr;
	}

	owned::SDL_RWops open_sdl(const char* filename) override;
	bool list_dir(const char* directory, std::set<std::string, vanilla::CaseInsensitive>& output) override;
};

std::unique_ptr<vanilla::Vfs> vanilla::open_nsis(owned::SDL_RWops rw)
{
	return std::make_unique<NsisVfs>(std::move(rw));
}

bool NsisVfs::list_dir(const char* directory, std::set<std::string, vanilla::CaseInsensitive>& output)
{
	return archive.list_dir(directory, output);
}

// ----------------------------------------------------------------------------
// Constants

static_assert(sizeof(int) == 4);

const size_t SEARCH_START = 0x8000;
const size_t SEARCH_INCREMENT = 0x200;

const size_t FIRSTHEADER_SIZE = sizeof(firstheader);
const size_t MAGIC_OFFSET = offsetof(firstheader, siginfo);
const size_t MAGIC_SIZE = 16;
const uint8_t NULLSOFT_MAGIC[MAGIC_SIZE + 1] = "\xEF\xBE\xAD\xDENullsoftInst";
static_assert(MAGIC_OFFSET + MAGIC_SIZE <= FIRSTHEADER_SIZE);

const uint32_t SIZE_COMPRESSED = 0x80000000;

// ----------------------------------------------------------------------------
// Decoding the file list

NsisVfs::NsisVfs(owned::SDL_RWops fptr)
	: archive_rw(nullptr)
{
	// Find the "first header" in the file.
	Sint64 file_size = SDL_RWseek(fptr, 0, RW_SEEK_END);
	if (file_size < 0)
		return;

	firstheader fh;
	size_t firstheader_start = 0;
	for (size_t search = SEARCH_START; search < file_size - FIRSTHEADER_SIZE; search += SEARCH_INCREMENT)
	{
		if (SDL_RWseek(fptr, search, RW_SEEK_SET) < 0)
			return;
		if (!SDL_RWread(fptr, &fh, FIRSTHEADER_SIZE, 1))
			return;
		if (!memcmp(&fh.siginfo, NULLSOFT_MAGIC, MAGIC_SIZE))
		{
			firstheader_start = search;
			break;
		}
	}
	if (firstheader_start == 0)
		return;

	uint32_t header_size;
	if (!SDL_RWread(fptr, &header_size, 4, 1))
		return;

	if (header_size == 0x8000005D)
	{
		// This is the first 4 bytes of an LZMA stream instead of a size, which
		// means that the installer was compiled with `SetCompressor /SOLID`.
		if (SDL_RWseek(fptr, -4, RW_SEEK_CUR) < 0)
			return;

		std::vector<uint8_t> buffer(fh.length_of_all_following_data - FIRSTHEADER_SIZE);
		if (!SDL_RWread(fptr, buffer.data(), buffer.size(), 1))
			return;

		std::vector<uint8_t> datablock;
		if (!vanilla::lzma_decompress(datablock, buffer.data(), buffer.size(), 5))
			return;

		archive_rw = vanilla::create_vec_rwops(std::move(datablock));
		if (!SDL_RWread(archive_rw, &header_size, 4, 1))
		{
			archive_rw = nullptr;
			return;
		}
	}
	else
	{
		archive_rw = std::move(fptr);
	}

	// Decompress the "header block".
	std::vector<uint8_t> header;
	if (!extract_internal("<header block>", header_size & SIZE_COMPRESSED, header_size & ~SIZE_COMPRESSED, header))
	{
		archive_rw = nullptr;
		return;
	}
	datablock_start = SDL_RWtell(archive_rw);

	// Decode the block information.
	block_header blocks[BLOCKS_NUM];
	memcpy(blocks, &header[offsetof(::header, blocks)], sizeof(blocks));

	size_t code_offset = blocks[NB_ENTRIES].offset;
	size_t string_table_offset = blocks[NB_STRINGS].offset;
	datablock_start += blocks[NB_DATA].offset;

	// Decode the instructions.
	vanilla::Archive::Directory* working_directory = &archive.root;
	for (size_t offset = code_offset; offset < string_table_offset - sizeof(entry); offset += 4)
	{
		entry current;
		memcpy(&current, &header[offset], sizeof(entry));
		switch (current.which)
		{
			case EW_CREATEDIR:
			{
				offset += 4 * 2;
				const char* path = (const char*) &header[string_table_offset + current.offsets[0]];
				bool chdir = current.offsets[1];

				if (!chdir)
					break;

				if (const char* last_component = navigate_nsis(path, working_directory); last_component && strcmp(last_component, "") && strcmp(last_component, "."))
				{
					working_directory = &working_directory->directories[last_component];
				}

				break;
			}
			case EW_EXTRACTFILE:
			{
				offset += 4 * 6;
				const char* path = (const char*) &header[string_table_offset + current.offsets[1]];
				size_t data_idx = current.offsets[2];

				if (const char* last_component = navigate_nsis(path, working_directory))
				{
					working_directory->files.insert(std::make_pair(std::string(last_component), data_idx));
				}

				break;
			}
		}
	}

	// Hooray!
}

const char* NsisVfs::navigate_nsis(const char* path, vanilla::Archive::Directory*& current)
{
	if ((uint8_t) path[0] == NS_VAR_CODE)
	{
		int var = ((path[2] & 0x7f) << 7) | (path[1] & 0x7f);
		// 21 is $INSTDIR and 29 is a user variable which appears
		// to usually match it.
		if (var == 21 || var == 29 || var == 31)
		{
			current = &archive.root;
		}
		else
			return nullptr;
		path += 3;

		if (path[0] == '\\' || path[0] == '/')
			++path;
	}

	if (!path[0])
		return nullptr;

	return vanilla::Archive::navigate(path, current);
}

// ----------------------------------------------------------------------------
// Extracting individual files

owned::SDL_RWops NsisVfs::open_sdl(const char* path)
{
	size_t offset = archive.get_file(path);
	if (offset == SIZE_MAX)
		return nullptr;

	if (int r = SDL_RWseek(archive_rw, datablock_start + offset, RW_SEEK_SET); r < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "NsisVfs::open_sdl(%s): seek error: %s", path, SDL_GetError());
		return nullptr;
	}

	uint32_t size;
	if (!SDL_RWread(archive_rw, &size, 4, 1))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "NsisVfs::open_sdl(%s): read error: %s", path, SDL_GetError());
		return nullptr;
	}

	// Optimization to avoid extra copies of already-decompressed data.
	if ((archive_rw->type == SDL_RWOPS_MEMORY || archive_rw->type == SDL_RWOPS_MEMORY_RO) && !(size & SIZE_COMPRESSED))
	{
		return owned::SDL_RWFromConstMem(archive_rw->hidden.mem.here, size);
	}

	std::vector<uint8_t> result;
	if (!extract_internal(path, size & SIZE_COMPRESSED, size & ~SIZE_COMPRESSED, result))
		return nullptr;
	return vanilla::create_vec_rwops(std::move(result));
}

bool NsisVfs::extract_internal(const char* path, bool is_compressed, uint32_t size, std::vector<uint8_t>& result)
{
	std::vector<uint8_t> compressed(size);
	size_t got = SDL_RWread(archive_rw, compressed.data(), 1, compressed.size());
	if (got < compressed.size())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "NsisVfs::extract_internal(%s): expected %u, got %u: %s", path, (unsigned int)(compressed.size()), (unsigned int)(got), SDL_GetError());
		return false;
	}

	if (is_compressed)
	{
		return vanilla::lzma_decompress(result, compressed.data(), compressed.size(), 5);
	}
	else
	{
		result = std::move(compressed);
		return true;
	}
}
