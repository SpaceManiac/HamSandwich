#include "appdata.h"

#ifdef SDL_UNPREFIXED
	#include <SDL_platform.h>
#else  // SDL_UNPREFIXED
	#include <SDL2/SDL_platform.h>
#endif  // SDL_UNPREFIXED

// TODO: allow overriding this
#define APPDATA_NAME "DrLunatic"

#define ASSETOPEN_MODE "rb"

#ifdef _WIN32
// Windows ----------------------------------------------------------

#include <io.h>
#include <shlobj.h> // for SHGetFolderPath
#ifdef _MSC_VER
#include <direct.h>
#endif

FILE* AppdataOpen(const char* file, const char* mode)
{
	char buffer[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, buffer);
	strcat(buffer, "\\Hamumu");
	mkdir(buffer);
	strcat(buffer, "\\");
	strcat(buffer, APPDATA_NAME);
	mkdir(buffer);
	strcat(buffer, "\\");
	strcat(buffer, file);
	return fopen(buffer, mode);
}

FILE* AssetOpen(const char* file)
{
	return fopen(file, ASSETOPEN_MODE);
}

#elif defined(__ANDROID__) && __ANDROID__
// Android ----------------------------------------------------------

#include <SDL_system.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define MKDIR_MODE 0777

static int mkdir_one(const char *path, mode_t mode) {
	if (mkdir(path, mode) != 0 && errno != EEXIST)
		return -1;
	return 0;
}

static int mkdir_parents(const char *path, mode_t mode) {
	char *copypath = strdup(path);
	char *start = copypath;
	char *next;

	int status = 0;
	while (status == 0 && (next = strchr(start, '/'))) {
		if (next != start) {
			// skip the root directory and double-slashes
			*next = '\0';
			status = mkdir_one(copypath, mode);
			*next = '/';
		}
		start = next + 1;
	}

	free(copypath);
	return status;
}

FILE* AppdataOpen(const char* file, const char* mode)
{
	char buffer[1024];

	// Only use external storage if it is both readable and writeable
	int need_flags = SDL_ANDROID_EXTERNAL_STORAGE_READ | SDL_ANDROID_EXTERNAL_STORAGE_WRITE;
	if ((SDL_AndroidGetExternalStorageState() & need_flags) == need_flags)
	{
		strcpy(buffer, SDL_AndroidGetExternalStoragePath());
	}
	else
	{
		strcpy(buffer, SDL_AndroidGetInternalStoragePath());
	}

	mkdir(buffer, MKDIR_MODE);
	strcat(buffer, file);
    return fopen(buffer, mode);
}

FILE* AssetOpen(const char* file)
{
	// Check internal storage to see if we've already extracted the file.
	char fname_buf[1024];
	sprintf(fname_buf, "%s/%s", SDL_AndroidGetInternalStoragePath(), file);

	FILE* fp = fopen(fname_buf, ASSETOPEN_MODE);
	if (!fp) {
		// Not in internal storage, so ask SDL to pull it from the asset system.
		SDL_RWops *rw = SDL_RWFromFile(file, ASSETOPEN_MODE);
		if (!rw) {
			return nullptr;
		}

		mkdir_parents(fname_buf, MKDIR_MODE);
		fp = fopen(fname_buf, "wb");
		if (!fp) {
			return nullptr;
		}

		// Copy everything.
		char buffer[4096];
		int read;
		while ((read = SDL_RWread(rw, buffer, 1, sizeof(buffer))) > 0) {
			fwrite(buffer, 1, read, fp);
		}

		// Return a FILE* pointing to the extracted asset.
		fclose(fp);
		fp = fopen(fname_buf, ASSETOPEN_MODE);
	}
	return fp;
}

#else
// Default ----------------------------------------------------------

FILE* AppdataOpen(const char* file, const char* mode)
{
    return fopen(file, mode);
}

FILE* AssetOpen(const char* file)
{
	return fopen(file, ASSETOPEN_MODE);
}

#endif
