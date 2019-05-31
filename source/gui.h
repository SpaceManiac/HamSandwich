#ifndef GUI_H
#define GUI_H

#include "globals.h"
#include "fontawesome.h"
#include <functional>
#include <vector>

struct GuiRect {
    int left, top, right, bottom;
};

struct KbdShortcut {
    int mods, keycode;

    bool operator==(const KbdShortcut& other) {
        return mods == other.mods && keycode == other.keycode;
    };
    bool operator!=(const KbdShortcut& other) { return !(*this == other); }
};

class Gui {
    int mx, my;
    bool justClicked;
    KbdShortcut justTyped;
    std::string tooltip;

public:
    Gui();

    void render();
    bool handleEvent(const SDL_Event &evt);

    bool element(GuiRect rect, TTF_Font *font, const std::string &text, const std::string &desc, KbdShortcut shortcut);
    bool button(GuiRect rect, const std::string &text, const std::string &desc, KbdShortcut shortcut);
    bool iconButton(int x, int y, FAChar ch, const std::string &desc, KbdShortcut shortcut);
};

#endif // GUI_H
