#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

extern int DISPLAY_WIDTH;
extern int DISPLAY_HEIGHT;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *gFont, *gIconFont;

enum Align {
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_CENTER,
};
void DrawText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, Align align, SDL_Color color, const char* text);

#endif // GLOBALS_H
