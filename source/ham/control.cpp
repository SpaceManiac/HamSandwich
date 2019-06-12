#include "control.h"
#include "mgldraw.h"
#include "log.h"
#include "softjoystick.h"
#include <vector>
#include <algorithm>

static byte lastScanCode;

// normal mapped controls: kbd, joystick, etc.
static byte keyState, keyTap;
// fixed arrow keys and return controls
static byte arrowState, arrowTap;

// joysticks
static std::vector<SDL_Joystick*> joysticks;
static byte oldJoy;

// mappings
static const int NUM_KEYBOARDS = 4;
static const int NUM_CONTROLS = 8;
static const int NUM_JOYBTNS = 4;

static byte kb[NUM_CONTROLS][NUM_KEYBOARDS];
static byte joyBtn[NUM_JOYBTNS];

static byte GetJoyState();

void InitControls(void)
{
	lastScanCode=0;
	keyState=0;
	keyTap=0;
	arrowState=0;
	arrowTap=0;
	oldJoy=0;

	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		SDL_Joystick* joystick = SDL_JoystickOpen(i);
		if (joystick)
			joysticks.push_back(joystick);
		else
			LogError("JoystickOpen(%d, %s): %s", i, SDL_JoystickNameForIndex(i), SDL_GetError());
	}
}

byte GetControls() {
	return keyState | GetJoyState() | SoftJoystickState();
}

byte GetTaps() {
	GetJoyState();  // Updates keyTap.
	byte result = keyTap;
	keyTap = 0;
	return result | SoftJoystickTaps();
}

byte GetArrows() {
	return arrowState;
}

byte GetArrowTaps() {
	byte result = arrowTap;
	arrowTap = 0;
	return result;
}

byte LastScanCode() {
	byte c = lastScanCode;
	lastScanCode = 0;
	return c;
}

const char *ScanCodeText(byte s) {
	return SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode) s));
}

// Options menu support.
dword GetJoyButtons() {
	dword held = 0;

	for (SDL_Joystick* joystick : joysticks) {
		int n = SDL_JoystickNumButtons(joystick);
		int j = 1;
		for (int i = 0; i < n && i < 32; ++i) {
			if (SDL_JoystickGetButton(joystick, i)) {
				held |= j;
			}
			j *= 2;
		}
	}

	return held;
}

void SetKeyboardBindings(int keyboard, int nkeys, const byte* keys) {
	nkeys = std::min(nkeys, NUM_CONTROLS);
	for (int i = 0; i < nkeys; ++i) {
		kb[i][keyboard] = keys[i];
	}
}

void SetJoystickBindings(int nbuttons, const byte* buttons) {
	SoftJoystickNumButtons(nbuttons);
	memcpy(joyBtn, buttons, std::min(nbuttons, NUM_JOYBTNS));
}

// Called upon SDL events
void ControlKeyDown(byte k)
{
	int i,j;
	byte bit;

	lastScanCode=k;

	for(i=0;i<NUM_KEYBOARDS;i++)
	{
		bit=1;
		for(j=0;j<NUM_CONTROLS;j++)
		{
			if(k==kb[j][i])
			{
				keyState|=bit;
				keyTap|=bit;
			}
			bit*=2;
		}
	}

	// always track arrows, no matter what the keys are, for menus
	switch(k) {
		case SDL_SCANCODE_UP:
			arrowState |= CONTROL_UP;
			arrowTap |= CONTROL_UP;
			break;
		case SDL_SCANCODE_DOWN:
			arrowState |= CONTROL_DN;
			arrowTap |= CONTROL_DN;
			break;
		case SDL_SCANCODE_LEFT:
			arrowState |= CONTROL_LF;
			arrowTap |= CONTROL_LF;
			break;
		case SDL_SCANCODE_RIGHT:
			arrowState |= CONTROL_RT;
			arrowTap |= CONTROL_RT;
			break;
		case SDL_SCANCODE_RETURN:
			arrowState |= CONTROL_B1;
			arrowTap |= CONTROL_B1;
			break;
	}
}

void ControlKeyUp(byte k)
{
	int i,j;
	byte bit;

	for(i=0;i<NUM_KEYBOARDS;i++)
	{
		bit=1;
		for(j=0;j<NUM_CONTROLS;j++)
		{
			if(k==kb[j][i])
			{
				keyState&=(~bit);
			}
			bit*=2;
		}
	}

	// always track arrows, no matter what the keys are, for menus
	switch(k) {
		case SDL_SCANCODE_UP:
			arrowState &= ~CONTROL_UP;
			break;
		case SDL_SCANCODE_DOWN:
			arrowState &= ~CONTROL_DN;
			break;
		case SDL_SCANCODE_LEFT:
			arrowState &= ~CONTROL_LF;
			break;
		case SDL_SCANCODE_RIGHT:
			arrowState &= ~CONTROL_RT;
			break;
		case SDL_SCANCODE_RETURN:
			arrowState &= ~CONTROL_B1;
			break;
	}
}

static byte GetJoyState(void)
{
	const int DEADZONE = 8192;
	byte joyState = 0;

	for (auto iter = joysticks.begin(); iter != joysticks.end(); ++iter)
	{
		SDL_Joystick* joystick = *iter;
		if (!SDL_JoystickGetAttached(joystick))
		{
			// Drop disconnected joysticks.
			LogDebug("Joystick removed: %s", SDL_JoystickName(joystick));
			iter = joysticks.erase(iter);
			if (iter == joysticks.end())
				break;
			continue;
		}

		byte hat = SDL_HAT_CENTERED;
		if (SDL_JoystickNumHats(joystick) > 0)
			hat = SDL_JoystickGetHat(joystick, 0);

		if(SDL_JoystickGetAxis(joystick, 0) < -DEADZONE || (hat & SDL_HAT_LEFT))
		{
			if(!(oldJoy&CONTROL_LF))
				keyTap|=CONTROL_LF;
			joyState|=CONTROL_LF;
		}
		else if(SDL_JoystickGetAxis(joystick, 0) > DEADZONE || (hat & SDL_HAT_RIGHT))
		{
			if(!(oldJoy&CONTROL_RT))
				keyTap|=CONTROL_RT;
			joyState|=CONTROL_RT;
		}
		if(SDL_JoystickGetAxis(joystick, 1) < -DEADZONE || (hat & SDL_HAT_UP))
		{
			if(!(oldJoy&CONTROL_UP))
				keyTap|=CONTROL_UP;
			joyState|=CONTROL_UP;
		}
		else if(SDL_JoystickGetAxis(joystick, 1) > DEADZONE || (hat & SDL_HAT_DOWN))
		{
			if(!(oldJoy&CONTROL_DN))
				keyTap|=CONTROL_DN;
			joyState|=CONTROL_DN;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[0]))
		{
			if(!(oldJoy&CONTROL_B1))
				keyTap|=CONTROL_B1;
			joyState|=CONTROL_B1;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[1]))
		{
			if(!(oldJoy&CONTROL_B2))
				keyTap|=CONTROL_B2;
			joyState|=CONTROL_B2;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[2]))
		{
			if(!(oldJoy&CONTROL_B3))
				keyTap|=CONTROL_B3;
			joyState|=CONTROL_B3;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[3]))
		{
			if(!(oldJoy&CONTROL_B4))
				keyTap|=CONTROL_B4;
			joyState|=CONTROL_B4;
		}
	}

	// Add newly connected joysticks.
	for (int i = joysticks.size(); i < SDL_NumJoysticks(); ++i) {
		SDL_Joystick* joystick = SDL_JoystickOpen(i);
		if (joystick) {
			LogDebug("Joystick added: %s", SDL_JoystickName(joystick));
			joysticks.push_back(joystick);
		}
	}

	oldJoy=joyState;

	return joyState;
}
