#include "control.h"
#include <windows.h>
#include <mmsystem.h>
#include "mgldraw.h"

byte keyState=0;
byte keyTap=0;

int joyMinX=2000000000,joyMinY=2000000000;
int joyMaxX=0,joyMaxY=0;
int joyCX,joyCY;
int joyDeadX,joyDeadY;

byte joystickOn=1;
MGLDraw *ctrlmgl;
byte msButton,msButton2;

void ControlKeyDown(char k)
{
	switch(k)
	{
		case VK_NUMPAD8:
		case VK_UP:
			keyState|=CONTROL_UP;
			keyTap|=CONTROL_UP;
			break;
		case VK_NUMPAD2:
		case VK_DOWN:
			keyState|=CONTROL_DN;
			keyTap|=CONTROL_DN;
			break;
		case VK_NUMPAD4:
		case VK_LEFT:
			keyState|=CONTROL_LF;
			keyTap|=CONTROL_LF;
			break;
		case VK_NUMPAD6:
		case VK_RIGHT:
			keyState|=CONTROL_RT;
			keyTap|=CONTROL_RT;
			break;
		case 'Z':
		case VK_CONTROL:
		case VK_RETURN:
			keyState|=CONTROL_B1;
			keyTap|=CONTROL_B1;
			break;
		case 'X':
		case VK_SHIFT:
			keyState|=CONTROL_B2;
			keyTap|=CONTROL_B2;
			break;
	}
}


void ControlKeyUp(char k)
{
	switch(k)
	{
		case VK_NUMPAD8:
		case VK_UP:
			keyState&=(~CONTROL_UP);
			break;
		case VK_NUMPAD2:
		case VK_DOWN:
			keyState&=(~CONTROL_DN);
			break;
		case VK_NUMPAD4:
		case VK_LEFT:
			keyState&=(~CONTROL_LF);
			break;
		case VK_NUMPAD6:
		case VK_RIGHT:
			keyState&=(~CONTROL_RT);
			break;
		case 'Z':
		case VK_CONTROL:
		case VK_RETURN:
			keyState&=(~CONTROL_B1);
			break;
		case 'X':
		case VK_SHIFT:
			keyState&=(~CONTROL_B2);
			break;
	}
}

void InitControls(MGLDraw *mgl)
{
	MMRESULT result;
	JOYINFOEX joyInfo;

	ctrlmgl=mgl;
	ctrlmgl->LastKeyPressed();
	msButton=0;
	msButton2=0;

	if(joystickOn)
	{
		joyInfo.dwFlags=JOY_RETURNCENTERED|JOY_RETURNX|JOY_RETURNY;
		result=joyGetPosEx(JOYSTICKID1, &joyInfo);
		if(result!=JOYERR_NOERROR)
		{
			joystickOn=0;
			return;
		}
		joyCX=joyInfo.dwXpos;
		joyCY=joyInfo.dwYpos;
		joyMinX=joyInfo.dwXpos/2;
		joyMinY=joyInfo.dwYpos/2;
		joyMaxX=(joyInfo.dwXpos*3)/2;
		joyMaxY=(joyInfo.dwYpos*3)/2;
	}
}

byte GetJoyState(void)
{
	MMRESULT result;
	JOYINFOEX joyInfo;
	int joyX,joyY;
	byte joyState;

	joyInfo.dwFlags=JOY_RETURNBUTTONS|JOY_RETURNX|JOY_RETURNY;
	result=joyGetPosEx(JOYSTICKID1, &joyInfo);
	if(result!=JOYERR_NOERROR)
	{
		//joystickOn=0;
		return 0;
	}
	joyX=(int)joyInfo.dwXpos;
	joyY=(int)joyInfo.dwYpos;
	if(joyX<joyMinX)
	{
		joyMinX=joyX;
	}
	if(joyX>joyMaxX)
	{
		joyMaxX=joyX;
	}
	if(joyY<joyMinY)
	{
		joyMinY=joyY;
	}
	if(joyY>joyMaxY)
	{
		joyMaxY=joyY;
	}
	joyDeadX=(joyMaxX-joyMinX)/8;
	joyDeadY=(joyMaxY-joyMinY)/8;
	joyState=0;
	if(joyX-joyCX<-joyDeadX)
		joyState|=CONTROL_LF;
	if(joyX-joyCX>joyDeadX)
		joyState|=CONTROL_RT;
	if(joyY-joyCY<-joyDeadY)
		joyState|=CONTROL_UP;
	if(joyY-joyCY>joyDeadY)
		joyState|=CONTROL_DN;
	if(joyInfo.dwButtons&1)
		joyState|=CONTROL_B1;
	if(joyInfo.dwButtons&2)
		joyState|=CONTROL_B2;

	return joyState;
}

byte GetControls(void)
{
	byte joyState=0;

	if(joystickOn)
		joyState=GetJoyState();

	return (keyState|joyState);
}

byte GetTaps(void)
{
	byte tapState=keyTap;

	keyTap=0;

	return tapState;
}

byte LastKeyPressed(void)
{
	return ctrlmgl->LastKeyPressed();
}

byte MouseClick(void)
{
	if(ctrlmgl->MouseDown())
	{
		if(!msButton)
		{
			msButton=1;
			return 1;
		}
	}
	else
		msButton=0;

	return 0;
}

byte MouseHeld(void)
{
	if(ctrlmgl->MouseDown())
	{
		msButton=1;
		return 1;
	}
	else
		msButton=0;

	return 0;
}

byte RMouseClick(void)
{
	if(ctrlmgl->RMouseDown())
	{
		if(!msButton2)
		{
			msButton2=1;
			return 1;
		}
	}
	else
		msButton2=0;

	return 0;
}

byte RMouseHeld(void)
{
	if(ctrlmgl->RMouseDown())
	{
		msButton2=1;
		return 1;
	}
	else
		msButton2=0;

	return 0;
}
