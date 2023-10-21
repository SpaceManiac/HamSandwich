#ifndef APPDATA_H
#define APPDATA_H

#include <stdio.h>
#include <vector>
#include <string>
#include <set>
#include "owned_sdl.h"

typedef struct SDL_RWops SDL_RWops;
struct HamSandwichMetadata;
namespace vanilla
{
	class VfsStack;
}

// Methods for accessing assets/saves 'correctly' on each platform. In some
// situations, Appdata and Asset folders may overlap, so names should not be
// reused. See appdata.cpp for implementation details for each platform.

const char* EscapeBinDirectory();
void AppdataInit(const HamSandwichMetadata* metadata);
bool AppdataIsInit();
vanilla::VfsStack& AppdataVfs();

// Open for reading.
FILE* AssetOpen(const char* filename);
owned::SDL_RWops AssetOpen_SDL_Owned(const char* filename);
// Open for writing or appending.
FILE* AppdataOpen_Write(const char* filename);
owned::SDL_RWops AppdataOpen_Write_SDL(const char* filename);
//FILE* AppdataOpen_Append(const char* filename);
void AppdataDelete(const char* filename);
// On platforms that need it, ensure appdata is really saved.
void AppdataSync();

// List a directory.
std::vector<std::string> ListDirectory(const char* directory, const char* extension = nullptr, size_t maxlen = 0);

// Aliases.
FILE* AppdataOpen(const char* filename);
FILE* AssetOpen_Write(const char* filename);

#endif  // APPDATA_H
