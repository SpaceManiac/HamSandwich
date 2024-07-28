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

// Methods for accessing assets/saves sensibly on each platform. Writes always
// go to the write location; reads check write location first then descend the
// stack. See appdata.cpp for details on what the stack is on each platform.

const char* EscapeBinDirectory();
void AppdataInit(const HamSandwichMetadata* metadata);
bool AppdataIsInit();
vanilla::VfsStack* Vfs();

// Open for reading.
owned::SDL_RWops AppdataOpen(const char* filename);
// Open for writing.
owned::SDL_RWops AppdataOpen_Write(const char* filename);
// Delete
void AppdataDelete(const char* filename);
// List a directory.
std::vector<std::string> ListDirectory(const char* directory, const char* extension = nullptr, size_t maxlen = 0);

// On platforms that need it, ensure appdata is really saved.
void AppdataSync();

#endif  // APPDATA_H
