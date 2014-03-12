#ifndef GUI_H
#define GUI_H

#include "globals.h"
#include "fontawesome.h"
#include <functional>

typedef Rect<int> GuiRect;

struct GuiElement {
    // type field later, for now all buttons
    GuiRect size;
    Font font;
    String text;
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
        elements.push_back(elem);
    }

    void addButton(GuiRect rect, String text, std::function<void()> func = nullptr) {
        add({ rect, gFont, text, func });
    }

    void addIconButton(int x, int y, FAChar ch, std::function<void()> func = nullptr) {
        GuiRect rect(x, y, x + 20, y + 20);
        add({ rect, gIconFont, FA::str(ch), func});
    }

};

#endif // GUI_H
