#include "appdata.h"
#include "log.h"
#include "erase_if.h"
#include "jamultypes.h"
#include "metadata.h"
#include "extern.h"
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

#include <SDL_rwops.h>
#include <SDL_messagebox.h>

#include <vanilla_extract.h>

using vanilla::Vfs;
using vanilla::WriteVfs;
using vanilla::Mount;
using vanilla::VfsStack;

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

static Mount init_vfs_spec(const char* what, const char* spec) {
	std::string spec2 = spec;
	char* mountpoint = spec2.data();
	char* kind = mountpoint + strcspn(mountpoint, "@");
	*kind++ = 0;
	char* param = kind + strcspn(kind, "@");
	*param++ = 0;

	if (!strcmp(kind, "stdio")) {
		return { vanilla::open_stdio(param), mountpoint };
	} else if (!strcmp(kind, "zip")) {
		return { vanilla::open_zip(param), mountpoint };
	} else if (!strcmp(kind, "nsis")) {
		SDL_RWops* fp = SDL_RWFromFile(param, "rb");
		if (!fp) {
			LogError("%s: failed to open '%s' in VFS spec '%s'", what, param, spec);
			return { nullptr };
		}
		return { vanilla::open_nsis(fp), mountpoint };
	} else if (!strcmp(kind, "inno")) {
		FILE* fp = fopen(param, "rb");
		if (!fp) {
			LogError("%s: failed to open '%s' in VFS spec '%s'", what, param, spec);
			return { nullptr };
		}
		return { vanilla::open_inno(fp), mountpoint };
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
	buffer.append(g_HamExtern.GetHamSandwichMetadata()->appdata_folder_name);

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
	buffer.append(g_HamExtern.GetHamSandwichMetadata()->appdata_folder_name);

	VfsStack result;
	result.write_mount = vanilla::open_stdio(buffer.c_str());
	result.push_back(vanilla::open_stdio(""));
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
// Android configuration

#include <SDL_system.h>

static VfsStack default_vfs_stack() {
	VfsStack result;
	int need_flags = SDL_ANDROID_EXTERNAL_STORAGE_READ | SDL_ANDROID_EXTERNAL_STORAGE_WRITE;
	if ((SDL_AndroidGetExternalStorageState() & need_flags) == need_flags) {
		result.write_mount = vanilla::open_stdio(SDL_AndroidGetExternalStoragePath());
		result.push_back(vanilla::open_stdio(SDL_AndroidGetInternalStoragePath()));
	} else {
		result.write_mount = vanilla::open_stdio(SDL_AndroidGetInternalStoragePath());
	}
	result.push_back(vanilla::open_android());
	return result;
}

// ----------------------------------------------------------------------------
#else
// Naive stdio configuration

static bool detect_installers(VfsStack* result, const HamSandwichMetadata* meta) {
	// `appdata/$NAME/`
	std::string appdata = "appdata/";
	appdata.append(meta->appdata_folder_name);
	result->write_mount = vanilla::open_stdio(appdata.c_str());

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
	const HamSandwichMetadata* meta = g_HamExtern.GetHamSandwichMetadata();
	VfsStack result;

	if (detect_installers(&result, meta)) {
		// If we found all the installers we were looking for, use 'em,
		// and also use `appdata/$NAME`
		std::string appdata = "appdata/";
		appdata.append(meta->appdata_folder_name);
		appdata.append("/");
		printf("all installers found; chdir(%s)\n", appdata.c_str());
		vanilla::mkdir_parents(appdata.c_str());
		chdir(appdata.c_str());
		result.write_mount = vanilla::open_stdio(".");
	}

	return result;
}

// ----------------------------------------------------------------------------
#endif


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
	if (const char *appdata_spec = SDL_getenv("HSW_APPDATA"); appdata_spec && *appdata_spec) {
		result.write_mount = vanilla::open_stdio(appdata_spec);

		char buffer[32];
		for (int i = 0; i < 1024; ++i) {
			sprintf(buffer, "HSW_ASSETS_%d", i);
			if (const char* asset_spec = SDL_getenv(buffer); asset_spec && *asset_spec) {
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
		cmdline.append(g_HamExtern.GetHamSandwichMetadata()->appdata_folder_name);

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
			std::string third = g_HamExtern.GetHamSandwichMetadata()->appdata_folder_name;
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
