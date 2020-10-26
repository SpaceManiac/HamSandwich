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
