#include <string.h>
#include <vector>
#include "nsis_exehead/fileform.h"
#include "lzma_helpers.h"
#include "nsis.h"
#include "vec_rw.h"

#ifdef SDL_UNPREFIXED
	#include <SDL_rwops.h>
#else  // SDL_UNPREFIXED
	#include <SDL2/SDL_rwops.h>
#endif  // SDL_UNPREFIXED

namespace nsis {

static_assert(sizeof(int) == 4);

#ifndef __GNUC__
#define strcasecmp _stricmp
#endif // __GNUC__

bool CaseInsensitive::operator() (const std::string& lhs, const std::string& rhs) const
{
	return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

// ----------------------------------------------------------------------------
// Constants

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

const char* navigate(uint8_t* path, Directory** working_directory, Directory* install_dir);

Archive::~Archive()
{
	if (archive_rw)
	{
		SDL_RWclose(archive_rw);
	}
}

Archive::Archive(FILE* fptr)
	: archive_rw(nullptr)
{
	// Find the "first header" in the file.
	if (fseek(fptr, 0, SEEK_END))
		return;
	size_t file_size = ftell(fptr);

	firstheader fh;
	size_t firstheader_start = 0;
	for (size_t search = SEARCH_START; search < file_size - FIRSTHEADER_SIZE; search += SEARCH_INCREMENT)
	{
		if (fseek(fptr, search, SEEK_SET))
			return;
		if (!fread(&fh, FIRSTHEADER_SIZE, 1, fptr))
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
	if (!fread(&header_size, 4, 1, fptr))
		return;

	if (header_size == 0x8000005D)
	{
		// This is the first 4 bytes of an LZMA stream instead of a size, which
		// means that the installer was compiled with `SetCompressor /SOLID`.
		if (fseek(fptr, -4, SEEK_CUR))
			return;

		std::vector<uint8_t> buffer(fh.length_of_all_following_data - FIRSTHEADER_SIZE);
		if (!fread(buffer.data(), buffer.size(), 1, fptr))
			return;

		std::vector<uint8_t> datablock;
		if (!lzma_helpers::decompress_all(datablock, buffer.data(), buffer.size(), 5))
			return;

		archive_rw = create_vec_rwops(std::move(datablock));
		if (!SDL_RWread(archive_rw, &header_size, 4, 1))
		{
			SDL_RWclose(archive_rw);
			archive_rw = nullptr;
			return;
		}
	}
	else
	{
		archive_rw = SDL_RWFromFP(fptr, SDL_TRUE);
		fptr = nullptr;
	}

	// Decompress the "header block".
	std::vector<uint8_t> header;
	if (!extract_internal(header_size & SIZE_COMPRESSED, header_size & ~SIZE_COMPRESSED, header))
	{
		SDL_RWclose(archive_rw);
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
	Directory* working_directory = &install_dir;
	for (size_t offset = code_offset; offset < string_table_offset - sizeof(entry); offset += 4)
	{
		entry current = *(entry*)&header[offset];
		switch (current.which)
		{
			case EW_CREATEDIR:
			{
				offset += 4 * 2;
				uint8_t* path = &header[string_table_offset + current.offsets[0]];
				bool chdir = current.offsets[1];

				if (!chdir)
					break;

				if (const char* last_component = navigate(path, &working_directory, &install_dir))
				{
					working_directory = &working_directory->directories[last_component];
				}

				break;
			}
			case EW_EXTRACTFILE:
			{
				offset += 4 * 6;
				uint8_t* path = &header[string_table_offset + current.offsets[1]];
				size_t data_idx = current.offsets[2];

				if (const char* last_component = navigate(path, &working_directory, &install_dir))
				{
					working_directory->files.insert(std::make_pair<std::string, File>(last_component, data_idx));
				}

				break;
			}
		}
	}

	// Hooray!
}

const char* navigate(uint8_t* path, Directory** working_directory, Directory* install_dir)
{
	if (path[0] == NS_VAR_CODE)
	{
		int var = ((path[2] & 0x7f) << 7) | (path[1] & 0x7f);
		// 21 is $INSTDIR and 29 is a user variable which appears
		// to usually match it.
		if (var == 21 || var == 29 || var == 31)
		{
			*working_directory = install_dir;
		}
		else
			return nullptr;
		path += 3;

		if (path[0] == '\\' || path[0] == '/')
			++path;
	}

	if (!path[0])
		return nullptr;

	uint8_t* last_component = path;
	for (uint8_t* current = path; *current; ++current)
	{
		if (*current == '\\' || *current == '/') {
			*current = 0;
			*working_directory = &(*working_directory)->directories[(const char*) last_component];
			last_component = current + 1;
		}
	}

	return (const char*) last_component;
}

// ----------------------------------------------------------------------------
// Extracting individual files

SDL_RWops* Archive::open_file(File file)
{
	if (SDL_RWseek(archive_rw, datablock_start + file.offset, SEEK_SET) < 0)
	{
		fprintf(stderr, "nsis::Archive::extract: fseek error\n");
		return nullptr;
	}

	uint32_t size;
	if (!SDL_RWread(archive_rw, &size, 4, 1))
	{
		fprintf(stderr, "nsis::Archive::extract: fread size error\n");
		return nullptr;
	}

	// Optimization to avoid extra copies of already-decompressed data.
	if ((archive_rw->type == SDL_RWOPS_MEMORY || archive_rw->type == SDL_RWOPS_MEMORY_RO) && !(size & SIZE_COMPRESSED))
	{
		return SDL_RWFromConstMem(archive_rw->hidden.mem.here, size);
	}

	std::vector<uint8_t> result;
	if (!extract_internal(size & SIZE_COMPRESSED, size & ~SIZE_COMPRESSED, result))
		return nullptr;
	return create_vec_rwops(std::move(result));
}

bool Archive::extract_internal(bool is_compressed, uint32_t size, std::vector<uint8_t>& result)
{
	std::vector<uint8_t> compressed(size);
	size_t got = SDL_RWread(archive_rw, compressed.data(), 1, compressed.size());
	if (got < compressed.size())
	{
		fprintf(stderr, "nsis::Archive::extract: expected %zu, got %zu\n", compressed.size(), got);
		return false;
	}

	if (is_compressed)
	{
		return lzma_helpers::decompress_all(result, compressed.data(), compressed.size(), 5);
	}
	else
	{
		result = std::move(compressed);
		return true;
	}
}

}  // namespace nsis
