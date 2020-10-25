#ifndef LIBEXTRACT_NSIS_H
#define LIBEXTRACT_NSIS_H

#include <string>
#include <map>
#include <stdio.h>

namespace nsis {

struct CaseInsensitive {
	bool operator() (const std::string& lhs, const std::string& rhs) const;
};

struct Directory {
	std::map<std::string, size_t, CaseInsensitive> files;
	std::map<std::string, Directory, CaseInsensitive> directories;
};

class Archive {
	FILE* fptr;
	size_t firstheader_start;
	size_t datablock_start;

	Directory install_dir;

public:
	explicit Archive(FILE* fptr);
	~Archive();

	bool populate_file_list();
	const Directory& root() const { return install_dir; }
};

}  // namespace nsis

#endif  // LIBEXTRACT_NSIS_H
