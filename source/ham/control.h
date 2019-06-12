#ifndef HAMCONTROL_H
#define HAMCONTROL_H

#include "jamultypes.h"

// The various control flags.
enum {
	// Directional states.
	CONTROL_UP = 1,
	CONTROL_DN = 2,
	CONTROL_LF = 4,
	CONTROL_RT = 8,
	// Called B1-4, but actually whatever button is bound to that control.
	CONTROL_B1 = 16,
	CONTROL_B2 = 32,
	CONTROL_B3 = 64,
	CONTROL_B4 = 128,
};

void InitControls();

// Get controls which are currently held. Union of all keyboards and joysticks.
byte GetControls();
byte GetTaps();  // Tapped since last GetTaps().

// Get arrow keys (non-mappable) which are currently held. Return=B1.
byte GetArrows();
byte GetArrowTaps();  // Tapped since last GetArrowTaps().

byte LastScanCode();
const char *ScanCodeText(byte s);

// Options menu support.
dword GetJoyButtons();
void SetKeyboardBindings(int keyboard, int nkeys, const byte* keys);
void SetJoystickBindings(int nbuttons, const byte* buttons);

// TODO: eliminate this, it's only used in text menus where capital letters
// are already being generated.
byte ShiftState();

#endif  // HAMCONTROL_H
