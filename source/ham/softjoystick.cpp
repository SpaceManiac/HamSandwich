#include "softjoystick.h"
#include "control.h"
#include <algorithm>
#include <math.h>

#include <SDL_image.h>

#ifndef M_PI
	#define M_PI 3.1415926535897
#endif

static constexpr byte MAX_BUTTONS = 4;
static byte numButtons = 2;
static byte state = 0;
static byte taps = 0;

void SoftJoystickNumButtons(byte n) {
	numButtons = std::max((byte) 1, std::min(n, MAX_BUTTONS));
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
	if (tex)
		SDL_RenderCopy(renderer, tex.get(), NULL, &rect);
}

SoftJoystick::SoftJoystick(MGLDraw* mgl) {
	SDL_Renderer* renderer = mgl->renderer;
	stick.tex.reset(IMG_LoadTexture(renderer, "soft_stick.png"));
	trough.tex.reset(IMG_LoadTexture(renderer, "soft_trough.png"));
	esc.tex.reset(IMG_LoadTexture(renderer, "soft_esc.png"));
	keyboard.tex.reset(IMG_LoadTexture(renderer, "soft_keyboard.png"));
	button[0].tex.reset(IMG_LoadTexture(renderer, "soft_b1.png"));
	button[1].tex.reset(IMG_LoadTexture(renderer, "soft_b2.png"));
	button[2].tex.reset(IMG_LoadTexture(renderer, "soft_b3.png"));
	button[3].tex.reset(IMG_LoadTexture(renderer, "soft_b4.png"));
	mode[0].tex.reset(IMG_LoadTexture(renderer, "soft_mouse.png"));
	mode[1].tex.reset(IMG_LoadTexture(renderer, "soft_gamepad.png"));
}

void SoftJoystick::update(MGLDraw* mgl, float scale) {
	int spare = std::clamp(
		// Preferred: the entire width of the letterbox.
		(int)(mgl->winWidth - mgl->xRes * scale) / 2,
		// Minimum: buttons too small will be unclickable.
		// Also, forbid negative values if actually there is no letterbox.
		80,
		// Maximum: buttons too big will overlap vertically.
		mgl->winHeight / (2 + MAX_BUTTONS)
	);

	int right = mgl->winWidth - spare;
	trough.rect = { 0, 0, 3 * spare, 3 * spare };
	stick.rect = { 0, 0, 3 * spare, 3 * spare };
	if (state & CONTROL_UP) stick.rect.y -= spare;
	if (state & CONTROL_DN) stick.rect.y += spare;
	if (state & CONTROL_LF) stick.rect.x -= spare;
	if (state & CONTROL_RT) stick.rect.x += spare;

	esc.rect = { right, mgl->winHeight - spare, spare, spare };
	keyboard.rect = { right, mgl->winHeight - 2 * spare, spare, spare };
	for (int i = 0; i < numButtons; ++i) {
		button[i].rect = { right, i * spare, spare, spare };
	}

	mode[0].rect = mode[1].rect = { 0, mgl->winHeight - spare, spare, spare };
}

void SoftJoystick::render(SDL_Renderer* renderer) {
	if (enableStick) {
		stick.draw(renderer);
		trough.draw(renderer);
	}
	esc.draw(renderer);
	keyboard.draw(renderer);
	for (int i = 0; i < numButtons; ++i) {
		button[i].draw(renderer);
	}
	mode[enableStick].draw(renderer);
}

void SoftJoystick::handle_event(MGLDraw *mgl, const SDL_Event& e) {
	if (e.type == SDL_FINGERDOWN) {
		int x = (int)(e.tfinger.x * mgl->winWidth);
		int y = (int)(e.tfinger.y * mgl->winHeight);
		if (rect_contains(esc.rect, x, y)) {
			mgl->lastKeyPressed = SDLK_ESCAPE;
		} else if (rect_contains(keyboard.rect, x, y)) {
			SDL_StartTextInput();
		} else if (rect_contains(mode[0].rect, x, y)) {
			enableStick = !enableStick;
		}

		int bit = CONTROL_B1;
		for (int i = 0; i < numButtons; ++i) {
			if (rect_contains(button[i].rect, x, y)) {
				fingerHeld[e.tfinger.fingerId] |= bit;
				state |= bit;
				taps |= bit;
			}
			bit *= 2;
		}
	}

	if (enableStick && (e.type == SDL_FINGERDOWN || e.type == SDL_FINGERMOTION)) {
		int x = (int)(e.tfinger.x * mgl->winWidth);
		int y = (int)(e.tfinger.y * mgl->winHeight);

		if (rect_contains(trough.rect, x, y)) {
			const int DEADZONE = 2048;

			byte curState = 0;

			int ax = (x - trough.rect.x - trough.rect.w / 2) * 32767 / trough.rect.w;
			int ay = (y - trough.rect.y - trough.rect.h / 2) * 32767 / trough.rect.h;
			if (ax * ax + ay * ay >= DEADZONE * DEADZONE) {
				double angle = atan2(ay, ax) * 180.0 / M_PI;
				if (angle < 22.5 + -4*45) {
					curState = CONTROL_LF;
				} else if (angle < 22.5 + -3*45) {
					curState = CONTROL_LF | CONTROL_UP;
				} else if (angle < 22.5 + -2*45) {
					curState = CONTROL_UP;
				} else if (angle < 22.5 + -1*45) {
					curState = CONTROL_UP | CONTROL_RT;
				} else if (angle < 22.5 + 0*45) {
					curState = CONTROL_RT;
				} else if (angle < 22.5 + 1*45) {
					curState = CONTROL_RT | CONTROL_DN;
				} else if (angle < 22.5 + 2*45) {
					curState = CONTROL_DN;
				} else if (angle < 22.5 + 3*45) {
					curState = CONTROL_DN | CONTROL_LF;
				} else if (angle < 22.5 + 4*45) {
					curState = CONTROL_LF;
				}
			}

			byte last = fingerHeld[e.tfinger.fingerId];
			fingerHeld[e.tfinger.fingerId] = curState;
			recalculate_state();
			taps |= (curState & ~last);
		}
	}

	if (e.type == SDL_FINGERUP) {
		auto iter = fingerHeld.find(e.tfinger.fingerId);
		if (iter != fingerHeld.end()) {
			fingerHeld.erase(iter);
			recalculate_state();
		}
	}
}

void SoftJoystick::recalculate_state() {
	state = 0;
	for (const auto& pair : fingerHeld) {
		state |= pair.second;
	}
}
