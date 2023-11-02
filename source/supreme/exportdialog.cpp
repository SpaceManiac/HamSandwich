#define __STDC_FORMAT_MACROS
#include "exportdialog.h"
#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <inttypes.h>
#include <zip.h>
#include "world.h"
#include "display.h"
#include "appdata.h"
#include "vanilla_extract.h"
#include "log.h"
#include "steam.h"
#include "guy.h"
#include "editor.h"
#include "winpch.h"

#ifdef HAS_STEAM_API
#ifdef _WIN32
#include <windows.h>
#endif
#include <steam/steam_api.h>
#endif

namespace
{
	enum class FileKind
	{
		Root,
		DependencyMissing,
		DependencyAppdata,
		DependencyOtherAddon,
		DependencyBaseGame,
	};

	struct FileEntry
	{
		FileKind kind;
		std::string filename;

		bool operator<(const FileEntry& other) const
		{
			return kind < other.kind || (kind == other.kind && filename < other.filename);
		}
	};

	std::set<std::string> seen;
	std::vector<FileEntry> files;
	size_t offset;
	constexpr size_t FILES_PER_PAGE = 29;

	std::string title, zipName;
	std::vector<std::string> warnings;

	std::string saveZipResult;

	constexpr int midX = 390;

	bool saveTxtOnly = false;
	std::string saveTxtResult;

	enum class ButtonId
	{
		None,
		Close,
		SaveReqFilesTxt,
		SaveZip,
		PublishToSteamWorkshop,
		ViewWorkshopItem,
	};
	ButtonId curButton;

#ifdef HAS_STEAM_API
	class SteamWorkshopUpload final
	{
		CCallResult<SteamWorkshopUpload, CreateItemResult_t> create_result;
		CCallResult<SteamWorkshopUpload, SubmitItemUpdateResult_t> submit_update_result;

		void SaveWorkshopDataFile()
		{
			owned::FILE f { AssetOpen_Write(workshopDataFilename.c_str()) };
			fprintf(f.get(), "workshop_item_id=%" PRIu64 "\n", workshopItemId);
		}

		void CreateCallback(CreateItemResult_t* result, bool)
		{
			if (result->m_bUserNeedsToAcceptWorkshopLegalAgreement)
			{
				LogError("User needs to accept Workshop legal agreement");
				progress = Progress::Failed;
				progressMessage = "EULA";
				return;
			}

			if (result->m_eResult != k_EResultOK)
			{
				LogError("SteamUGC::CreateItem: %d\n", result->m_eResult);
				progress = Progress::Failed;
				progressMessage = "Failed to create item (code " + std::to_string(result->m_eResult) + ")";
				return;
			}

			workshopItemId = result->m_nPublishedFileId;

			SaveWorkshopDataFile();

			SubmitUpdate();
		}

		void SubmitUpdate()
		{
			UGCUpdateHandle_t handle = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), workshopItemId);

			if (!SteamUGC()->SetItemTitle(handle, title.c_str()))
			{
				LogError("SteamUGC::SetItemTitle failed for title \"%s\"", title.c_str());
				progress = Progress::Failed;
				progressMessage = "Invalid title";
				return;
			}

			if (!SteamUGC()->SetItemContent(handle, folder.c_str()))
			{
				LogError("SteamUGC::SetItemContent failed for folder \"%s\"", folder.c_str());
				progress = Progress::Failed;
				progressMessage = "Invalid content folder";
				return;
			}

			const char* tags[] = {"World"};
			SteamParamStringArray_t tagArray = { tags, SDL_arraysize(tags) };
			if (!SteamUGC()->SetItemTags(handle, &tagArray))
			{
				progress = Progress::Failed;
				progressMessage = "Invalid tags";
				return;
			}

			if (!SteamUGC()->SetItemPreview(handle, preview.c_str()))
			{
				progress = Progress::Failed;
				progressMessage = "Invalid preview file";
				return;
			}

			if (!SteamUGC()->SetItemVisibility(handle, k_ERemoteStoragePublishedFileVisibilityPublic))
			{
				progress = Progress::Failed;
				progressMessage = "Invalid visibility";
				return;
			}

			submit_update_result.Set(SteamUGC()->SubmitItemUpdate(handle, nullptr), this, &SteamWorkshopUpload::SubmitUpdateCallback);
			progress = Progress::Working;
			progressMessage = "Uploading content, please wait...";
		}

		void SubmitUpdateCallback(SubmitItemUpdateResult_t* result, bool)
		{
			if (result->m_bUserNeedsToAcceptWorkshopLegalAgreement)
			{
				LogError("User needs to accept Workshop legal agreement");
				progress = Progress::Failed;
				progressMessage = "EULA";
				return;
			}

			if (result->m_eResult != k_EResultOK)
			{
				LogError("SteamUGC::SubmitItemUpdate: %d\n", result->m_eResult);
				LogError("  title: %s", title.c_str());
				LogError("  folder: %s", folder.c_str());
				LogError("  preview: %s", preview.c_str());
				progress = Progress::Failed;
				progressMessage = "Failed to submit update (code " + std::to_string(result->m_eResult) + ")";
				return;
			}

			progress = Progress::Succeeded;
			progressMessage = "Workshop upload OK!";
		}

	public:
		uint64_t workshopItemId = 0;

		enum class Progress
		{
			Idle,
			Working,
			Succeeded,
			Failed,
		};
		Progress progress = Progress::Idle;
		std::string progressMessage;

		std::string folder, preview, workshopDataFilename;

		void LoadWorkshopDataFile(std::string source)
		{
			workshopDataFilename = std::move(source);
			if (owned::FILE f { AppdataOpen(workshopDataFilename.c_str()) })
			{
				char buf[128];
				while (fgets(buf, SDL_arraysize(buf), f.get()))
				{
					char* eq = strchr(buf, '=');
					if (eq)
					{
						*eq = '\0';
						if (!strcmp(buf, "workshop_item_id"))
						{
							workshopItemId = atoll(eq + 1);
						}
					}
				}
			}
		}

		void Start()
		{
			if (folder.empty())
			{
				progress = Progress::Failed;
				progressMessage = "Error preparing files to upload";
			}
			else if (preview.empty())
			{
				progress = Progress::Failed;
				progressMessage = "Error preparing preview";
			}
			else if (!workshopItemId)
			{
				progress = Progress::Working;
				progressMessage = "Registering new item...";
				create_result.Set(SteamUGC()->CreateItem(SteamUtils()->GetAppID(), k_EWorkshopFileTypeCommunity), this, &SteamWorkshopUpload::CreateCallback);
			}
			else
			{
				SubmitUpdate();
			}
		}
	} steamWorkshopUpload;
#endif  // HAS_STEAM_API
}

static bool IncludeKind(FileKind kind)
{
	return kind == FileKind::Root || kind == FileKind::DependencyAppdata || kind == FileKind::DependencyOtherAddon;
}

static void AddDependency(std::string_view part1, std::string_view part2)
{
	if (part2.empty())
		return;

	std::string fname;
	fname.reserve(part1.size() + part2.size());
	fname.append(part1);
	fname.append(part2);

	if (!seen.insert(fname).second)
		return;

	FileKind kind = FileKind::DependencyMissing;
	vanilla::VfsMeta meta;
	if (AppdataVfs().query_bottom(fname.c_str(), &meta))
	{
		switch (meta.kind)
		{
			case vanilla::VfsSourceKind::Addon:
				kind = FileKind::DependencyOtherAddon;
				break;
			case vanilla::VfsSourceKind::Appdata:
			case vanilla::VfsSourceKind::Unspecified:
				kind = FileKind::DependencyAppdata;
				break;
			case vanilla::VfsSourceKind::BaseGame:
				kind = FileKind::DependencyBaseGame;
				break;
		}
	}

	files.push_back({ kind, std::move(fname) });
}

static bool BadCharacter(char ch)
{
	// Most restrictive of Linux, Windows, and Mac.
	return !(ch >= 32 && ch < 127 && ch != '/' && ch != '<' && ch != '>' && ch != ':' && ch != '"' && ch != '\\' && ch != '|' && ch != '?' && ch != '*');
}

static void SaveReqFilesTxt()
{
	FILE *f = AppdataOpen_Write("req_files.txt");
	fprintf(f, "# World: %s\n", title.c_str());
	for (const auto& file : files)
	{
		switch (file.kind)
		{
			case FileKind::Root:
				fprintf(f, "# Root: %s\n", file.filename.c_str());
				break;
			case FileKind::DependencyMissing:
				fprintf(f, "%s    # MISSING!\n", file.filename.c_str());
				break;
			case FileKind::DependencyAppdata:
			case FileKind::DependencyOtherAddon:
				fprintf(f, "%s\n", file.filename.c_str());
				break;
			case FileKind::DependencyBaseGame:
				fprintf(f, "# Base game: %s\n", file.filename.c_str());
				break;
			default:
				break;
		}
	}
	fclose(f);
	AppdataSync();
	saveTxtResult = "Saved OK!";
}

static void SaveZip()
{
	constexpr size_t BUFSIZE = 16 * 1024;
	char buf[BUFSIZE];

	std::string pathname = "exports/";
	pathname.append(zipName);
	pathname.append(".zip");
	vanilla::mkdir_parents(pathname);

	zipFile zf = zipOpen(pathname.c_str(), false);
	if (!zf)
	{
		saveZipResult = "Error opening zip file";
		return;
	}

	int err = ZIP_OK;
	for (const auto& file : files)
	{
		zip_fileinfo zi;
		zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour = 0;
		zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
		zi.dosDate = 0;
		zi.internal_fa = 0;
		zi.external_fa = 0;

		if (IncludeKind(file.kind))
		{
			err = zipOpenNewFileInZip(
				zf,
				file.filename.c_str(),
				&zi,
				/*extrafield_local*/ nullptr, 0,
				/*extrafield_global*/ nullptr, 0,
				/*comment*/ nullptr,
				/*method*/ Z_DEFLATED,
				/*level*/ Z_DEFAULT_COMPRESSION
			);
			if (err != ZIP_OK)
				break;

			owned::SDL_RWops rw = AssetOpen_SDL_Owned(file.filename.c_str());
			if (!rw)
			{
				err = 10;
				break;
			}
			while (true)
			{
				size_t read = SDL_RWread(rw, buf, 1, BUFSIZE);
				if (read == 0)
					break;
				err = zipWriteInFileInZip(zf, buf, read);
			}
			if (err != ZIP_OK)
				break;

			err = zipCloseFileInZip(zf);
			if (err != ZIP_OK)
				break;
		}
	}

	if (err == ZIP_OK)
	{
		err = zipClose(zf, nullptr);
	}
	else
	{
		zipClose(zf, nullptr);
	}

	if (err == ZIP_OK)
	{
		saveZipResult = "Saved OK!";
	}
	else
	{
		saveZipResult = "Zip error ";
		saveZipResult.append(std::to_string(err));
	}
}

#ifdef HAS_STEAM_API
static std::string TempName()
{
	std::string base;
#ifdef _WIN32
	char buf[MAX_PATH];
	size_t len = GetTempPathA(SDL_arraysize(buf), buf);
	base.assign(buf, len);
#else
	base = "/tmp";
#endif

	base.append("/supreme_");
	base += std::to_string(time(nullptr));
	return base;
}

static std::string PrepareWorkshopFolder()
{
	constexpr size_t BUFSIZE = 16 * 1024;
	char buf[BUFSIZE];

	std::string destFolder = TempName();

	for (const auto& file : files)
	{
		if (IncludeKind(file.kind))
		{
			std::string destFilename = destFolder;
			destFilename.append("/");
			destFilename.append(file.filename);
			vanilla::mkdir_parents(destFilename);
			owned::FILE f = owned::fopen(destFilename.c_str(), "wb");
			if (!f)
			{
				LogError("Workshop: Failed to open for writing: %s", destFilename.c_str());
				return "";
			}

			owned::SDL_RWops rw = AssetOpen_SDL_Owned(file.filename.c_str());
			if (!rw)
			{
				LogError("Workshop: Failed to open for reading: %s", file.filename.c_str());
				return "";
			}

			while (true)
			{
				size_t read = SDL_RWread(rw, buf, 1, BUFSIZE);
				if (read == 0)
					break;
				if (fwrite(buf, 1, read, f) < read)
				{
					LogError("Workshop: Error while writing to: %s", destFilename.c_str());
					return "";
				}
			}
		}
	}

	return destFolder;
}

static std::string PrepareWorkshopPreview()
{
	std::string pngFilename = TempName();
	pngFilename.append(".png");
	SDL_Log("Steam Workshop preview file: %s", pngFilename.c_str());

	MGLDraw* mgl = GetDisplayMGL();
	mgl->ClearScreen();

	EditorSelectMap(0);
	world_t* world = EditorGetWorld();
	Map* map = EditorGetMap();

	PutCamera(goodguy->x, goodguy->y);
	UpdateCamera(goodguy->x >> FIXSHIFT, goodguy->y >> FIXSHIFT, 0, 0, map);

	editing = 0;
	RenderGuys(true);
	RenderItAll(world, map, MAP_SHOWWALLS | MAP_SHOWLIGHTS | MAP_SHOWBADGUYS | MAP_SHOWPICKUPS | MAP_SHOWOTHERITEMS);
	editing=1;

	if (!mgl->SavePNG(pngFilename.c_str()))
	{
		LogError("Failed to save preview to %s\n", pngFilename.c_str());
		return "";
	}
	mgl->ClearScreen();

	return pngFilename;
}

static void PublishToSteamWorkshop()
{
	steamWorkshopUpload.folder = PrepareWorkshopFolder();
	steamWorkshopUpload.preview = PrepareWorkshopPreview();
	steamWorkshopUpload.Start();
}
#endif  // HAS_STEAM_API

void InitExportDialog(const world_t* world, const char* filename)
{
	seen.clear();
	files.clear();
	warnings.clear();
	saveZipResult.clear();
	saveTxtResult.clear();
	offset = 0;
	saveTxtOnly = false;

#ifdef HAS_STEAM_API
	steamWorkshopUpload.workshopItemId = 0;
	steamWorkshopUpload.progress = SteamWorkshopUpload::Progress::Idle;
	steamWorkshopUpload.progressMessage = "";

	if (filename)
	{
		std::string workshopDataFilename = filename;
		workshopDataFilename.append(".workshop");
		steamWorkshopUpload.LoadWorkshopDataFile(std::move(workshopDataFilename));
	}
#endif  // HAS_STEAM_API

	if (filename)
	{
		seen.insert(filename);
		files.push_back({ FileKind::Root, filename });
	}

	for (int i = 0; i < world->numMaps; ++i)
	{
		const Map* map = world->map[i];
		AddDependency("music/", map->song);

		for (int j = 0; j < MAX_SPECIAL; ++j)
		{
			const special_t& spcl = map->special[j];
			if (spcl.x != 255)
			{
				for (int k = 0; k < NUM_EFFECTS; ++k)
				{
					const effect_t& effect = spcl.effect[k];
					byte type = effect.type;
					if (type == EFF_PICTURE || type == EFF_MONSGRAPHICS || type == EFF_ITEMGRAPHICS)
					{
						std::string_view text = effect.text;
						if (size_t comma = text.find(','); comma != std::string_view::npos)
						{
							text = text.substr(0, comma);
						}
						AddDependency("user/", text);
					}
					else if (type == EFF_SONG)
					{
						AddDependency("music/", effect.text);
					}
				}
			}
		}
	}

	std::sort(files.begin(), files.end());

	for (const auto& file : files)
	{
		if (file.kind == FileKind::DependencyMissing)
		{
			warnings.push_back("Some files are missing");
			break;
		}
	}

	title = world->map[0]->name;
	if (std::string_view("New World") == title)
	{
		warnings.push_back("World name not set");
	}
	if (!strcmp(world->author, "Nobody") || !strcmp(world->author, "Unknown Author") || !strcmp(world->author, "Author Unknown!"))
	{
		warnings.push_back("Author name not set");
	}

	// Filter out characters that are not permissible in filenames.
	zipName = title;
	zipName.erase(std::remove_if(zipName.begin(), zipName.end(), BadCharacter), zipName.end());

	if (!filename)
	{
		SaveReqFilesTxt();
		saveTxtOnly = true;
	}
}

void RenderExportDialog(MGLDraw *mgl, int msx, int msy)
{
	curButton = ButtonId::None;

	// Background, title, exit
	mgl->FillBox(0, 0, mgl->GetWidth()-1, mgl->GetHeight()-1, 8);
	Print(5, 3, saveTxtOnly ? "Requirements scan" : "Export: File List", 0, 1);

	if (msx >= mgl->GetWidth() - 100 && msx <= mgl->GetWidth() - 5 && msy >= 3 && msy <= 3+14)
	{
		mgl->FillBox(mgl->GetWidth() - 100, 3, mgl->GetWidth() - 5, 3 + 14, 8+32*1);
		curButton = ButtonId::Close;
	}
	mgl->Box(mgl->GetWidth() - 100, 3, mgl->GetWidth() - 5, 3+14, 31);
	Print(mgl->GetWidth() - 100 + 2, 3 + 2, "Close", 0, 1);

	// Left half: the file list proper
	mgl->FillBox(2, 18, midX - 10, 18 + 2 + 14*FILES_PER_PAGE, 0);
	mgl->Box(2, 18, midX - 10, 18 + 2 + 14*FILES_PER_PAGE, 31);

	size_t start = offset, end = std::min(files.size(), offset + FILES_PER_PAGE);
	int y = 7 + 13;
	for (size_t i = start; i < end; ++i)
	{
		//int color = 0;
		switch (files[i].kind)
		{
			case FileKind::Root:
				mgl->FillBox(3, y-1, midX - 11, y + 13, 1*32 + 4);
				Print(5, y, "World", 0, 1);
				break;
			case FileKind::DependencyMissing:
				mgl->FillBox(3, y-1, midX - 11, y + 13, 4*32 + 4);
				Print(5, y, "MISSING!!", 0, 1);
				break;
			case FileKind::DependencyAppdata:
			case FileKind::DependencyOtherAddon:
				// TODO: make "other addon" dependencies optional.
				mgl->FillBox(3, y-1, midX - 11, y + 13, 1*32 + 4);
				Print(5, y, "Include", 0, 1);
				break;
			case FileKind::DependencyBaseGame:
				mgl->FillBox(3, y-1, midX - 11, y + 13, 0);
				Print(5, y, "Base game", 0, 1);
				break;
			default:
				break;
		}
		Print(100, y, files[i].filename.c_str(), 0, 1);
		y += 14;
	}
	y = 7 + 13 + 14 * FILES_PER_PAGE;

	y += 5;
	char buf[256];
	sprintf(buf, "Viewing %u-%u of %u files.", (unsigned int)(start+1), (unsigned int)(end), (unsigned int)(files.size()));
	Print(5, y, buf, 0, 1);

	int included = 0;
	for (const auto& file : files)
	{
		included += IncludeKind(file.kind) ? 1 : 0;
	}
	y += 14;
	sprintf(buf, "Including %d of %u files.", included, (unsigned int)(files.size()));
	Print(5, y, buf, 0, 1);

	y += 14;
	if (msx >= 5 && msx <= 5 + 200 && msy >= y && msy <= y+14)
	{
		mgl->FillBox(5, y, 5 + 200, y + 14, 8+32*1);
		curButton = ButtonId::SaveReqFilesTxt;
	}
	mgl->Box(5, y, 5 + 200, y + 14, 31);
	Print(5 + 2, y + 2, "Save list to req_files.txt", 0, 1);
	Print(5 + 210, y + 2, saveTxtResult.c_str(), 0, 1);

	// Right half: buttons to go to the next step
	if (!saveTxtOnly)
	{
		sprintf(buf, "%s.zip", zipName.c_str());
		Print(midX, 20, buf, 0, 1);

		if (msx >= midX && msx <= midX + 200 && msy >= 35 && msy <= 35+14)
		{
			mgl->FillBox(midX, 35, midX + 200, 35+14, 8+32*1);
			curButton = ButtonId::SaveZip;
		}
		mgl->Box(midX, 35, midX + 200, 35+14, 31);
		Print(midX + 2, 35 + 2, "Save to 'exports' folder", 0, 1);
		Print(midX, 53, saveZipResult, 0, 1);

#ifdef HAS_STEAM_API
		if (SteamManager::Get()->CanUploadToWorkshop())
		{
			y = 200;
			Print(midX, y, "- OR -", 0, 1);
			y += 14;
			Print(midX, y, steamWorkshopUpload.workshopItemId ? "Publish update to Steam Workshop" : "Publish to Steam Workshop", 0, 1);
			y += 14;

			if (msx >= midX && msx <= midX + 200 && msy >= y && msy <= y+14)
			{
				mgl->FillBox(midX, y, midX + 200, y+14, 8+32*1);
				curButton = ButtonId::PublishToSteamWorkshop;
			}
			mgl->Box(midX, y, midX + 200, y+14, 31);
			Print(midX + 2, y + 2, "Upload and publish", 0, 1);
			y += 18;

			if (steamWorkshopUpload.workshopItemId)
			{
				if (msx >= midX && msx <= midX + 200 && msy >= y && msy <= y+14)
				{
					mgl->FillBox(midX, y, midX + 200, y+14, 8+32*1);
					curButton = ButtonId::ViewWorkshopItem;
				}
				mgl->Box(midX, y, midX + 200, y+14, 31);
				Print(midX + 2, y + 2, "View in Workshop", 0, 1);
			}
			y += 18;

			Print(midX, y, steamWorkshopUpload.progressMessage, 0, 1);
			y += 14;
		}
#endif  // HAS_STEAM_API
	}

	if (!warnings.empty())
	{
		y = 400;
		Print(midX, 400, "Warning!", 0, 1);
		for (const auto& warning : warnings)
		{
			y += 14;
			Print(midX, y, "-", 0, 1);
			Print(midX + 10, y, warning.c_str(), 0, 1);
		}
	}
}

bool ExportDialogClick(int msx, int msy)
{
	switch (curButton)
	{
		case ButtonId::Close:
			return false;
		case ButtonId::SaveReqFilesTxt:
			SaveReqFilesTxt();
			break;
		case ButtonId::SaveZip:
			SaveZip();
			break;
#ifdef HAS_STEAM_API
		case ButtonId::PublishToSteamWorkshop:
			PublishToSteamWorkshop();
			break;
		case ButtonId::ViewWorkshopItem:
		{
			std::string url = "https://steamcommunity.com/sharedfiles/filedetails/?id=";
			url.append(std::to_string(steamWorkshopUpload.workshopItemId));
			//SteamManager::Get()->OpenUrl(url.c_str());
			SteamFriends()->ActivateGameOverlayToWebPage(url.c_str());
			break;
		}
#endif
		default:
			break;
	}

	return true;
}

bool ExportDialogKey(char key)
{
	if (key == 27)
		return false;
	return true;
}

void ExportDialogScroll(int msz)
{
	offset -= std::min(msz, static_cast<int>(offset));
	offset = std::min(offset, files.size() - std::min(files.size(), FILES_PER_PAGE));
}
