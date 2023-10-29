#include "vfs.h"
#include <string.h>
#include <SDL.h>

#ifdef __ANDROID__
#include <SDL_system.h>
#include <unistd.h>
#endif

using namespace vanilla;

#ifndef __GNUC__
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

// ----------------------------------------------------------------------------
// Simple helpers

bool vanilla::ends_with(std::string_view lhs, std::string_view rhs)
{
	return lhs.size() >= rhs.size() && lhs.compare(lhs.size() - rhs.size(), std::string_view::npos, rhs) == 0;
}

bool vanilla::CaseInsensitive::operator() (const std::string& lhs, const std::string& rhs) const
{
	return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

// ----------------------------------------------------------------------------
// fp_from_bundle
#ifdef __ANDROID__

// tmpfile replacement for Android
static FILE* android_tmpfile()
{
	std::string buf = SDL_AndroidGetInternalStoragePath();
	buf.append("/tmp.XXXXXX");
	int fd = mkstemp(buf.data());
	if (fd < 0)
		return nullptr;
	FILE* fp = fdopen(fd, "w+b");
	unlink(buf.c_str());
	return fp;
}
#define tmpfile android_tmpfile

#endif  // __ANDROID__

// Helpers for using printf, SDL_LogError, &c. with std::string_view arguments.
#define FSPEC_STRING_VIEW "%.*s"
#define FMT_STRING_VIEW(X) (int)(X).size(), (X).data()

// Copy an SDL_RWops to a temporary file, either in memory or in /tmp or similar, so it can be a FILE*
static owned::FILE fp_from_bundle(std::string_view filename, SDL_RWops* rw)
{
	if (!rw)
		return nullptr;

	owned::FILE fp { tmpfile() };
	if (!fp)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "fp_from_bundle(%.*s) bad tmpfile: %s", FMT_STRING_VIEW(filename), strerror(errno));
		return nullptr;
	}

	// Copy everything from the SDL_RWops to the FILE.
	char buffer[16 * 1024];
	int read;
	while ((read = SDL_RWread(rw, buffer, 1, sizeof(buffer))) > 0)
	{
		fwrite(buffer, 1, read, fp);
	}

	if (read < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "fp_from_bundle(%.*s) bad SDL_RWread: %s", FMT_STRING_VIEW(filename), SDL_GetError());
		return nullptr;
	}

	// Reset the stream and return it
	fseek(fp, 0, SEEK_SET);
	return fp;
}

// ----------------------------------------------------------------------------
// Vfs default implementation

owned::FILE Vfs::open_stdio(const char* filename)
{
	auto sdl = open_sdl(filename);
	return fp_from_bundle(filename, sdl.get());
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

static void list_dir_inner(std::string_view prefix, Vfs* vfs, const char* directory, std::set<std::string, CaseInsensitive>& output)
{
	std::set<std::string, vanilla::CaseInsensitive> intermediate;
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

void VfsStack::list_dir(const char* directory, std::set<std::string, CaseInsensitive>& output)
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
