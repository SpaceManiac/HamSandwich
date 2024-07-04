#ifndef OPENURL_H
#define OPENURL_H

#include <SDL_version.h>

#if SDL_VERSION_ATLEAST(2, 0, 14)
#include <SDL_misc.h>
#else

// Cheap workaround for our Emscripten build still being on an old SDL.
#define FAKE_SDL_OPENURL
extern int SDL_OpenURL(const char *url);

#endif  // SDL_VERSION_ATLEAST(2, 0, 14)

#endif  // OPENURL_H
