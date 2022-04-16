#include "vanilla_extract.h"
#include <SDL.h>
#include <unzip.h>
#include "vec_rw.h"

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
	if (unzLocateFile(zip, filename, 0) != UNZ_OK)
		return nullptr;
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
	std::string_view directory = directory_raw;

	unz_global_info64 gi;
	if (unzGetGlobalInfo64(zip, &gi) != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::list_dir(): bad unzGetGlobalInfo64");
		return false;
	}
	if (unzGoToFirstFile(zip) != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::list_dir(): bad unzGoToFirstFile");
		return false;
	}
	for (size_t i = 0; i < gi.number_entry; ++i)
	{
		unz_file_info64 file_info;
		char filename_buf[1024];
		if (unzGetCurrentFileInfo64(zip, &file_info, filename_buf, sizeof(filename_buf), nullptr, 0, nullptr, 0) != UNZ_OK)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::list_dir(): bad unzGetCurrentFileInfo64");
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
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::list_dir(): bad unzGoToNextFile");
				return false;
			}
		}
	}
	return true;
}
