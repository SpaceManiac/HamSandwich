#ifndef LIBEXTRACT_NSIS_H
#define LIBEXTRACT_NSIS_H

#include <vector>
#include <string>
#include <stdio.h>

namespace nsis {

class Archive {
	FILE* fptr;
	size_t firstheader_start;
	size_t datablock_start;

public:
	explicit Archive(FILE* fptr);
	~Archive();

	bool populate_file_list();
};

}  // namespace nsis

#endif  // LIBEXTRACT_NSIS_H
