#include "globals.h"
#include "editor.h"

using namespace std;

Display display;
Font gFont, gIconFont;

/*AssetContext<Bitmap> graphics([](std::string key) {
    Bitmap result(("gfx/" + key + ".png").c_str());
    if (!result) result = Bitmap(8, 8);
    return result;
});*/

//AssetContext<Bitmap> graphics(AssetContext<Bitmap>::prefixLoader("graphics/"));
//AssetContext<Bitmap> graphics(AssetContext<Bitmap>::constructLoader);

int main(int argc, char** argv) {
    InitAttempt(al_init());
    InitAttempt(al_install_keyboard());
    InitAttempt(al_install_mouse());
    InitAttempt(al_init_image_addon());
    al_init_font_addon();
    InitAttempt(al_init_ttf_addon());
    InitAttempt(al_init_primitives_addon());

    InitAttempt(gFont.load("vera.ttf", 14));
    InitAttempt(gIconFont.load("fontawesome.ttf", 14));

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    InitAttempt(display = Display(DISPLAY_WIDTH, DISPLAY_HEIGHT));

    display.setWindowTitle("JspEdit 2");
    //display.setWindowIcon(graphics["gamepad.png"]);
    //al_hide_mouse_cursor(display.get());

    if (argc > 1) {
        editor::loadOnStartup(argv[1]);
    }

    int ret = ExceptionTrapVoid(editor::main);

    // we need to clean up assets here, because if they're left in the global
    // scope they'll be destroyed after Allegro shuts down, which we don't want
    display.reset();
    gFont.reset();
    gIconFont.reset();
    //graphics.clear();

    return ret;
}

