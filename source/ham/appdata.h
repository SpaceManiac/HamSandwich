#ifndef APPDATA_H
#define APPDATA_H

#include <vector>
#include <string>
#include "jamultypes.h"
#include "owned_sdl.h"

typedef struct SDL_IOStream SDL_IOStream;
struct HamSandwichMetadata;
namespace vanilla
{
	class VfsStack;
}

// Methods for accessing assets/saves sensibly on each platform. Writes always
// go to the write location; reads check write location first then descend the
// stack. See appdata.cpp for details on what the stack is on each platform.

HAM_API const char* EscapeBinDirectory();
HAM_API void AppdataInit(const HamSandwichMetadata* metadata);
HAM_API bool AppdataIsInit();
HAM_API vanilla::VfsStack* Vfs();

// Open for reading.
HAM_API owned::SDL_IOStream AppdataOpen(const char* filename);
// Open for writing.
HAM_API owned::SDL_IOStream AppdataOpen_Write(const char* filename);
// Delete file. Only applies to written files; cannot delete asset files.
HAM_API void AppdataDelete(const char* filename);
// List the contents of a directory, mixing .
HAM_API std::vector<std::string> ListDirectory(const char* directory, const char* extension = nullptr, size_t maxlen = 0);

// On platforms that need it, ensure appdata is really saved.
HAM_API void AppdataSync();

#endif  // APPDATA_H
