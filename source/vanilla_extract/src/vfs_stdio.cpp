#include "vfs_stdio.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <SDL_rwops.h>
#include <SDL_log.h>

#if __has_include(<filesystem>)
#include <filesystem>
namespace filesystem = std::filesystem;
#else
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem::v1;
#endif

#ifndef _MSC_VER
#include <unistd.h>
#endif

// ----------------------------------------------------------------------------
// mkdir_parents
#ifdef _WIN32
#include <direct.h>

#define platform_mkdir(path) _mkdir(path)
#else
#define platform_mkdir(path) mkdir(path, 0755)
#endif

static int mkdir_one(const char *path)
{
	if (platform_mkdir(path) != 0 && errno != EEXIST)
		return -1;
	return 0;
}

bool vanilla::mkdir_parents(std::string_view path)
{
	std::string copypath { path };
	char *start = copypath.data();

	while (true)
	{
		size_t span = strcspn(start, "/\\");
		if (span == strlen(start))
			break;
		char *next = start + span;
		if (next != start)
		{
			// skip the root directory and double-slashes
			*next = '\0';
			int status = mkdir_one(copypath.c_str());
			if (status != 0)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "mkdir_parents(\"%.*s\"): mkdir(\"%s\"): %s", (int)path.size(), path.data(), copypath.c_str(), strerror(errno));
				return false;
			}
			*next = '/';
		}
		start = next + 1;
	}
	return true;
}

// ----------------------------------------------------------------------------
// Stdio VFS implementation
class StdioVfs : public vanilla::WriteVfs
{
	std::string prefix;
	owned::FILE open_stdio_internal(const char* filename, const char* mode, bool write);
public:
	explicit StdioVfs(std::string&& prefix) : prefix(prefix) {}
	owned::SDL_RWops open_sdl(const char* filename) override;
	owned::SDL_RWops open_write_sdl(const char* filename) override;
	bool list_dir(const char* directory, std::set<std::string, vanilla::CaseInsensitive>* output) override;
	bool delete_file(const char* filename) override;
	bool rename(const char* from, const char* to)  override;
};

std::unique_ptr<vanilla::WriteVfs> vanilla::open_stdio(std::string_view prefix)
{
	return std::make_unique<StdioVfs>(std::string { prefix });
}

owned::FILE StdioVfs::open_stdio_internal(const char* file, const char* mode, bool write)
{
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(file);
	if (write)
	{
		vanilla::mkdir_parents(buffer);
	}
	owned::FILE fp { fopen(buffer.c_str(), mode) };

#ifndef _WIN32
	// On non-Windows, try to case-correct file lookups
	if (!fp)
	{
		size_t i = buffer.rfind("/");
		buffer[i] = '\0';

		std::set<std::string, vanilla::CaseInsensitive> temp;
		list_dir(&buffer.c_str()[prefix.length() + 1], &temp);

		for (const auto& name : temp)
		{
			if (!strcasecmp(&buffer[i + 1], name.c_str()))
			{
				buffer[i] = '/';
				memcpy(&buffer[i + 1], name.data(), name.length());
				fp.reset(fopen(buffer.c_str(), mode));
				break;
			}
		}
	}
#endif

	if (!fp && write)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "fopen(%s, %s): %s", buffer.c_str(), mode, strerror(errno));
	}
	return fp;
}

#if defined(_WIN32)
// The public MSVC binaries of SDL2 are compiled without support for SDL_RWFromFP.

owned::SDL_RWops StdioVfs::open_sdl(const char* filename)
{
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(filename);
	return owned::SDL_RWFromFile(buffer.c_str(), "rb");
}

owned::SDL_RWops StdioVfs::open_write_sdl(const char* filename)
{
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(filename);
	vanilla::mkdir_parents(buffer);
	return owned::SDL_RWFromFile(buffer.c_str(), "wb");
}

#else
// Delegate to open_stdio above.

owned::SDL_RWops StdioVfs::open_sdl(const char* filename)
{
	owned::FILE fp = open_stdio_internal(filename, "rb", false);
	return fp ? owned::SDL_RWFromFP(std::move(fp)) : nullptr;
}

owned::SDL_RWops StdioVfs::open_write_sdl(const char* filename)
{
	owned::FILE fp = open_stdio_internal(filename, "wb", true);
	return fp ? owned::SDL_RWFromFP(std::move(fp)) : nullptr;
}

#endif

#ifdef __GNUC__
#include <dirent.h>

bool StdioVfs::list_dir(const char* directory, std::set<std::string, vanilla::CaseInsensitive>* output)
{
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(directory);
	DIR* dir = opendir(buffer.c_str());
	if (!dir)
		return false;

	while (struct dirent *dp = readdir(dir))
		output->insert(dp->d_name);

	closedir(dir);
	return true;
}

#elif defined(_MSC_VER)
#include <io.h>

bool StdioVfs::list_dir(const char* directory, std::set<std::string, vanilla::CaseInsensitive>* output)
{
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(directory);
	buffer.append("/*");

	struct _finddata_t finddata;
	long hFile = _findfirst(buffer.c_str(), &finddata);
	if (hFile == -1)
		return false;

	do
	{
		output->insert(finddata.name);
	} while (_findnext(hFile, &finddata) == 0);

	_findclose(hFile);
	return true;
}

#endif  // __GNUC__ and _MSC_VER

bool StdioVfs::delete_file(const char* file)
{
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(file);

	int status =
#ifdef _MSC_VER
		_unlink
#else
		unlink
#endif
		(buffer.c_str());

	if (status == 0)
	{
		return true;
	}
	else
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "unlink(%s): %s", buffer.c_str(), strerror(errno));
		return false;
	}
}

bool StdioVfs::rename(const char* from, const char* to)
{
	std::string from2 = prefix;
	from2.append("/");
	from2.append(from);
	std::string to2 = prefix;
	to2.append("/");
	to2.append(to);

	std::error_code fs_error;
	filesystem::rename(from2, to2, fs_error);
	if (fs_error)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "rename(%s, %s): %s\n", from2.c_str(), to2.c_str(), fs_error.message().c_str());
		return false;
	}
	return true;
}
