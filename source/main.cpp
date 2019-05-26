#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_ttf.h>
#include <string>
#include <unistd.h>  // for chdir
#include "globals.h"
#include "editor.h"

using namespace std;

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *gFont, *gIconFont;

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    string exe = argv[0];
    string dir = exe.substr(0, exe.find_last_of("\\/"));
    chdir(dir.c_str());

    gFont = TTF_OpenFont("vera.ttf", 14);
    gIconFont = TTF_OpenFont("fontawesome.ttf", 14);

    SDL_CreateWindowAndRenderer(DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "JspEdit 3");

    if (argc > 1) {
        editor::loadOnStartup(argv[1]);
    }

    editor::main();

    TTF_CloseFont(gIconFont);
    TTF_CloseFont(gFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
