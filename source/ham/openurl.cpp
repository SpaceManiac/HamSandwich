#include "openurl.h"

#ifdef FAKE_SDL_OPENURL
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

int SDL_OpenURL(const char *url)
{
    EM_ASM({
        window.open(UTF8ToString($0), "_blank");
    }, url);

    return 0;
}

#endif  // __EMSCRIPTEN__
#endif  // FAKE_SDL_OPENURL
