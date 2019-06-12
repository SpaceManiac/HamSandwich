#include "softjoystick.h"
#include "control.h"
#include <algorithm>

#ifdef SDL_UNPREFIXED
	#include <SDL_image.h>
#else  // SDL_UNPREFIXED
	#include <SDL2/SDL_image.h>
#endif

static byte numButtons = 2;
static byte state = 0;
static byte taps = 0;

void SoftJoystickNumButtons(byte n) {
	numButtons = std::max((byte) 1, std::min(n, (byte) 4));
}

byte SoftJoystickState() {
	return state;
}

byte SoftJoystickTaps() {
	byte w = taps;
	taps = 0;
	return w;
}

static inline bool rect_contains(const SDL_Rect &r, int x, int y) {
    return r.x <= x && x < r.x + r.w && r.y <= y && y < r.y + r.h;
}

void Element::draw(SDL_Renderer* renderer) {
	SDL_RenderCopy(renderer, tex, NULL, &rect);
}

SoftJoystick::SoftJoystick(MGLDraw* mgl) {
	SDL_Renderer* renderer = mgl->renderer;
	stick.tex = IMG_LoadTexture(renderer, "soft_stick.png");
	trough.tex = IMG_LoadTexture(renderer, "soft_trough.png");
	esc.tex = IMG_LoadTexture(renderer, "soft_esc.png");
	keyboard.tex = IMG_LoadTexture(renderer, "soft_keyboard.png");
	button[0].tex = IMG_LoadTexture(renderer, "soft_b1.png");
	button[1].tex = IMG_LoadTexture(renderer, "soft_b2.png");
	button[2].tex = IMG_LoadTexture(renderer, "soft_b3.png");
	button[3].tex = IMG_LoadTexture(renderer, "soft_b4.png");
}

SoftJoystick::~SoftJoystick() {
	SDL_DestroyTexture(stick.tex);
	SDL_DestroyTexture(trough.tex);
	SDL_DestroyTexture(esc.tex);
	SDL_DestroyTexture(keyboard.tex);
	SDL_DestroyTexture(button[0].tex);
	SDL_DestroyTexture(button[1].tex);
	SDL_DestroyTexture(button[2].tex);
	SDL_DestroyTexture(button[3].tex);
}

void SoftJoystick::update(MGLDraw* mgl, float scale) {
	int spare = (int)(mgl->winWidth - mgl->xRes * scale) / 2;
	int right = mgl->winWidth - spare;
	trough.rect = { 0, mgl->winHeight - 3 * spare, 3 * spare, 3 * spare };
	stick.rect = { 0, mgl->winHeight - 3 * spare, 3 * spare, 3 * spare };
	if (state & CONTROL_UP) stick.rect.y -= spare;
	if (state & CONTROL_DN) stick.rect.y += spare;
	if (state & CONTROL_LF) stick.rect.x -= spare;
	if (state & CONTROL_RT) stick.rect.x += spare;

	esc.rect = { right, 0, spare, spare };
	keyboard.rect = { right, spare, spare, spare };
	for (int i = 0; i < numButtons; ++i) {
		button[i].rect = { right, mgl->winHeight - (i + 1) * spare, spare, spare };
	}
}

void SoftJoystick::render(SDL_Renderer* renderer) {
	stick.draw(renderer);
	trough.draw(renderer);
	esc.draw(renderer);
	keyboard.draw(renderer);
	for (int i = 0; i < numButtons; ++i) {
		button[i].draw(renderer);
	}
}

void SoftJoystick::handle_event(MGLDraw *mgl, const SDL_Event& e) {
	switch (e.type) {
		case SDL_FINGERDOWN: {
			int x = (int)(e.tfinger.x * mgl->winWidth);
			int y = (int)(e.tfinger.y * mgl->winHeight);
			if (rect_contains(esc.rect, x, y)) {
				mgl->lastKeyPressed = SDLK_ESCAPE;
			} else if (rect_contains(keyboard.rect, x, y)) {
				SDL_StartTextInput();
			}

			int bit = CONTROL_B1;
			for (int i = 0; i < numButtons; ++i) {
				if (rect_contains(button[i].rect, x, y)) {
					state |= bit;
					taps |= bit;
				}
				bit *= 2;
			}
			break;
		}

		case SDL_FINGERUP: {
			int x = (int)(e.tfinger.x * mgl->winWidth);
			int y = (int)(e.tfinger.y * mgl->winHeight);

			int bit = CONTROL_B1;
			for (int i = 0; i < numButtons; ++i) {
				if (rect_contains(button[i].rect, x, y)) {
					state &= ~bit;
				}
				bit *= 2;
			}
			break;
		}

		case SDL_FINGERMOTION: {
			int x = (int)(e.tfinger.x * mgl->winWidth);
			int y = (int)(e.tfinger.y * mgl->winHeight);

			byte last = state & (CONTROL_UP | CONTROL_DN | CONTROL_LF | CONTROL_RT);
			state &= ~last;

			if (rect_contains(trough.rect, x, y)) {
				const int DEADZONE = 4096;

				int ax = (x - trough.rect.x - trough.rect.w / 2) * 32767 / trough.rect.w;
				int ay = (y - trough.rect.y - trough.rect.h / 2) * 32767 / trough.rect.h;
				if (ay < -DEADZONE) state |= CONTROL_UP;
				else if (ay > DEADZONE) state |= CONTROL_DN;
				if (ax < -DEADZONE) state |= CONTROL_LF;
				else if (ax > DEADZONE) state |= CONTROL_RT;
			}
			taps |= (state & ~last);
			break;
		}
	}
}
