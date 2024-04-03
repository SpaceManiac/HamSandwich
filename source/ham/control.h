#ifndef HAMCONTROL_H
#define HAMCONTROL_H

#include "jamultypes.h"

// The various control flags.
enum {
	// Directional states.
	CONTROL_UP = (1 << 0),
	CONTROL_DN = (1 << 1),
	CONTROL_LF = (1 << 2),
	CONTROL_RT = (1 << 3),
	// Called B1-4, but actually whatever button is bound to that control.
	CONTROL_B1 = (1 << 4),
	CONTROL_B2 = (1 << 5),
	CONTROL_B3 = (1 << 6),
	CONTROL_B4 = (1 << 7),
};

void InitControls();

// Get controls which are currently held. Union of all keyboards and joysticks.
byte GetKeyControls();
byte GetControls();
byte GetPlayerControls(byte player);
byte GetTaps();  // Tapped since last GetTaps().

// Get arrow keys (non-mappable) which are currently held. Return=B1.
byte GetArrows();
byte GetArrowTaps();  // Tapped since last GetArrowTaps().

byte LastScanCode();
const char *ScanCodeText(byte s);

dword GetJoyButtons();  // Joystick-specific indexes.
dword GetGamepadButtons();  // x & (1 << SDL_GAMECONTROLLER_BUTTON_*)
void GetLeftStick(int16_t* x, int16_t* y, byte* dpad);

// Options menu support.
void SetKeyboardBindings(int keyboard, int nkeys, const byte* keys);
void SetJoystickBindings(int nbuttons, const byte* buttons);
void ControlSetUseJoystick(byte player, byte joystickNumber);

#endif  // HAMCONTROL_H
