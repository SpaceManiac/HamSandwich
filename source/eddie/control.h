#ifndef CONTROL_H
#define CONTROL_H

#include "jamultypes.h"

#define CONTROL_UP 1
#define CONTROL_DN 2
#define CONTROL_LF 4
#define CONTROL_RT 8
#define CONTROL_B1 16
#define CONTROL_B2 32
#define CONTROL_B3 64
#define CONTROL_B4 128

class MGLDraw;

void InitControls(MGLDraw *mgl);
void ControlKeyUp(char k);
void ControlKeyDown(char k);
byte GetControls(void);
byte GetTaps(void);

byte LastKeyPressed(void);
byte MouseClick(void);
byte MouseHeld(void);
byte RMouseClick(void);
byte RMouseHeld(void);

#endif