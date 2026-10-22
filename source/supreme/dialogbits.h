#ifndef DIALOGBITS_H
#define DIALOGBITS_H

#include "game.h"
#include "map.h"
#include "world.h"

enum ButtonType : byte
{
	BTN_NONE,   // not in use
	BTN_NORMAL, // regular clicky text button
	BTN_CHECK,  // checkbox
	BTN_STATIC, // just static text
	BTN_RADIO,  // like a normal button, but it can be 'on' where it's lit up. when you turn one on, the others all turn off automatically
	BTN_TILE,   // a normal button, that shows a picture of a tile in it
	BTN_TILE2,  // a normal button, that shows a picture of a tile in it... but if it's tile 0, it's different
	BTN_COLOR,  // a normal button that shows a color in it (value 0-7)
};

// checkbox states
enum : byte
{
	CHECK_OFF,
	CHECK_MIXED,
	CHECK_ON,
};

typedef void (*buttonFunc)(int);

void ClearButtons(void);
void ClearButtons(int minID, int maxID);
byte MakeButton(ButtonType type, int id, byte on, int x, int y, int width, int height, std::string_view txt, buttonFunc func);
void SetButtonState(int id, byte on);
byte GetButtonState(int id);
void CheckButtons(int mouseX, int mouseY);
void RenderButtons(int mouseX, int mouseY, MGLDraw *mgl);
byte CheckButton(int mouseX, int mouseY, int id);
byte CheckButtonCallback(int mouseX, int mouseY, int id, buttonFunc func);
void RenderButton(int mouseX, int mouseY, int id, MGLDraw *mgl);
void RadioOn(int id, int minID, int maxID);

byte PointInRect(int x, int y, int rx1, int ry1, int rx2, int ry2);
void RenderButtonImage(int mouseX, int mouseY, int x, int y, int width, int height, std::string_view txt);
void RenderButtonImageLit(int mouseX, int mouseY, int x, int y, int width, int height, std::string_view txt);
void RenderSliderImage(int mouseX, int mouseY, int x, int y, byte setting);
void RenderCheckbox(byte light, int x, int y, int v, std::string_view txt, MGLDraw *editmgl);

#endif
