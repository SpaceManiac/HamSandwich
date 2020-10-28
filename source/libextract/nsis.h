#ifndef LIBEXTRACT_NSIS_H
#define LIBEXTRACT_NSIS_H

#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <stdint.h>
#include "common.h"

struct SDL_RWops;

namespace nsis {

class Archive {
	SDL_RWops* archive_rw;
	size_t datablock_start;

	sauce::Directory install_dir;

	bool extract_internal(bool compressed, uint32_t size, std::vector<uint8_t>& result);

	Archive(const Archive&) = delete;
	Archive& operator=(const Archive&) = delete;

public:
	explicit Archive(FILE* fptr);
	Archive(Archive&&) = default;
	Archive& operator=(Archive&&) = default;
	~Archive();

	bool is_ok() { return archive_rw != nullptr; }
	const sauce::Directory& root() const { return install_dir; }
	SDL_RWops* open_file(sauce::File file);
};

}  // namespace nsis

#endif  // LIBEXTRACT_NSIS_H
