#include "vfs_zip.h"
#include "vfs.h"
#include "vfs_stdio.h"
#include <SDL.h>
#include <unzip.h>
#include "vec_rw.h"

using namespace vanilla;

// ----------------------------------------------------------------------------
// Zip VFS implementation

class ZipVfs : public Vfs
{
	unzFile zip;
	ZipVfs(const ZipVfs& other) = delete;
	ZipVfs(ZipVfs&& other) = delete;
	ZipVfs& operator=(const ZipVfs& other) = delete;
	ZipVfs& operator=(ZipVfs&& other) = delete;
public:
	ZipVfs(const char* fname) : zip(unzOpen(fname)) {}
	~ZipVfs();

	FILE* open_stdio(const char* filename);
	SDL_RWops* open_sdl(const char* filename);
	bool list_dir(const char* directory, std::set<std::string>& output);
};

std::unique_ptr<Vfs> vanilla::open_zip(const char* filename)
{
	return std::make_unique<ZipVfs>(filename);
}

ZipVfs::~ZipVfs()
{
	unzClose(zip);
}

FILE* ZipVfs::open_stdio(const char* filename)
{
	SDL_RWops* rw = open_sdl(filename);
	return rw ? fp_from_bundle(filename, "rb", rw, ".zip_tmp", true) : nullptr;
}

SDL_RWops* ZipVfs::open_sdl(const char* filename)
{
	if (unzLocateFile(zip, filename, 0) != UNZ_OK)
		return nullptr;
	unz_file_info64 file_info;
	if (unzGetCurrentFileInfo64(zip, &file_info, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "ZipVfs::open_sdl(%s): bad unzGetCurrentFileInfo64", filename);
		return nullptr;
	}
	if (unzOpenCurrentFile(zip) != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "ZipVfs::open_sdl(%s): bad unzOpenCurrentFile", filename);
		return nullptr;
	}
	std::vector<uint8_t> buffer(file_info.uncompressed_size);
	if (unzReadCurrentFile(zip, buffer.data(), file_info.uncompressed_size) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "ZipVfs::open_sdl(%s): bad unzReadCurrentFile", filename);
		return nullptr;
	}
	unzCloseCurrentFile(zip);
	return vanilla::create_vec_rwops(std::move(buffer));
}

bool ZipVfs::list_dir(const char* directory_raw, std::set<std::string>& output)
{
	std::string_view directory = directory_raw;

	unz_global_info64 gi;
	if (unzGetGlobalInfo64(zip, &gi) != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "ZipVfs::list_dir(): bad unzGetGlobalInfo64");
		return false;
	}
	if (unzGoToFirstFile(zip) != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "ZipVfs::list_dir(): bad unzGoToFirstFile");
		return false;
	}
	for (size_t i = 0; i < gi.number_entry; ++i)
	{
		unz_file_info64 file_info;
		char filename_buf[1024];
		if (unzGetCurrentFileInfo64(zip, &file_info, filename_buf, sizeof(filename_buf), nullptr, 0, nullptr, 0) != UNZ_OK)
		{
			SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "ZipVfs::list_dir(): bad unzGetCurrentFileInfo64");
			return false;
		}

		std::string_view filename(filename_buf, file_info.size_filename);
		if (filename.substr(0, directory.size()) == directory && filename[directory.size()] == '/')
		{
			output.insert(std::string(filename.substr(directory.size() + 1)));
		}

		if (i + 1 < gi.number_entry)
		{
			if (unzGoToNextFile(zip) != UNZ_OK)
			{
				SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "ZipVfs::list_dir(): bad unzGoToNextFile");
				return false;
			}
		}
	}
	return true;
}
