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

enum Align {
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_CENTER,
};
void DrawText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, Align align, SDL_Color color, const char* text);

int Col_LockTexture(SDL_Texture *texture, SDL_Rect *rect, SDL_Color **pixels, int *pitch);

#endif // GLOBALS_H
