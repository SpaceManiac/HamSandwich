#ifndef VANILLA_EXTRACT_NSIS_H
#define VANILLA_EXTRACT_NSIS_H

#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <stdint.h>
#include "base_archive.h"

struct SDL_RWops;

namespace vanilla {
namespace nsis {

class Archive : public vanilla::Archive
{
	SDL_RWops* archive_rw;
	size_t datablock_start;

	const char* navigate_nsis(const char* path, Directory*& current);
	bool extract_internal(bool compressed, uint32_t size, std::vector<uint8_t>& result);

public:
	explicit Archive(SDL_RWops* fptr);
	~Archive();

	bool is_ok() { return archive_rw != nullptr; }
	SDL_RWops* open_file(const char* path);
};

}  // namespace nsis
}  // namespace vanilla

#endif  // VANILLA_EXTRACT_NSIS_H
