#include "vfs_stdio.h"
#include "vfs.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <SDL_rwops.h>
#include <SDL_log.h>

#ifndef _MSC_VER
#include <unistd.h>
#include <sys/wait.h>
#endif

using namespace vanilla;

// ----------------------------------------------------------------------------
// Stdio helpers
#ifdef _WIN32
#include <direct.h>

#define platform_mkdir(path) _mkdir(path)
#else
#define platform_mkdir(path) mkdir(path, 0777)
#endif

static int mkdir_one(const char *path)
{
	if (platform_mkdir(path) != 0 && errno != EEXIST)
		return -1;
	return 0;
}

int vanilla::mkdir_parents(const char *path)
{
	std::string copypath = path;
	char *start = copypath.data();
	char *next;

	int status = 0;
	while (status == 0 && (next = strchr(start, '/')))
	{
		if (next != start)
		{
			// skip the root directory and double-slashes
			*next = '\0';
			status = mkdir_one(copypath.c_str());
			*next = '/';
		}
		start = next + 1;
	}

	if (status != 0)
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "mkdir_parents(%s): %s", copypath.c_str(), strerror(errno));

	return status;
}

// ----------------------------------------------------------------------------
// "Extract to temporary directory" helper
FILE* vanilla::fp_from_bundle(const char* file, const char* mode, SDL_RWops* rw, const char* tempdir, bool reuse_safe)
{
	// Check internal storage to see if we've already extracted the file.
	std::string fname_buf = tempdir;
	fname_buf.append("/");
	fname_buf.append(file);
	if (reuse_safe)
	{
		FILE* fp = fopen(fname_buf.c_str(), mode);
		if (fp)
		{
			return fp;
		}
	}
	// If we have, delete it and extract it again, in case it's changed.
	unlink(fname_buf.c_str());

	mkdir_parents(fname_buf.c_str());
	FILE* fp = fopen(fname_buf.c_str(), "wb");
	if (!fp)
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "fp_from_bundle(%s) bad save: %s", file, strerror(errno));
		return nullptr;
	}

	// Copy everything.
	char buffer[4096];
	int read;
	while ((read = SDL_RWread(rw, buffer, 1, sizeof(buffer))) > 0)
	{
		fwrite(buffer, 1, read, fp);
	}

	// Return a FILE* pointing to the extracted asset.
	fclose(fp);
	fp = fopen(fname_buf.c_str(), mode);
	if (!fp)
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "fp_from_bundle(%s) bad readback: %s", file, strerror(errno));
	}
	return fp;
}

// ----------------------------------------------------------------------------
// Stdio VFS implementation
class StdioVfs : public WriteVfs
{
	std::string prefix;
	FILE* open_stdio_internal(const char* filename, const char* mode, bool write);
public:
	StdioVfs(std::string prefix) : prefix(prefix) {}
	FILE* open_stdio(const char* filename);
	SDL_RWops* open_sdl(const char* filename);
	FILE* open_write_stdio(const char* filename);
	bool list_dir(const char* directory, std::set<std::string>& output);
	bool delete_file(const char* filename);
};

std::unique_ptr<vanilla::WriteVfs> vanilla::open_stdio(const char* prefix)
{
	return std::make_unique<StdioVfs>(prefix);
}

FILE* StdioVfs::open_stdio_internal(const char* file, const char* mode, bool write)
{
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(file);
	if (write)
	{
		vanilla::mkdir_parents(buffer.c_str());
	}
	FILE* fp = fopen(buffer.c_str(), mode);

#ifndef _WIN32
	// On non-Windows, try to case-correct file lookups
	if (!fp)
	{
		size_t i = buffer.rfind("/");
		buffer[i] = '\0';

		std::set<std::string> temp;
		list_dir(buffer.c_str(), temp);

		for (const auto& name : temp)
		{
			if (!strcasecmp(&buffer[i + 1], name.c_str()))
			{
				buffer[i] = '/';
				memcpy(&buffer[i + 1], name.data(), name.length());
				fp = fopen(buffer.c_str(), mode);
				break;
			}
		}
	}
#endif

	if (!fp && write)
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "fopen(%s, %s): %s", buffer.c_str(), mode, strerror(errno));
	}
	return fp;
}

FILE* StdioVfs::open_write_stdio(const char* file)
{
	return open_stdio_internal(file, "wb", true);
}

FILE* StdioVfs::open_stdio(const char* file)
{
	return open_stdio_internal(file, "rb", false);
}

SDL_RWops* StdioVfs::open_sdl(const char* filename)
{
#if defined(_WIN32) && !defined(__GNUC__)
	// The public MSVC binaries of SDL2 are compiled without support for SDL_RWFromFP.
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(filename);
	return SDL_RWFromFile(buffer.c_str(), "rb");
#else
	// Delegate to open_stdio above.
	FILE* fp = open_stdio(filename);
	return fp ? SDL_RWFromFP(fp, SDL_TRUE) : nullptr;
#endif
}

#ifdef __GNUC__
#include <dirent.h>

bool StdioVfs::list_dir(const char* directory, std::set<std::string>& output)
{
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(directory);
	DIR* dir = opendir(buffer.c_str());
	if (!dir)
		return false;

	while (struct dirent *dp = readdir(dir))
		output.insert(dp->d_name);

	closedir(dir);
	return true;
}

#elif defined(_MSC_VER)
#include <io.h>

bool StdioVfs::list_dir(const char* directory, std::set<std::string>& output)
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
		output.insert(finddata.name);
	} while (_findnext(hFile, &finddata) == 0);

	_findclose(hFile);
	return true;
}

#endif  // __GNUC__ and _MSC_VER

bool StdioVfs::delete_file(const char* file) {
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
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "unlink(%s): %s", buffer.c_str(), strerror(errno));
		return false;
	}
}
