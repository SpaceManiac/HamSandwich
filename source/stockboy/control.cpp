#include "control.h"
#include "mgldraw.h"
#include "options.h"
#include "profile.h"

// text strings corresponding to scan codes 0-88
char scanCodeTable[128][16]={
	//0
	"Null","Escape","1","2","3","4","5","6","7","8","9","0",
	//12
	"-","=","Backspace","Tab","Q","W","E","R","T","Y","U","I","O","P",
	//26
	"[","]","Enter","Control","A","S","D","F","G","H","J","K","L",";",
	//40
	"'","`","LeftShift","\\","Z","X","C","V","B","N","M",",",".","/",
	//54
	"RightShift","PrintScreen","Alt","Space","CapsLock","F1","F2","F3",
	//62
	"F4","F5","F6","F7","F8","F9","F10","NumLock","ScrollLock",
	//71
	"Home","Up","PageUp","Keypad-","Left","Keypad5","Right","Keypad+",
	//79
	"End","Down","PageDown","Insert","Delete","","","","F11","F12"};


byte arrowState;
byte arrowTap;
byte keyState;
byte keyTap;

int joyMinX=2000000000,joyMinY=2000000000;
int joyMaxX=0,joyMaxY=0;
int joyCX,joyCY;
int joyDeadX,joyDeadY;

byte joystickOn=1;
byte oldJoy;

byte lastScanCode;

byte kb[8][4];
byte joyBtn[2];
byte shiftState;

void ControlKeyDown(char k)
{
	int i,j;
	byte bit;

	lastScanCode=k;

	// track shift keys being held
	if(k==42)
		shiftState|=1;
	if(k==54)
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
	if(k==72)
	{
		arrowState|=CONTROL_UP;
		arrowTap|=CONTROL_UP;
	}
	if(k==80)
	{
		arrowState|=CONTROL_DN;
		arrowTap|=CONTROL_DN;
	}
	if(k==75)
	{
		arrowState|=CONTROL_LF;
		arrowTap|=CONTROL_LF;
	}
	if(k==77)
	{
		arrowState|=CONTROL_RT;
		arrowTap|=CONTROL_RT;
	}
	if(k==28)
	{
		arrowState|=CONTROL_B1;
		arrowTap|=CONTROL_B1;
	}
}

void ControlKeyUp(char k)
{
	int i,j;
	byte bit;

	if(k==42)
		shiftState&=(~1);
	if(k==54)
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
	if(k==72)
		arrowState&=(~CONTROL_UP);
	if(k==80)
		arrowState&=(~CONTROL_DN);
	if(k==75)
		arrowState&=(~CONTROL_LF);
	if(k==77)
		arrowState&=(~CONTROL_RT);
	if(k==28)
		arrowState&=(~CONTROL_B1);
}

void InitControls(void)
{
	MMRESULT result;
	JOYCAPS   joyCaps;

	lastScanCode=0;
	shiftState=0;
	keyState=0;
	keyTap=0;
	arrowState=0;
	arrowTap=0;

	if(joystickOn)
	{
		result=joyGetDevCaps(JOYSTICKID1,&joyCaps,sizeof(JOYCAPS));
		if(result!=JOYERR_NOERROR)
		{
			joystickOn=0;
			return;
		}
		joyCX=(joyCaps.wXmax-joyCaps.wXmin)/2+joyCaps.wXmin;
		joyCY=(joyCaps.wYmax-joyCaps.wYmin)/2+joyCaps.wYmin;
		joyMinX=joyCaps.wXmin;
		joyMinY=joyCaps.wYmin;
		joyMaxX=joyCaps.wXmax;
		joyMaxY=joyCaps.wYmax;
		oldJoy=0;
	}
}

byte GetJoyState(void)
{
	MMRESULT result;
	JOYINFOEX joyInfo;
	int joyX,joyY;
	byte joyState;

	memset(&joyInfo,0,sizeof(JOYINFOEX));
	joyInfo.dwSize=sizeof(JOYINFOEX);
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
	{
		if(!(oldJoy&CONTROL_LF))
			keyTap|=CONTROL_LF;
		joyState|=CONTROL_LF;
	}
	if(joyX-joyCX>joyDeadX)
	{
		if(!(oldJoy&CONTROL_RT))
			keyTap|=CONTROL_RT;
		joyState|=CONTROL_RT;
	}
	if(joyY-joyCY<-joyDeadY)
	{
		if(!(oldJoy&CONTROL_UP))
			keyTap|=CONTROL_UP;
		joyState|=CONTROL_UP;
	}
	if(joyY-joyCY>joyDeadY)
	{
		if(!(oldJoy&CONTROL_DN))
			keyTap|=CONTROL_DN;
		joyState|=CONTROL_DN;
	}
	if(joyInfo.dwButtons&(joyBtn[0]))
	{
		if(!(oldJoy&CONTROL_B1))
			keyTap|=CONTROL_B1;
		joyState|=CONTROL_B1;
	}
	if(joyInfo.dwButtons&(joyBtn[1]))
	{
		if(!(oldJoy&CONTROL_B2))
			keyTap|=CONTROL_B2;
		joyState|=CONTROL_B2;
	}
	oldJoy=joyState;

	return joyState;
}

dword GetJoyButtons(void)
{
	MMRESULT result;
	JOYINFOEX joyInfo;

	if(!joystickOn)
		return 0;

	memset(&joyInfo,0,sizeof(JOYINFOEX));
	joyInfo.dwSize=sizeof(JOYINFOEX);
	joyInfo.dwFlags=JOY_RETURNBUTTONS|JOY_RETURNX|JOY_RETURNY;
	result=joyGetPosEx(JOYSTICKID1, &joyInfo);
	if(result!=JOYERR_NOERROR)
	{
		return 0;
	}
	return joyInfo.dwButtons;
}

byte GetControls(void)
{
	if(joystickOn)
		return GetJoyState()|(keyState);
	else
		return keyState;
}

byte GetTaps(void)
{
	byte tapState;

	if(joystickOn)
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
	return joystickOn;
}

void SetKeys(byte keys[8])
{
	memcpy(kb,keys,8);
}

char *ScanCodeText(byte s)
{
	return scanCodeTable[s];
}

void ApplyControlSettings(void)
{
	int i;

	for(i=0;i<5;i++)
	{
		kb[i][0]=profile.keys[i];
		kb[i][1]=0;
		kb[i][2]=0;
		kb[i][3]=0;
	}
	joyBtn[0]=1;
	joyBtn[1]=2;
}

byte ShiftState(void)
{
	return shiftState;
}