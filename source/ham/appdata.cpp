#include "appdata.h"
#include "log.h"
#include <string>

#ifdef SDL_UNPREFIXED
	#include <SDL_platform.h>
	#include <SDL_rwops.h>
#else  // SDL_UNPREFIXED
	#include <SDL2/SDL_platform.h>
	#include <SDL2/SDL_rwops.h>
#endif  // SDL_UNPREFIXED

/*
The interface in appdata.h currently distinguishes between AppdataOpen
and AssetOpen, but the implementation treats them the same, instead switching
based on whether the mode is read-only.

We always first try to use the read-write ("appdata") directory. If the mode
is read-only, we then try to use the read-only ("asset") directories in
sequence.

Emscripten
	RW: /appdata/$GAME
	Ro: /
		Provided by build process + JS environment
Android
	if external storage is writeable:
		RW: $EXTERNAL
		Ro: $INTERNAL
	else:
		RW: $INTERNAL
	Ro: SDL_RWFromFile, which reads from .apk
		Tempfiles in $INTERNAL/.bundle_tmp when needed
		Provided by build process
Default
	RW: $PWD

Psuedocode for SDL_RWFromFile(file, mode):
    if android:
        if file is absolute:
            return fopen(file, mode) if successful
        else:
            return fopen($INTERNAL/file, mode) if successful
        return bundled asset from .apk
    else if windows:
        return windows_file_open(file, mode)
    else:
        if apple and mode is readonly:
            return fopen($APP_BUNDLE/file, mode) if successful
        return fopen(file, mode)
*/

// Common code
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

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

	if (status != 0)
		LogError("mkdirs(%s): %s", copypath, strerror(errno));

	free(copypath);
	return status;
}

static bool is_write_mode(const char* mode) {
	return mode[0] == 'w' || mode[0] == 'a' || (mode[0] == 'r' && mode[1] == '+');
}
#endif

// TODO: re-enable this when there's some means of porting existing installs.
#if 0  // #ifdef _WIN32
// Windows ----------------------------------------------------------

#include <io.h>
#include <shlobj.h> // for SHGetFolderPath
#ifdef _MSC_VER
#include <direct.h>
#endif

FILE* AppdataOpen(const char* file, const char* mode) {
	char get_folder_path[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, get_folder_path);

	std::string buffer = get_folder_path;
	buffer.append("\\Hamumu\\");
	buffer.append(AppdataFolderName());
	buffer.append("\\");
	buffer.append(file);
	if (is_write_mode(mode)) {
		mkdir_parents(file, MKDIR_MODE);  // TODO: mode is meaningless on win32
	}
	return fopen(file, mode);
}

FILE* AssetOpen(const char* file, const char* mode) {
	return fopen(file, mode);
}

SDL_RWops* AssetOpen_SDL(const char* file, const char* mode) {
	return SDL_RWFromFile(file, mode);
}

#elif defined(__EMSCRIPTEN__)
// Emscripten -------------------------------------------------------

#include <emscripten.h>

static FILE* open_file(const char* file, const char* mode) {
	bool write_mode = is_write_mode(mode);

	// RW: /appdata/$GAME
	std::string buffer = "/appdata/";
	buffer.append(AppdataFolderName());
	buffer.append("/");
	buffer.append(file);
	if (write_mode) {
		mkdir_parents(buffer.c_str(), MKDIR_MODE);
	}
	FILE* fp = fopen(buffer.c_str(), mode);
	if (fp) {
		return fp;
	}

	if (write_mode) {
		LogError("fopen(%s, %s): %s", buffer.c_str(), mode, strerror(errno));
		return nullptr;
	}

	// Ro: /
	return fopen(file, mode);
}

#define HAS_APPDATA_SYNC
void AppdataSync() {
	EM_ASM(
		Module.fsSave();
	);
}

#elif defined(__ANDROID__) && __ANDROID__
// Android ----------------------------------------------------------

#include <SDL_system.h>
#include <string.h>

static FILE* open_storage(const char* file, const char* mode, bool write_mode) {
	std::string buffer;

	// Only use external storage if it is both readable and writeable
	int need_flags = SDL_ANDROID_EXTERNAL_STORAGE_READ | SDL_ANDROID_EXTERNAL_STORAGE_WRITE;
	if ((SDL_AndroidGetExternalStorageState() & need_flags) == need_flags) {
		// RW: $EXTERNAL
		buffer = SDL_AndroidGetExternalStoragePath();
		buffer.append("/");
		buffer.append(file);
		if (write_mode) {
			mkdir_parents(buffer.c_str(), MKDIR_MODE);
		}
		FILE* fp = fopen(buffer.c_str(), mode);
		if (fp) {
			return fp;
		}

		if (write_mode) {
			LogError("open_storage external fopen(%s, %s): %s", buffer.c_str(), mode, strerror(errno));
			return nullptr;
		}
		// Ro: $INTERNAL
	} // else RW: $INTERNAL

	buffer = SDL_AndroidGetInternalStoragePath();
	buffer.append("/");
	buffer.append(file);
	FILE* fp = fopen(buffer.c_str(), mode);
	if (fp) {
		return fp;
	}

	if (write_mode) {
		LogError("open_storage internal fopen(%s, %s): %s", buffer.c_str(), mode, strerror(errno));
	}
	return nullptr;
}

// requires !is_write_mode(mode)
static FILE* open_bundle(const char* file, const char* mode) {
	// Ro: apk bundle
	LogDebug("open_bundle(%s, %s)", file, mode);

	// Check internal storage to see if we've already extracted the file.
	std::string fname_buf = SDL_AndroidGetInternalStoragePath();
	// Use a directory which definitely doesn't overlap with appdata.
	fname_buf.append("/.bundle_tmp/");
	fname_buf.append(file);
	// If we have, delete it and extract it again, in case it's changed.
	unlink(fname_buf.c_str());

	// Not in internal storage, so ask SDL to pull it from the asset system.
	SDL_RWops *rw = SDL_RWFromFile(file, mode);
	if (!rw) {
		LogError("open_bundle(%s) bad SDL: %s", file, SDL_GetError());
		return nullptr;
	}

	mkdir_parents(fname_buf.c_str(), MKDIR_MODE);
	FILE* fp = fopen(fname_buf.c_str(), "wb");
	if (!fp) {
		LogError("open_bundle(%s) bad save: %s", file, strerror(errno));
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
	fp = fopen(fname_buf.c_str(), mode);
	if (!fp) {
		LogError("open_bundle(%s) bad readback: %s", file, strerror(errno));
	}
	return fp;
}

static FILE* open_file(const char* file, const char* mode) {
	bool write_mode = is_write_mode(mode);

	FILE* fp = open_storage(file, mode, write_mode);
	if (fp || write_mode) {
		// open_storage already printed the error
		return fp;
	}

	// Ro: .apk bundle via
	return open_bundle(file, mode);
}

#define HAS_ASSETOPEN_SDL
SDL_RWops* AssetOpen_SDL(const char* file, const char* mode) {
	bool write_mode = is_write_mode(mode);

	FILE* fp = open_storage(file, mode, write_mode);
	if (fp) {
		return SDL_RWFromFP(fp, SDL_TRUE);
	}
	if (write_mode) {
		// open_storage already printed the error
		return nullptr;
	}

	// Will try to read from Android internal storage, or else
	// pull from the asset system.
	SDL_RWops* rw = SDL_RWFromFile(file, mode);
	if (!rw) {
		LogError("AssetOpen_SDL(%s, %s): %s", file, mode, SDL_GetError());
	}
	return rw;
}

#else
// Default ----------------------------------------------------------

static FILE* open_file(const char* file, const char* mode) {
	FILE* fp = fopen(file, mode);
	if (!fp) {
		LogError("open_file(%s, %s): %s", file, mode, strerror(errno));
	}
	return fp;
}

#endif
// Common -----------------------------------------------------------

FILE* AppdataOpen(const char* file, const char* mode) {
	return open_file(file, mode);
}

FILE* AssetOpen(const char* file, const char* mode) {
	return open_file(file, mode);
}

#ifndef HAS_ASSETOPEN_SDL
SDL_RWops* AssetOpen_SDL(const char* file, const char* mode) {
	FILE* fp = open_file(file, mode);
	return fp ? SDL_RWFromFP(fp, SDL_TRUE) : nullptr;
}
#endif

#ifndef HAS_APPDATA_SYNC
void AppdataSync() {}
#endif
