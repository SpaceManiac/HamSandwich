#include "vanilla_extract.h"
#include <SDL.h>
#include <unzip.h>
#include "vec_rw.h"
#include "base_archive.h"

using namespace vanilla;

// ----------------------------------------------------------------------------
// For Android's benefit, a wrapper from SDL_RWops to zip i/o

static void* zsdl_open64(void* userdata, const void* filename, int mode)
{
	return userdata;
}

static uLong zsdl_read(void* userdata, void* stream, void* buf, uLong size)
{
	return SDL_RWread((SDL_RWops*) stream, buf, 1, size);
}

static uLong zsdl_write(void* userdata, void* stream, const void* buf, uLong size)
{
	return SDL_RWwrite((SDL_RWops*) stream, buf, 1, size);
}

static ZPOS64_T zsdl_tell64(void* userdata, void* stream)
{
	return SDL_RWtell((SDL_RWops*) stream);
}

// Skip writing a switch by directly mapping the seek types.
static_assert(ZLIB_FILEFUNC_SEEK_SET == RW_SEEK_SET);
static_assert(ZLIB_FILEFUNC_SEEK_CUR == RW_SEEK_CUR);
static_assert(ZLIB_FILEFUNC_SEEK_END == RW_SEEK_END);
static long zsdl_seek64(void* userdata, void* stream, ZPOS64_T offset, int origin)
{
	SDL_RWseek((SDL_RWops*) stream, offset, origin);
	return 0;
}

static int zsdl_close(void* userdata, void* stream)
{
	return SDL_RWclose((SDL_RWops*) stream);
}

static int zsdl_testerror(void* userdata, void* stream)
{
	return 0;
}

// ----------------------------------------------------------------------------
// Zip VFS implementation

class ZipVfs : public Vfs
{
	unzFile zip;
	std::vector<unz64_file_pos> files;
	vanilla::Archive archive;

	ZipVfs(const ZipVfs& other) = delete;
	ZipVfs(ZipVfs&& other) = delete;
	ZipVfs& operator=(const ZipVfs& other) = delete;
	ZipVfs& operator=(ZipVfs&& other) = delete;
public:
	ZipVfs(SDL_RWops* rw)
	{
		zlib_filefunc64_def sdl_zlib_io =
		{
			zsdl_open64,
			zsdl_read,
			zsdl_write,
			zsdl_tell64,
			zsdl_seek64,
			zsdl_close,
			zsdl_testerror,
			rw,
		};
		zip = unzOpen2_64(nullptr, &sdl_zlib_io);

		// Traverse the .zip file linearly once, building a directory tree.
		// This is significantly faster than unzLocateFile, which does this
		// linear traversal for every single lookup.
		int err = unzGoToFirstFile(zip);
		while (err == UNZ_OK)
		{
			char filename[256+1];
			err = unzGetCurrentFileInfo64(
				zip,
				nullptr,
				filename, sizeof(filename)-1,
				nullptr, 0,
				nullptr, 0);
			if (err == UNZ_OK)
			{
				unz64_file_pos file_pos;
				unzGetFilePos64(zip, &file_pos);

				size_t location = files.size();
				files.push_back(file_pos);

				vanilla::Archive::Directory* current = &archive.root;
				if (const char* last_component = vanilla::Archive::navigate(filename, current))
				{
					current->files.insert(make_pair(std::string(last_component), location));
				}

				err = unzGoToNextFile(zip);
			}
		}
		if (err != UNZ_OK && err != UNZ_END_OF_LIST_OF_FILE)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs: bad construction: %d", err);
		}
	}
	~ZipVfs();

	SDL_RWops* open_sdl(const char* filename);
	bool list_dir(const char* directory, std::set<std::string>& output);
};

std::unique_ptr<Vfs> vanilla::open_zip(SDL_RWops* rw)
{
	return std::make_unique<ZipVfs>(rw);
}

ZipVfs::~ZipVfs()
{
	unzClose(zip);
}

SDL_RWops* ZipVfs::open_sdl(const char* filename)
{
	size_t file = archive.get_file(filename);
	if (file == SIZE_MAX)
		return nullptr;
	if (unzGoToFilePos64(zip, &files[file]) != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::open_sdl(%s): bad unzGoToFilePos64", filename);
		return nullptr;
	}
	unz_file_info64 file_info;
	if (unzGetCurrentFileInfo64(zip, &file_info, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::open_sdl(%s): bad unzGetCurrentFileInfo64", filename);
		return nullptr;
	}
	if (unzOpenCurrentFile(zip) != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::open_sdl(%s): bad unzOpenCurrentFile", filename);
		return nullptr;
	}
	std::vector<uint8_t> buffer(file_info.uncompressed_size);
	if (unzReadCurrentFile(zip, buffer.data(), file_info.uncompressed_size) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::open_sdl(%s): bad unzReadCurrentFile", filename);
		return nullptr;
	}
	unzCloseCurrentFile(zip);
	return vanilla::create_vec_rwops(std::move(buffer));
}

bool ZipVfs::list_dir(const char* directory_raw, std::set<std::string>& output)
{
	return archive.list_dir(directory_raw, output);
}
