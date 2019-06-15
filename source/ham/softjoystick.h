#ifndef SOFTJOYSTICK_H
#define SOFTJOYSTICK_H

#include "jamultypes.h"
#include "mgldraw.h"
#include <map>

void SoftJoystickNumButtons(byte n);
byte SoftJoystickState();
byte SoftJoystickTaps();

struct Element {
	SDL_Rect rect;
	SDL_Texture* tex;
	void draw(SDL_Renderer* renderer);
};

class SoftJoystick {
	Element stick;
	Element trough;
	Element esc;
	Element keyboard;
	Element button[4];

	std::map<SDL_FingerID, byte> fingerHeld;

	void recalculate_state();

public:
	SoftJoystick(MGLDraw* mgl);
	~SoftJoystick();

	void update(MGLDraw *mgl, float scale);
	void render(SDL_Renderer* renderer);
	void handle_event(MGLDraw *mgl, const SDL_Event& e);
};

#endif  // SOFTJOYSTICK_H
