#include "gui.h"
#include <string>
#include <SDL3/SDL.h>

Gui::Gui()
    : mx(0)
    , my(0)
    , justClicked(false)
{
}

bool Gui::handleEvent(const SDL_Event &evt) {
    switch (evt.type) {
    case SDL_EVENT_MOUSE_MOTION:
        mx = evt.motion.x;
        my = evt.motion.y;
        return false;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        mx = evt.button.x;
        my = evt.button.y;
        if (evt.button.button == 1) {
            justClicked = true;
        }
        return false;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        mx = evt.button.x;
        my = evt.button.y;
        return false;

    case SDL_EVENT_KEY_DOWN: {
        int mod = SDL_GetModState(), mod2 = 0;
        // Expand L/R hotkeys to match
        if (mod & SDL_KMOD_SHIFT)
            mod2 |= SDL_KMOD_SHIFT;
        if (mod & SDL_KMOD_CTRL)
            mod2 |= SDL_KMOD_CTRL;
        if (mod & SDL_KMOD_ALT)
            mod2 |= SDL_KMOD_ALT;

        justTyped = { mod2, evt.key.scancode };
        return false;
    }

    default:
        return false;
    }
}

bool Gui::element(SDL_Rect r, const std::string &text, const std::string &desc, KbdShortcut shortcut) {
    SDL_Color black { 0, 0, 0, 255 };

    bool hovered = rect_contains(r, mx, my);
    bool focused = hovered && justClicked;

	SDL_FRect fr = { (float)r.x, (float)r.y, (float)r.w, (float)r.h };

    if (focused) {
        SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
        SDL_RenderFillRect(renderer, &fr);
    } else if (hovered || isHighlight) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &fr);
        isHighlight = false;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderLine(renderer, r.x - 1, r.y, r.x - 1, r.y + r.h - 1);
    SDL_RenderLine(renderer, r.x + r.w, r.y, r.x + r.w, r.y + r.h - 1);
    SDL_RenderLine(renderer, r.x, r.y - 1, r.x + r.w - 1, r.y - 1);
    SDL_RenderLine(renderer, r.x, r.y + r.h, r.x + r.w - 1, r.y + r.h);

    DrawText(renderer, r.x + r.w / 2, r.y + 2, ALIGN_CENTER, black, text);

    if (hovered && !desc.empty()) {
        tooltip = desc;
    }

    return (hovered && justClicked) || hotkey(shortcut);
}

void Gui::render() {
    DrawText(renderer, 2, DISPLAY_HEIGHT - 20, ALIGN_LEFT, { 128, 128, 128, 255 }, "JspEdit 3 by SpaceManiac");

    if (!tooltip.empty()) {
		int width = FontStrLen(tooltip, gFont);
		int height = gFont->height + 3;

        SDL_FRect rect = { (float)(mx + 12), (float)my, (float)(width + 8), (float)(height + 8) };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderRect(renderer, &rect);

		DrawText(renderer, mx + 18, my + 4, ALIGN_LEFT, { 0, 0, 0, 255 }, tooltip);

        tooltip.clear();
    }

    justClicked = false;
    justTyped = { 0, 0 };
}

bool Gui::button(SDL_Rect rect, const std::string &text, const std::string &desc, KbdShortcut shortcut) {
    return element(rect, text, desc, shortcut);
}

bool Gui::iconButton(int x, int y, char ch, const std::string &desc, KbdShortcut shortcut) {
    return element({ x, y, 20, 20 }, { ch }, desc, shortcut);
}

bool Gui::hotkey(KbdShortcut shortcut) {
    return (shortcut != KbdShortcut { 0, 0 } && justTyped == shortcut);
}
