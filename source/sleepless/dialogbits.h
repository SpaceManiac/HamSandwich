#ifndef DIALOGBITS_H
#define DIALOGBITS_H

#include "world.h"
#include "map.h"
#include "game.h"

#define MAX_BUTTONS		128
#define BTN_TXT_LEN		32

#define BTN_NONE		0		// not in use
#define BTN_NORMAL		1		// regular clicky text button
#define BTN_CHECK		2		// checkbox
#define BTN_STATIC		3		// just static text
#define BTN_RADIO		4		// like a normal button, but it can be 'on' where it's lit up
								// when you turn one on, the others all turn off automatically
#define BTN_TILE		5		// a normal button, that shows a picture of a tile in it
#define BTN_TILE2		6		// a normal button, that shows a picture of a tile in it... but if it's tile 0, it's different
#define BTN_COLOR		7		// a normal button that shows a color in it (value 0-7)

// checkbox states
#define CHECK_OFF		0
#define CHECK_MIXED		1
#define CHECK_ON		2

typedef void (*buttonFunc)(int);

typedef struct button_t
{
	byte type;
	int id;
	byte on;
	int x,y;
	int width,height;
	char text[BTN_TXT_LEN];
	buttonFunc callback;
} button_t;

void ClearButtons(void);
void ClearButtons(int minID,int maxID);
byte MakeButton(byte type,int id,byte on,int x,int y,int width,int height,const char *txt,buttonFunc func);
void SetButtonState(int id,byte on);
byte GetButtonState(int id);
void CheckButtons(int mouseX,int mouseY);
void RenderButtons(int mouseX,int mouseY,MGLDraw *mgl);
byte CheckButton(int mouseX,int mouseY,int id);
byte CheckButtonCallback(int mouseX,int mouseY,int id,buttonFunc func);
void RenderButton(int mouseX,int mouseY,int id,MGLDraw *mgl);
void RadioOn(int id,int minID,int maxID);

byte PointInRect(int x,int y,int rx1,int ry1,int rx2,int ry2);
void RenderButtonImage(int mouseX,int mouseY,int x,int y,int width,int height, const char *txt);
void RenderButtonImageLit(int mouseX,int mouseY,int x,int y,int width,int height,const char *txt);
void RenderSliderImage(int mouseX,int mouseY,int x,int y,byte setting);
void RenderCheckbox(byte light,int x,int y,int v,const char *txt,MGLDraw *editmgl);

#endif
