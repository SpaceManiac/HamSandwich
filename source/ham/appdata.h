#ifndef APPDATA_H
#define APPDATA_H

#include <stdio.h>

typedef struct SDL_RWops SDL_RWops;

// Methods for accessing assets/saves 'correctly' on each platform. In some
// situations, Appdata and Asset folders may overlap, so names should not be
// reused. See appdata.cpp for implementation details for each platform.

// Use for saves, options, config, etc.
FILE* AppdataOpen(const char* file, const char* mode);

// Use for graphics, worlds, etc. Writes may go to a different location than reads.
FILE* AssetOpen(const char* file, const char* mode);
SDL_RWops* AssetOpen_SDL(const char* file, const char* mode);

#endif  // APPDATA_H
