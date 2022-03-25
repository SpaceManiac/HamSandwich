#include "vfs.h"
#include <SDL.h>

using namespace vanilla;

// ----------------------------------------------------------------------------
// VfsStack implementation

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

const char* Mount::matches(const char* filename) const
{
	if (mountpoint.empty())
		return filename;  // Blank mountpoint, always match.

	if (strncasecmp(filename, mountpoint.c_str(), mountpoint.size()) == 0)
	{
		if (filename[mountpoint.size()] == '/')
		{
			return &filename[mountpoint.size() + 1];
		}
		else if (filename[mountpoint.size()] == '0')
		{
			return "";
		}
	}

	return nullptr;  // No match.
}

void VfsStack::push_back(std::unique_ptr<Vfs>&& entry, std::string mountpoint)
{
	mounts.push_back(Mount { std::move(entry), mountpoint });
}

SDL_RWops* VfsStack::open_sdl(const char* filename)
{
	if (write_mount)
	{
		if (SDL_RWops* rw = write_mount->open_sdl(filename))
		{
			return rw;
		}
	}

	for (auto& mount : mounts)
	{
		if (const char* subfilename = mount.matches(filename))
		{
			if (SDL_RWops* rw = mount.vfs->open_sdl(subfilename))
			{
				return rw;
			}
		}
	}

	SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "AssetOpen_SDL(%s): not found in any vfs", filename);
	return nullptr;
}
