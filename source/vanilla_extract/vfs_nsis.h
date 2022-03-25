#ifndef VANILLA_EXTRACT_VFS_NSIS_H
#define VANILLA_EXTRACT_VFS_NSIS_H

#include <memory>

struct SDL_RWops;

namespace vanilla
{
	class Vfs;

	std::unique_ptr<Vfs> open_nsis(SDL_RWops* rw);
}

#endif  // VANILLA_EXTRACT_VFS_NSIS_H
