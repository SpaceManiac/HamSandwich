#ifndef VANILLA_EXTRACT_VFS_INNO_H
#define VANILLA_EXTRACT_VFS_INNO_H

#include <memory>

namespace vanilla
{
	class Vfs;

	std::unique_ptr<Vfs> open_inno(FILE* fp);
}

#endif  // VANILLA_EXTRACT_VFS_INNO_H
