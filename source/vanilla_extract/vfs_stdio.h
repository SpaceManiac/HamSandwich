#ifndef VANILLA_EXTRACT_STDIO_H
#define VANILLA_EXTRACT_STDIO_H

#include <memory>
#include <stdio.h>

struct SDL_RWops;

namespace vanilla
{
	class WriteVfs;

	std::unique_ptr<WriteVfs> open_stdio(const char* prefix);

	int mkdir_parents(const char* path);
	FILE* fp_from_bundle(const char* file, const char* mode, SDL_RWops* rw, const char* tempdir, bool reuse_safe);
}

#endif  // VANILLA_EXTRACT_STDIO_H
