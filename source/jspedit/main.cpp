#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <map>
#include "globals.h"
#include "editor.h"
#include "jamulfont.h"
#include "fontawesome.h"

extern const size_t embed_verdana_size;
extern const unsigned char embed_verdana[];
extern const size_t embed_icons_size;
extern const unsigned char embed_icons[];
#if !defined(_WIN32) && !defined(__clang__)
extern const size_t embed_game_icon_size;
extern const unsigned char embed_game_icon[];
#endif

using namespace std;

SDL_Window *window;
SDL_Renderer *renderer;
mfont_t gFontBuf, *gFont;
SDL_Texture *gIcons;

int DISPLAY_WIDTH = 1024;
int DISPLAY_HEIGHT = 768;

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    IMG_Init(IMG_INIT_PNG);

    window = SDL_CreateWindow("JspEdit 3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_GetWindowSize(window, &DISPLAY_WIDTH, &DISPLAY_HEIGHT);

#if !defined(_WIN32) && !defined(__clang__)
    SDL_Surface *surface = IMG_Load_RW(SDL_RWFromConstMem(embed_game_icon, embed_game_icon_size), true);
    SDL_SetWindowIcon(window, surface);
    SDL_FreeSurface(surface);
#endif

    SDL_Surface *icons = IMG_Load_RW(SDL_RWFromConstMem(embed_icons, embed_icons_size), true);
    gIcons = SDL_CreateTextureFromSurface(renderer, icons);
    SDL_FreeSurface(icons);

    SDL_RWops *rwFont = SDL_RWFromConstMem(embed_verdana, embed_verdana_size);
    gFont = &gFontBuf;
    FontLoad(rwFont, gFont);
    SDL_RWclose(rwFont);

    if (argc > 1) {
        editor::loadOnStartup(argv[1]);
    }

    editor::main();

    SDL_DestroyTexture(gIcons);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}

void DrawText(SDL_Renderer *renderer, int x, int y, Align align, SDL_Color color, std::string_view text) {
    int width = 0;

    for (char ch : text)
    {
        if (ch == ' ')
            width += gFont->spaceSize;
        else if (ch >= gFont->firstChar && ch < gFont->firstChar + gFont->numChars)
            width += CharWidth(ch, gFont) + gFont->gapSize;
        else
            width += 16;
    }

    if (align == ALIGN_RIGHT)
    {
        x -= width;
    }
    else if (align == ALIGN_CENTER)
    {
        x -= width / 2;
    }

    for (char ch : text)
    {
        if (ch == ' ')
        {
            x += gFont->spaceSize;
        }
        else if (ch >= gFont->firstChar && ch < gFont->firstChar + gFont->numChars)
        {
            const uint8_t* data = gFont->chars[ch - gFont->firstChar];
            uint8_t charWidth = *data++;

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            for (int j = 0; j < gFont->height; ++j)
            {
                for (int i = 0; i < charWidth; ++i)
                {
                    if (*data++)
                        SDL_RenderDrawPoint(renderer, x + i, y + j + 3);
                }
            }

            x += charWidth + gFont->gapSize;
        }
        else if (ch > 0)
        {
            SDL_Rect source = { 15 * (ch - 1), 0, 15, 15 };
            SDL_Rect dest = { x, y + 1, 15, 15 };

            SDL_SetTextureColorMod(gIcons, color.r, color.g, color.b);
            SDL_RenderCopy(renderer, gIcons, &source, &dest);
            x += 16;
        }
    }
}
