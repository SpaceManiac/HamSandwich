#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL.h>
#include <string>
#include <string_view>
#include "jamulfont.h"

extern int DISPLAY_WIDTH;
extern int DISPLAY_HEIGHT;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern mfont_t *gFont;
extern SDL_Texture *gIcons;

enum Align {
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_CENTER,
};
void DrawText(SDL_Renderer *renderer, int x, int y, Align align, SDL_Color color, std::string_view text);

#endif // GLOBALS_H
