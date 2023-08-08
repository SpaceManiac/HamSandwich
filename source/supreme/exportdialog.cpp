#include "exportdialog.h"
#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include "world.h"
#include "display.h"
#include "appdata.h"
#include "vanilla_extract.h"

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

	constexpr int midX = 405;

	bool saveTxtOnly = false;
	std::string saveTxtResult;

	enum class ButtonId
	{
		None,
		SaveReqFilesTxt,
		SaveZip,
	};
	ButtonId curButton;
}

static void AddDependency(std::string_view part1, std::string_view part2)
{
	std::string fname;
	fname.reserve(part1.size() + part2.size());
	fname.append(part1);
	fname.append(part2);

	if (!seen.insert(fname).second)
		return;

	FileKind kind;
	vanilla::VfsSourceKind sourceKind;
	if (AppdataVfs().query_bottom(fname.c_str(), &sourceKind))
	{
		switch (sourceKind)
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
	else
	{
		kind = FileKind::DependencyMissing;
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

void InitExportDialog(const world_t* world, const char* filename)
{
	seen.clear();
	files.clear();
	warnings.clear();
	saveZipResult.clear();
	offset = 0;
	saveTxtOnly = false;

	if (filename)
	{
		seen.insert(filename);
		files.push_back({ FileKind::Root, filename });
	}

	for (int i = 0; i < world->numMaps; ++i)
	{
		const Map* map = world->map[i];
		if (map->song[0])
		{
			AddDependency("music/", map->song);
		}

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
	if (!strcmp(world->author, "Nobody"))
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
	sprintf(buf, "Viewing %zu-%zu of %zu files.", start+1, end, files.size());
	Print(5, y, buf, 0, 1);

	int included = 0;
	for (const auto& file : files)
	{
		switch (file.kind)
		{
			case FileKind::Root:
			case FileKind::DependencyAppdata:
			case FileKind::DependencyOtherAddon:
				included += 1;
			default:
				break;
		}
	}
	y += 14;
	sprintf(buf, "Including %d of %zu files.", included, files.size());
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
		Print(midX, 18, buf, 0, 1);

		if (msx >= midX && msx <= midX + 200 && msy >= 35 && msy <= 35+14)
		{
			mgl->FillBox(midX, 35, midX + 200, 35+14, 8+32*1);
			curButton = ButtonId::SaveZip;
		}
		mgl->Box(midX, 35, midX + 200, 35+14, 31);
		Print(midX + 2, 35 + 2, "Save to addons folder", 0, 1);
	}

	Print(midX, 53, saveZipResult.c_str(), 0, 1);

	if (!warnings.empty())
	{
		Print(midX, 400, "Warning!", 0, 1);
		int i = 0;
		for (const auto& warning : warnings)
		{
			++i;
			Print(midX, 400 + 14 * i, "-", 0, 1);
			Print(midX + 10, 400 + 14 * i, warning.c_str(), 0, 1);
		}
	}
}

bool ExportDialogClick(int msx, int msy)
{
	switch (curButton)
	{
		case ButtonId::SaveReqFilesTxt:
			SaveReqFilesTxt();
			break;
		case ButtonId::SaveZip:
			// TODO
			saveZipResult = "Saved OK!";
			break;
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
