#include "appdata.h"
#include "log.h"
#include "erase_if.h"
#include "jamultypes.h"
#include "nsis.h"
#include "inno.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

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
#ifdef _WIN32
#include <direct.h>

#define platform_mkdir(path) _mkdir(path)
#else
#define platform_mkdir(path) mkdir(path, 0777)
#endif

static int mkdir_one(const char *path) {
	if (platform_mkdir(path) != 0 && errno != EEXIST)
		return -1;
	return 0;
}

static int mkdir_parents(const char *path) {
	std::string copypath = path;
	char *start = copypath.data();
	char *next;

	int status = 0;
	while (status == 0 && (next = strchr(start, '/'))) {
		if (next != start) {
			// skip the root directory and double-slashes
			*next = '\0';
			status = mkdir_one(copypath.c_str());
			*next = '/';
		}
		start = next + 1;
	}

	if (status != 0)
		LogError("mkdirs(%s): %s", copypath.c_str(), strerror(errno));

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
	virtual bool delete_file(const char* file);
};

bool Vfs::delete_file(const char* file) {
	(void) file;
	return false;
}

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
	bool delete_file(const char* file);
};

FILE* StdioVfs::open_stdio(const char* file, const char* mode, bool write) {
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(file);
	if (write) {
		mkdir_parents(buffer.c_str());
	}
	FILE* fp = fopen(buffer.c_str(), mode);

#ifndef _WIN32
	// On non-Windows, try to case-correct file lookups
	if (!fp) {
		size_t i = buffer.rfind("/");
		buffer[i] = '\0';

		std::vector<std::string> temp;
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

	struct _finddata_t finddata;
	long hFile = _findfirst(buffer.c_str(), &finddata);
	if (hFile == -1) {
		return false;
	}

	do {
		output.push_back(finddata.name);
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

	if (status == 0) {
		return true;
	} else {
		LogError("unlink(%s): %s", buffer.c_str(), strerror(errno));
		return false;
	}
}

// ----------------------------------------------------------------------------
// "Extract to temporary directory" helper
FILE* fp_from_bundle(const char* file, const char* mode, SDL_RWops* rw, const char* tempdir, bool reuse_safe) {
	// Check internal storage to see if we've already extracted the file.
	std::string fname_buf = tempdir;
	fname_buf.append("/");
	fname_buf.append(file);
	if (reuse_safe) {
		FILE* fp = fopen(fname_buf.c_str(), mode);
		if (fp) {
			return fp;
		}
	}
	// If we have, delete it and extract it again, in case it's changed.
	unlink(fname_buf.c_str());

	mkdir_parents(fname_buf.c_str());
	FILE* fp = fopen(fname_buf.c_str(), "wb");
	if (!fp) {
		LogError("fp_from_bundle(%s) bad save: %s", file, strerror(errno));
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
		LogError("fp_from_bundle(%s) bad readback: %s", file, strerror(errno));
	}
	return fp;
}

// ----------------------------------------------------------------------------
// NSIS VFS implementation

class NsisVfs : public Vfs {
	nsis::Archive archive;
public:
	NsisVfs(FILE* fp) : archive(fp) {}
	FILE* open_stdio(const char* file, const char* mode, bool write);
	SDL_RWops* open_sdl(const char* file, const char* mode, bool write);
	bool list_dir(const char* directory, std::vector<std::string>& output);
};

FILE* NsisVfs::open_stdio(const char* file, const char* mode, bool write) {
	SDL_RWops* rw = open_sdl(file, mode, write);
	return rw ? fp_from_bundle(file, mode, rw, ".nsis_tmp", true) : nullptr;
}

SDL_RWops* NsisVfs::open_sdl(const char* file, const char* mode, bool write) {
	if (write) {
		LogError("NsisVfs(%s, %s): does not support write modes", file, mode);
		return nullptr;
	}

	return archive.open_file(file);
}

bool NsisVfs::list_dir(const char* directory, std::vector<std::string>& output) {
	return archive.list_dir(directory, output);
}

// ----------------------------------------------------------------------------
// Inno VFS implementation

class InnoVfs : public Vfs {
	inno::Archive archive;
public:
	InnoVfs(FILE* fp) : archive(fp) {}
	FILE* open_stdio(const char* file, const char* mode, bool write);
	SDL_RWops* open_sdl(const char* file, const char* mode, bool write);
	bool list_dir(const char* directory, std::vector<std::string>& output);
};

FILE* InnoVfs::open_stdio(const char* file, const char* mode, bool write) {
	SDL_RWops* rw = open_sdl(file, mode, write);
	return rw ? fp_from_bundle(file, mode, rw, ".inno_tmp", true) : nullptr;
}

SDL_RWops* InnoVfs::open_sdl(const char* file, const char* mode, bool write) {
	if (write) {
		LogError("InnoVfs(%s, %s): does not support write modes", file, mode);
		return nullptr;
	}

	return archive.open_file(file);
}

bool InnoVfs::list_dir(const char* directory, std::vector<std::string>& output) {
	return archive.list_dir(directory, output);
}

// ----------------------------------------------------------------------------
#if 0  // #ifdef _WIN32
// Windows %APPDATA% configuration (not currently in use)

#include <io.h>
#include <shlobj.h> // for SHGetFolderPath
#ifdef _MSC_VER
#include <direct.h>
#endif

static VfsStack default_vfs_stack() {
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

static VfsStack default_vfs_stack() {
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
		HamSandwich.fsSync();
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
	SDL_RWops* rw = open_sdl(file, mode, write);
	if (!rw) {
		return nullptr;
	}

	// Use a directory which definitely doesn't overlap with appdata.
	std::string tempdir = SDL_AndroidGetInternalStoragePath();
	tempdir.append("/.bundle_tmp");
	return fp_from_bundle(file, mode, rw, tempdir.c_str(), false);
}

SDL_RWops* AndroidBundleVfs::open_sdl(const char* file, const char* mode, bool write) {
	if (write) {
		LogError("AndroidBundleVfs(%s, %s): does not support write modes", file, mode);
		return nullptr;
	}

	SDL_RWops* rw = SDL_RWFromFile(file, mode);
	if (!rw && write) {
		LogError("SDL_RWFromFile(%s, %s): %s", file, mode, SDL_GetError());
	}
	return rw;
}

#include "appdata_jni.inc"

static VfsStack default_vfs_stack() {
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

static VfsStack default_vfs_stack() {
	VfsStack result;
	result.push_back(std::make_unique<StdioVfs>("."));
	return result;
}

// ----------------------------------------------------------------------------
#endif
// Common implementation and interface

static std::unique_ptr<Vfs> init_vfs_spec(const char* what, const char* spec) {
	std::string spec2 = spec;
	char* mountpoint = spec2.data();
	char* kind = mountpoint + strcspn(mountpoint, "@") + 1;
	kind[-1] = 0;
	char* param = kind + strcspn(kind, "@") + 1;
	param[-1] = 0;

	if (!strcmp(kind, "stdio")) {
		return std::make_unique<StdioVfs>(param);
	} else if (!strcmp(kind, "nsis")) {
		FILE* fp = fopen(param, "rb");
		if (!fp) {
			LogError("%s: failed to open '%s' in VFS spec '%s'", what, param, spec);
			return nullptr;
		}
		return std::make_unique<NsisVfs>(fp);
	} else if (!strcmp(kind, "inno")) {
		FILE* fp = fopen(param, "rb");
		if (!fp) {
			LogError("%s: failed to open '%s' in VFS spec '%s'", what, param, spec);
			return nullptr;
		}
		return std::make_unique<InnoVfs>(fp);
	} else {
		LogError("%s: unknown kind '%s' in VFS spec '%s'", what, kind, spec);
		return nullptr;
	}
}

static VfsStack init_vfs_stack() {
	if (const char *appdata_spec = getenv("HSW_APPDATA")) {
		VfsStack result;
		if (auto vfs = init_vfs_spec("HSW_APPDATA", appdata_spec)) {
			result.push_back(std::move(vfs));
		}

		char buffer[32];
		for (int i = 0; i < 1024; ++i) {
			sprintf(buffer, "HSW_ASSETS_%d", i);
			if (const char* asset_spec = getenv(buffer)) {
				if (auto vfs = init_vfs_spec(buffer, asset_spec)) {
					result.push_back(std::move(vfs));
				}
			} else {
				break;
			}
		}
		return result;
	} else {
		return default_vfs_stack();
	}
}

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

void AppdataDelete(const char* file) {
	if (vfs_stack.empty()) {
		vfs_stack = init_vfs_stack();
	}

	vfs_stack.front()->delete_file(file);
}

#ifndef HAS_APPDATA_SYNC
void AppdataSync() {}
#endif
