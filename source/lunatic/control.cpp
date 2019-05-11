#include "control.h"
#include "mgldraw.h"
#include "options.h"

// text strings corresponding to scan codes 0-88
char scanCodeTable[128][16] = {
	// 0
	"Null", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
	// 16
	"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3", "4",
	// 32
	"5", "6", "7", "8", "9", "Numpad0", "Numpad1", "Numpad2", "Numpad3", "Numpad4", "Numpad5", "Numpad6", "Numpad7", "Numpad8", "Numpad9", "F1",
	// 48
	"F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "Escape", "~", "-", "=", "Backspace",
	// 64
	"Tab", "[", "]", "Enter", ":", "\"", "\\", "\\ 2", ",", "Stop", "/", "Space", "Insert", "Delete", "Home", "End",
	// 80
	"PageUp", "PageDown", "Left", "Right", "Up", "Down", "Numpad/", "Numpad*", "Numpad-", "Numpad+", "NumpadDel", "NumpadEnter", "PrintScreen", "Pause", "ABNT-C1", "Yen",
	// 96
	"Kana", "Convert", "NoConvert", "@", "Circumflex", ": 2", "Kanji", "Numpad=", "`", ";", "Command", "Unknown1", "Unknown2", "Unknown3", "Unknown4", "Unknown5",
	// 112
	"Unknown6", "Unknown7", "Unknown8", "LShift", "RShift", "LControl", "RControl", "Alt", "AltGr", "LWin", "RWin", "Menu", "ScrollLock", "NumLock", "CapsLock", "Maximum"
};


byte arrowState;
byte keyState;
byte keyTap;
byte arrowTap;

int joyMinX = 2000000000, joyMinY = 2000000000;
int joyMaxX = 0, joyMaxY = 0;
int joyCX, joyCY;
int joyDeadX, joyDeadY;

byte joystickOn = 1;
byte oldJoy;

byte lastScanCode;

byte kb[8][4];
byte joyBtn[2];

void ControlKeyDown(char k)
{
	int i, j;
	byte bit;

	lastScanCode = k;

	for (i = 0; i < 4; i++)
	{
		bit = 1;
		for (j = 0; j < 8; j++)
		{
			if (k == kb[j][i])
			{
				keyState |= bit;
				keyTap |= bit;
			}
			bit *= 2;
		}
	}
	// always track arrows, no matter what the keys are, for menus
	if (k == KEY_UP)
	{
		arrowState |= CONTROL_UP;
		arrowTap |= CONTROL_UP;
	}
	if (k == 85)
	{
		arrowState |= CONTROL_DN;
		arrowTap |= CONTROL_DN;
	}
	if (k == 82)
	{
		arrowState |= CONTROL_LF;
		arrowTap |= CONTROL_LF;
	}
	if (k == 83)
	{
		arrowState |= CONTROL_RT;
		arrowTap |= CONTROL_RT;
	}
	if (k == 67)
	{
		arrowState |= CONTROL_B1;
		arrowTap |= CONTROL_B1;

	}
}

void ControlKeyUp(char k)
{
	int i, j;
	byte bit;

	for (i = 0; i < 4; i++)
	{
		bit = 1;
		for (j = 0; j < 8; j++)
		{
			if (k == kb[j][i])
			{
				keyState &= (~bit);
			}
			bit *= 2;
		}
	}

	// always track arrows, no matter what the keys are, for menus
	// keys updated for PixelToaster
	if (k == 84)
		arrowState &= (~CONTROL_UP);
	else if (k == 85)
		arrowState &= (~CONTROL_DN);
	else if (k == 82)
		arrowState &= (~CONTROL_LF);
	else if (k == 83)
		arrowState &= (~CONTROL_RT);
	else if (k == 67)
		arrowState &= (~CONTROL_B1);
}

void InitControls(void)
{
	MMRESULT result;
	JOYCAPS joyCaps;

	lastScanCode = 0;

	keyState = 0;
	keyTap = 0;
	arrowState = 0;

	if (joystickOn)
	{
		result = joyGetDevCaps(JOYSTICKID1, &joyCaps, sizeof (JOYCAPS));
		if (result != JOYERR_NOERROR)
		{
			joystickOn = 0;
			return;
		}
		joyCX = (joyCaps.wXmax - joyCaps.wXmin) / 2 + joyCaps.wXmin;
		joyCY = (joyCaps.wYmax - joyCaps.wYmin) / 2 + joyCaps.wYmin;
		joyMinX = joyCaps.wXmin;
		joyMinY = joyCaps.wYmin;
		joyMaxX = joyCaps.wXmax;
		joyMaxY = joyCaps.wYmax;
		oldJoy = 0;
	}
}

byte GetJoyState(void)
{
	MMRESULT result;
	JOYINFOEX joyInfo;
	int joyX, joyY;
	byte joyState;

	memset(&joyInfo, 0, sizeof (JOYINFOEX));
	joyInfo.dwSize = sizeof (JOYINFOEX);
	joyInfo.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY;
	result = joyGetPosEx(JOYSTICKID1, &joyInfo);
	if (result != JOYERR_NOERROR)
	{
		return 0;
	}
	joyX = (int) joyInfo.dwXpos;
	joyY = (int) joyInfo.dwYpos;
	if (joyX < joyMinX)
	{
		joyMinX = joyX;
	}
	if (joyX > joyMaxX)
	{
		joyMaxX = joyX;
	}
	if (joyY < joyMinY)
	{
		joyMinY = joyY;
	}
	if (joyY > joyMaxY)
	{
		joyMaxY = joyY;
	}
	joyDeadX = (joyMaxX - joyMinX) / 8;
	joyDeadY = (joyMaxY - joyMinY) / 8;
	joyState = 0;
	if (joyX - joyCX<-joyDeadX)
		joyState |= CONTROL_LF;
	if (joyX - joyCX > joyDeadX)
		joyState |= CONTROL_RT;
	if (joyY - joyCY<-joyDeadY)
		joyState |= CONTROL_UP;
	if (joyY - joyCY > joyDeadY)
		joyState |= CONTROL_DN;
	if (joyInfo.dwButtons & (joyBtn[0]))
	{
		if (!(oldJoy & CONTROL_B1))
			keyTap |= CONTROL_B1;
		joyState |= CONTROL_B1;
	}
	if (joyInfo.dwButtons & (joyBtn[1]))
	{
		if (!(oldJoy & CONTROL_B2))
			keyTap |= CONTROL_B2;
		joyState |= CONTROL_B2;
	}
	oldJoy = joyState;

	return joyState;
}

dword GetJoyButtons(void)
{
	MMRESULT result;
	JOYINFOEX joyInfo;

	if (!joystickOn)
		return 0;

	memset(&joyInfo, 0, sizeof (JOYINFOEX));
	joyInfo.dwSize = sizeof (JOYINFOEX);
	joyInfo.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY;
	result = joyGetPosEx(JOYSTICKID1, &joyInfo);
	if (result != JOYERR_NOERROR)
	{
		return 0;
	}
	return joyInfo.dwButtons;
}

byte GetControls(void)
{
	if (joystickOn)
		return GetJoyState() | (keyState);
	else
		return keyState;
}

byte GetTaps(void)
{
	byte tapState;

	if (joystickOn)
		GetJoyState();

	tapState = keyTap | arrowTap;

	keyTap = 0;
	arrowTap = 0;

	return tapState;
}

byte GetArrows(void)
{
	return arrowState;
}

byte LastScanCode(void)
{
	byte c;

	c = lastScanCode;
	lastScanCode = 0;
	return c;
}

byte JoystickAvailable(void)
{
	return joystickOn;
}

void SetKeys(byte keys[8])
{
	memcpy(kb, keys, 8);
}

char *ScanCodeText(byte s)
{
	static char unknown[16] = "Unknown";
	char* r = scanCodeTable[s];
	if (r[0] == 0) return unknown; // empty string
	return r;
}

void ApplyControlSettings(void)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		kb[i][0] = opt.control[0][i];
		kb[i][1] = opt.control[1][i];
		kb[i][2] = 0;
		kb[i][3] = 0;
	}
	joyBtn[0] = (1 << opt.joyCtrl[0]);
	joyBtn[1] = (1 << opt.joyCtrl[1]);
}
