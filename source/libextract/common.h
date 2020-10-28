#ifndef LIBEXTRACT_COMMON_H
#define LIBEXTRACT_COMMON_H

#include <vector>
#include <string>
#include <map>
#include <stdint.h>

typedef struct ISzAlloc ISzAlloc;
typedef const ISzAlloc* ISzAllocPtr;

namespace nsis {
	class Archive;
}
namespace inno {
	class Archive;
}

namespace sauce {

extern const ISzAllocPtr allocator;

bool decompress_lzma(std::vector<uint8_t>& dest, uint8_t* src, size_t srclen, size_t propsize);

struct CaseInsensitive {
	bool operator() (const std::string& lhs, const std::string& rhs) const;
};

class File {
	size_t offset;
	File(size_t offset) : offset(offset) {}
	friend class nsis::Archive;
	friend class inno::Archive;
};

struct Directory {
	std::map<std::string, File, CaseInsensitive> files;
	std::map<std::string, Directory, CaseInsensitive> directories;
};

}  // namespace sauce

#endif  // LIBEXTRACT_COMMON_H
