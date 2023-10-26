#include "vfs_zip.h"
#include <SDL.h>
#include <unzip.h>
#include "vec_rw.h"
#include "base_archive.h"

using namespace vanilla;

// ----------------------------------------------------------------------------
// For Android's benefit, a wrapper from SDL_RWops to zip i/o

static void* zsdl_open64(void* userdata, const void* filename, int mode)
{
	(void)filename;
	(void)mode;
	return userdata;
}

static uLong zsdl_read(void* userdata, void* stream, void* buf, uLong size)
{
	(void)userdata;
	return SDL_RWread((SDL_RWops*) stream, buf, 1, size);
}

static uLong zsdl_write(void* userdata, void* stream, const void* buf, uLong size)
{
	(void)userdata;
	return SDL_RWwrite((SDL_RWops*) stream, buf, 1, size);
}

static ZPOS64_T zsdl_tell64(void* userdata, void* stream)
{
	(void)userdata;
	return SDL_RWtell((SDL_RWops*) stream);
}

// Skip writing a switch by directly mapping the seek types.
static_assert(ZLIB_FILEFUNC_SEEK_SET == RW_SEEK_SET);
static_assert(ZLIB_FILEFUNC_SEEK_CUR == RW_SEEK_CUR);
static_assert(ZLIB_FILEFUNC_SEEK_END == RW_SEEK_END);
static long zsdl_seek64(void* userdata, void* stream, ZPOS64_T offset, int origin)
{
	(void)userdata;
	SDL_RWseek((SDL_RWops*) stream, offset, origin);
	return 0;
}

static int zsdl_close(void* userdata, void* stream)
{
	(void)userdata;
	return SDL_RWclose((SDL_RWops*) stream);
}

static int zsdl_testerror(void* userdata, void* stream)
{
	(void)userdata;
	(void)stream;
	return 0;
}

// ----------------------------------------------------------------------------
// Owned unzFile

#if !(defined(STRICTUNZIP) || defined(STRICTZIPUNZIP))
typedef void unzFile__;
#endif

namespace owned
{
	namespace _deleter
	{
		struct unzFile__
		{
			void operator()(::unzFile ptr)
			{
				int err;
				if (ptr && (err = unzClose(ptr)) != UNZ_OK)
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "unzClose: %d", err);
				}
			}
		};
	}

	typedef std::unique_ptr<::unzFile__, _deleter::unzFile__> unzFile;
}

// ----------------------------------------------------------------------------
// Zip VFS implementation

class ZipVfs : public Vfs
{
	owned::unzFile zip;
	std::vector<unz64_file_pos> files;
	vanilla::Archive archive;
public:
	explicit ZipVfs(owned::SDL_RWops rw)
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
			rw.release(),  // Our zsdl_close above will call SDL_RWclose.
		};
		zip.reset(unzOpen2_64(nullptr, &sdl_zlib_io));

		// Traverse the .zip file linearly once, building a directory tree.
		// This is significantly faster than unzLocateFile, which does this
		// linear traversal for every single lookup.
		int err = unzGoToFirstFile(zip.get());
		while (err == UNZ_OK)
		{
			char filename[256+1];
			err = unzGetCurrentFileInfo64(
				zip.get(),
				nullptr,
				filename, sizeof(filename)-1,
				nullptr, 0,
				nullptr, 0
			);
			if (err == UNZ_OK)
			{
				unz64_file_pos file_pos;
				unzGetFilePos64(zip.get(), &file_pos);

				size_t location = files.size();
				files.push_back(file_pos);

				vanilla::Archive::Directory* current = &archive.root;
				if (const char* last_component = vanilla::Archive::navigate(filename, current))
				{
					current->files.insert(make_pair(std::string(last_component), location));
				}

				err = unzGoToNextFile(zip.get());
			}
		}
		if (err != UNZ_OK && err != UNZ_END_OF_LIST_OF_FILE)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs: bad construction: %d", err);
		}
	}

	owned::SDL_RWops open_sdl(const char* filename) override;
	bool list_dir(const char* directory, std::set<std::string, CaseInsensitive>& output) override;
};

std::unique_ptr<Vfs> vanilla::open_zip(owned::SDL_RWops rw)
{
	return std::make_unique<ZipVfs>(std::move(rw));
}

owned::SDL_RWops ZipVfs::open_sdl(const char* filename)
{
	size_t file = archive.get_file(filename);
	if (file == SIZE_MAX)
		return nullptr;
	if (int r = unzGoToFilePos64(zip.get(), &files[file]); r != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::open_sdl(%s): bad unzGoToFilePos64: %d", filename, r);
		return nullptr;
	}
	unz_file_info64 file_info;
	if (int r = unzGetCurrentFileInfo64(zip.get(), &file_info, nullptr, 0, nullptr, 0, nullptr, 0); r != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::open_sdl(%s): bad unzGetCurrentFileInfo64: %d", filename, r);
		return nullptr;
	}
	if (int r = unzOpenCurrentFile(zip.get()); r != UNZ_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::open_sdl(%s): bad unzOpenCurrentFile: %d", filename, r);
		return nullptr;
	}
	std::vector<uint8_t> buffer(file_info.uncompressed_size);
	if (int r = unzReadCurrentFile(zip.get(), buffer.data(), file_info.uncompressed_size); r < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ZipVfs::open_sdl(%s): bad unzReadCurrentFile: %d", filename, r);
		return nullptr;
	}
	unzCloseCurrentFile(zip.get());
	return vanilla::create_vec_rwops(std::move(buffer));
}

bool ZipVfs::list_dir(const char* directory_raw, std::set<std::string, CaseInsensitive>& output)
{
	return archive.list_dir(directory_raw, output);
}
