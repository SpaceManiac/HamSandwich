#include <string.h>
#include <vector>
#include "nsis_exehead/fileform.h"
#include "lzma_helpers.h"
#include "nsis.h"

namespace nsis {

static_assert(LITTLE_ENDIAN);
static_assert(sizeof(int) == 4);

// ----------------------------------------------------------------------------
// Constants

const size_t SEARCH_START = 0x8000;
const size_t SEARCH_INCREMENT = 0x200;

const size_t FIRSTHEADER_SIZE = 28;
const size_t MAGIC_OFFSET = 4;
const size_t MAGIC_SIZE = 16;
const uint8_t NULLSOFT_MAGIC[MAGIC_SIZE + 1] = "\xEF\xBE\xAD\xDENullsoftInst";
static_assert(MAGIC_OFFSET + MAGIC_SIZE <= FIRSTHEADER_SIZE);

const uint32_t SIZE_COMPRESSED = 0x80000000;

// ----------------------------------------------------------------------------
// Decoding the file list

Archive::Archive(FILE* fptr)
	: fptr(fptr)
{
}

Archive::~Archive()
{
	fclose(fptr);
}

const char* navigate(uint8_t* path, Directory** working_directory, Directory* install_dir);

bool Archive::populate_file_list()
{
	// Find the "first header" in the file.
	if (fseek(fptr, 0, SEEK_END))
		return false;
	size_t file_size = ftell(fptr);

	uint8_t firstheader_buf[FIRSTHEADER_SIZE];
	firstheader_start = 0;
	for (size_t search = SEARCH_START; search < file_size - FIRSTHEADER_SIZE; search += SEARCH_INCREMENT)
	{
		if (fseek(fptr, search, SEEK_SET))
			return false;
		if (!fread(firstheader_buf, FIRSTHEADER_SIZE, 1, fptr))
			return false;
		if (!memcmp(&firstheader_buf[MAGIC_OFFSET], NULLSOFT_MAGIC, MAGIC_SIZE))
		{
			firstheader_start = search;
			break;
		}
	}
	if (firstheader_start == 0)
		return false;

	// Decompress the "header block".
	uint32_t header_block_size;
	if (!fread(&header_block_size, 4, 1, fptr))
		return false;

	std::vector<uint8_t> header;
	{
		std::vector<uint8_t> compressed_header;
		compressed_header.resize(header_block_size & ~SIZE_COMPRESSED);
		if (!fread(compressed_header.data(), compressed_header.size(), 1, fptr))
			return false;
		datablock_start = ftell(fptr);

		if (header_block_size & SIZE_COMPRESSED)
		{
			if (!lzma_helpers::decompress_all(header, compressed_header.data(), compressed_header.size(), 5))
				return false;
		}
		else
		{
			header = std::move(compressed_header);
		}
	}

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
					working_directory->files[last_component] = data_idx;
				}

				break;
			}
		}
	}

	// Hooray!
	return true;
}

const char* navigate(uint8_t* path, Directory** working_directory, Directory* install_dir)
{
	if (path[0] == NS_VAR_CODE)
	{
		int var = ((path[2] & 0x7f) << 7) | (path[1] & 0x7f);
		// 21 is $INSTDIR and 29 is a user variable which appears
		// to usually match it.
		if (var == 21 || var == 29)
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

#ifndef __GNUC__
#define strcasecmp _stricmp
#endif // __GNUC__

bool CaseInsensitive::operator() (const std::string& lhs, const std::string& rhs) const
{
	return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

}  // namespace nsis
