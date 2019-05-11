#include "control.h"
#include "mgldraw.h"
#include "progress.h"
#include "config.h"

// text strings corresponding to scan codes 0-88
char scanCodeTable[128][16]={
	//0
	"Null","Escape","1","2","3","4","5","6","7","8","9","0",
	//12
	"-","=","Backspace","Tab","Q","W","E","R","T","Y","U","I","O","P",
	//26
	"[","]","Enter","Ctrl","A","S","D","F","G","H","J","K","L",";",
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

char allegroCodeTable[][16]={
	// 0
	"NULL","A","B","C","D","E","F","G","H","I","J","K",
	// 12
	"L","M","N","O","P","Q","R","S","T","U","V","W",
	// 24
	"X","Y","Z","0","1","2","3","4","5","6","7","8","9",
	// 37
	"Keypad 0","Keypad 1","Keypad 2","Keypad 3","Keypad 4","Keypad 5",
	// 43
	"Keypad 6","Keypad 7","Keypad 8","Keypad 9","F1","F2","F3","F4",
	// 51
	"F5","F6","F7","F8","F9","F10","F11","F12","Escape","`","-","=",
	//63	(#71 is the mysterious "backslash2")
	"Backspace","Tab","[","]","Enter","Colon","\"","\\","\\",",",".",
	// 74
	"/","Space","Insert","Delete","Home","End","PageUp","PageDown",
	//82
	"Left","Right","Up","Down","/","*","-","+",".","Pad Enter","PrintScreen",
	// 93
	"Pause","ABNT_C1","YEN","KANA","CONVERT","NOCONVERT","AT","CIRCUMFLEX",
	// 101
	"COLON2","KANJI","Pad =","Backquote",";","Command","?1","?2","?3","?4",
	// 111
	"?5","?6","?7","?8","LeftShift","RightShift","LeftCtrl","RightCtrl","Alt",
	// 120
	"ALTGR","Left Windows","Right Windows","Menu Key","Scroll Lock","Num Lock","Caps Lock"
};

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

byte kb[6][4];
byte joyBtn[2];
byte shiftState;

void MyKeebieCallback(int scancode)
{
	byte s;

	s=(byte)scancode;
	if(s&128)
		ControlKeyUp((char)(s&127));
	else
		ControlKeyDown((char)s);
}

void ControlKeyDown(char k)
{
	int i,j;
	byte bit;

	lastScanCode=k;

	// track shift keys being held
  	if(k==KEY_LSHIFT)
		shiftState|=1;
	if(k==KEY_RSHIFT)
		shiftState|=2;

	for(i=0;i<4;i++)
	{
		bit=1;
		for(j=0;j<6;j++)
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
	if(k==KEY_UP)
	{
		arrowState|=CONTROL_UP;
		arrowTap|=CONTROL_UP;
	}
	if(k==KEY_DOWN)
	{
		arrowState|=CONTROL_DN;
		arrowTap|=CONTROL_DN;
	}
	if(k==KEY_LEFT)
	{
		arrowState|=CONTROL_LF;
		arrowTap|=CONTROL_LF;
	}
	if(k==KEY_RIGHT)
	{
		arrowState|=CONTROL_RT;
		arrowTap|=CONTROL_RT;
	}
	if(k==KEY_ENTER)
	{
		arrowState|=CONTROL_B1;
		arrowTap|=CONTROL_B1;
	}

	_globalMGLDraw->SetLastKey((char)scancode_to_ascii((int)k));
}

void ControlKeyUp(char k)
{
	int i,j;
	byte bit;

	if(k==KEY_LSHIFT)
		shiftState&=(~1);
	if(k==KEY_RSHIFT)
		shiftState&=(~2);

	for(i=0;i<4;i++)
	{
		bit=1;
		for(j=0;j<6;j++)
		{
			if(k==kb[j][i])
			{
				keyState&=(~bit);
			}
			bit*=2;
		}
	}

	// always track arrows, no matter what the keys are, for menus
	if(k==KEY_UP)
		arrowState&=(~CONTROL_UP);
	if(k==KEY_DOWN)
		arrowState&=(~CONTROL_DN);
	if(k==KEY_LEFT)
		arrowState&=(~CONTROL_LF);
	if(k==KEY_RIGHT)
		arrowState&=(~CONTROL_RT);
	if(k==KEY_ENTER)
		arrowState&=(~CONTROL_B1);
}

void InitControls(void)
{
	keyboard_lowlevel_callback=MyKeebieCallback;
	lastScanCode=0;
	shiftState=0;
	keyState=0;
	keyTap=0;
	arrowState=0;
	arrowTap=0;

	if(config.joystick && install_joystick(JOY_TYPE_AUTODETECT)==0)
		joystickOn=1;
	else
		joystickOn=0;
}

byte GetJoyState(void)
{
	byte joyState;

	poll_joystick();

	joyState=0;

	if(joy[0].stick[0].axis[0].d1)
	{
		if(!(oldJoy&CONTROL_LF))
			keyTap|=CONTROL_LF;
		joyState|=CONTROL_LF;
	}
	if(joy[0].stick[0].axis[0].d2)
	{
		if(!(oldJoy&CONTROL_RT))
			keyTap|=CONTROL_RT;
		joyState|=CONTROL_RT;
	}
	if(joy[0].stick[0].axis[1].d1)
	{
		if(!(oldJoy&CONTROL_UP))
			keyTap|=CONTROL_UP;
		joyState|=CONTROL_UP;
	}
	if(joy[0].stick[0].axis[1].d2)
	{
		if(!(oldJoy&CONTROL_DN))
			keyTap|=CONTROL_DN;
		joyState|=CONTROL_DN;
	}
	if(joy[0].button[0].b)
	{
		if(!(oldJoy&CONTROL_B1))
			keyTap|=CONTROL_B1;
		joyState|=CONTROL_B1;
	}
	if(joy[0].button[1].b)
	{
		if(!(oldJoy&CONTROL_B2))
			keyTap|=CONTROL_B2;
		joyState|=CONTROL_B2;
	}
	if(joy[0].num_buttons>2 && joy[0].button[2].b)
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
	byte b;

	poll_joystick();

	b=0;
	if(joy[0].button[0].b)
		b|=1;
	if(joy[0].button[1].b)
		b|=2;
	if(joy[0].num_buttons>2 && joy[0].button[2].b)
		b|=4;
	if(joy[0].num_buttons>3 && joy[0].button[3].b)
		b|=8;

	return b;
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

char *AllegroCodeText(byte s)
{
	return allegroCodeTable[s];
}

void ApplyControlSettings(void)
{
	int i;

	for(i=0;i<6;i++)
	{
		kb[i][0]=profile.control[0][i];
		kb[i][1]=profile.control[1][i];
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
