#include "appdata.h"
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
#endif

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <sys/wait.h>
#endif

#include <SDL.h>

#include "vanilla_extract.h"

#include "log.h"
#include "erase_if.h"
#include "jamultypes.h"
#include "metadata.h"
#include "extern.h"

using vanilla::Vfs;
using vanilla::WriteVfs;
using vanilla::Mount;
using vanilla::VfsStack;

static const HamSandwichMetadata* globalMetadata = nullptr;

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

static Mount init_vfs_spec(const char* what, const char* mountpoint, const char* kind, const char* param) {
	// For Steam releases, check if this installer is pre-extracted.
	struct stat sb;
	stat(param, &sb);
	if ((sb.st_mode & S_IFMT) == S_IFDIR && strcmp(kind, "stdio")) {
		SDL_Log("%s: ignoring '%s' and assuming '%s' is pre-extracted", what, kind, param);
		return { vanilla::open_stdio(param), mountpoint, { vanilla::VfsSourceKind::BaseGame } };
	}

	// Guess at kind. Not strictly accurate.
	// Environment variable path currently uses this guess.
	// Builtin knows more details.
	if (!strcmp(kind, "stdio")) {
		return { vanilla::open_stdio(param), mountpoint, { vanilla::VfsSourceKind::Appdata } };
	} else if (!strcmp(kind, "zip")) {
		owned::SDL_RWops fp = owned::SDL_RWFromFile(param, "rb");
		if (!fp) {
			LogError("%s: failed to open '%s' in VFS spec '%s@%s@%s'", what, param, mountpoint, kind, param);
			return { nullptr };
		}
		return { vanilla::open_zip(std::move(fp)), mountpoint, { vanilla::VfsSourceKind::Addon } };
	} else if (!strcmp(kind, "nsis")) {
		owned::SDL_RWops fp = owned::SDL_RWFromFile(param, "rb");
		if (!fp) {
			LogError("%s: failed to open '%s' in VFS spec '%s@%s@%s'", what, param, mountpoint, kind, param);
			return { nullptr };
		}
		return { vanilla::open_nsis(std::move(fp)), mountpoint, { vanilla::VfsSourceKind::BaseGame } };
	} else if (!strcmp(kind, "inno")) {
		owned::SDL_RWops fp = owned::SDL_RWFromFile(param, "rb");
		if (!fp) {
			LogError("%s: failed to open '%s' in VFS spec '%s@%s@%s'", what, param, mountpoint, kind, param);
			return { nullptr };
		}
		return { vanilla::open_inno(fp.get()), mountpoint, { vanilla::VfsSourceKind::BaseGame } };
	}
	else if (!strcmp(kind, "inno3")) {
		owned::SDL_RWops fp = owned::SDL_RWFromFile(param, "rb");
		if (!fp) {
			LogError("%s: failed to open '%s' in VFS spec '%s@%s@%s'", what, param, mountpoint, kind, param);
			return { nullptr };
		}
		return { vanilla::open_inno3(std::move(fp)), mountpoint, { vanilla::VfsSourceKind::BaseGame } };
	}
#ifdef __ANDROID__
	else if (!strcmp(kind, "android")) {
		return { vanilla::open_android(param), mountpoint, { vanilla::VfsSourceKind::BaseGame } };
	}
#endif
	else {
		LogError("%s: unknown kind '%s' in VFS spec '%s@%s@%s'", what, kind, mountpoint, kind, param);
		return { nullptr };
	}
}

static Mount init_vfs_spec_str(const char* what, const char* spec) {
	std::string spec2 = spec;
	char* mountpoint = spec2.data();
	char* kind = mountpoint + strcspn(mountpoint, "@");
	*kind++ = 0;
	char* param = kind + strcspn(kind, "@");
	*param++ = 0;
	return init_vfs_spec(what, mountpoint, kind, param);
}

// ----------------------------------------------------------------------------
#if 0  // #ifdef _WIN32
// Windows %APPDATA% configuration (not currently in use)

#include <io.h>
#include <shlobj.h> // for SHGetFolderPath
#ifdef _MSC_VER
#include <direct.h>
#endif

static VfsStack default_vfs_stack(bool* error) {
	char get_folder_path[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, get_folder_path);

	std::string buffer = get_folder_path;
	buffer.append("\\Hamumu\\");
	buffer.append(globalMetadata->appdata_folder_name);

	VfsStack result;
	result.push_back(std::make_unique<StdioVfs>("."));
	result.set_appdata(std::make_unique<StdioVfs>(buffer));
	return result;
}

// ----------------------------------------------------------------------------
#elif defined(__EMSCRIPTEN__)
// Emscripten configuration

#include <emscripten.h>

static VfsStack default_vfs_stack(bool* error) {
	std::string buffer = "/appdata/";
	buffer.append(globalMetadata->appdata_folder_name);

	VfsStack result;
	result.push_back(vanilla::open_stdio(""), "", vanilla::VfsSourceKind::BaseGame);
	result.set_appdata(vanilla::open_stdio(buffer));
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

static VfsStack default_vfs_stack(bool* error) {
	(void) error;

	VfsStack result;
	int need_flags = SDL_ANDROID_EXTERNAL_STORAGE_READ | SDL_ANDROID_EXTERNAL_STORAGE_WRITE;
	result.push_back(vanilla::open_android(), "", vanilla::VfsSourceKind::BaseGame);
	if ((SDL_AndroidGetExternalStorageState() & need_flags) == need_flags) {
		result.push_back(vanilla::open_stdio(SDL_AndroidGetInternalStoragePath()), "", vanilla::VfsSourceKind::Appdata);
		result.set_appdata(vanilla::open_stdio(SDL_AndroidGetExternalStoragePath()));
	} else {
		result.set_appdata(vanilla::open_stdio(SDL_AndroidGetInternalStoragePath()));
	}
	return result;
}

// ----------------------------------------------------------------------------
#else
// Naive stdio configuration

static VfsStack default_vfs_stack(bool* error) {
	const HamSandwichMetadata* meta = globalMetadata;
	VfsStack result;

	// Lowest priority: installers in order.
	for (int i = 0; meta->default_asset_specs[i].mountpoint; ++i) {
		const auto& spec = meta->default_asset_specs[i];
		if (spec.should_auto_mount()) {
			auto mount = init_vfs_spec("built-in", spec.mountpoint, spec.kind, spec.param);
			mount.meta.kind = spec.optional ? vanilla::VfsSourceKind::Addon : vanilla::VfsSourceKind::BaseGame;
			if (mount.vfs) {
				SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "auto %d %s: %s@%s@%s", i, spec.optional ? "enabled" : "required", spec.mountpoint, spec.kind, spec.param);
				result.push_back(std::move(mount));
			} else {
				LogError("detect_installers: failed to mount builtin[%d]: %s@%s@%s", i, spec.mountpoint, spec.kind, spec.param);
				*error = true;
			}
		} else {
			SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "auto %d disabled: %s@%s@%s", i, spec.mountpoint, spec.kind, spec.param);
		}
	}

	// High priority: custom assets provided directly.
	std::string assets = "assets/";
	assets.append(meta->appdata_folder_name);
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "auto assets: %s", assets.c_str());
	result.push_back(vanilla::open_stdio(assets), "", vanilla::VfsSourceKind::BaseGame);

	// Use `appdata/$NAME` as our appdata folder.
	std::string appdata = "appdata/";
	appdata.append(meta->appdata_folder_name);
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "auto appdata: %s", appdata.c_str());
	result.set_appdata(vanilla::open_stdio(appdata));

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

static VfsStack vfs_stack_from_env(bool* error) {
	VfsStack result;
	if (const char *appdata_spec = SDL_getenv("HSW_APPDATA"); appdata_spec && *appdata_spec) {
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "HSW_APPDATA=%s", appdata_spec);
		result.set_appdata(vanilla::open_stdio(appdata_spec));

		char buffer[32];
		for (int i = 0; i < 1024; ++i) {
			sprintf(buffer, "HSW_ASSETS_%d", i);
			if (const char* asset_spec = SDL_getenv(buffer); asset_spec && *asset_spec) {
				SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s=%s", buffer, asset_spec);
				auto mount = init_vfs_spec_str(buffer, asset_spec);
				if (mount.vfs) {
					result.push_back(std::move(mount));
				} else {
					LogError("vfs_stack_from_env: failed to mount %s=%s", buffer, asset_spec);
					*error = true;
				}
			} else {
				break;
			}
		}
	} else {
		result = default_vfs_stack(error);
	}
	return result;
}

static bool check_assets(VfsStack& vfs) {
	// Every game has this asset, so use it to sanity check.
	return vfs.open_sdl("sound/snd001.wav") != nullptr;
}

static char bin_dir_buf[1024] = {0};

const char* EscapeBinDirectory() {
#ifndef __ANDROID__
	if (!bin_dir_buf[0]) {
		if (!getcwd(bin_dir_buf, sizeof(bin_dir_buf))) {
			perror("EscapeBinDirectory: getcwd");
			strcpy(bin_dir_buf, ".");
		}
		if (vanilla::ends_with(bin_dir_buf, "/build/install") || vanilla::ends_with(bin_dir_buf, "\\build\\install")) {
			(void)chdir("../..");
		}
	}
#endif
	return bin_dir_buf;
}

static bool run_download_helper() {
#if defined(_WIN32)
	std::string executable = bin_dir_buf;
	executable.append("/launcher.exe");
	if (GetFileAttributesA(executable.c_str()) != INVALID_FILE_ATTRIBUTES) {
		std::string cmdline = executable;
		cmdline.append(" --mini-gui ");
		cmdline.append(globalMetadata->appdata_folder_name);

		STARTUPINFOA startupInfo = {};
		PROCESS_INFORMATION processInfo = {};
		startupInfo.cb = sizeof(startupInfo);
		if (CreateProcessA(executable.data(), cmdline.data(), nullptr, nullptr, false, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &processInfo)) {
			WaitForSingleObject(processInfo.hProcess, INFINITE);
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
			return true;
		}
	}
#elif !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
	struct stat sb;
	std::string executable = bin_dir_buf;
	executable.append("/launcher");

	if (stat(executable.c_str(), &sb) == 0) {
		int child_pid = fork();
		if (child_pid == 0) {
			char second[] = "--mini-gui";
			std::string third = globalMetadata->appdata_folder_name;
			char* const argv[] = { executable.data(), second, third.data(), nullptr };
			exit(execv(argv[0], argv));
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
	bool error = false;
	VfsStack result = vfs_stack_from_env(&error);
	if (!error && check_assets(result)) {
		return result;
	}

	if (run_download_helper()) {
		error = false;
		result = vfs_stack_from_env(&error);
		if (!error && check_assets(result)) {
			return result;
		}
	}

	missing_assets_message();
	exit(1);
}

static void filter_files(std::set<std::string, vanilla::CaseInsensitive>* files, const char* extension = nullptr, size_t maxlen = 0)
{
	if (extension || maxlen > 0)
	{
		size_t extlen = extension ? strlen(extension) : 0;
		erase_if(*files, [=](const std::string& value)
		{
			size_t len = value.size();
			if (maxlen > 0 && len >= maxlen)
			{
				return true;
			}
			if (extlen > 0 && (len < extlen || strcasecmp(extension, &value.c_str()[len - extlen])))
			{
				return true;
			}
			return false;
		});
	}
}

std::vector<AddonSpec> AddonSpec::SearchAddons(vanilla::WriteVfs* vfs)
{
	std::set<std::string, vanilla::CaseInsensitive> file_list;
	vfs->list_dir(".", file_list);
	filter_files(&file_list, ".zip");

	std::vector<AddonSpec> result;
	while (!file_list.empty())
	{
		result.push_back({ vfs, std::move(file_list.extract(file_list.begin()).value()) });
	}
	return result;
}

static void import_addons(VfsStack* target)
{
	std::string path = "addons/";
	path.append(globalMetadata->appdata_folder_name);
	auto addons = vanilla::open_stdio(path);
	for (const auto& spec : AddonSpec::SearchAddons(addons.get()))
	{
		if (spec.is_enabled())
		{
			SDL_Log("mounting addon: %s/%s", path.c_str(), spec.filename.c_str());
			target->push_back(vanilla::open_zip(addons->open_sdl(spec.filename.c_str())), "", vanilla::VfsSourceKind::Addon);
		}
		else
		{
			SDL_Log("addon disabled: %s/%s", path.c_str(), spec.filename.c_str());
		}
	}

	// Hacky to put this here, but too lazy to expose a proper API for this.
	if (std::string_view("sleepless") == globalMetadata->appdata_folder_name)
	{
		target->push_back(vanilla::open_zip(target->open_sdl("worlds/sleepiest_world.zip")), "", vanilla::VfsSourceKind::BaseGame);
	}
}

// Android does not play nice with static initializers.
static VfsStack vfs_stack;

// ----------------------------------------------------------------------------
// Public interface

void AppdataInit(const HamSandwichMetadata* metadata) {
	globalMetadata = metadata;
	LogInit();
	EscapeBinDirectory();
	vfs_stack = init_vfs_stack();
	import_addons(&vfs_stack);
}

bool AppdataIsInit() {
	return !vfs_stack.empty();
}

vanilla::VfsStack& AppdataVfs() {
	return vfs_stack;
}

FILE* AssetOpen(const char* filename) {
	return vfs_stack.open_stdio(filename).release();
}

owned::SDL_RWops AssetOpen_SDL_Owned(const char* filename) {
	return vfs_stack.open_sdl(filename);
}

FILE* AppdataOpen_Write(const char* filename) {
	return vfs_stack.open_write_stdio(filename).release();
}

owned::SDL_RWops AppdataOpen_Write_SDL(const char* filename) {
	return vfs_stack.open_write_sdl(filename);
}

void AppdataDelete(const char* filename) {
	vfs_stack.delete_file(filename);
}

#ifndef HAS_APPDATA_SYNC
void AppdataSync() {}
#endif

std::vector<std::string> ListDirectory(const char* directory, const char* extension, size_t maxlen) {
	std::set<std::string, vanilla::CaseInsensitive> output;
	vfs_stack.list_dir(directory, output);
	filter_files(&output, extension, maxlen);
	return { output.begin(), output.end() };
}

// Aliases.
FILE* AppdataOpen(const char* filename) {
	return AssetOpen(filename);
}

FILE* AssetOpen_Write(const char* filename) {
	return AppdataOpen_Write(filename);
}
