#include "control.h"
#include "mgldraw.h"
#include "log.h"
#include "softjoystick.h"
#include <vector>
#include <algorithm>
#include <memory>

// SDL_GameController
//add:
namespace owned
{
	namespace _deleter
	{
		struct SDL_GameController
		{
			void operator()(::SDL_GameController* ptr) { return SDL_GameControllerClose(ptr); }
		};
	}

	typedef std::unique_ptr<::SDL_GameController, _deleter::SDL_GameController> SDL_GameController;

	inline SDL_GameController SDL_GameControllerOpen(int device_index)
	{
		return SDL_GameController{ ::SDL_GameControllerOpen(device_index) };
	}
}
//end

static byte lastScanCode;

// normal mapped controls: kbd, joystick, etc.
static byte keyState, keyTap;
static byte oldKeys;		// AA fix!
// fixed arrow keys and return controls
static byte arrowState, arrowTap;
static byte oldArrows;		// AA fix!

// joysticks
//static std::vector<SDL_Joystick*> joysticks;
//replace:
static std::vector<owned::SDL_GameController> joysticks;
//end
static byte oldJoy;

static byte gpstartdelay;	// AA start delay

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
	oldKeys=0;		// AA fix
	arrowState=0;
	arrowTap=0;
	oldArrows=0;	// AA fix
	oldJoy=0;
	gpstartdelay=0;	// AA fix

	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		//SDL_Joystick* joystick = SDL_JoystickOpen(i);
		//replace:
		if (!SDL_IsGameController(i)) {
			LogDebug("Skipping non-gamepad joystick: %s", SDL_JoystickNameForIndex(i));
			continue;
		}

		owned::SDL_GameController joystick = owned::SDL_GameControllerOpen(i);
		//end
		if (joystick)
		{
			//add:
			LogDebug("Gamepad init: %s", SDL_GameControllerName(joystick.get()));
			//end
			//joysticks.push_back(joystick);
			//replace:
			joysticks.push_back(std::move(joystick));
			//end
		}
		else
		{
			//LogError("JoystickOpen(%d, %s): %s", i, SDL_JoystickNameForIndex(i), SDL_GetError());
			//replace:
			LogError("SDL_GameControllerOpen(%d, %s): %s", i, SDL_GameControllerNameForIndex(i), SDL_GetError());
			//end
		}
	}
}

byte GetControls() {
	return keyState | GetJoyState() | SoftJoystickState();
}

byte GetTaps() {
	GetJoyState();  // Updates keyTap.
	byte result = keyTap;
	keyTap = 0;
	oldKeys = keyState;			// AA fix
	return result | SoftJoystickTaps();
}

byte GetArrows() {
	return arrowState;
}

byte GetArrowTaps() {
	byte result = arrowTap;
	arrowTap = 0;
	oldArrows = arrowState;		// AA fix
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
	dword held = 0;

	//for (SDL_Joystick* joystick : joysticks) {
	//	int n = SDL_JoystickNumButtons(joystick);
	//replace:
	for (owned::SDL_GameController& gamepad : joysticks) {
		SDL_Joystick* joystick = SDL_GameControllerGetJoystick(gamepad.get());
		int n = SDL_JoystickNumButtons(joystick);
	//end
		int j = 1;
		for (int i = 0; i < n && i < 32; ++i) {
			if (SDL_JoystickGetButton(joystick, i)) {//already?
				held |= j;
			}
			j *= 2;
		}
	}

	return held;
}

//Gamepad add
static_assert(SDL_CONTROLLER_BUTTON_MAX <= 8 * sizeof(dword));
dword GetGamepadButtons() {
	dword held = 0;

	for (owned::SDL_GameController& gamepad : joysticks) {
		for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; ++button) {
			if (SDL_GameControllerGetButton(gamepad.get(), static_cast<SDL_GameControllerButton>(button))) {
				held |= (1 << button);
			}
		}
	}

	return held;
}

void SetKeyboardBindings(int keyboard, int nkeys, const byte* keys) {
	nkeys = std::min(nkeys, NUM_CONTROLS);
	for (int i = 0; i < nkeys; ++i) {
		kb[i][keyboard] = keys[i];
	}
}

void SetJoystickBindings(int nbuttons, const byte* buttons) {
	SoftJoystickNumButtons(nbuttons);
	memcpy(joyBtn, buttons, std::min(nbuttons, NUM_JOYBTNS));
}

// Called upon SDL events
void ControlKeyDown(byte k)
{
	int i,j;
	byte bit;

	lastScanCode=k;

	for(i=0;i<NUM_KEYBOARDS;i++)
	{
		bit=1;
		for(j=0;j<NUM_CONTROLS;j++)
		{
			if(k==kb[j][i])
			{
				if (!(oldKeys&bit))
					keyTap |= bit;
				keyState|=bit;
			}
			bit*=2;
		}
	}

	// always track arrows, no matter what the keys are, for menus
	switch(k) {
		case SDL_SCANCODE_UP:
			if (!(oldArrows&CONTROL_UP))	// AA fix
				arrowTap |= CONTROL_UP;
			arrowState |= CONTROL_UP;
			break;
		case SDL_SCANCODE_DOWN:
			if (!(oldArrows&CONTROL_DN))	// AA fix
				arrowTap |= CONTROL_DN;
			arrowState |= CONTROL_DN;
			break;
		case SDL_SCANCODE_LEFT:
			if (!(oldArrows&CONTROL_LF))	// AA fix
				arrowTap |= CONTROL_LF;
			arrowState |= CONTROL_LF;
			break;
		case SDL_SCANCODE_RIGHT:
			if (!(oldArrows&CONTROL_RT))	// AA fix
				arrowTap |= CONTROL_RT;
			arrowState |= CONTROL_RT;
			break;
		case SDL_SCANCODE_RETURN:
			if (!(oldArrows&CONTROL_B1))	// AA fix
				arrowTap |= CONTROL_B1;
			arrowState |= CONTROL_B1;
			break;
	}
}

void ControlKeyUp(byte k)
{
	int i,j;
	byte bit;

	for(i=0;i<NUM_KEYBOARDS;i++)
	{
		bit=1;
		for(j=0;j<NUM_CONTROLS;j++)
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
		case SDL_SCANCODE_RETURN:
			arrowState &= ~CONTROL_B1;
			break;
	}
}

static byte GetJoyState(void)
{
	const int DEADZONE = 8192;
	byte joyState = 0;

	for (auto iter = joysticks.begin(); iter != joysticks.end(); ++iter)
	{
		//SDL_Joystick* joystick = *iter;
		//replace:
		SDL_GameController* gamepad = iter->get();
		SDL_Joystick* joystick = SDL_GameControllerGetJoystick(gamepad);
		//end
		if (!SDL_JoystickGetAttached(joystick))
		{
			// Drop disconnected joysticks.
			//LogDebug("Joystick removed: %s", SDL_JoystickName(joystick));
			//replace:
			LogDebug("Gamepad removed: %s", SDL_GameControllerName(gamepad));
			//end
			iter = joysticks.erase(iter);
			if (iter == joysticks.end())
				break;
			continue;
		}

		byte hat = SDL_HAT_CENTERED;
		if (SDL_JoystickNumHats(joystick) > 0)
			hat = SDL_JoystickGetHat(joystick, 0);

		//if(SDL_JoystickGetAxis(joystick, 0) < -DEADZONE || (hat & SDL_HAT_LEFT))
		//replace:
		if (SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX) < -DEADZONE || SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
		//end
		{
			if(!(oldJoy&CONTROL_LF))
				keyTap|=CONTROL_LF;
			joyState|=CONTROL_LF;
		}
		//else if(SDL_JoystickGetAxis(joystick, 0) > DEADZONE || (hat & SDL_HAT_RIGHT))
		//replace:
		else if (SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX) > DEADZONE || SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
		//end
		{
			if(!(oldJoy&CONTROL_RT))
				keyTap|=CONTROL_RT;
			joyState|=CONTROL_RT;
		}

		//if(SDL_JoystickGetAxis(joystick, 1) < -DEADZONE || (hat & SDL_HAT_UP))
		//replace:
		if (SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY) < -DEADZONE || SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP))
		//end
		{
			if(!(oldJoy&CONTROL_UP))
				keyTap|=CONTROL_UP;
			joyState|=CONTROL_UP;
		}
		//else if(SDL_JoystickGetAxis(joystick, 1) > DEADZONE || (hat & SDL_HAT_DOWN))
		//replace:
		else if (SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY) > DEADZONE || SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
		//end
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
		if(SDL_JoystickGetButton(joystick, joyBtn[3]))
		{
			if(!(oldJoy&CONTROL_B4))
				keyTap|=CONTROL_B4;
			joyState|=CONTROL_B4;
		}
	}

	// Add newly connected joysticks.
	for (int i = joysticks.size(); i < SDL_NumJoysticks(); ++i) {
		//SDL_Joystick* joystick = SDL_JoystickOpen(i);
		//if (joystick) {
		//	LogDebug("Joystick added: %s", SDL_JoystickName(joystick));
		//	joysticks.push_back(joystick);
		//replace:
		owned::SDL_GameController gamepad = owned::SDL_GameControllerOpen(i);
		if (gamepad) {
			LogDebug("Gamepad added: %s", SDL_GameControllerName(gamepad.get()));
			joysticks.push_back(std::move(gamepad));
		//end
		}
	}

	oldJoy=joyState;

	return joyState;
}

// AA start delay fixes

bool GetGamepadStart()
{
	return gpstartdelay==0 && (GetGamepadButtons() & (1 << SDL_CONTROLLER_BUTTON_START));
}

void FreezeGamepadStart()
{
	gpstartdelay = 10;
}

void FreeGamepadStart()
{
	gpstartdelay = 0;
}

void AdvanceGamepadStart()
{
	if (gpstartdelay>0)
		--gpstartdelay;
}
