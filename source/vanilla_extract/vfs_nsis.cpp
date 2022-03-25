#include "vfs_nsis.h"
#include "vfs.h"
#include "vfs_stdio.h"
#include "nsis.h"

// ----------------------------------------------------------------------------
// NSIS VFS implementation

class NsisVfs : public vanilla::Vfs
{
	vanilla::nsis::Archive archive;
public:
	NsisVfs(SDL_RWops* fp) : archive(fp) {}
	FILE* open_stdio(const char* filename);
	SDL_RWops* open_sdl(const char* filename);
	bool list_dir(const char* directory, std::set<std::string>& output);
};

std::unique_ptr<vanilla::Vfs> vanilla::open_nsis(SDL_RWops* rw)
{
	return std::make_unique<NsisVfs>(rw);
}

FILE* NsisVfs::open_stdio(const char* filename)
{
	SDL_RWops* rw = open_sdl(filename);
	return rw ? vanilla::fp_from_bundle(filename, "rb", rw, ".nsis_tmp", true) : nullptr;
}

SDL_RWops* NsisVfs::open_sdl(const char* filename)
{
	return archive.open_file(filename);
}

bool NsisVfs::list_dir(const char* directory, std::set<std::string>& output)
{
	return archive.list_dir(directory, output);
}
