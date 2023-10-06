#include "control.h"
#include <math.h>
#include <vector>
#include <algorithm>
#include "mgldraw.h"
#include "log.h"
#include "softjoystick.h"
#include "owned_sdl.h"

static const int NUM_KEYBOARDS = 4;
static const int NUM_CONTROLS = 8;
static const int NUM_JOYBTNS = 4;

static byte lastScanCode;

// normal mapped controls: kbd, joystick, etc.
static byte keyState, keyTap;
static byte keyState2[NUM_KEYBOARDS];
// fixed arrow keys and return controls
static byte arrowState, arrowTap;

// joysticks
static std::vector<owned::SDL_GameController> joysticks;
static byte oldJoy;

// mappings
static byte kb[NUM_CONTROLS][NUM_KEYBOARDS];
static byte joyBtn[NUM_JOYBTNS];
static byte playerUsesJoystick[NUM_KEYBOARDS] = { 3, 3, 3, 3 };

static byte GetJoyState();

void InitControls(void)
{
	lastScanCode=0;
	keyState=0;
	keyTap=0;
	arrowState=0;
	arrowTap=0;
	oldJoy=0;
	memset(keyState2, 0, sizeof(keyState2));

	/*
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (!SDL_IsGameController(i)) {
			LogDebug("Skipping non-gamepad joystick: %s", SDL_JoystickNameForIndex(i));
			continue;
		}

		owned::SDL_GameController joystick = owned::SDL_GameControllerOpen(i);
		if (joystick) {
			LogDebug("Gamepad init: %s", SDL_GameControllerName(joystick.get()));
			joysticks.push_back(std::move(joystick));
		}
		else
			LogError("SDL_GameControllerOpen(%d, %s): %s", i, SDL_GameControllerNameForIndex(i), SDL_GetError());
	}
	*/
}

byte GetKeyControls() {
	return keyState;
}

byte GetControls() {
	return keyState | GetJoyState() | SoftJoystickState();
}

byte GetPlayerControls(byte player)
{
	switch (playerUsesJoystick[player])
	{
		case 0:  // Keyboard1
			return keyState2[0] | SoftJoystickState();
		case 1:  // Keyboard2
			return keyState2[1];
		case 2:  // Joystick
			return GetJoyState();
		default:  // ???
			return GetControls();
	}
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

// Gamepad special
dword GetJoyButtons() {
	dword held = 0;

	for (owned::SDL_GameController& gamepad : joysticks) {
		SDL_Joystick* joystick = SDL_GameControllerGetJoystick(gamepad.get());
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

static_assert(SDL_CONTROLLER_BUTTON_MAX <= 8 * sizeof(dword));
dword GetGamepadButtons() {
	dword held = 0;

	for (owned::SDL_GameController& gamepad : joysticks) {
		for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; ++button) {
			if (SDL_GameControllerGetButton(gamepad.get(), static_cast<SDL_GameControllerButton>(button))) {
				held |= (1 << button);
			}
		}
	}

	return held;
}

void GetLeftStick(int16_t* x, int16_t* y, byte* dpad)
{
	*dpad = 0;
	int32_t xx = 0, yy = 0;
	for (auto iter = joysticks.begin(); iter != joysticks.end(); ++iter)
	{
		SDL_GameController* gamepad = iter->get();
		SDL_Joystick* joystick = SDL_GameControllerGetJoystick(gamepad);
		if (!SDL_JoystickGetAttached(joystick))
		{
			// Drop disconnected joysticks.
			LogDebug("Gamepad removed: %s", SDL_GameControllerName(gamepad));
			iter = joysticks.erase(iter);
			if (iter == joysticks.end())
				break;
			continue;
		}

		xx += SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX);
		yy += SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY);

		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
		{
			*dpad|=CONTROL_LF;
		}
		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
		{
			*dpad|=CONTROL_RT;
		}

		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP))
		{
			*dpad|=CONTROL_UP;
		}
		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
		{
			*dpad|=CONTROL_DN;
		}

		if(SDL_JoystickGetButton(joystick, joyBtn[0]))
		{
			*dpad|=CONTROL_B1;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[1]))
		{
			*dpad|=CONTROL_B2;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[2]))
		{
			*dpad|=CONTROL_B3;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[3]))
		{
			*dpad|=CONTROL_B4;
		}
	}
	*x = int16_t(std::clamp(xx, int32_t(INT16_MIN), int32_t(INT16_MAX)));
	*y = int16_t(std::clamp(yy, int32_t(INT16_MIN), int32_t(INT16_MAX)));
}

// Options menu support.
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

void ControlSetUseJoystick(byte player, byte joystickNumber) {
	playerUsesJoystick[player] = joystickNumber;
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
				keyState2[i] |= bit;
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
				keyState2[i] &= ~bit;
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

void ControlHandleNewGamepad(int which)
{
	owned::SDL_GameController gamepad = owned::SDL_GameControllerOpen(which);
	if (gamepad) {
		LogDebug("Gamepad added: %s", SDL_GameControllerName(gamepad.get()));
		joysticks.push_back(std::move(gamepad));
	}
}

static byte GetJoyState(void)
{
	constexpr int DEADZONE = 8192;
	byte joyState = 0;

	for (auto iter = joysticks.begin(); iter != joysticks.end(); ++iter)
	{
		SDL_GameController* gamepad = iter->get();
		SDL_Joystick* joystick = SDL_GameControllerGetJoystick(gamepad);
		if (!SDL_JoystickGetAttached(joystick))
		{
			// Drop disconnected joysticks.
			LogDebug("Gamepad removed: %s", SDL_GameControllerName(gamepad));
			iter = joysticks.erase(iter);
			if (iter == joysticks.end())
				break;
			continue;
		}

		int leftX = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX);
		int leftY = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY);
		if (leftX * leftX + leftY * leftY > DEADZONE * DEADZONE)
		{
			double angle = atan2(leftY, leftX) * 180.0 / M_PI;
			if (angle < 22.5 + -4*45) {
				joyState |= CONTROL_LF;
			} else if (angle < 22.5 + -3*45) {
				joyState |= CONTROL_LF | CONTROL_UP;
			} else if (angle < 22.5 + -2*45) {
				joyState |= CONTROL_UP;
			} else if (angle < 22.5 + -1*45) {
				joyState |= CONTROL_UP | CONTROL_RT;
			} else if (angle < 22.5 + 0*45) {
				joyState |= CONTROL_RT;
			} else if (angle < 22.5 + 1*45) {
				joyState |= CONTROL_RT | CONTROL_DN;
			} else if (angle < 22.5 + 2*45) {
				joyState |= CONTROL_DN;
			} else if (angle < 22.5 + 3*45) {
				joyState |= CONTROL_DN | CONTROL_LF;
			} else if (angle < 22.5 + 4*45) {
				joyState |= CONTROL_LF;
			}
		}

		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
		{
			joyState|=CONTROL_LF;
		}
		else if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
		{
			joyState|=CONTROL_RT;
		}

		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP))
		{
			joyState|=CONTROL_UP;
		}
		else if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
		{
			joyState|=CONTROL_DN;
		}

		if(SDL_JoystickGetButton(joystick, joyBtn[0]))
		{
			joyState|=CONTROL_B1;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[1]))
		{
			joyState|=CONTROL_B2;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[2]))
		{
			joyState|=CONTROL_B3;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[3]))
		{
			joyState|=CONTROL_B4;
		}
	}

	keyTap |= joyState & ~oldJoy;
	oldJoy = joyState;
	return joyState;
}
