#ifndef VFS_STDIO_H
#define VFS_STDIO_H

#include "vfs.h"

namespace vanilla
{
	std::unique_ptr<WriteVfs> open_stdio(std::string_view prefix);
}

#endif
