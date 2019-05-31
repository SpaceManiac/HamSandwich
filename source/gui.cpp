#include "gui.h"
#include <SDL2/SDL.h>

Gui::Gui()
    : mx(0)
    , my(0)
    , justClicked(false)
{
}

bool Gui::handleEvent(const SDL_Event &evt) {
    switch (evt.type) {
    case SDL_MOUSEMOTION:
        mx = evt.motion.x;
        my = evt.motion.y;
        return false;

    case SDL_MOUSEBUTTONDOWN:
        mx = evt.button.x;
        my = evt.button.y;
        if (evt.button.button == 1) {
            justClicked = true;
        }
        return false;

    case SDL_MOUSEBUTTONUP:
        mx = evt.button.x;
        my = evt.button.y;
        return false;

    case SDL_KEYDOWN:
        justTyped = { SDL_GetModState() & 0xff, evt.key.keysym.scancode };
        return false;

    default:
        return false;
    }
}

bool Gui::element(GuiRect r, TTF_Font *font, const std::string &text, const std::string &desc, KbdShortcut shortcut) {
    if (!font)
        font = gFont;

    SDL_Color black { 0, 0, 0, 255 };

    bool hovered = r.left <= mx && mx <= r.right && r.top <= my && my <= r.bottom;
    bool focused = hovered && justClicked;

    if (focused) {
        printf("focused on %s\n", text.c_str());
    }

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

    DrawText(renderer, font, (r.left + r.right) / 2, r.top + 2, ALIGN_CENTER, black, text.c_str());

    if (hovered && !desc.empty()) {
        tooltip = desc;
    }

    return (hovered && justClicked) || (justTyped == shortcut);
}

void Gui::render() {
    DrawText(renderer, gFont, 2, DISPLAY_HEIGHT - 20, ALIGN_LEFT, { 128, 128, 128, 255 }, "JspEdit 3 by SpaceManiac");

    if (!tooltip.empty()) {
        SDL_Surface *surface = TTF_RenderUTF8_Blended(gFont, tooltip.c_str(), { 0, 0, 0, 255 });
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

        tooltip.clear();
    }

    justClicked = false;
    justTyped = { 0, 0 };
}

bool Gui::button(GuiRect rect, const std::string &text, const std::string &desc, KbdShortcut shortcut) {
    return element(rect, gFont, text, desc, shortcut);
}

bool Gui::iconButton(int x, int y, FAChar ch, const std::string &desc, KbdShortcut shortcut) {
    return element({ x, y, x + 20, y + 20 }, gIconFont, FA::str(ch), desc, shortcut);
}
