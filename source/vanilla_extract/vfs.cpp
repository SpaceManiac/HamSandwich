#include "vanilla_extract.h"
#include <string.h>
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

std::unique_ptr<WriteVfs> VfsStack::set_appdata(std::unique_ptr<WriteVfs> new_value)
{
	std::swap(new_value, write_mount);
	return new_value;
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

	for (auto iter = mounts.rbegin(); iter != mounts.rend(); ++iter)
	{
		if (const char* subfilename = iter->matches(filename))
		{
			if (SDL_RWops* rw = iter->vfs->open_sdl(subfilename))
			{
				return rw;
			}
		}
	}

	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "open_sdl(%s): not found in any vfs", filename);
	return nullptr;
}

FILE* VfsStack::open_stdio(const char* filename)
{
	if (write_mount)
	{
		if (FILE* fp = write_mount->open_stdio(filename))
		{
			return fp;
		}
	}

	for (auto iter = mounts.rbegin(); iter != mounts.rend(); ++iter)
	{
		if (const char* subfilename = iter->matches(filename))
		{
			if (FILE* fp = iter->vfs->open_stdio(subfilename))
			{
				return fp;
			}
		}
	}

	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "open_stdio(%s): not found in any vfs", filename);
	return nullptr;
}

SDL_RWops* VfsStack::open_write_sdl(const char* filename)
{
	if (write_mount)
	{
		return write_mount->open_write_sdl(filename);
	}
	else
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "open_write_sdl(%s): appdata not mounted", filename);
		return nullptr;
	}
}

FILE* VfsStack::open_write_stdio(const char* filename)
{
	if (write_mount)
	{
		return write_mount->open_write_stdio(filename);
	}
	else
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "open_write_stdio(%s): appdata not mounted", filename);
		return nullptr;
	}
}

bool VfsStack::delete_file(const char* filename)
{
	if (write_mount)
	{
		return write_mount->delete_file(filename);
	}
	else
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "delete_file(%s): appdata not mounted", filename);
		return false;
	}
}

void VfsStack::list_dir(const char* directory, std::set<std::string>& output)
{
	if (write_mount)
	{
		write_mount->list_dir(directory, output);
	}

	for (auto iter = mounts.rbegin(); iter != mounts.rend(); ++iter)
	{
		if (iter->mountpoint.empty())
		{
			iter->vfs->list_dir(directory, output);
		}
		else if (const char* subdirectory = iter->matches(directory))
		{
			std::string mountpoint_slash = iter->mountpoint;
			mountpoint_slash.push_back('/');

			std::set<std::string> intermediate;
			iter->vfs->list_dir(subdirectory, intermediate);

			for (auto each : intermediate)
			{
				each.insert(0, mountpoint_slash);
				output.insert(each);
			}
		}
	}
}
