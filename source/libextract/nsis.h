#ifndef LIBEXTRACT_NSIS_H
#define LIBEXTRACT_NSIS_H

#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <stdint.h>

namespace nsis {

struct CaseInsensitive {
	bool operator() (const std::string& lhs, const std::string& rhs) const;
};

class File {
	size_t offset;
	File(size_t offset) : offset(offset) {}
	friend class Archive;
};

struct Directory {
	std::map<std::string, File, CaseInsensitive> files;
	std::map<std::string, Directory, CaseInsensitive> directories;
};

class Archive {
	FILE* fptr;
	size_t firstheader_start;
	size_t datablock_start;

	Directory install_dir;

	Archive(const Archive&) = delete;
	Archive& operator=(const Archive&) = delete;

public:
	explicit Archive(FILE* fptr);
	Archive(Archive&&) = default;
	Archive& operator=(Archive&&) = default;
	~Archive();

	bool populate_file_list();
	const Directory& root() const { return install_dir; }
	bool extract(File file, std::vector<uint8_t>& result);
};

}  // namespace nsis

#endif  // LIBEXTRACT_NSIS_H
