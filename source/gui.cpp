#include "gui.h"

void Gui::render() {
    for (GuiElement& elem : elements) {
        GuiRect r = elem.size;

        bool hovered = (hover == &elem);
        bool focused = (focus == &elem);

        //r.setClipping();
        Color black(0, 0, 0);

        if (focused) {
            gfx::fillRect(r.getLeft(), r.getTop(), r.getRight() - 1, r.getBottom() - 1, Color(220, 220, 220));
        } else if (hovered) {
            gfx::fillRect(r.getLeft(), r.getTop(), r.getRight() - 1, r.getBottom() - 1, Color(255, 255, 255));
        }

        gfx::line(r.getLeft(), r.getTop(), r.getLeft(), r.getBottom() - 1, black, 1);
        gfx::line(r.getRight(), r.getTop(), r.getRight(), r.getBottom() - 1, black, 1);
        gfx::line(r.getLeft(), r.getTop(), r.getRight() - 1, r.getTop(), black, 1);
        gfx::line(r.getLeft(), r.getBottom(), r.getRight() - 1, r.getBottom(), black, 1);

        elem.font.draw(r.getCenterX() - 1, r.getTop() + 2, ALLEGRO_ALIGN_CENTER, black, elem.text);
        //gfx::roundRect(r.getLeft(), r.getTop(), r.getRight(), r.getBottom(), 1, 1, black, 1);
    }
    //GuiRect(Point<int>(0, 0), Bitmap::getTarget().getSize()).setClipping();
}

GuiElement* Gui::elemAt(int x, int y) {
    for (GuiElement& elem : elements) {
        if (elem.size.intersects(x, y)) {
            return &elem;
        }
    }
    return nullptr;
}

bool Gui::handleEvent(Event evt) {
    switch (evt.getType()) {
    case ALLEGRO_EVENT_MOUSE_AXES:
    case ALLEGRO_EVENT_MOUSE_WARPED:
        hover = elemAt(evt.getMouseX(), evt.getMouseY());
        break;

    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        focus = hover = elemAt(evt.getMouseX(), evt.getMouseY());
        break;

    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
        hover = elemAt(evt.getMouseX(), evt.getMouseY());
        if (hover == focus && focus && focus->func) {
            focus->func();
        }
        focus = nullptr;
        break;
    default:
        return false;
    }
    return true;
}
