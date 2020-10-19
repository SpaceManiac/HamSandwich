#include "appdata.h"
#include "log.h"
#include "erase_if.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

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

// ----------------------------------------------------------------------------
// Stdio helpers
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

// ----------------------------------------------------------------------------
// VFS interface
class Vfs {
	Vfs(const Vfs&) = delete;
	Vfs(Vfs&&) = default;
	Vfs& operator=(const Vfs&) = delete;
	Vfs& operator=(Vfs&&) = default;
public:
	Vfs() {}
	virtual ~Vfs() {}

	virtual FILE* open_stdio(const char* file, const char* mode, bool write) = 0;
	virtual SDL_RWops* open_sdl(const char* file, const char* mode, bool write) = 0;
	virtual bool list_dir(const char* directory, std::vector<std::string>& output) = 0;
};

typedef std::vector<std::unique_ptr<Vfs>> VfsStack;

// ----------------------------------------------------------------------------
// Stdio VFS implementation
class StdioVfs : public Vfs {
	std::string prefix;
public:
	StdioVfs(std::string prefix) : prefix(prefix) {}
	FILE* open_stdio(const char* file, const char* mode, bool write);
	SDL_RWops* open_sdl(const char* file, const char* mode, bool write);
	bool list_dir(const char* directory, std::vector<std::string>& output);
};

FILE* StdioVfs::open_stdio(const char* file, const char* mode, bool write) {
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(file);
	if (write) {
		mkdir_parents(buffer.c_str(), MKDIR_MODE);
	}
	FILE* fp = fopen(buffer.c_str(), mode);
	if (!fp && write) {
		LogError("fopen(%s, %s): %s", buffer.c_str(), mode, strerror(errno));
	}
	return fp;
}

SDL_RWops* StdioVfs::open_sdl(const char* file, const char* mode, bool write) {
	FILE* fp = open_stdio(file, mode, write);
	return fp ? SDL_RWFromFP(fp, SDL_TRUE) : nullptr;
}

#ifdef __GNUC__
#include <dirent.h>

bool StdioVfs::list_dir(const char* directory, std::vector<std::string>& output) {
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(directory);
	DIR* dir = opendir(buffer.c_str());
	if (!dir) {
		return false;
	}

	while (struct dirent *dp = readdir(dir)) {
		output.push_back(dp->d_name);
	}

	closedir(dir);
	return true;
}

#elif defined(_MSC_VER)
#include <io.h>

bool StdioVfs::list_dir(const char* directory, std::vector<std::string>& output) {
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(directory);
	buffer.append("/*");

	struct _finddata_t filedata;
	long hFile = _findfirst(buf, &filedata);
	if (hFile == -1) {
		return false;
	}

	do {
		output.push_back(finddata.name);
	} while (_findnext(hFile, &filedata) == 0);

	_findclose(hFile);
	return true;
}

#endif  // __GNUC__ and _MSC_VER

// ----------------------------------------------------------------------------
#if 0  // #ifdef _WIN32
// Windows %APPDATA% configuration (not currently in use)

#include <io.h>
#include <shlobj.h> // for SHGetFolderPath
#ifdef _MSC_VER
#include <direct.h>
#endif

VfsStack init_vfs_stack() {
	char get_folder_path[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, get_folder_path);

	std::string buffer = get_folder_path;
	buffer.append("\\Hamumu\\");
	buffer.append(AppdataFolderName());

	VfsStack result;
	result.push_back(std::make_unique<StdioVfs>(buffer));
	result.push_back(std::make_unique<StdioVfs>("."));
	return result;
}

// ----------------------------------------------------------------------------
#elif defined(__EMSCRIPTEN__)
// Emscripten configuration

#include <emscripten.h>

VfsStack init_vfs_stack() {
	std::string buffer = "/appdata/";
	buffer.append(AppdataFolderName());

	VfsStack result;
	result.push_back(std::make_unique<StdioVfs>(buffer));
	result.push_back(std::make_unique<StdioVfs>(""));
	return result;
}

#define HAS_APPDATA_SYNC
void AppdataSync() {
	EM_ASM(
		Module.fsSave();
	);
}

// ----------------------------------------------------------------------------
#elif defined(__ANDROID__) && __ANDROID__
// Android VFS implementation and Android configuration

#include <SDL_system.h>
#include <string.h>

class AndroidBundleVfs : public Vfs {
public:
	FILE* open_stdio(const char* file, const char* mode, bool write);
	SDL_RWops* open_sdl(const char* file, const char* mode, bool write);
	bool list_dir(const char* directory, std::vector<std::string>& output);
};

FILE* AndroidBundleVfs::open_stdio(const char* file, const char* mode, bool write) {
	if (write) {
		LogError("open_bundle(%s, %s) does not support write modes", file, mode);
		return nullptr;
	}

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

SDL_RWops* AndroidBundleVfs::open_sdl(const char* file, const char* mode, bool write) {
	if (write) {
		LogError("open_bundle(%s, %s) does not support write modes", file, mode);
		return nullptr;
	}

	SDL_RWops* rw = SDL_RWFromFile(file, mode);
	if (!rw && write) {
		LogError("SDL_RWFromFile(%s, %s): %s", file, mode, SDL_GetError());
	}
	return rw;
}

bool AndroidBundleVfs::list_dir(const char* directory, std::vector<std::string>& output) {
	// TODO
	return false;
}

VfsStack init_vfs_stack() {
	VfsStack result;
	int need_flags = SDL_ANDROID_EXTERNAL_STORAGE_READ | SDL_ANDROID_EXTERNAL_STORAGE_WRITE;
	if ((SDL_AndroidGetExternalStorageState() & need_flags) == need_flags) {
		result.push_back(std::make_unique<StdioVfs>(SDL_AndroidGetExternalStoragePath()));
	}
	result.push_back(std::make_unique<StdioVfs>(SDL_AndroidGetInternalStoragePath()));
	result.push_back(std::make_unique<AndroidBundleVfs>());
	return result;
}

// ----------------------------------------------------------------------------
#else
// Naive stdio configuration

VfsStack init_vfs_stack() {
	VfsStack result;
	result.push_back(std::make_unique<StdioVfs>("."));
	return result;
}

// ----------------------------------------------------------------------------
#endif
// Common implementation and interface

// Android does not play nice with static initializers.
static VfsStack vfs_stack;

FILE* AppdataOpen(const char* file, const char* mode) {
	return AssetOpen(file, mode);
}

FILE* AssetOpen(const char* file, const char* mode) {
	if (vfs_stack.empty()) {
		vfs_stack = init_vfs_stack();
	}

	bool write = is_write_mode(mode);
	if (write) {
		return vfs_stack.front()->open_stdio(file, mode, write);
	}

	for (auto& vfs : vfs_stack) {
		FILE* fp = vfs->open_stdio(file, mode, write);
		if (fp) {
			return fp;
		}
	}

	LogError("AssetOpen(%s, %s): not found in any vfs", file, mode);
	return nullptr;
}

SDL_RWops* AssetOpen_SDL(const char* file, const char* mode) {
	if (vfs_stack.empty()) {
		vfs_stack = init_vfs_stack();
	}

	bool write = is_write_mode(mode);
	if (write) {
		return vfs_stack.front()->open_sdl(file, mode, write);
	}

	for (auto& vfs : vfs_stack) {
		SDL_RWops* rw = vfs->open_sdl(file, mode, write);
		if (rw) {
			return rw;
		}
	}

	LogError("AssetOpen_SDL(%s, %s): not found in any vfs", file, mode);
	return nullptr;
}

std::vector<std::string> ListDirectory(const char* directory, const char* extension, size_t maxlen) {
	if (vfs_stack.empty()) {
		vfs_stack = init_vfs_stack();
	}

	std::vector<std::string> output;
	for (auto& vfs : vfs_stack) {
		vfs->list_dir(directory, output);
	}

	if (extension || maxlen > 0) {
		size_t extlen = extension ? strlen(extension) : 0;
		erase_if(output, [=](const std::string& value) {
			size_t len = value.size();
			if (maxlen > 0 && len >= maxlen) {
				return true;
			}
			if (extlen > 0 && (len < extlen || strcasecmp(extension, &value.c_str()[len - extlen]))) {
				return true;
			}
			return false;
		});
	}

	return output;
}

#ifndef HAS_APPDATA_SYNC
void AppdataSync() {}
#endif
