#include "gui.h"
#include <SDL2/SDL.h>

void Gui::render() {
    SDL_Color black { 0, 0, 0, 255 };
    for (GuiElement& elem : elements) {
        GuiRect r = elem.size;

        bool hovered = (hover == &elem);
        bool focused = (focus == &elem);

        if (focused) {
            SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
            SDL_Rect rect = {r.left, r.top, r.right - r.left, r.bottom - r.top};
            SDL_RenderFillRect(renderer, &rect);
        } else if (hovered) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_Rect rect = {r.left, r.top, r.right - r.left, r.bottom - r.top};
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLine(renderer, r.left, r.top, r.left, r.bottom);
        SDL_RenderDrawLine(renderer, r.right, r.top, r.right, r.bottom);
        SDL_RenderDrawLine(renderer, r.left, r.top, r.right, r.top);
        SDL_RenderDrawLine(renderer, r.left, r.bottom, r.right, r.bottom);

        DrawText(renderer, elem.font, (r.left + r.right) / 2, r.top + 2, ALIGN_CENTER, black, elem.text.c_str());
    }

    if (hover && !hover->tooltip.empty()) {
        DrawText(renderer, gFont, 2, DISPLAY_HEIGHT - 20, ALIGN_LEFT, black, hover->tooltip.c_str());
    } else {
        DrawText(renderer, gFont, 2, DISPLAY_HEIGHT - 20, ALIGN_LEFT, { 128, 128, 128, 255 }, "JspEdit 3 by SpaceManiac");
    }
}

GuiElement* Gui::elemAt(int x, int y) {
    for (GuiElement& elem : elements) {
        if (elem.size.left <= x && x <= elem.size.right && elem.size.top <= y && y <= elem.size.bottom) {
            return &elem;
        }
    }
    return nullptr;
}

bool Gui::handleEvent(const SDL_Event &evt) {
    GuiElement* prev = focus;

    switch (evt.type) {
    case SDL_MOUSEMOTION:
        hover = elemAt(evt.motion.x, evt.motion.y);
        return false;

    case SDL_MOUSEBUTTONDOWN:
        focus = hover = elemAt(evt.button.x, evt.button.y);
        return focus;

    case SDL_MOUSEBUTTONUP:
        hover = elemAt(evt.button.x, evt.button.y);
        if (hover == focus && focus && focus->func) {
            focus->func();
        }
        focus = nullptr;
        return prev;

    case SDL_KEYUP:
        for (GuiElement elem : elements) {
            int mod = SDL_GetModState() & 0xff;
            if (elem.shortcut.mods == mod && elem.shortcut.keycode == evt.key.keysym.scancode && elem.func) {
                elem.func();
                return true;
            }
        }
        return false;

    default:
        return false;
    }
}
