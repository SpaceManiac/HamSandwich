#ifndef VANILLA_EXTRACT_INNO_H
#define VANILLA_EXTRACT_INNO_H

#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <stdint.h>
#include "base_archive.h"

struct SDL_RWops;

namespace vanilla {
namespace inno {

class Archive : public vanilla::Archive
{
	struct DataEntry
	{
		uint32_t chunk_offset;
		uint64_t file_size;
		uint64_t chunk_size;
	};
	std::vector<DataEntry> data_entries;
	std::vector<uint8_t> setup_1_bin;

public:
	explicit Archive(FILE* fptr);
	~Archive();

	bool is_ok() { return setup_1_bin.size(); }
	SDL_RWops* open_file(const char* path);
};

}  // namespace inno
}  // namespace vanilla

#endif  // VANILLA_EXTRACT_INNO_H
