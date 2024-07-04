#ifndef VFS_INNO_H
#define VFS_INNO_H

#include "vfs.h"

namespace vanilla
{
	// Inno 4.0.5, bulk 7zipped
	std::unique_ptr<Vfs> open_inno(SDL_RWops* rw);
}

#endif
