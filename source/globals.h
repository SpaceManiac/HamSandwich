#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

enum {
    DISPLAY_WIDTH = 1024,
    DISPLAY_HEIGHT = 768
};

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *gFont, *gIconFont;

#endif // GLOBALS_H
