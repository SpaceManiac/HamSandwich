#include "appdata.h"
#include "log.h"
#include "erase_if.h"
#include "jamultypes.h"
#include "nsis.h"
#include "inno.h"
#include "metadata.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <set>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#ifndef _MSC_VER
#include <unistd.h>
#include <sys/wait.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <SDL_platform.h>
#include <SDL_rwops.h>
#include <SDL_messagebox.h>

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

Pseudocode for SDL_RWFromFile(file, mode):
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

// A single VFS provider, read-only by default.
class Vfs {
	Vfs(const Vfs&) = delete;
	Vfs(Vfs&&) = default;
	Vfs& operator=(const Vfs&) = delete;
	Vfs& operator=(Vfs&&) = default;
public:
	Vfs() {}
	virtual ~Vfs() {}

	virtual FILE* open_stdio(const char* filename) = 0;
	virtual SDL_RWops* open_sdl(const char* filename) = 0;
	virtual bool list_dir(const char* directory, std::set<std::string>& output) = 0;
};

// A single writeable VFS provider.
class WriteVfs : public Vfs {
public:
	WriteVfs() {}
	virtual ~WriteVfs() {}

	virtual FILE* open_write_stdio(const char* filename) = 0;
	virtual bool delete_file(const char* filename) = 0;
};

// A pair of Vfs and mountpoint.
struct Mount {
	std::unique_ptr<Vfs> vfs;
	std::string mountpoint;

	Mount(std::unique_ptr<Vfs>&& vfs, const std::string& mountpoint = "")
		: vfs(std::move(vfs))
		, mountpoint(mountpoint)
	{
		// Strip trailing '/' from mountpoint.
		if (!this->mountpoint.empty() && this->mountpoint.back() == '/') {
			this->mountpoint.erase(this->mountpoint.size() - 1);
		}
	}

	const char* matches(const char* filename) const;
};

// A full filesystem, including a list of mounts and the write (appdata) mount.
struct VfsStack {
	std::vector<Mount> mounts;
	std::unique_ptr<WriteVfs> write_mount;
public:
	VfsStack() {}

	void push_back(std::unique_ptr<Vfs>&& entry, std::string mountpoint = "");

	// Returns true if this VfsStack is empty and therefore not useable.
	bool empty() const { return mounts.empty() && !write_mount; }

	// Get the Vfs that should be used for writing.
	WriteVfs* appdata() { return write_mount.get(); }

	// Forward to children
	SDL_RWops* open_sdl(const char* filename);
};

// ----------------------------------------------------------------------------
// Stdio VFS implementation
class StdioVfs : public WriteVfs {
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

FILE* StdioVfs::open_stdio_internal(const char* file, const char* mode, bool write) {
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

	if (!fp && write) {
		LogError("fopen(%s, %s): %s", buffer.c_str(), mode, strerror(errno));
	}
	return fp;
}

FILE* StdioVfs::open_write_stdio(const char* file) {
	return open_stdio_internal(file, "wb", true);
}

FILE* StdioVfs::open_stdio(const char* file) {
	return open_stdio_internal(file, "rb", false);
}

SDL_RWops* StdioVfs::open_sdl(const char* filename) {
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

bool StdioVfs::list_dir(const char* directory, std::set<std::string>& output) {
	std::string buffer = prefix;
	buffer.append("/");
	buffer.append(directory);
	DIR* dir = opendir(buffer.c_str());
	if (!dir) {
		return false;
	}

	while (struct dirent *dp = readdir(dir)) {
		output.insert(dp->d_name);
	}

	closedir(dir);
	return true;
}

#elif defined(_MSC_VER)
#include <io.h>

bool StdioVfs::list_dir(const char* directory, std::set<std::string>& output) {
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
	vanilla::nsis::Archive archive;
public:
	NsisVfs(SDL_RWops* fp) : archive(fp) {}
	FILE* open_stdio(const char* filename);
	SDL_RWops* open_sdl(const char* filename);
	bool list_dir(const char* directory, std::set<std::string>& output);
};

FILE* NsisVfs::open_stdio(const char* filename) {
	SDL_RWops* rw = open_sdl(filename);
	return rw ? fp_from_bundle(filename, "rb", rw, ".nsis_tmp", true) : nullptr;
}

SDL_RWops* NsisVfs::open_sdl(const char* filename) {
	return archive.open_file(filename);
}

bool NsisVfs::list_dir(const char* directory, std::set<std::string>& output) {
	return archive.list_dir(directory, output);
}

// ----------------------------------------------------------------------------
// Inno VFS implementation

class InnoVfs : public Vfs {
	vanilla::inno::Archive archive;
public:
	InnoVfs(FILE* fp) : archive(fp) {}
	FILE* open_stdio(const char* filename);
	SDL_RWops* open_sdl(const char* filename);
	bool list_dir(const char* directory, std::set<std::string>& output);
};

FILE* InnoVfs::open_stdio(const char* filename) {
	SDL_RWops* rw = open_sdl(filename);
	return rw ? fp_from_bundle(filename, "rb", rw, ".inno_tmp", true) : nullptr;
}

SDL_RWops* InnoVfs::open_sdl(const char* filename) {
	return archive.open_file(filename);
}

bool InnoVfs::list_dir(const char* directory, std::set<std::string>& output) {
	return archive.list_dir(directory, output);
}

static Mount init_vfs_spec(const char* what, const char* spec) {
	std::string spec2 = spec;
	char* mountpoint = spec2.data();
	char* kind = mountpoint + strcspn(mountpoint, "@");
	*kind++ = 0;
	char* param = kind + strcspn(kind, "@");
	*param++ = 0;

	if (!strcmp(kind, "stdio")) {
		return { std::make_unique<StdioVfs>(param), mountpoint };
	} else if (!strcmp(kind, "nsis")) {
		SDL_RWops* fp = SDL_RWFromFile(param, "rb");
		if (!fp) {
			LogError("%s: failed to open '%s' in VFS spec '%s'", what, param, spec);
			return { nullptr };
		}
		return { std::make_unique<NsisVfs>(fp), mountpoint };
	} else if (!strcmp(kind, "inno")) {
		FILE* fp = fopen(param, "rb");
		if (!fp) {
			LogError("%s: failed to open '%s' in VFS spec '%s'", what, param, spec);
			return { nullptr };
		}
		return { std::make_unique<InnoVfs>(fp), mountpoint };
	} else {
		LogError("%s: unknown kind '%s' in VFS spec '%s'", what, kind, spec);
		return { nullptr };
	}
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
	buffer.append(GetHamSandwichMetadata()->appdata_folder_name);

	VfsStack result;
	result.write_mount = std::make_unique<StdioVfs>(buffer);
	result.push_back(std::make_unique<StdioVfs>("."));
	return result;
}

// ----------------------------------------------------------------------------
#elif defined(__EMSCRIPTEN__)
// Emscripten configuration

#include <emscripten.h>

static VfsStack default_vfs_stack() {
	std::string buffer = "/appdata/";
	buffer.append(GetHamSandwichMetadata()->appdata_folder_name);

	VfsStack result;
	result.write_mount = std::make_unique<StdioVfs>(buffer);
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
	FILE* open_stdio(const char* filename);
	SDL_RWops* open_sdl(const char* filename);
	bool list_dir(const char* directory, std::set<std::string>& output);
};

FILE* AndroidBundleVfs::open_stdio(const char* filename) {
	SDL_RWops* rw = open_sdl(filename);
	if (!rw) {
		return nullptr;
	}

	// Use a directory which definitely doesn't overlap with appdata.
	std::string tempdir = SDL_AndroidGetInternalStoragePath();
	tempdir.append("/.bundle_tmp");
	return fp_from_bundle(file, "rb", rw, tempdir.c_str(), false);
}

SDL_RWops* AndroidBundleVfs::open_sdl(const char* filename) {
	return SDL_RWFromFile(file, mode);
}

#include "appdata_jni.inc"

static VfsStack default_vfs_stack() {
	VfsStack result;
	int need_flags = SDL_ANDROID_EXTERNAL_STORAGE_READ | SDL_ANDROID_EXTERNAL_STORAGE_WRITE;
	if ((SDL_AndroidGetExternalStorageState() & need_flags) == need_flags) {
		result.write_mount = std::make_unique<StdioVfs>(SDL_AndroidGetExternalStoragePath());
		result.push_back(std::make_unique<StdioVfs>(SDL_AndroidGetInternalStoragePath()));
	} else {
		result.write_mount = std::make_unique<StdioVfs>(SDL_AndroidGetInternalStoragePath());
	}
	result.push_back(std::make_unique<AndroidBundleVfs>());
	return result;
}

// ----------------------------------------------------------------------------
#else
// Naive stdio configuration

static bool detect_installers(VfsStack* result, const HamSandwichMetadata* meta) {
	// `appdata/$NAME/`
	std::string appdata = "appdata/";
	appdata.append(meta->appdata_folder_name);
	result->write_mount = std::make_unique<StdioVfs>(appdata);

	// Assets from specs
	for (int i = 0; meta->default_asset_specs[i]; ++i) {
		auto mount = init_vfs_spec("built-in", meta->default_asset_specs[i]);
		if (mount.vfs) {
			result->mounts.push_back(std::move(mount));
		} else {
			return false;
		}
	}
	return true;
}

static VfsStack default_vfs_stack() {
	const HamSandwichMetadata* meta = GetHamSandwichMetadata();
	VfsStack result;

	if (detect_installers(&result, meta)) {
		// If we found all the installers we were looking for, use 'em,
		// and also use `appdata/$NAME`
		std::string appdata = "appdata/";
		appdata.append(meta->appdata_folder_name);
		appdata.append("/");
		printf("all installers found; chdir(%s)\n", appdata.c_str());
		mkdir_parents(appdata.c_str());
		chdir(appdata.c_str());
		result.write_mount = std::make_unique<StdioVfs>(".");
	}

	return result;
}

// ----------------------------------------------------------------------------
#endif
// ----------------------------------------------------------------------------
// VfsStack implementation

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

const char* Mount::matches(const char* filename) const {
	if (mountpoint.empty()) {
		return filename;  // Blank mountpoint, always match.
	}

	if (strncasecmp(filename, mountpoint.c_str(), mountpoint.size()) == 0) {
		if (filename[mountpoint.size()] == '/') {
			return &filename[mountpoint.size() + 1];
		} else if (filename[mountpoint.size()] == '0') {
			return "";
		}
	}

	return nullptr;  // No match.
}

void VfsStack::push_back(std::unique_ptr<Vfs>&& entry, std::string mountpoint) {
	mounts.push_back(Mount { std::move(entry), mountpoint });
}

SDL_RWops* VfsStack::open_sdl(const char* filename) {
	if (write_mount) {
		if (SDL_RWops* rw = write_mount->open_sdl(filename)) {
			return rw;
		}
	}

	for (auto& mount : mounts) {
		if (const char* subfilename = mount.matches(filename)) {
			if (SDL_RWops* rw = mount.vfs->open_sdl(subfilename)) {
				return rw;
			}
		}
	}

	LogError("AssetOpen_SDL(%s): not found in any vfs", filename);
	return nullptr;
}

// Common implementation

static void missing_assets_message() {
	struct stat sb;
	// Tweak wording/ordering based on whether "installers" exists.
	if (stat("installers", &sb) == 0) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Missing Assets - HamSandwich",
			"The game's assets appear to be missing.\n"
			"\n"
			"Download the appropriate installer and save it in\n"
			"the \"installers\" folder.\n"
#if defined(_WIN32) && defined(NDEBUG)
			// Only talk about "nearby .dll files" on Windows release builds.
			"\n"
			"Alternatively, copy this .exe and nearby .dll files\n"
			"into an existing installation of the game."
#endif
			, nullptr);
	} else {
		// The .exe has been divorced from the install package.
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Missing Assets - HamSandwich",
			"The game's assets appear to be missing.\n"
			"\n"
#if defined(_WIN32) && defined(NDEBUG)
			// Only talk about "nearby .dll files" on Windows release builds.
			"Copy this .exe and nearby .dll files\n"
			"into an existing installation of the game.\n"
			"\n"
			"Alternatively, create "
#else
			"Create "
#endif
			"an \"installers\" folder and\n"
			"download the appropriate installers there.",
			nullptr);
	}
}

static VfsStack vfs_stack_from_env() {
	VfsStack result;
	if (const char *appdata_spec = getenv("HSW_APPDATA")) {
		result.write_mount = std::make_unique<StdioVfs>(appdata_spec);

		char buffer[32];
		for (int i = 0; i < 1024; ++i) {
			sprintf(buffer, "HSW_ASSETS_%d", i);
			if (const char* asset_spec = getenv(buffer)) {
				auto mount = init_vfs_spec(buffer, asset_spec);
				if (mount.vfs) {
					result.mounts.push_back(std::move(mount));
				}
			} else {
				break;
			}
		}
	} else {
		result = default_vfs_stack();
	}
	return result;
}

static bool check_assets(VfsStack& vfs) {
	// Every game has this asset, so use it to sanity check.
	SDL_RWops* check = vfs.open_sdl("graphics/verdana.jft");
	if (check) {
		SDL_RWclose(check);
		return true;
	}

	return false;
}

static bool run_download_helper() {
#if defined(_WIN32)
	if (GetFileAttributesA("launcher.exe") != INVALID_FILE_ATTRIBUTES) {
		std::string cmdline = "launcher.exe --mini-gui ";
		cmdline.append(GetHamSandwichMetadata()->appdata_folder_name);

		STARTUPINFOA startupInfo = {};
		PROCESS_INFORMATION processInfo = {};
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.dwFlags |= STARTF_USESHOWWINDOW;
		startupInfo.wShowWindow = SW_HIDE;
		if (CreateProcessA("launcher.exe", cmdline.data(), nullptr, nullptr, false, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &processInfo)) {
			WaitForSingleObject(processInfo.hProcess, INFINITE);
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
			return true;
		}
	}
#elif !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
	struct stat sb;
	if (stat("launcher", &sb) == 0) {
		int child_pid = fork();
		if (child_pid == 0) {
			char first[] = "./launcher";
			char second[] = "--mini-gui";
			std::string third = GetHamSandwichMetadata()->appdata_folder_name;
			char* const argv[] = { first, second, third.data(), nullptr };
			exit(execvp(argv[0], argv));
		} else if (child_pid > 0) {
			int wstatus;
			if (waitpid(child_pid, &wstatus, 0) >= 0) {
				if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0) {
					return true;
				}
			}
		}
	}
#endif
	return false;
}

static VfsStack init_vfs_stack() {
	VfsStack result = vfs_stack_from_env();
	if (check_assets(result)) {
		return result;
	}

	if (run_download_helper()) {
		result = vfs_stack_from_env();
		if (check_assets(result)) {
			return result;
		}
	}

	missing_assets_message();
	exit(1);
}

// Android does not play nice with static initializers.
static VfsStack vfs_stack;

// ----------------------------------------------------------------------------
// Public interface

void AppdataInit() {
	vfs_stack = init_vfs_stack();
}

bool AppdataIsInit() {
	return !vfs_stack.empty();
}

FILE* AssetOpen(const char* filename) {
	if (vfs_stack.write_mount) {
		if (FILE* fp = vfs_stack.write_mount->open_stdio(filename)) {
			return fp;
		}
	}

	for (auto& mount : vfs_stack.mounts) {
		if (const char* subfilename = mount.matches(filename)) {
			if (FILE* fp = mount.vfs->open_stdio(subfilename)) {
				return fp;
			}
		}
	}

	LogError("AssetOpen(%s): not found in any vfs", filename);
	return nullptr;
}

SDL_RWops* AssetOpen_SDL(const char* filename) {
	return vfs_stack.open_sdl(filename);
}

FILE* AppdataOpen_Write(const char* filename) {
	if (vfs_stack.write_mount) {
		return vfs_stack.write_mount->open_write_stdio(filename);
	} else {
		LogError("AppdataOpen_Write(%s): no write vfs mounted", filename);
		return nullptr;
	}
}

void AppdataDelete(const char* filename) {
	if (vfs_stack.write_mount) {
		vfs_stack.write_mount->delete_file(filename);
	} else {
		LogError("AppdataDelete(%s): no write vfs mounted", filename);
	}
}

#ifndef HAS_APPDATA_SYNC
void AppdataSync() {}
#endif

std::vector<std::string> ListDirectory(const char* directory, const char* extension, size_t maxlen) {
	std::set<std::string> output;

	if (vfs_stack.write_mount) {
		vfs_stack.write_mount->list_dir(directory, output);
	}

	for (auto& mount : vfs_stack.mounts) {
		if (mount.mountpoint.empty()) {
			mount.vfs->list_dir(directory, output);
		} else if (const char* subdirectory = mount.matches(directory)) {
			std::string mountpoint_slash = mount.mountpoint;
			mountpoint_slash.push_back('/');

			std::set<std::string> intermediate;
			mount.vfs->list_dir(subdirectory, intermediate);

			for (auto each : intermediate) {
				each.insert(0, mountpoint_slash);
				output.insert(each);
			}
		}
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

	return { output.begin(), output.end() };;
}

// Aliases.
FILE* AppdataOpen(const char* filename) {
	return AssetOpen(filename);
}

FILE* AssetOpen_Write(const char* filename) {
	return AppdataOpen_Write(filename);
}
