#include "vanilla_extract.h"
#include "inno.h"

// ----------------------------------------------------------------------------
// Inno VFS implementation

class InnoVfs : public vanilla::Vfs
{
	vanilla::inno::Archive archive;
public:
	InnoVfs(FILE* fp) : archive(fp) {}
	FILE* open_stdio(const char* filename);
	SDL_RWops* open_sdl(const char* filename);
	bool list_dir(const char* directory, std::set<std::string>& output);
};

std::unique_ptr<vanilla::Vfs> vanilla::open_inno(FILE* fp)
{
	return std::make_unique<InnoVfs>(fp);
}

FILE* InnoVfs::open_stdio(const char* filename)
{
	SDL_RWops* rw = open_sdl(filename);
	return rw ? vanilla::fp_from_bundle(filename, "rb", rw, ".inno_tmp", true) : nullptr;
}

SDL_RWops* InnoVfs::open_sdl(const char* filename)
{
	return archive.open_file(filename);
}

bool InnoVfs::list_dir(const char* directory, std::set<std::string>& output)
{
	return archive.list_dir(directory, output);
}
