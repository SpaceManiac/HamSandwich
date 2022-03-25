#include "vanilla_extract.h"
#include <SDL_rwops.h>
#include <SDL_log.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

// ----------------------------------------------------------------------------
// Stdio VFS implementation
class StdioVfs : public vanilla::WriteVfs
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
