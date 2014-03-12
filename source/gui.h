#ifndef GUI_H
#define GUI_H

#include "globals.h"
#include "fontawesome.h"
#include <functional>

typedef Rect<int> GuiRect;

struct KbdShortcut {
    int mods, keycode;
};

struct GuiElement {
    // type field later, for now all buttons
    GuiRect size;
    Font font;
    String text;
    KbdShortcut shortcut;
    std::function<void()> func;
};

class Gui {
    std::vector<GuiElement> elements;
    GuiElement *hover, *focus;

    GuiElement *elemAt(int x, int y);
public:

    void render();
    bool handleEvent(Event evt);

    void add(GuiElement elem) {
        if (!elem.font) elem.font = gFont;
        elements.push_back(elem);
    }

    void addButton(GuiRect rect, String text, KbdShortcut shortcut = {}, std::function<void()> func = nullptr) {
        add({ rect, gFont, text, shortcut, func });
    }

    void addIconButton(int x, int y, FAChar ch, KbdShortcut shortcut = {}, std::function<void()> func = nullptr) {
        GuiRect rect(x, y, x + 20, y + 20);
        add({ rect, gIconFont, FA::str(ch), shortcut, func});
    }

};

#endif // GUI_H
