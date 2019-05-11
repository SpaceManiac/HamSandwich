#ifndef CONTROL_H
#define CONTROL_H

#include "jamultypes.h"

// the various control flags
enum {
    CONTROL_UP = 1,
    CONTROL_DN = 2,
    CONTROL_LF = 4,
    CONTROL_RT = 8,
    CONTROL_B1 = 16,
    CONTROL_B2 = 32,
    CONTROL_B3 = 64,
    CONTROL_B4 = 128
};

// source of control
enum {
    CONTROL_KB1 = 0,
    CONTROL_KB2,
    CONTROL_JOY
};

void InitControls(void);
void ApplyControlSettings(void);

void ControlKeyUp(char k);
void ControlKeyDown(char k);
byte GetControls(void);
byte GetTaps(void);
byte GetArrows(void);
dword GetJoyButtons(void);

byte LastScanCode(void);

byte JoystickAvailable(void);
void SetKeys(byte keys[8]);

char *ScanCodeText(byte s);

#endif
