#include "control.h"
#include "mgldraw.h"

static byte lastScanCode;

// normal mapped controls: kbd, joystick, etc.
static byte keyState, keyTap;
// fixed arrow keys and return controls
static byte arrowState, arrowTap;
// 1=lshift, 2=rshift
static byte shiftState;

// joysticks
static SDL_Joystick* joystick;
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
	shiftState=0;
	oldJoy=0;

	if(SDL_NumJoysticks() > 0)
		joystick = SDL_JoystickOpen(0);
	else
		joystick = nullptr;
}

byte GetControls() {
	return keyState | GetJoyState();
}

byte GetTaps() {
	GetJoyState();  // Updates keyTap.
	byte result = keyTap;
	keyTap = 0;
	return result;
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
	if (!joystick)
		return 0;

	dword held = 0;

	int n = SDL_JoystickNumButtons(joystick);
	int j = 1;
	for (int i = 0; i < n && i < 32; ++i) {
		if (SDL_JoystickGetButton(joystick, i)) {
			held |= j;
		}
		j *= 2;
	}

	return held;
}

void SetKeyboardBindings(int keyboard, const byte keys[8]) {
	for (int i = 0; i < NUM_CONTROLS; ++i) {
		kb[i][keyboard] = keys[i];
	}
}

void SetJoystickBindings(const byte buttons[4]) {
	memcpy(joyBtn, buttons, NUM_JOYBTNS);
}

byte ShiftState() {
	return shiftState;
}

// Called upon SDL events
void ControlKeyDown(byte k)
{
	int i,j;
	byte bit;

	lastScanCode=k;

	for(i=0;i<4;i++)
	{
		bit=1;
		for(j=0;j<8;j++)
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
		// track shift keys being held
		case SDL_SCANCODE_LSHIFT:
			shiftState |= 1;
			break;
		case SDL_SCANCODE_RSHIFT:
			shiftState |= 2;
			break;
	}
}

void ControlKeyUp(byte k)
{
	int i,j;
	byte bit;

	for(i=0;i<4;i++)
	{
		bit=1;
		for(j=0;j<8;j++)
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
		// track shift keys being held
		case SDL_SCANCODE_LSHIFT:
			shiftState &= ~1;
			break;
		case SDL_SCANCODE_RSHIFT:
			shiftState &= ~2;
			break;
	}
}

static byte GetJoyState(void)
{
	if (!joystick)
		return 0;

	const int DEADZONE = 8192;
	byte joyState = 0;

	if(SDL_JoystickGetAxis(joystick, 0) < -DEADZONE)
	{
		if(!(oldJoy&CONTROL_LF))
			keyTap|=CONTROL_LF;
		joyState|=CONTROL_LF;
	}
	else if(SDL_JoystickGetAxis(joystick, 0) > DEADZONE)
	{
		if(!(oldJoy&CONTROL_RT))
			keyTap|=CONTROL_RT;
		joyState|=CONTROL_RT;
	}
	if(SDL_JoystickGetAxis(joystick, 1) < -DEADZONE)
	{
		if(!(oldJoy&CONTROL_UP))
			keyTap|=CONTROL_UP;
		joyState|=CONTROL_UP;
	}
	else if(SDL_JoystickGetAxis(joystick, 1) > DEADZONE)
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
	if (SDL_JoystickGetButton(joystick, joyBtn[3]))
	{
		if(!(oldJoy&CONTROL_B4))
			keyTap|=CONTROL_B4;
		joyState|=CONTROL_B4;
	}
	oldJoy=joyState;

	return joyState;
}
