#ifndef VFS_ZIP_H
#define VFS_ZIP_H

#include "vfs.h"

namespace vanilla
{
	std::unique_ptr<Vfs> open_zip(owned::SDL_RWops rw);
}

#endif
