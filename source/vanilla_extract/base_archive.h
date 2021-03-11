#ifndef VANILLA_EXTRACT_BASE_ARCHIVE_H
#define VANILLA_EXTRACT_BASE_ARCHIVE_H

#include <vector>
#include <string>
#include <map>
#include <stdint.h>

typedef struct ISzAlloc ISzAlloc;
typedef const ISzAlloc* ISzAllocPtr;

namespace vanilla {

extern const ISzAllocPtr allocator;

bool decompress_lzma(std::vector<uint8_t>& dest, uint8_t* src, size_t srclen, size_t propsize);

class Archive
{
	Archive(const Archive&) = delete;
	Archive& operator=(const Archive&) = delete;
	Archive(Archive&&) = delete;
	Archive& operator=(Archive&&) = delete;

protected:
	Archive() {}

	struct CaseInsensitive {
		bool operator() (const std::string& lhs, const std::string& rhs) const;
	};

	struct Directory {
		std::map<std::string, size_t, CaseInsensitive> files;
		std::map<std::string, Directory, CaseInsensitive> directories;
	};

	Directory root;

	static const char* navigate(const char* path, Directory*& current);
	static const char* navigate(const char* path, const Directory*& current);

	size_t get_file(const char* path) const;
	const Directory* get_directory(const char* path) const;

public:
	bool list_dir(const char* path, std::vector<std::string>& output) const;
	//SDL_RWops* open_file(const char* path);
};

}  // namespace vanilla

#endif  // VANILLA_EXTRACT_BASE_ARCHIVE_H
