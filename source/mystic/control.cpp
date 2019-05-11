#include "control.h"
#include "mgldraw.h"
#include "options.h"

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
byte keyState;
byte keyTap;

int joyMinX=2000000000,joyMinY=2000000000;
int joyMaxX=0,joyMaxY=0;
int joyCX,joyCY;
int joyDeadX,joyDeadY;

byte joystickOn=1;
byte oldJoy;
byte joyError=0;
byte joyID;

byte lastScanCode;

void ControlKeyDown(char k)
{
	int i,j;
	byte bit;

	lastScanCode=k;

	for(i=0;i<3;i++)
	{
		bit=1;
		for(j=0;j<8;j++)
		{
			if(k==opt.key[j][i])
			{
				keyState|=bit;
				keyTap|=bit;
			}
			bit*=2;
		}
	}
	// always track arrows, no matter what the keys are, for menus
	if(k==72)
		arrowState|=CONTROL_UP;
	if(k==80)
		arrowState|=CONTROL_DN;
	if(k==75)
		arrowState|=CONTROL_LF;
	if(k==77)
		arrowState|=CONTROL_RT;
}

void ControlKeyUp(char k)
{
	int i,j;
	byte bit;

	for(i=0;i<3;i++)
	{
		bit=1;
		for(j=0;j<8;j++)
		{
			if(k==opt.key[j][i])
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
}

void InitControls(void)
{
	MMRESULT result;
	JOYCAPS   joyCaps;
	int numJoys,i;

	lastScanCode=0;

	keyState=0;
	keyTap=0;
	arrowState=0;

	if(joystickOn)
	{
		numJoys=joyGetNumDevs();
		joyID=255;
		for(i=0;i<numJoys;i++)
		{
 			memset(&joyCaps,0,sizeof(JOYCAPS));

			result=joyGetDevCaps(i,&joyCaps,sizeof(JOYCAPS));
 			if(result!=JOYERR_NOERROR)
			{
				continue;
			}
			joyID=(byte)i;
			joyError=0;
			joyCX=(joyCaps.wXmax-joyCaps.wXmin)/2+joyCaps.wXmin;
			joyCY=(joyCaps.wYmax-joyCaps.wYmin)/2+joyCaps.wYmin;
			joyMinX=joyCaps.wXmin;
			joyMinY=joyCaps.wYmin;
			joyMaxX=joyCaps.wXmax;
			joyMaxY=joyCaps.wYmax;
			break;
		}
		if(joyID==255)
			joystickOn=0;	// couldn't find a working stick

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
	result=joyGetPosEx(joyID, &joyInfo);
	if(result!=JOYERR_NOERROR)
	{
		joyError++;
		if(joyError>100)
			joystickOn=0;
		return 0;
	}
	joyError=0;
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
	if(joyInfo.dwButtons&(1<<opt.joyCtrl[0]))
	{
		if(!(oldJoy&CONTROL_B1))
			keyTap|=CONTROL_B1;
		joyState|=CONTROL_B1;
	}
	if(joyInfo.dwButtons&(1<<opt.joyCtrl[1]))
	{
		if(!(oldJoy&CONTROL_B2))
			keyTap|=CONTROL_B2;
		joyState|=CONTROL_B2;
	}
	if(joyInfo.dwButtons&(1<<opt.joyCtrl[2]))
	{
		if(!(oldJoy&CONTROL_B3))
			keyTap|=CONTROL_B3;
		joyState|=CONTROL_B3;
	}
	if(joyInfo.dwButtons&(1<<opt.joyCtrl[3]))
	{
		if(!(oldJoy&CONTROL_B4))
			keyTap|=CONTROL_B4;
		joyState|=CONTROL_B4;
	}
	oldJoy=joyState;

	return joyState;
}

dword RawJoyButtons(void)
{
	MMRESULT result;
	JOYINFOEX joyInfo;

	memset(&joyInfo,0,sizeof(JOYINFOEX));
	joyInfo.dwSize=sizeof(JOYINFOEX);
	joyInfo.dwFlags=JOY_RETURNBUTTONS|JOY_RETURNX|JOY_RETURNY;
	result=joyGetPosEx(joyID, &joyInfo);
	if(result!=JOYERR_NOERROR)
	{
		joyError++;
		if(joyError>100)
			joystickOn=0;
		return 0;
	}
	joyError=0;
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

char *ScanCodeText(byte s)
{
	return scanCodeTable[s];
}
