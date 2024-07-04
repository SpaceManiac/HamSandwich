#ifndef VFS_NSIS_H
#define VFS_NSIS_H

#include "vfs.h"

namespace vanilla
{
	std::unique_ptr<Vfs> open_nsis(owned::SDL_RWops rw);
}

#endif
