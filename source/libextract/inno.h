#ifndef LIBEXTRACT_INNO_H
#define LIBEXTRACT_INNO_H

#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <stdint.h>

struct SDL_RWops;

namespace inno {

class Archive {
	class DataEntry {
		uint32_t chunk_offset;
		uint64_t file_size;
		uint64_t chunk_size;
	};
	std::vector<DataEntry> data_entries;

	Archive(const Archive&) = delete;
	Archive& operator=(const Archive&) = delete;

public:
	explicit Archive(FILE* fptr);
	Archive(Archive&&) = default;
	Archive& operator=(Archive&&) = default;
	~Archive();
};

}  // namespace inno

#endif  // LIBEXTRACT_INNO_H
