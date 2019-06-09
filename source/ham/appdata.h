#ifndef APPDATA_H
#define APPDATA_H

#include <stdio.h>

// Methods for accessing assets/saves 'correctly' on each platform.
// See appdata.cpp for implementation details.

// Use for saves, options, config, etc.
FILE* AppdataOpen(const char* file, const char* mode);

// Use for graphics, sounds, music, etc. Only mode "rb" is allowed.
FILE* AssetOpen(const char* file);

#endif  // APPDATA_H
