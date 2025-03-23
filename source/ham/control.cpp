#include "control.h"
#include <math.h>
#include <vector>
#include <algorithm>
#include <utility>
#include "mgldraw.h"
#include "log.h"
#include "softjoystick.h"
#include "owned_sdl.h"

static constexpr int NUM_KEYBOARDS = 4;

static byte lastScanCode;

// normal mapped controls: kbd, joystick, etc.
static dword keyState, keyTap;
static dword keyState2[NUM_KEYBOARDS];
// fixed arrow keys and return controls
static dword arrowState, arrowTap;
static dword lockOut;

// joysticks
static std::vector<owned::SDL_GameController> joysticks;
static SDL_GameController *activeController = nullptr;
static byte oldJoy;

// mappings
static byte kb[NUM_CONTROLS][NUM_KEYBOARDS];
static byte joyBtn[NUM_CONTROLS];
static byte playerUsesJoystick[NUM_KEYBOARDS] = { 3, 3, 3, 3 };

static byte GetJoyState();
static byte controlScheme;

static dword rawGamepad;

static bool dpadToMove = true;
static byte stickDeadZone;	// measured in pct
static bool playerInMenu = false;

static dword controls, taps, menuTap, menuControls, menuGamepad;
static byte reptCounter;

void InitControls(byte scheme)
{
	controlScheme = scheme;
	lockOut = 0;
	lastScanCode=0;
	keyState=0;
	keyTap=0;
	menuGamepad = 0;
	menuControls = 0;
	arrowState=0;
	arrowTap=0;
	oldJoy=0;
	dpadToMove = true;
	stickDeadZone = 50;
	controls = 0;
	taps = 0;
	reptCounter = 0;
	memset(keyState2, 0, sizeof(keyState2));
}

byte ControlScheme(void)
{
	return controlScheme;
}

byte GetKeyControls()
{
	return keyState;
}

byte GetControls()
{
	return keyState | GetJoyState() | SoftJoystickState();
}

byte GetPlayerControls(byte player)
{
	switch (playerUsesJoystick[player])
	{
		case 0:  // Keyboard1
			return keyState2[0] | SoftJoystickState();
		case 1:  // Keyboard2
			return keyState2[1];
		case 2:  // Joystick
			return GetJoyState();
		default:  // ???
			return GetControls();
	}
}

byte GetTaps()
{
	GetJoyState();  // Updates keyTap.
	byte result = keyTap;
	keyTap = 0;
	return result | SoftJoystickTaps();
}

byte GetArrows()
{
	return arrowState;
}

byte GetArrowTaps()
{
	byte result = arrowTap;
	arrowTap = 0;
	return result;
}

byte LastScanCode()
{
	byte c = lastScanCode;
	lastScanCode = 0;
	return c;
}

const char *ScanCodeText(byte s) {
	return SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode) s));
}

// Gamepad special
dword GetJoyButtons() {
	dword held = 0;

	for (owned::SDL_GameController& gamepad : joysticks) {
		SDL_Joystick* joystick = SDL_GameControllerGetJoystick(gamepad.get());
		int n = SDL_JoystickNumButtons(joystick);
		int j = 1;
		for (int i = 0; i < n && i < 32; ++i) {
			if (SDL_JoystickGetButton(joystick, i)) {
				held |= j;
			}
			j *= 2;
		}
	}

	return held;
}

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

void GetLeftStick(int16_t* x, int16_t* y, byte* dpad)
{
	*dpad = 0;
	int32_t xx = 0, yy = 0;
	for (auto iter = joysticks.begin(); iter != joysticks.end(); ++iter)
	{
		SDL_GameController* gamepad = iter->get();
		SDL_Joystick* joystick = SDL_GameControllerGetJoystick(gamepad);
		if (!SDL_JoystickGetAttached(joystick))
		{
			// Drop disconnected joysticks.
			LogDebug("Gamepad removed: %s", SDL_GameControllerName(gamepad));
			if (activeController == gamepad)
				activeController = nullptr;
			iter = joysticks.erase(iter);
			if (iter == joysticks.end())
				break;
			continue;
		}

		xx += SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX);
		yy += SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY);

		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
		{
			*dpad|=CONTROL_LF;
		}
		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
		{
			*dpad|=CONTROL_RT;
		}

		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP))
		{
			*dpad|=CONTROL_UP;
		}
		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
		{
			*dpad|=CONTROL_DN;
		}

		if(SDL_JoystickGetButton(joystick, joyBtn[0]))
		{
			*dpad|=CONTROL_B1;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[1]))
		{
			*dpad|=CONTROL_B2;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[2]))
		{
			*dpad|=CONTROL_B3;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[3]))
		{
			*dpad|=CONTROL_B4;
		}
	}
	*x = int16_t(std::clamp(xx, int32_t(INT16_MIN), int32_t(INT16_MAX)));
	*y = int16_t(std::clamp(yy, int32_t(INT16_MIN), int32_t(INT16_MAX)));
}

bool ShowGamepadText()
{
	return activeController != nullptr;
}

// Options menu support.
void SetKeyboardBindings(int keyboard, int nkeys, const byte* keys)
{
	nkeys = std::min(nkeys, (int)NUM_CONTROLS);
	for (int i = 0; i < nkeys; ++i) {
		kb[i][keyboard] = keys[i];
	}
}

void SetJoystickBindings(int nbuttons, const byte* buttons)
{
	SoftJoystickNumButtons(nbuttons);
	memcpy(joyBtn, buttons, std::min(nbuttons, (int)NUM_CONTROLS));
}

void ControlSetUseJoystick(byte player, byte joystickNumber)
{
	playerUsesJoystick[player] = joystickNumber;
}

// Called upon SDL events
void ControlKeyDown(SDL_Scancode k)
{
	int i,j;
	dword bit;

	lastScanCode=k;

	for(i=0;i<NUM_KEYBOARDS;i++)
	{
		bit=1;
		for(j=0;j<NUM_CONTROLS;j++)
		{
			if(k==kb[j][i])
			{
				if (playerInMenu && (k == SDL_SCANCODE_UP || k == SDL_SCANCODE_DOWN || k == SDL_SCANCODE_LEFT || k == SDL_SCANCODE_RIGHT || k == SDL_SCANCODE_ESCAPE || k == SDL_SCANCODE_RETURN))
				{
					// skip any key that overlaps the hardcoded menu controls
				}
				else
				{
					keyState |= bit;
					keyTap |= bit;
					keyState2[i] |= bit;
				}
			}
			bit*=2;
		}
	}

	// always track arrows, no matter what the keys are, for menus
	switch(k)
	{
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
		case SDL_SCANCODE_RETURN:
			arrowState |= CONTROL_B1;
			arrowTap |= CONTROL_B1;
			break;
		case SDL_SCANCODE_ESCAPE:
			arrowState |= CONTROL_ESCAPE;
			arrowTap |= CONTROL_ESCAPE;
			break;
		default: break;
	}
}

void ControlKeyUp(SDL_Scancode k)
{
	int i,j;
	dword bit;

	for(i=0;i<NUM_KEYBOARDS;i++)
	{
		bit=1;
		for(j=0;j<NUM_CONTROLS;j++)
		{
			if(k==kb[j][i])
			{
				keyState&=(~bit);
				keyState2[i] &= ~bit;
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
		case SDL_SCANCODE_ESCAPE:
			arrowState &= ~CONTROL_ESCAPE;
			break;
		default: break;
	}
}

void ControlHandleNewGamepad(int which)
{
	owned::SDL_GameController gamepad = owned::SDL_GameControllerOpen(which);
	if (gamepad)
	{
		LogDebug("Gamepad added: %s", SDL_GameControllerName(gamepad.get()));
		activeController = gamepad.get();
		joysticks.push_back(std::move(gamepad));
	}
}

void ControlHandleEvent(const SDL_Event &e)
{
	if (e.type == SDL_KEYDOWN)
	{
		ControlKeyDown(e.key.keysym.scancode);
		activeController = nullptr;
	}
	else if (e.type == SDL_KEYUP)
	{
		ControlKeyUp(e.key.keysym.scancode);
		activeController = nullptr;
	}
	else if (e.type == SDL_CONTROLLERDEVICEADDED)
	{
		ControlHandleNewGamepad(e.cdevice.which);
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		activeController = nullptr;
	}
	else if (e.type == SDL_CONTROLLERBUTTONDOWN)
	{
		activeController = SDL_GameControllerFromInstanceID(e.cbutton.which);
	}
}

static byte GetJoyState(void)
{
	constexpr int DEADZONE = 8192;
	byte allJoyState = 0;

	for (auto iter = joysticks.begin(); iter != joysticks.end(); ++iter)
	{
		byte joyState = 0;
		SDL_GameController* gamepad = iter->get();
		SDL_Joystick* joystick = SDL_GameControllerGetJoystick(gamepad);
		if (!SDL_JoystickGetAttached(joystick))
		{
			// Drop disconnected joysticks.
			LogDebug("Gamepad removed: %s", SDL_GameControllerName(gamepad));
			if (activeController == gamepad)
				activeController = nullptr;
			iter = joysticks.erase(iter);
			if (iter == joysticks.end())
				break;
			continue;
		}

		int leftX = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX);
		int leftY = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY);
		if (leftX * leftX + leftY * leftY > DEADZONE * DEADZONE)
		{
			double angle = atan2(leftY, leftX) * 180.0 / M_PI;
			if (angle < 22.5 + -4*45) {
				joyState |= CONTROL_LF;
			} else if (angle < 22.5 + -3*45) {
				joyState |= CONTROL_LF | CONTROL_UP;
			} else if (angle < 22.5 + -2*45) {
				joyState |= CONTROL_UP;
			} else if (angle < 22.5 + -1*45) {
				joyState |= CONTROL_UP | CONTROL_RT;
			} else if (angle < 22.5 + 0*45) {
				joyState |= CONTROL_RT;
			} else if (angle < 22.5 + 1*45) {
				joyState |= CONTROL_RT | CONTROL_DN;
			} else if (angle < 22.5 + 2*45) {
				joyState |= CONTROL_DN;
			} else if (angle < 22.5 + 3*45) {
				joyState |= CONTROL_DN | CONTROL_LF;
			} else if (angle < 22.5 + 4*45) {
				joyState |= CONTROL_LF;
			}
		}

		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
		{
			joyState|=CONTROL_LF;
		}
		else if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
		{
			joyState|=CONTROL_RT;
		}

		if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP))
		{
			joyState|=CONTROL_UP;
		}
		else if(SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
		{
			joyState|=CONTROL_DN;
		}

		if(SDL_JoystickGetButton(joystick, joyBtn[0]))
		{
			joyState|=CONTROL_B1;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[1]))
		{
			joyState|=CONTROL_B2;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[2]))
		{
			joyState|=CONTROL_B3;
		}
		if(SDL_JoystickGetButton(joystick, joyBtn[3]))
		{
			joyState|=CONTROL_B4;
		}

		if (joyState)
		{
			activeController = gamepad;
		}
		allJoyState |= joyState;
	}

	keyTap |= allJoyState & ~oldJoy;
	oldJoy = allJoyState;
	return allJoyState;
}

SDL_GameController* ActiveController()
{
	return activeController;
}

//------------------------ MYSTIC MODE
void SetInMenu(bool inMenu)
{
	playerInMenu = inMenu;
}

bool ButtonHeld(dword c)
{
	return (playerInMenu && (menuControls & c)) || (controls & c);
}

bool AutoRepeatTapped(dword c)
{
	if ((taps & c) || (playerInMenu && (menuTap&c)))
	{
		reptCounter = CONTROL_REPEAT_FRAMES;
		return true;
	}
	if (reptCounter == 0 && ((controls & c) || (playerInMenu && (menuControls & c))))
	{
		reptCounter = CONTROL_REPEAT_FRAMES;
		return true;
	}
	return false;
}

bool ButtonTapped(dword c)
{
	if (taps & c) return true;
	return playerInMenu && (menuTap & c);
}

dword RawGamepadToControls(void)
{
	dword c = 0;
	for (int i = 0; i < NUM_CONTROLS; i++)
	{
		if (joyBtn[i] != 255 && (rawGamepad & (1 << joyBtn[i])))
			c|=(1<<i);
	}
	return c;
}

dword GetRawGamepad(void)
{
	return rawGamepad;
}

void UpdateControls(void)
{
	dword oldMenu = menuControls;
	byte oldControls = controls;
	taps = 0;
	menuTap = 0;
	UpdateRawJoystick();
	controls = keyState | RawGamepadToControls();
	menuControls = arrowState | menuGamepad;

	if (reptCounter > 0)
		reptCounter--;

	for (int i = 0; i < NUM_CONTROLS; i++)
	{
		if (i <= CTL_ID_ESCAPE)	// menu keys are only in the first 8 controls
		{
			if (!(oldMenu & (1 << i)) && (menuControls & (1 << i)))
				menuTap |= (1 << i);
		}
		if (!(oldControls & (1 << i)) && (controls & (1 << i)))
			taps |= (1 << i);	// if this is the first frame the control is down, then it is a tap
		if(!(controls&(1<<i)))
			lockOut&=(~(1<<i));	// if you've released a locked-out key, then we unlock it
	}
	if ((oldControls == 0 && menuControls==0) || reptCounter >= CONTROL_REPEAT_FRAMES)
		reptCounter = 0;

	controls &= (~lockOut);
	taps &= (~lockOut);
	menuTap &= (~lockOut);
	menuControls &= (~lockOut);
}

void UpdateRawJoystick(void)
{
	int lx, ly, rx, ry;

	rawGamepad = 0;
	lx = ly = rx = ry = 0;
	int ltpos = 0, rtpos = 0;
	menuGamepad = 0;
	for (auto iter = joysticks.begin(); iter != joysticks.end(); ++iter)
	{
		SDL_GameController* gamepad = iter->get();
		SDL_Joystick* joystick = SDL_GameControllerGetJoystick(gamepad);
		if (!SDL_JoystickGetAttached(joystick))
		{
			// Drop disconnected joysticks.
			LogDebug("Gamepad removed: %s", SDL_GameControllerName(gamepad));
			if (activeController == gamepad)
				activeController = nullptr;
			iter = joysticks.erase(iter);
			if (iter == joysticks.end())
				break;
			continue;
		}
		for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
		{
			if (SDL_GameControllerGetButton(gamepad, (SDL_GameControllerButton)i))
			{
				rawGamepad |= (1 << i);
				if (i == SDL_CONTROLLER_BUTTON_DPAD_UP)
					menuGamepad |= CONTROL_UP;
				if (i == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
					menuGamepad |= CONTROL_DN;
				if (i == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
					menuGamepad |= CONTROL_LF;
				if (i == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
					menuGamepad |= CONTROL_RT;
				if (i == SDL_CONTROLLER_BUTTON_START)
					menuGamepad |= CONTROL_ESCAPE;

				if (i == SDL_CONTROLLER_BUTTON_A)
					menuGamepad |= CONTROL_B1;
				if (i == SDL_CONTROLLER_BUTTON_B)
					menuGamepad |= CONTROL_B2;
			}
		}

		lx += SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX);
		ly += SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY);
		rx += SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_RIGHTX);
		ry += SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_RIGHTY);
		ltpos += SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
		rtpos += SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
	}
	short deadZone;
	deadZone = SDL_JOYSTICK_AXIS_MAX * (short)stickDeadZone / 100;
	if (lx < -deadZone || (dpadToMove && (rawGamepad & (1 << SDL_CONTROLLER_BUTTON_DPAD_LEFT))))
	{
		rawGamepad |= (1 << (RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_LF));
		menuGamepad |= CONTROL_LF;
	}
	if (ly < -deadZone || (dpadToMove && (rawGamepad & (1 << SDL_CONTROLLER_BUTTON_DPAD_UP))))
	{
		rawGamepad |= (1 << (RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_UP));
		menuGamepad |= CONTROL_UP;
	}
	if (lx > deadZone || (dpadToMove && (rawGamepad & (1 << SDL_CONTROLLER_BUTTON_DPAD_RIGHT))))
	{
		rawGamepad |= (1 << (RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_RT));
		menuGamepad |= CONTROL_RT;
	}
	if (ly > deadZone || (dpadToMove && (rawGamepad & (1 << SDL_CONTROLLER_BUTTON_DPAD_DOWN))))
	{
		rawGamepad |= (1 << (RAWGAMEPADAXIS_BASE + RawGamepadAxis::LS_DN));
		menuGamepad |= CONTROL_DN;
	}
	if (rx < -deadZone)
		rawGamepad |= (1 << (RAWGAMEPADAXIS_BASE + RawGamepadAxis::RS_LF));
	if (ry < -deadZone)
		rawGamepad |= (1 << (RAWGAMEPADAXIS_BASE + RawGamepadAxis::RS_UP));
	if (rx > deadZone)
		rawGamepad |= (1 << (RAWGAMEPADAXIS_BASE + RawGamepadAxis::RS_RT));
	if (ry > deadZone)
		rawGamepad |= (1 << (RAWGAMEPADAXIS_BASE + RawGamepadAxis::RS_DN));
	if (ltpos > SDL_JOYSTICK_AXIS_MAX / 2)	// don't use dead zone for triggers, just halfway pressed is it
		rawGamepad |= (1 << (RAWGAMEPADAXIS_BASE + RawGamepadAxis::LT));
	if (rtpos > SDL_JOYSTICK_AXIS_MAX / 2)
		rawGamepad |= (1 << (RAWGAMEPADAXIS_BASE + RawGamepadAxis::RT));

	// if you're in a menu, disable any gamepad buttons that overlap the menu buttons
	if (playerInMenu)
		rawGamepad &= ~((1 << SDL_CONTROLLER_BUTTON_DPAD_DOWN) |
			(1 << SDL_CONTROLLER_BUTTON_DPAD_LEFT) |
			(1 << SDL_CONTROLLER_BUTTON_DPAD_UP) |
			(1 << SDL_CONTROLLER_BUTTON_DPAD_RIGHT) |
			(1 << SDL_CONTROLLER_BUTTON_A) |
			(1 << SDL_CONTROLLER_BUTTON_START) |
			(1 << SDL_CONTROLLER_BUTTON_B));
}

void LockOutControl(dword c,bool locked)
{
	if (locked)
		lockOut |= c;
	else
		lockOut &= (~c);
}

void SetDpadToMode(bool yep)
{
	dpadToMove = yep;
}
