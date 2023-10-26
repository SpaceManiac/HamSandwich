#ifndef VFS_INNO3_H
#define VFS_INNO3_H

#include "vfs.h"

namespace vanilla
{
	// Inno 3.0.4, root uncompressed
	std::unique_ptr<Vfs> open_inno3(owned::SDL_RWops rw);
}

#endif
