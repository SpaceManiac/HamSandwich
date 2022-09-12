#include "vanilla_extract.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <SDL.h>
#include <SDL_platform.h>

#ifdef __ANDROID__
#include <SDL_system.h>
#endif

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

int vanilla::mkdir_parents(std::string_view path)
{
	std::string copypath { path };
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
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "mkdir_parents(%s): %s", copypath.c_str(), strerror(errno));

	return status;
}

// ----------------------------------------------------------------------------
// tmpfile replacement for Android
#ifdef __ANDROID__

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

#endif

// Helpers for using printf, SDL_LogError, &c. with std::string_view arguments.
#define FSPEC_STRING_VIEW "%.*s"
#define FMT_STRING_VIEW(X) (int)(X).size(), (X).data()

// ----------------------------------------------------------------------------
// "Extract to temporary directory" helper
owned::FILE vanilla::fp_from_bundle(std::string_view filename, SDL_RWops* rw)
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
