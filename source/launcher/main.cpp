// HamSandwich launcher, responsible for:
// - Allowing the player to select which game to play and add-ons to enable
// - Downloading assets from https://hamumu.itch.io
// Based on imgui's example_sdl_opengl2.

#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl2.h>
#include <json.h>
#include <curl/curl.h>
#include "jamulfont.h"
#include "appdata.h"
#include "sha256.h"
#include "vanilla_extract.h"
#include "metadata.h"
#include "ico.h"
#include "owned_stdio.h"
#include "owned_curl.h"

#if __has_include(<filesystem>)
#include <filesystem>
namespace filesystem = std::filesystem;
#else
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem::v1;
#endif

#ifdef __MACOSX__
#include <crt_externs.h>
#define environ *_NSGetEnviron()
#endif

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <shellapi.h>
#include <shlobj.h>
#include <string>

void OpenFolder(std::string_view folder)
{
	filesystem::path buf = filesystem::absolute(folder);
	ShellExecuteW(nullptr, L"explore", buf.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}
#else
#include <unistd.h>
#include <sys/wait.h>
#include <string>

void OpenFolder(std::string_view folder)
{
	filesystem::path buf = filesystem::absolute(folder);
	std::string url = "file://";
	url.append(buf);
	SDL_OpenURL(url.c_str());
}
#endif

// Embed Hamumu editor font for extra flavor.
extern const unsigned char embed_verdana[];
extern const size_t embed_verdana_size;

void Verdana(ImGuiIO* io)
{
	mfont_t jamfont;
	if (FontLoad(owned::SDL_RWFromConstMem(embed_verdana, embed_verdana_size).get(), &jamfont) != FONT_OK)
		return;

	ImFontConfig config;
	config.SizePixels = jamfont.height;
	ImFont* font = io->Fonts->AddFontDefault(&config);
	std::vector<int> rect_ids(jamfont.numChars);
	for (int i = 0; i < jamfont.numChars; ++i)
	{
		rect_ids[i] = io->Fonts->AddCustomRectFontGlyph(font, jamfont.firstChar + i, *jamfont.chars[i], jamfont.height + 1, *jamfont.chars[i] + jamfont.gapSize);
	}
	io->Fonts->Build();

	uint8_t* tex_pixels = nullptr;
	int tex_width, tex_height;
	io->Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);

	for (int c = 0; c < jamfont.numChars; ++c)
	{
		if (const ImFontAtlasCustomRect* rect = io->Fonts->GetCustomRectByIndex(rect_ids[c]))
		{
			auto chrWidth = *(jamfont.chars[c]);
			auto src = jamfont.chars[c] + 1;
			for (int y = 0; y < jamfont.height; y++)
			{
				ImU32* dst = (ImU32*)tex_pixels + (rect->Y + y + 1) * tex_width + (rect->X);

				for (int x = 0; x < chrWidth; x++)
				{
					if (*src)
						*dst = IM_COL32(255, 255, 255, 255);
					dst++;
					src++;
				}
			}
		}
	}
}

// Embedded metadata json and embedded icons for each game.
extern const unsigned char embed_launcher_json[];
extern const size_t embed_launcher_json_size;

struct Icon
{
	const char* name;
	const unsigned char* data;
	const size_t size;
};
const extern Icon embed_icons[];

struct LoadedIcon
{
	GLuint texture = 0;
	int width = 0, height = 0;

	LoadedIcon() {}
	explicit LoadedIcon(SDL_Surface* img)
	{
		width = img->w;
		height = img->h;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
	}
};

void print_curl_error(CURL* transfer, const char* explainer, CURLcode result)
{
	fprintf(stderr, "%s: %d: %s\n", explainer, result, curl_easy_strerror(result));
	if (result == CURLE_HTTP_RETURNED_ERROR)
	{
		long code = 0;
		if (curl_easy_getinfo(transfer, CURLINFO_RESPONSE_CODE, &code) == CURLE_OK)
		{
			fprintf(stderr, "    HTTP %ld\n", code);
		}
	}
}

// Asset and installer handling
struct Asset
{
	std::string mountpoint;
	std::string kind;
	std::string filename;
	std::string sha256sum;
	std::string link;
	long file_id;
	std::string description;
	bool required;
	bool enabled;

	owned::CURL metadata_download;
	std::string metadata_progress;
	owned::CURL content_download;
	owned::FILE content_progress;
	my_sha256_ctx content_hash;
	curl_off_t content_total_size;
	curl_off_t content_finished_size;

	explicit Asset(const jt::Json& installer)
		: mountpoint(installer.contains("mountpoint") ? installer["mountpoint"].getString() : "")
		, kind(installer["kind"].getString())
		, filename(installer["filename"].getString())
		, sha256sum(installer.contains("sha256sum") ? installer["sha256sum"].getString() : "")
		, link(installer["link"].getString())
		, file_id(installer.contains("file_id") && installer["file_id"].isLong() ? installer["file_id"].getLong() : 0)
		, description(installer["description"].getString())
		, required(!installer.contains("optional") || !installer["optional"].getBool())
		, enabled(is_enabled(filename))
	{
	}

	static bool is_enabled(std::string_view filename)
	{
		std::string enabled_file = "installers/";
		enabled_file.append(filename);
		enabled_file.append(".enabled");
		return filesystem::exists(enabled_file);
	}

	static size_t metadata_write_callback(void* data, size_t size, size_t nmemb, Asset* self)
	{
		size_t total = size * nmemb;
		self->metadata_progress.append((char*)data, total);
		return total;
	}

	static size_t content_write_callback(void* data, size_t size, size_t nmemb, Asset* self)
	{
		size_t res = fwrite(data, size, nmemb, self->content_progress);
		my_sha256_update(&self->content_hash, static_cast<const uint8_t*>(data), size * nmemb);
		return res;
	}

	static int content_progress_callback(Asset* self, curl_off_t total_down, curl_off_t finished_down, curl_off_t, curl_off_t)
	{
		self->content_total_size = total_down;
		self->content_finished_size = finished_down;
		return 0;
	}

	void start_download(CURLM* downloads)
	{
		if (metadata_download || content_download)
			return;

		if (file_id)
		{
			// An itch.io file ID is specified, so use the Itch download path.
			std::ostringstream full_url;
			full_url << link << "/file/" << file_id;

			metadata_progress = "";
			metadata_download = owned::curl_easy_init();
			curl_easy_setopt(metadata_download.get(), CURLOPT_URL, full_url.str().c_str());
			curl_easy_setopt(metadata_download.get(), CURLOPT_POST, true);
			curl_easy_setopt(metadata_download.get(), CURLOPT_POSTFIELDSIZE, 0);
			curl_easy_setopt(metadata_download.get(), CURLOPT_WRITEFUNCTION, Asset::metadata_write_callback);
			curl_easy_setopt(metadata_download.get(), CURLOPT_WRITEDATA, this);
			curl_easy_setopt(metadata_download.get(), CURLOPT_FAILONERROR, true);
			curl_multi_add_handle(downloads, metadata_download.get());
		}
		else
		{
			// Treat it as a raw URL.
			start_content_download(downloads, link);
		}
	}

	void start_content_download(CURLM* downloads, const std::string& url)
	{
		std::string full_path = "installers/";
		full_path.append(filename);
		full_path.append(".part");
		content_progress = owned::fopen(full_path.c_str(), "wb");  // TODO: error handling
		my_sha256_init(&content_hash);

		if (!content_download)
			content_download = owned::curl_easy_init();

		curl_easy_setopt(content_download.get(), CURLOPT_URL, url.c_str());
		//curl_easy_setopt(content_download,.get() CURLOPT_BUFFERSIZE, 102400L);
		//curl_easy_setopt(content_download,.get() CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
		curl_easy_setopt(content_download.get(), CURLOPT_WRITEFUNCTION, Asset::content_write_callback);
		curl_easy_setopt(content_download.get(), CURLOPT_WRITEDATA, this);
		curl_easy_setopt(content_download.get(), CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(content_download.get(), CURLOPT_XFERINFOFUNCTION, Asset::content_progress_callback);
		curl_easy_setopt(content_download.get(), CURLOPT_XFERINFODATA, this);
		curl_easy_setopt(content_download.get(), CURLOPT_FAILONERROR, true);

		curl_multi_add_handle(downloads, content_download.get());
	}

	int transfer_completed(CURLM* downloads, CURL* transfer, CURLcode result)
	{
		if (transfer == metadata_download.get())
		{
			metadata_download = nullptr;

			if (result != CURLE_OK)
			{
				print_curl_error(transfer, "Metadata download failed", result);
				return 0;
			}

			auto [status, message] = jt::Json::parse(metadata_progress);
			if (status != jt::Json::success)
			{
				fprintf(stderr, "Error parsing itch metadata: %s\n", jt::Json::StatusToString(status));
				return 0;
			}
			std::string url = message["url"].getString();

			content_download = std::move(metadata_download);
			start_content_download(downloads, url);
			return 1;
		}
		else if (transfer == content_download.get())
		{
			content_progress.reset();
			content_download.reset();

			if (result != CURLE_OK)
			{
				print_curl_error(transfer, "Content download failed", result);
				return 0;
			}

			if (sha256sum.size() == 64)
			{
				uint8_t bindigest[32];
				if (my_sha256_final(bindigest, &content_hash))
				{
					fprintf(stderr, "sha256 finalization failed\n");
					return 0;
				}

				std::string hexdigest;
				for (int i = 0; i < 32; ++i)
				{
					hexdigest.push_back("0123456789abcdef"[bindigest[i] >> 4]);
					hexdigest.push_back("0123456789abcdef"[bindigest[i] & 0xf]);
				}

				if (hexdigest != sha256sum)
				{
					fprintf(stderr, "sha256 mismatch\nexpected: %s\n     got: %s\n", sha256sum.c_str(), hexdigest.c_str());
					return 0;
				}
			}

			std::string full_path = "installers/";
			full_path.append(filename);
			std::string part_path = full_path;
			part_path.append(".part");

			filesystem::rename(part_path, full_path);  // TODO: error handling
			printf("Finished: %s\n", filename.c_str());

			curl_easy_cleanup(transfer);
			return 0;
		}

		return 0;
	}
};

#ifdef _WIN32
std::string VirtualStoreOf(std::string_view original)
{
	// SHGetFolderPathA is deprecated formally, but its replacement SHGetKnownFolderPath
	// has no narrow ('A') version, so use SHGetFolderPathA to do the conversion.
	char appdata_local[MAX_PATH];
	if (SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, appdata_local) != S_OK)
	{
		return "";
	}

	std::string result = appdata_local;
	result.append("/VirtualStore");
	result.append(original.substr(2)); // everything after C:
	return result;
}
#endif

struct RetailProfile
{
	bool enabled = true; // TODO: persistence
	std::string program_files;
	std::string virtual_store;

	explicit RetailProfile(const std::string& registry_key)
	{
		if (registry_key.empty())
		{
			return;
		}

		// HKEY_CURRENT_USER/Software/Hamumu Software/$registry_key/InstalledIn REG_SZ
#ifdef _WIN32
		std::string stuff = "Software\\Hamumu Software\\";
		stuff.append(registry_key);
		//stuff.append("\\InstalledIn");

		char installed_in[2 * MAX_PATH];
		DWORD installed_in_size = sizeof(installed_in);
		LSTATUS error = RegGetValueA(HKEY_CURRENT_USER, stuff.c_str(), "InstalledIn", RRF_RT_REG_SZ, nullptr, installed_in, &installed_in_size);
		if (error != ERROR_SUCCESS)
		{
			// NB: error 2 is file not found, means there is no retail installation.
			printf("%s/InstalledIn: error %ld\n", registry_key.c_str(), error);
			return;
		}

		program_files.assign(installed_in, installed_in_size - 1);  // -1 because size includes the null terminator
		if (!filesystem::is_directory(program_files))
		{
			printf("registry for %s had \"%s\" but it's not a directory\n", registry_key.c_str(), program_files.c_str());
			program_files.clear();
			return;
		}

		virtual_store = VirtualStoreOf(program_files);
		if (!filesystem::is_directory(virtual_store))
		{
			printf("projected VirtualStore \"%s\" is not a directory, ignoring\n", virtual_store.c_str());
			virtual_store.clear();
		}
#endif
	}
};

struct Game
{
	std::string id;
	std::string title;
	bool excluded;
	std::string appdata_folder_name;
	RetailProfile retail_profile;
	std::string addons_folder;

	std::vector<Asset> assets;
	const Icon* icon = nullptr;
	LoadedIcon loaded_icon;

	Game(std::string_view id, const jt::Json& manifest)
		: id(id)
		, title(manifest["title"].getString())
		, excluded(manifest.contains("excluded") && manifest["excluded"].isBool() && manifest["excluded"].getBool())
		, appdata_folder_name(manifest.contains("appdataName") ? manifest["appdataName"].getString() : id)
		, retail_profile(manifest.contains("registry_key") ? manifest["registry_key"].getString() : "")
		, addons_folder("addons/")
	{
		addons_folder.append(appdata_folder_name);
		if (manifest.contains("installers") && manifest["installers"].isArray())
		{
			for (const auto& installer : manifest["installers"].getArray())
			{
				assets.emplace_back(installer);
			}
		}
	}

	bool start_missing_downloads(CURLM* downloads)
	{
		bool any_missing = false;
		for (auto& asset : assets)
		{
			if (asset.required || asset.enabled)
			{
				std::string full_path = "installers/";
				full_path.append(asset.filename);
				if (filesystem::exists(full_path))
				{
					printf("OK: %s\n", asset.filename.c_str());
				}
				else
				{
					printf("Downloading: %s\n", asset.filename.c_str());
					asset.start_download(downloads);
					any_missing = true;
				}
			}
		}
		return any_missing;
	}

	bool is_ready_to_play() const
	{
		for (const auto& asset : assets)
		{
			if (asset.required || asset.enabled)
			{
				std::string full_path = "installers/";
				full_path.append(asset.filename);
				if (!filesystem::exists(full_path))
					return false;
			}
		}
		return true;
	}
};

struct Launcher
{
	std::map<std::string, const Icon*> icons_by_id;
	std::vector<Game> games;
	Game* current_game = nullptr;
	bool wants_to_play = false;
	bool wants_fullscreen = true;

	owned::CURLM downloads;

	Launcher()
	{
		downloads = owned::curl_multi_init();

		const Icon* current = embed_icons;
		while (current->name)
		{
			icons_by_id[current->name] = current;
			++current;
		}

		// Load non-excluded games in order.
		size_t idx = 0;
		auto [status, launcher_metadata] = jt::Json::parse({(const char*)embed_launcher_json, embed_launcher_json_size});
		games.reserve(launcher_metadata["project_list"].getArray().size() + launcher_metadata["project_metadata"].getObject().size());
		for (const auto& idJ : launcher_metadata["project_list"].getArray())
		{
			auto id = idJ.getString();
			auto value = launcher_metadata["project_metadata"][id];
			launcher_metadata["project_metadata"].getObject().erase(id);
			games.emplace_back(id, value);
			games.back().icon = icons_by_id[id];

			if (id == "supreme")
				idx = games.size() - 1;
		}

		// Load excluded games. The launcher knows how to download their assets,
		// but not everything else.
		for (const auto& game : launcher_metadata["project_metadata"].getObject())
		{
			games.emplace_back(game.first, game.second);
			games.back().icon = icons_by_id[game.first];
		}

		if (idx < games.size())
			current_game = &games[idx];
	}

	// Returns number of ongoing transfers.
	int update_transfers()
	{
		int running_downloads;
		CURLMcode mc = curl_multi_perform(downloads.get(), &running_downloads);
		if (mc == CURLM_OK)
		{
			int queued;
			while (CURLMsg* msg = curl_multi_info_read(downloads.get(), &queued))
			{
				if (msg->msg == CURLMSG_DONE)
				{
					CURL* transfer = msg->easy_handle;
					CURLcode result = msg->data.result;
					curl_multi_remove_handle(downloads.get(), transfer);

					for (auto& game : games)
					{
						for (auto& asset : game.assets)
						{
							running_downloads += asset.transfer_completed(downloads.get(), transfer, result);
						}
					}
				}
			}
		}
		return running_downloads;
	}
};

void CopyToAddonsFolder(std::string_view addonsFolder, std::string_view sourcePath)
{
	// .zip files only
	if (!vanilla::ends_with(sourcePath, ".zip"))
	{
		return;
	}

	std::string_view sourceLeaf = sourcePath;
	if (size_t x = sourcePath.find_last_of("/\\"); x != std::string_view::npos)
	{
		sourceLeaf = sourcePath.substr(x + 1);
	}
	filesystem::create_directories(addonsFolder);

	std::string destinationPath { addonsFolder };
	destinationPath.append("/");
	destinationPath.append(sourceLeaf);

	// no overwriting
	filesystem::copy_file(sourcePath, destinationPath, filesystem::copy_options::skip_existing);
}

// ----------------------------------------------------------------------------
// Platform-specific child process spawning

// HideWindowAndWaitForChild keeps the launcher open in the background while
// the child runs. If the child exits OK, the launcher quietly exits too. If
// the child crashed, bring the launcher back so the user can try again.

// LaunchTool simply executes the given process.

// LaunchGame also supplies HSW_* environment variables and a "window" argument.

#if defined(_WIN32)
// Launch for Windows
bool HideWindowAndWaitForChild(SDL_Window* window, const PROCESS_INFORMATION& processInfo)
{
	SDL_HideWindow(window);  // Hide window now that we know the child process was created.
	WaitForSingleObject(processInfo.hProcess, INFINITE);
	DWORD exitCode;
	if (!GetExitCodeProcess(processInfo.hProcess, &exitCode))
		exitCode = -1;
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
	if (exitCode == 0)
		return true;  // Success, so clean up and exit in the background.
	fprintf(stderr, "child exited with code: %lu\n", exitCode);

	// Child process failed, so bring the launcher window back.
	SDL_ShowWindow(window);  // Undo Hide
	return false;
}

bool LaunchTool(std::string_view bin_dir, std::string_view exename, SDL_Window* window)
{
	std::string cmdline { bin_dir };
	cmdline.append("/");
	cmdline.append(exename);
	cmdline.append(".exe");

	STARTUPINFOA startupInfo = {};
	PROCESS_INFORMATION processInfo = {};
	startupInfo.cb = sizeof(startupInfo);
	if (CreateProcessA(cmdline.c_str(), cmdline.data(), nullptr, nullptr, false, 0, nullptr, nullptr, &startupInfo, &processInfo))
	{
		return HideWindowAndWaitForChild(window, processInfo);
	}
	else
	{
		fprintf(stderr, "CreateProcess failed: %lu\n", GetLastError());
		return false;
	}
}

bool LaunchGame(std::string_view bin_dir, const Game* game, bool fullscreen, SDL_Window* window)
{
	std::string cmdline { bin_dir };
	cmdline.append("/");
	cmdline.append(game->id);
	cmdline.append(".exe");
	std::string executable = cmdline;
	if (!fullscreen)
		cmdline.append(" window");

	std::ostringstream environment;
	const char* originalEnv = GetEnvironmentStringsA();
	while (*originalEnv)
	{
		environment << originalEnv << '\0';
		originalEnv += strlen(originalEnv) + 1;
	}

	int i = 0;

	// Lowest priority: installers in order.
	bool doneRetailProfile = false;
	auto doRetailProfile = [&]()
	{
		doneRetailProfile = true;
		if (game->retail_profile.enabled)
		{
			// NB: VirtualStore comes after and thus overrides Program Files
			if (!game->retail_profile.program_files.empty())
			{
				environment << "HSW_ASSETS_" << i++ << "=@stdio@" << game->retail_profile.program_files << '\0';
			}
			if (!game->retail_profile.virtual_store.empty())
			{
				environment << "HSW_ASSETS_" << i++ << "=@stdio@" << game->retail_profile.virtual_store << '\0';
			}
		}
	};
	for (const auto& asset : game->assets)
	{
		// In the middle: retail profile.
		if (!asset.required && !doneRetailProfile)
		{
			doRetailProfile();
		}

		if (asset.required || asset.enabled)
		{
			environment << "HSW_ASSETS_" << i++ << "=" << asset.mountpoint << "@" << asset.kind << "@installers/" << asset.filename << '\0';
		}
	}
	if (!doneRetailProfile)
	{
		doRetailProfile();
	}

	// High priority: custom assets provided directly.
	environment << "HSW_ASSETS_" << i++ << "=@stdio@assets/" << game->appdata_folder_name << '\0';

	// Lastly: the game itself mounts addons/$game/*.zip
	// And appdata/$game/ overrides them all, of course.
	environment << "HSW_APPDATA=appdata/" << game->appdata_folder_name << '\0';

	// Finish if off and launch.
	environment << '\0';
	STARTUPINFOA startupInfo = {};
	PROCESS_INFORMATION processInfo = {};
	startupInfo.cb = sizeof(startupInfo);
	if (CreateProcessA(executable.c_str(), cmdline.data(), nullptr, nullptr, false, 0, environment.str().data(), nullptr, &startupInfo, &processInfo))
	{
		return HideWindowAndWaitForChild(window, processInfo);
	}
	else
	{
		fprintf(stderr, "CreateProcess failed: %lu\n", GetLastError());
		return false;
	}
}
// End Launch for Windows
#elif !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
// Launch for Linux and Mac
bool HideWindowAndWaitForChild(SDL_Window* window, int child_pid)
{
	SDL_HideWindow(window);  // Hide window. Unfortunately happens even if execvp fails.
	SDL_MinimizeWindow(window);  // Minimize on MacOS for now, though it's a little wonky.

	int wstatus;
	// NB: SDL catches SIGINT and SIGTERM and turns them into a QUIT event and
	// clean exit, so those won't bubble up as errors here.
	if (waitpid(child_pid, &wstatus, 0) >= 0 && WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0)
		return true;  // Success, so clean up and exit in the background.
	if (WIFEXITED(wstatus))
		fprintf(stderr, "child exited with code: %d\n", WEXITSTATUS(wstatus));
	else if (WIFSIGNALED(wstatus))
		fprintf(stderr, "child exited with signal: %d\n", WTERMSIG(wstatus));
	else
		fprintf(stderr, "child exited in unknown way: %x\n", wstatus);

	// Child process failed, so bring the launcher window back.
	SDL_RestoreWindow(window);  // Undo Minimize.
	SDL_ShowWindow(window);  // Undo Hide.
	return false;
}

bool LaunchTool(std::string_view bin_dir, std::string_view exename, SDL_Window* window)
{
	int child_pid = fork();
	if (child_pid == 0)
	{
		std::string program { bin_dir };
		program.append("/");
		program.append(exename);
		char* argv[2] = { program.data(), nullptr };

		execv(argv[0], argv);
		perror("execv");
		exit(1);
	}
	else if (child_pid > 0)
	{
		return HideWindowAndWaitForChild(window, child_pid);
	}
	else
	{
		perror("fork");
		return false;
	}
}

bool LaunchGame(std::string_view bin_dir, const Game* game, bool fullscreen, SDL_Window* window)
{
	int child_pid = fork();
	if (child_pid == 0)
	{
		// We're in the child, so either exec or exit with an error code.
		std::string program { bin_dir };
		program.append("/");
		program.append(game->id);
		char window[] = "window";
		char* argv[3] = { program.data(), nullptr, nullptr };
		if (!fullscreen)
			argv[1] = window;

		std::vector<std::string> envs;
		std::vector<char*> raw_envs;

		char** originalEnv = environ;
		while (originalEnv && *originalEnv)
		{
			raw_envs.push_back(*originalEnv);
			++originalEnv;
		}

		int i = 0;
		std::ostringstream environment;

		// Lowest priority: installers in order.
		for (const auto& asset : game->assets)
		{
			if (asset.required || asset.enabled)
			{
				environment.str("");
				environment << "HSW_ASSETS_" << i++ << "=" << asset.mountpoint << "@" << asset.kind << "@installers/" << asset.filename;
				envs.push_back(environment.str());
				raw_envs.push_back(envs.back().data());
			}
		}

		// High priority: custom assets provided directly.
		environment.str("");
		environment << "HSW_ASSETS_" << i++ << "=@stdio@assets/" << game->appdata_folder_name;
		envs.push_back(environment.str());
		raw_envs.push_back(envs.back().data());

		// Lastly: the game itself mounts addons/$game/*.zip
		// And appdata/$game/ overrides them all, of course.
		environment.str("");
		environment << "HSW_APPDATA=appdata/" << game->appdata_folder_name;
		envs.push_back(environment.str());
		raw_envs.push_back(envs.back().data());

		// Finish it off and launch.
		raw_envs.push_back(nullptr);
		execve(argv[0], argv, raw_envs.data());
		perror("execve");
		exit(1);
	}
	else if (child_pid > 0)
	{
		return HideWindowAndWaitForChild(window, child_pid);
	}
	else
	{
		perror("fork");
		return false;
	}
}
// End Launch for Linux and Mac
#endif

// ----------------------------------------------------------------------------
// Main code
enum class Action
{
	Gui,
	MiniCli,
	MiniGui,
};

const char GAME_SELECTION_FILENAME[] = "appdata/game_selection.txt";

int main(int argc, char** argv)
{
	const char* bin_dir = EscapeBinDirectory();
	filesystem::create_directories("installers");

	// Set up curl
	curl_global_init(CURL_GLOBAL_DEFAULT);
	Action action = Action::Gui;
	Launcher launcher;

	const char* wanted_game = nullptr;
	for (int i = 1; i < argc; ++i)
	{
		std::string_view arg = argv[i];
		if (arg == "window")
		{
			launcher.wants_fullscreen = false;
		}
		else if (arg == "--mini-cli")
		{
			action = Action::MiniCli;
		}
		else if (arg == "--mini-gui")
		{
			action = Action::MiniGui;
		}
		else if (arg == "--all")
		{
			for (auto& game : launcher.games)
			{
				for (auto& asset : game.assets)
				{
					asset.enabled = true;
				}
			}
		}
		else
		{
			wanted_game = argv[i];
			launcher.current_game = nullptr;
			for (auto& game : launcher.games)
			{
				if (arg == game.id)
				{
					launcher.current_game = &game;
				}
			}
		}
	}

	if (action == Action::MiniCli || action == Action::MiniGui)
	{
		if (!launcher.current_game)
		{
			fprintf(stderr, "launcher: unknown game \"%s\", try one of:\n", wanted_game);
			for (auto& game : launcher.games)
			{
				fprintf(stderr, "    %s\n", game.id.c_str());
			}
			return 1;
		}

		if (!launcher.current_game->start_missing_downloads(launcher.downloads.get()))
		{
			return 0;
		}

		if (action == Action::MiniCli)
		{
			while (launcher.update_transfers());
			return 0;
		}
	}
	else
	{
		// Try to preselect the last selected game.
		owned::FILE game_selection_file = owned::fopen(GAME_SELECTION_FILENAME, "rb");
		if (game_selection_file)
		{
			char game_selection[256];
			fgets(game_selection, sizeof game_selection, game_selection_file.get());
			// If the file exists but its content is invalid, select nothing.
			// If the file didn't exist select the default (Supreme) as usual.
			launcher.current_game = nullptr;
			for (auto& game : launcher.games)
			{
				if (game_selection == game.id)
				{
					launcher.current_game = &game;
				}
			}
		}
	}

	const char* const fullscreen_file = "installers/.fullscreen";
	launcher.wants_fullscreen = filesystem::exists(fullscreen_file);

	// Setup SDL
	// (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
	// depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// Setup window
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* window = SDL_CreateWindow("HamSandwich Launcher", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 480, window_flags);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL2_Init();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
	Verdana(&io);

	// Load icon textures
	// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#example-for-opengl-users
	for (auto& game : launcher.games)
	{
		if (game.icon)
		{
			auto img = ReadIcoFile(owned::SDL_RWFromConstMem(game.icon->data, game.icon->size), 32);
			if (img)
			{
				// For best results, bake a 32x32 PNG frame into the .ico file.
				// OpenGL will handle any necessary up- or down-scaling.
				game.loaded_icon = LoadedIcon(img.get());
			}
			else
			{
				printf("Failed to load %s icon: %s\n", game.id.c_str(), IMG_GetError());
			}
		}
	}

	LoadedIcon jspedit_icon;
	{
		const Icon* icon = launcher.icons_by_id["jspedit"];
		if (icon)
		{
			auto img = ReadIcoFile(owned::SDL_RWFromConstMem(icon->data, icon->size), 32);
			if (img)
			{
				jspedit_icon = LoadedIcon(img.get());
			}
		}
	}

	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);

	// Main loop
	bool done = false;
	while (!done)
	{
		int running_downloads = launcher.update_transfers();
		SDL_GL_SetSwapInterval(running_downloads ? 0 : 1); // Enable vsync iff nothing is downloading.

		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
			if (event.type == SDL_DROPFILE)
			{
				CopyToAddonsFolder(launcher.current_game->addons_folder, event.drop.file);
				SDL_free(event.drop.file);
			}
		}

		int windowWidth, windowHeight;
		float leftPaneWidth = 300;
		SDL_GetWindowSize(window, &windowWidth, &windowHeight);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ leftPaneWidth, (float)windowHeight });
		ImGui::BeginDisabled(action == Action::MiniGui);
		if (ImGui::Begin("Games", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			const int vertical_padding = 1;

			for (auto& game : launcher.games)
			{
				if (game.excluded)
					continue;

				ImGui::PushID(game.id.c_str());
				if (ImGui::Selectable("", launcher.current_game == &game, ImGuiSelectableFlags_AllowDoubleClick, { 0, 32 + 2 * vertical_padding }))
				{
					launcher.wants_to_play = false;
					launcher.current_game = &game;
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						launcher.wants_to_play = true;
						game.start_missing_downloads(launcher.downloads.get());
					}

					// On click, save the selection.
					owned::FILE game_selection_file = owned::fopen(GAME_SELECTION_FILENAME, "wb");
					if (game_selection_file)
					{
						fputs(launcher.current_game->id.c_str(), game_selection_file.get());
					}
				}
				if (game.loaded_icon.texture)
				{
					ImGui::SameLine(8);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + vertical_padding);
					ImGui::Image((void*)(intptr_t)game.loaded_icon.texture, { 32, 32 });
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - vertical_padding);
				}
				ImGui::SameLine(48);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (8 + vertical_padding));
				ImGui::Text("%s", game.title.c_str());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (8 + vertical_padding));
				ImGui::PopID();
			}

			ImGui::Separator();
			ImGui::Text("Tools");

			ImGui::PushID("jspedit");
			const int selectable_padding = 2;
			if (ImGui::Button("", { ImGui::GetContentRegionAvail().x, 32 + 2 * (vertical_padding + selectable_padding) }))
			{
				if (LaunchTool(bin_dir, "jspedit", window))
				{
					// Success, so clean up and exit in the background.
					break;
				}
			}
			if (jspedit_icon.texture)
			{
				ImGui::SameLine(8 + 2);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (vertical_padding + selectable_padding));
				ImGui::Image((void*)(intptr_t)jspedit_icon.texture, { 32, 32 });
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (vertical_padding + selectable_padding));
			}
			ImGui::SameLine(48);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (8 + vertical_padding - selectable_padding));
			ImGui::Text("JspEdit 3");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (8 + vertical_padding - selectable_padding));
			ImGui::PopID();

#ifdef GIT_VERSION
			ImGui::Separator();
			ImGui::Text("Version: %.*s", 10, GIT_VERSION);
#endif
		}
		ImGui::End();

		if (launcher.current_game)
		{
			ImGui::SetNextWindowPos({ leftPaneWidth, 0 });
			ImGui::SetNextWindowSize({ (float)windowWidth - leftPaneWidth, (float)windowHeight });

			std::string window_title = launcher.current_game->title;
			window_title.append("###current_game");
			ImGui::Begin(window_title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing);

			float paneWidth = ImGui::GetContentRegionAvail().x + 16;

			// ----------------------------------------------------------------
			// Play button
			const char* message = launcher.wants_to_play ? "Downloading...###game_play"
				: launcher.current_game->is_ready_to_play() ? "Play###game_play"
				: "Download & Play###game_play";
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0x23, 0x6c, 0x2a, 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0x0a, 0xaa, 0x0f, 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0x35, 0xc8, 0x43, 255));
			if (ImGui::Button(message, { 196, 0 }))
			{
				launcher.wants_to_play = !launcher.wants_to_play;
				if (launcher.wants_to_play)
					launcher.current_game->start_missing_downloads(launcher.downloads.get());
			}
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			if (ImGui::Checkbox("Fullscreen", &launcher.wants_fullscreen))
			{
				if (launcher.wants_fullscreen)
				{
					std::ofstream dummy { fullscreen_file };
				}
				else
				{
					filesystem::remove(fullscreen_file);
				}
			}
			ImGui::SameLine(paneWidth - 160);
			if (ImGui::Button("Open save folder", { 160, 0 }))
			{
				std::string to_open = "appdata/";
				to_open.append(launcher.current_game->appdata_folder_name);
				filesystem::create_directories(to_open);
				OpenFolder(to_open);
			}

			// ----------------------------------------------------------------
			// Content selection
			ImGui::Dummy({ 0, 4 });
			ImGui::Separator();
			ImGui::Text("Content selection:");

			bool doneRetailProfile = false;
			auto doRetailProfile = [&]()
			{
				doneRetailProfile = true;
				if (!launcher.current_game->retail_profile.program_files.empty())
				{
					ImGui::PushID("retail_profile");
					ImGui::Checkbox("Save data from detected retail installation (read-only)", &launcher.current_game->retail_profile.enabled);
					ImGui::SameLine(paneWidth - 128);
					if (ImGui::Button("Open", { 128, 0 }))
					{
						if (!launcher.current_game->retail_profile.virtual_store.empty())
						{
							OpenFolder(launcher.current_game->retail_profile.virtual_store);
						}
						else
						{
							OpenFolder(launcher.current_game->retail_profile.program_files);
						}
					}
					ImGui::PopID();
				}
			};
			for (auto& asset : launcher.current_game->assets)
			{
				// For now, stick the retail profile before the first non-required asset.
				// A better design would be including this in the asset stack as its own entry.
				if (!asset.required && !doneRetailProfile)
				{
					doRetailProfile();
				}

				ImGui::PushID(asset.filename.c_str());
				if (asset.required || launcher.wants_to_play)
				{
					bool dummy = asset.required || asset.enabled;
					ImGui::BeginDisabled();
					ImGui::Checkbox(asset.description.c_str(), &dummy);
					ImGui::EndDisabled();
				}
				else
				{
					if (ImGui::Checkbox(asset.description.c_str(), &asset.enabled))
					{
						std::string enabled_file = "installers/";
						enabled_file.append(asset.filename);
						enabled_file.append(".enabled");
						if (asset.enabled)
						{
							std::ofstream dummy { enabled_file };
						}
						else
						{
							filesystem::remove(enabled_file);
						}
					}
				}

				ImGui::SameLine(paneWidth - 128);

				std::string fullPath = "installers/";
				fullPath.append(asset.filename);
				if (filesystem::exists(fullPath))
				{
					if (asset.required)
					{
						ImGui::BeginDisabled();
						ImGui::Button("Ready###asset_download", { 128, 0 });
						ImGui::EndDisabled();
					}
					else
					{
						if (ImGui::Button("Delete###asset_download", { 128, 0 }))
						{
							asset.enabled = false;
							filesystem::remove(fullPath);
							fullPath.append(".enabled");
							filesystem::remove(fullPath);
						}
					}
				}
				else
				{
					if (asset.metadata_download || asset.content_download)
					{
						std::ostringstream message;
						if (asset.content_total_size > 0)
						{
							int percent = asset.content_finished_size * 100 / asset.content_total_size;
							message << percent << "%" << "###asset_download";
						}
						else
						{
							message << "Starting...###asset_download";
						}

						if (ImGui::Button(message.str().c_str(), { 128, 0 }))
						{
							if (asset.metadata_download)
							{
								curl_multi_remove_handle(launcher.downloads.get(), asset.metadata_download.get());
								asset.metadata_download.reset();
							}
							if (asset.content_download)
							{
								curl_multi_remove_handle(launcher.downloads.get(), asset.content_download.get());
								asset.content_download.reset();
							}
						}
					}
					else
					{
						if (ImGui::Button("Download###asset_download", { 128, 0 }))
							asset.start_download(launcher.downloads.get());
					}
				}
				ImGui::PopID();
			}
			if (!doneRetailProfile)
			{
				doRetailProfile();
			}

			// ----------------------------------------------------------------
			// Addons
			ImGui::Dummy({ 0, 4 });
			ImGui::Separator();
			ImGui::Text("Local addons:");
			ImGui::SameLine(paneWidth - 160);
			if (ImGui::Button("Open addons folder", { 160, 0 }))
			{
				filesystem::create_directories(launcher.current_game->addons_folder);
				OpenFolder(launcher.current_game->addons_folder);
			}

			auto addons_vfs = vanilla::open_stdio(launcher.current_game->addons_folder);
			bool any_addons = false;
			for (auto& spec : AddonSpec::SearchAddons(addons_vfs.get()))
			{
				any_addons = true;
				bool enabled = spec.is_enabled();
				if (ImGui::Checkbox(spec.filename.c_str(), &enabled))
					spec.set_enabled(enabled);
			}

			if (!any_addons)
			{
				ImGui::Text("Add .zip files to the addons folder and they will be loaded automatically.");
			}

			// ----------------------------------------------------------------
			ImGui::End();
		}
		ImGui::EndDisabled(); // pairs with Action::MiniGui

		// Rendering
		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);

		if (launcher.current_game && launcher.current_game->is_ready_to_play())
		{
			if (action == Action::MiniGui)
			{
				break;
			}
			else if (launcher.wants_to_play)
			{
				launcher.wants_to_play = false;
				if (LaunchGame(bin_dir, launcher.current_game, launcher.wants_fullscreen, window))
				{
					// Success, so clean up and exit in the background.
					break;
				}
			}
		}
	}

	// Cleanup
	launcher.downloads.reset();
	curl_global_cleanup();

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
