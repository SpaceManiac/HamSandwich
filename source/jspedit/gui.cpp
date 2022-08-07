#include "gui.h"
#include <SDL.h>

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

    case SDL_KEYDOWN: {
        int mod = SDL_GetModState(), mod2 = 0;
        // Expand L/R hotkeys to match
        if (mod & KMOD_SHIFT)
            mod2 |= KMOD_SHIFT;
        if (mod & KMOD_CTRL)
            mod2 |= KMOD_CTRL;
        if (mod & KMOD_ALT)
            mod2 |= KMOD_ALT;

        justTyped = { mod2, evt.key.keysym.scancode };
        return false;
    }

    default:
        return false;
    }
}

bool Gui::element(SDL_Rect r, TTF_Font *font, const std::string &text, const std::string &desc, KbdShortcut shortcut) {
    if (!font)
        font = gFont;

    SDL_Color black { 0, 0, 0, 255 };

    bool hovered = rect_contains(r, mx, my);
    bool focused = hovered && justClicked;

    if (focused) {
        SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
        SDL_RenderFillRect(renderer, &r);
    } else if (hovered || isHighlight) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &r);
        isHighlight = false;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, r.x - 1, r.y, r.x - 1, r.y + r.h - 1);
    SDL_RenderDrawLine(renderer, r.x + r.w, r.y, r.x + r.w, r.y + r.h - 1);
    SDL_RenderDrawLine(renderer, r.x, r.y - 1, r.x + r.w - 1, r.y - 1);
    SDL_RenderDrawLine(renderer, r.x, r.y + r.h, r.x + r.w - 1, r.y + r.h);

    DrawText(renderer, font, r.x + r.w / 2, r.y + 2, ALIGN_CENTER, black, text.c_str());

    if (hovered && !desc.empty()) {
        tooltip = desc;
    }

    return (hovered && justClicked) || hotkey(shortcut);
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

bool Gui::button(SDL_Rect rect, const std::string &text, const std::string &desc, KbdShortcut shortcut) {
    return element(rect, gFont, text, desc, shortcut);
}

bool Gui::iconButton(int x, int y, FAChar ch, const std::string &desc, KbdShortcut shortcut) {
    return element({ x, y, 20, 20 }, gIconFont, FA::str(ch), desc, shortcut);
}

bool Gui::hotkey(KbdShortcut shortcut) {
    return (shortcut != KbdShortcut { 0, 0 } && justTyped == shortcut);
}
