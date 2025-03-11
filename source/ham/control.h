#ifndef HAMCONTROL_H
#define HAMCONTROL_H

#include "jamultypes.h"

#define NORMAL_CONTROL_SCHEME (0)
#define MYSTIC_CONTROL_SCHEME (1)

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
	CONTROL_ESCAPE = (1 << 8),
	CONTROL_QC_1 = (1 << 9),	// quick-cast buttons
	CONTROL_QC_2 = (1 << 10),
	CONTROL_QC_3 = (1 << 11),
	CONTROL_QC_4 = (1 << 12),
	CONTROL_QC_5 = (1 << 13),
	CONTROL_QC_6 = (1 << 14),
	CONTROL_QC_7 = (1 << 15),
	CONTROL_QC_8 = (1 << 16),
	CONTROL_QC_9 = (1 << 17),
	CONTROL_QC_0 = (1 << 18),
	CONTROL_LAST = CONTROL_QC_0,
};

enum {
	CTL_ID_UP = 0,
	CTL_ID_DN,
	CTL_ID_LF,
	CTL_ID_RT,
	CTL_ID_B1,
	CTL_ID_B2,
	CTL_ID_B3,
	CTL_ID_B4,
	CTL_ID_ESCAPE,
	CTL_ID_QC_1,
	CTL_ID_QC_2,
	CTL_ID_QC_3,
	CTL_ID_QC_4,
	CTL_ID_QC_5,
	CTL_ID_QC_6,
	CTL_ID_QC_7,
	CTL_ID_QC_8,
	CTL_ID_QC_9,
	CTL_ID_QC_0,
	NUM_CONTROLS,
};

#define CONTROL_REPEAT_FRAMES (10)	// how many frames between auto-repeats in menus

extern bool dpadToMove;
extern byte stickDeadZone;

enum RawGamepadAxis :byte
{
	LS_UP,
	LS_DN,
	LS_LF,
	LS_RT,
	RS_UP,
	RS_DN,
	RS_LF,
	RS_RT,
	LT,
	RT,
};
#define RAWGAMEPADAXIS_BASE (22)

void InitControls(byte scheme=NORMAL_CONTROL_SCHEME);

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
bool ShowGamepadText();

// Options menu support.
void SetKeyboardBindings(int keyboard, int nkeys, const byte* keys);
void SetJoystickBindings(int nbuttons, const byte* buttons);
void ControlSetUseJoystick(byte player, byte joystickNumber);

typedef struct _SDL_GameController SDL_GameController;
SDL_GameController* ActiveController();

// mystic control mode --------------
//dword GetMysticControls(void);
//dword GetMysticTaps(void);
//dword AutoRepeatControls(void);
bool AutoRepeatTapped(dword c,bool menu=true);
bool ButtonTapped(dword c,bool menu);
bool ButtonHeld(dword c,bool menu);
dword RawGamepadToControls(void);
dword GetRawGamepad(void);
void UpdateControls(void);
void UpdateRawJoystick(void);
void LockOutControl(dword c,bool locked);
byte ControlScheme(void);
void SetDpadToMode(bool yep);

#endif  // HAMCONTROL_H
