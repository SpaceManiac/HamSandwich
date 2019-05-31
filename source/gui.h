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
};

struct GuiElement {
    // type field later, for now all buttons
    GuiRect size;
    TTF_Font *font;
    std::string text;
    std::string tooltip;
    KbdShortcut shortcut;
    std::function<void()> func;
};

class Gui {
    std::vector<GuiElement> elements;
    GuiElement *hover, *focus;
    int mx, my;
    bool justClicked;
    std::string tooltip;

    GuiElement *elemAt(int x, int y);
public:
    Gui();

    void render();
    bool handleEvent(const SDL_Event &evt);

    void add(GuiElement elem) {
        if (!elem.font) elem.font = gFont;
        elements.push_back(elem);
    }

    void addButton(GuiRect rect, std::string text, std::string desc, KbdShortcut shortcut, std::function<void()> func) {
        add({ rect, gFont, text, desc, shortcut, func });
    }

    void addIconButton(int x, int y, FAChar ch, std::string desc, KbdShortcut shortcut, std::function<void()> func) {
        GuiRect rect = {x, y, x + 20, y + 20};
        add({ rect, gIconFont, FA::str(ch), desc, shortcut, func});
    }

    bool element(GuiRect rect, TTF_Font *font, const std::string &text, const std::string &desc, KbdShortcut shortcut);
    bool button(GuiRect rect, const std::string &text, const std::string &desc, KbdShortcut shortcut);
    bool iconButton(int x, int y, FAChar ch, const std::string &desc, KbdShortcut shortcut);
};

#endif // GUI_H
