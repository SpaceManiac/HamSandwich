#include "vanilla_extract.h"
#include <SDL.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <SDL_rwops.h>
#include <SDL_log.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

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
