#include "control.h"
#include "mgldraw.h"
#include "config.h"
#include "options.h"

byte arrowState;
byte arrowTap;
byte keyState;
byte keyTap;

SDL_Joystick* joystick;
byte oldJoy;

byte lastScanCode;

byte kb[8][4];
byte joyBtn[2];
byte shiftState;

void ControlKeyDown(byte k)
{
	int i,j;
	byte bit;

	lastScanCode=k;

	// track shift keys being held
	if(k==SDL_SCANCODE_LSHIFT)
		shiftState|=1;
	if(k==SDL_SCANCODE_RSHIFT)
		shiftState|=2;

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
	if(k==SDL_SCANCODE_UP)
	{
		arrowState|=CONTROL_UP;
		arrowTap|=CONTROL_UP;
	}
	if(k==SDL_SCANCODE_DOWN)
	{
		arrowState|=CONTROL_DN;
		arrowTap|=CONTROL_DN;
	}
	if(k==SDL_SCANCODE_LEFT)
	{
		arrowState|=CONTROL_LF;
		arrowTap|=CONTROL_LF;
	}
	if(k==SDL_SCANCODE_RIGHT)
	{
		arrowState|=CONTROL_RT;
		arrowTap|=CONTROL_RT;
	}
	if(k==SDL_SCANCODE_RETURN)
	{
		arrowState|=CONTROL_B1;
		arrowTap|=CONTROL_B1;
	}

	//_globalMGLDraw->SetLastKey((char)scancode_to_ascii((int)k));
}

void ControlKeyUp(byte k)
{
	int i,j;
	byte bit;

	if(k==SDL_SCANCODE_LSHIFT)
		shiftState&=(~1);
	if(k==SDL_SCANCODE_RSHIFT)
		shiftState&=(~2);

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
	if(k==SDL_SCANCODE_UP)
		arrowState&=(~CONTROL_UP);
	if(k==SDL_SCANCODE_DOWN)
		arrowState&=(~CONTROL_DN);
	if(k==SDL_SCANCODE_LEFT)
		arrowState&=(~CONTROL_LF);
	if(k==SDL_SCANCODE_RIGHT)
		arrowState&=(~CONTROL_RT);
	if(k==SDL_SCANCODE_RETURN)
		arrowState&=(~CONTROL_B1);
}

void InitControls(void)
{
	lastScanCode=0;
	shiftState=0;
	keyState=0;
	keyTap=0;
	arrowState=0;
	arrowTap=0;

	if(config.joystick && SDL_NumJoysticks() > 0)
		joystick = SDL_JoystickOpen(0);
	else
		joystick = nullptr;
}

byte GetJoyState(void)
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
	if(SDL_JoystickGetButton(joystick, 0))
	{
		if(!(oldJoy&CONTROL_B1))
			keyTap|=CONTROL_B1;
		joyState|=CONTROL_B1;
	}
	if(SDL_JoystickGetButton(joystick, 1))
	{
		if(!(oldJoy&CONTROL_B2))
			keyTap|=CONTROL_B2;
		joyState|=CONTROL_B2;
	}
	if(SDL_JoystickGetButton(joystick, 2))
	{
		if(!(oldJoy&CONTROL_B3))
			keyTap|=CONTROL_B3;
		joyState|=CONTROL_B3;
	}
	oldJoy=joyState;

	return joyState;
}

byte GetJoyButtons(void)
{
	if (!joystick)
		return 0;

	byte b = 0;

	if(SDL_JoystickGetButton(joystick, 0))
		b|=1;
	if(SDL_JoystickGetButton(joystick, 1))
		b|=2;
	if(SDL_JoystickGetButton(joystick, 2))
		b|=4;
	if(SDL_JoystickGetButton(joystick, 3))
		b|=8;

	return b;
}

byte GetControls(void)
{
	if(joystick)
		return GetJoyState()|(keyState);
	else
		return keyState;
}

byte GetTaps(void)
{
	byte tapState;

	if(joystick)
		GetJoyState();

	tapState=keyTap;

	keyTap=0;

	return tapState;
}

byte GetArrows(void)
{
	return arrowState;
}

byte GetArrowTaps(void)
{
	byte tapState;

	tapState=arrowTap;
	arrowTap=0;

	return tapState;
}

byte LastScanCode(void)
{
	byte c;

	c=lastScanCode;
	lastScanCode=0;
	return c;
}

byte JoystickAvailable(void)
{
	return joystick != nullptr;
}

void SetKeys(byte keys[8])
{
	memcpy(kb,keys,8);
}

const char *AllegroCodeText(byte s)
{
	return SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode) s));
}

void ApplyControlSettings(void)
{
	int i;

	for(i=0;i<8;i++)
	{
		kb[i][0]=opt.control[0][i];
		kb[i][1]=opt.control[1][i];
		kb[i][2]=0;
		kb[i][3]=0;
	}
	joyBtn[0]=opt.joyCtrl[0];
	joyBtn[1]=opt.joyCtrl[1];
	joyBtn[2]=opt.joyCtrl[2];
	joyBtn[3]=opt.joyCtrl[3];
}

void ApplyControlSettingsMem(byte *data)
{
	int i;

	for(i=0;i<8;i++)
	{
		kb[i][0]=data[0*8+i];
		kb[i][1]=data[1*8+i];
		kb[i][2]=0;
		kb[i][3]=0;
	}
	joyBtn[0]=data[2*8+0];
	joyBtn[1]=data[2*8+1];
	joyBtn[2]=data[2*8+2];
	joyBtn[3]=data[2*8+3];
}

byte ShiftState(void)
{
	return shiftState;
}
