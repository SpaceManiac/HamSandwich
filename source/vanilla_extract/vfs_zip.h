#ifndef VANILLA_EXTRACT_ZIP_H
#define VANILLA_EXTRACT_ZIP_H

#include <memory>

namespace vanilla
{
	class Vfs;

	std::unique_ptr<Vfs> open_zip(const char* filename);
}

#endif  // VANILLA_EXTRACT_ZIP_H
