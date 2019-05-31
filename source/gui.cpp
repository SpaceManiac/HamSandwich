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
        SDL_RenderDrawLine(renderer, r.left - 1, r.top, r.left - 1, r.bottom - 1);
        SDL_RenderDrawLine(renderer, r.right, r.top, r.right, r.bottom - 1);
        SDL_RenderDrawLine(renderer, r.left, r.top - 1, r.right - 1, r.top - 1);
        SDL_RenderDrawLine(renderer, r.left, r.bottom, r.right - 1, r.bottom);

        DrawText(renderer, elem.font, (r.left + r.right) / 2, r.top + 2, ALIGN_CENTER, black, elem.text.c_str());
    }

    if (hover && !hover->tooltip.empty()) {
        SDL_Surface *surface = TTF_RenderUTF8_Blended(gFont, hover->tooltip.c_str(), black);
        if (!surface) {
            return;
        }
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect rect = { mx + 12, my, surface->w + 8, surface->h + 8 };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        rect = { mx + 16, my + 4, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
    DrawText(renderer, gFont, 2, DISPLAY_HEIGHT - 20, ALIGN_LEFT, { 128, 128, 128, 255 }, "JspEdit 3 by SpaceManiac");
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
        mx = evt.motion.x;
        my = evt.motion.y;
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

    case SDL_KEYDOWN:
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
