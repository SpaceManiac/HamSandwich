#include "vanilla_extract.h"
#include <string.h>
#include <SDL.h>

using namespace vanilla;

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

bool vanilla::ends_with(std::string_view lhs, std::string_view rhs)
{
	return lhs.size() >= rhs.size() && lhs.compare(lhs.size() - rhs.size(), std::string_view::npos, rhs) == 0;
}

// ----------------------------------------------------------------------------
// Mount implementation

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
		else if (filename[mountpoint.size()] == '\0')
		{
			return "";
		}
	}

	return nullptr;  // No match.
}

owned::SDL_RWops Mount::open_sdl(const char* filename)
{
	if (const char* subfilename = matches(filename))
	{
		return vfs->open_sdl(subfilename);
	}
	return nullptr;
}

owned::FILE Mount::open_stdio(const char* filename)
{
	if (const char* subfilename = matches(filename))
	{
		return vfs->open_stdio(subfilename);
	}
	return nullptr;
}

// ----------------------------------------------------------------------------
// VfsStack implementation

const static std::string_view TOMBSTONE_SUFFIX = ".$delete";

static std::string tombstone_name(std::string_view filename)
{
	std::string tombstone { filename };
	tombstone.append(TOMBSTONE_SUFFIX);
	return tombstone;
}

std::unique_ptr<WriteVfs> VfsStack::set_appdata(std::unique_ptr<WriteVfs> new_value)
{
	std::swap(new_value, write_mount);
	return new_value;
}

owned::SDL_RWops VfsStack::open_sdl(const char* filename)
{
	std::string tombstone = tombstone_name(filename);

	// Iterate top to bottom, so we stop as soon as we see a tombstone or the file.
	if (write_mount)
	{
		if (owned::SDL_RWops deleted = write_mount->open_sdl(tombstone.c_str()))
		{
			return nullptr;
		}
		if (owned::SDL_RWops rw = write_mount->open_sdl(filename))
		{
			return rw;
		}
	}

	for (auto iter = mounts.rbegin(); iter != mounts.rend(); ++iter)
	{
		if (owned::SDL_RWops deleted = iter->open_sdl(tombstone.c_str()))
		{
			return nullptr;
		}
		if (owned::SDL_RWops rw = iter->open_sdl(filename))
		{
			return rw;
		}
	}

	return nullptr;
}

owned::FILE VfsStack::open_stdio(const char* filename)
{
	std::string tombstone = tombstone_name(filename);

	// Iterate top to bottom, so we stop as soon as we see a tombstone or the file.
	if (write_mount)
	{
		if (owned::SDL_RWops deleted = write_mount->open_sdl(tombstone.c_str()))
		{
			return nullptr;
		}
		if (owned::FILE fp = write_mount->open_stdio(filename))
		{
			return fp;
		}
	}

	for (auto iter = mounts.rbegin(); iter != mounts.rend(); ++iter)
	{
		if (owned::SDL_RWops deleted = iter->open_sdl(tombstone.c_str()))
		{
			return nullptr;
		}
		if (owned::FILE fp = iter->open_stdio(filename))
		{
			return fp;
		}
	}

	return nullptr;
}

owned::SDL_RWops VfsStack::open_write_sdl(const char* filename)
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

owned::FILE VfsStack::open_write_stdio(const char* filename)
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

static void list_dir_inner(std::string_view prefix, Vfs* vfs, const char* directory, std::set<std::string>& output)
{
	std::set<std::string> intermediate;
	vfs->list_dir(directory, intermediate);
	for (auto each : intermediate)
	{
		if (each == "" || each == "." || each == "..")
			continue;

		each.insert(0, prefix);
		if (ends_with(each, TOMBSTONE_SUFFIX))
		{
			each.erase(each.size() - TOMBSTONE_SUFFIX.size());
			output.erase(each);
		}
		else
		{
			output.insert(std::move(each));
		}
	}
}

void VfsStack::list_dir(const char* directory, std::set<std::string>& output)
{
	// Iterate bottom to top, so higher tombstones can erase their children
	for (auto iter = mounts.begin(); iter != mounts.end(); ++iter)
	{
		if (iter->mountpoint.empty())
		{
			list_dir_inner("", iter->vfs.get(), directory, output);
		}
		else if (const char* subdirectory = iter->matches(directory))
		{
			std::string mountpoint_slash = iter->mountpoint;
			mountpoint_slash.push_back('/');
			list_dir_inner(mountpoint_slash, iter->vfs.get(), subdirectory, output);
		}
	}

	if (write_mount)
	{
		list_dir_inner("", write_mount.get(), directory, output);
	}
}

bool VfsStack::query_bottom(const char* filename, VfsMeta* meta)
{
	std::string tombstone = tombstone_name(filename);

	// Iterate top to bottom, so we stop as soon as we see a tombstone or the file.
	// However, if a file exists in an addon (ex: all_supreme_worlds.zip) and
	// ALSO the base game, mark it as base game by going all the way down.
	bool retval = false;
	if (write_mount)
	{
		if (owned::SDL_RWops deleted = write_mount->open_sdl(tombstone.c_str()))
		{
			return retval;
		}
		if (owned::FILE fp = write_mount->open_stdio(filename))
		{
			*meta = VfsMeta { VfsSourceKind::Appdata };
			retval = true;
		}
	}

	for (auto iter = mounts.rbegin(); iter != mounts.rend(); ++iter)
	{
		if (owned::SDL_RWops deleted = iter->open_sdl(tombstone.c_str()))
		{
			return retval;
		}
		if (owned::FILE fp = iter->open_stdio(filename))
		{
			*meta = iter->meta;
			retval = true;
		}
	}
	return retval;
}
