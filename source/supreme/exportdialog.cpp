#include "exportdialog.h"
#include <vector>
#include <string>
#include "world.h"
#include "display.h"
#include "appdata.h"

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
	};

	std::set<std::string> seen;
	std::vector<FileEntry> files;
}

static void AddDependency(const char* part1, const char* part2)
{
	std::string fname;
	fname.reserve(strlen(part1) + strlen(part2));
	fname.append(part1);
	fname.append(part2);

	if (!seen.insert(fname).second)
		return;

	// TODO: query vfs stack to determine kind

	files.push_back({ FileKind::DependencyAppdata, std::move(fname) });
}

void InitExportDialog(const world_t* world, const char* filename)
{
	seen.clear();
	seen.insert(filename);
	files.clear();
	files.push_back({ FileKind::Root, filename });

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
						AddDependency("user/", effect.text);
					}
				}
			}
		}
	}
}

void RenderExportDialog(MGLDraw *mgl, int msx, int msy)
{
	for (size_t i = 0; i < files.size(); ++i)
	{
		Print(5, 5 + 13*i, files[i].filename.c_str(), 0, 1);
	}
}

bool ExportDialogClick(int msx, int msy)
{
	return false;
}

bool ExportDialogKey(char key)
{
	return true;
}

void ExportDialogScroll(int msz)
{

}
