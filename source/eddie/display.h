#ifndef DISPLAY_H
#define DISPLAY_H

#include "mgldraw.h"
#include "jamulspr.h"

// Sprite sets
#define SPR_ROCKET 0
#define SPR_PEDE1  1
#define SPR_PEDE2  2
#define SPR_PEDE3  3
#define SPR_PEDE4  4
#define SPR_PEDE5  5
#define SPR_PEDE6  6
#define SPR_PEDE7  7
#define SPR_PEDE8  8
#define SPR_PEDE9  9
#define SPR_SHROOM 10
#define SPR_BULLET 11
#define SPR_MONSTER 12
#define SPR_STATUS 13
#define SPR_OPTION 14
#define SPR_LOGO   15
#define SPR_TITLE  16
#define SPR_QUEEN  17
#define MAX_SPR	   18

#define MAX_DISPLAY_OBJS 256

#define DISPLAY_XBORDER 128
#define DISPLAY_YBORDER 128

//display object flags
#define DISPLAY_DRAWME		1
#define DISPLAY_SHADOW		2
#define DISPLAY_GHOST		4
#define DISPLAY_GLOW		8
#define DISPLAY_TRANSTILE	16

typedef struct displayObj_t
{
	int x,y,z;
	sprite_t *spr;
	byte hue;
	char bright;
	byte flags;
	int prev,next;
} displayObj_t;

class DisplayList
{
	public:
		DisplayList(void);
		~DisplayList(void);

		bool DrawSprite(int x,int y,int z,byte hue,char bright,sprite_t *spr,byte flags);
		void ClearList(void);
		void Render(void);
	private:

		void HookIn(int me);
		int GetOpenSlot(void);
		

		displayObj_t dispObj[MAX_DISPLAY_OBJS];
		int head,nextfree;
};

void InitDisplay(MGLDraw *mgl);
void ExitDisplay(void);

byte FlipScreen(void);
void ClearScreen(void);

void SpriteRect(byte set,int s,int x,int y,int *rx,int *ry,int *rx2,int *ry2);
void Print(int x,int y,char *s,byte f);
void PrintBright(int x,int y,char *s,char brt,byte f);
void PrintGreen(int x,int y,char *s);
void CenterPrint(int y,char *s,byte f,char brt);
void CenterPrint2(int y,char *s,byte f,char brt);
void ColorCenterPrint(int y,char *s,byte f,byte col);
void PrintCursor(int x,int y,char *s,byte pos,byte blink,char bright,byte f);
int StringLength(char *s,byte f);

void SplashScreen(char *fname,int delay,byte sound,byte specialdeal);
void GetPaletteFromBMP(char *fname);

void Box(int x,int y,int x2,int y2,byte c);

void DrawBackgd(void);
void DrawStatus(void);
void SpecialDrawBackgd(int y);

void SprDraw(int x,int y,int z,byte hue,char bright,byte set,int s,byte flags);
void RenderAllSprites(void);
void RenderParticle(int x,int y,byte color,byte size);
void RenderGlowParticle(int x,int y,byte color,byte size);

void ShowSprite(byte set,int s,int x,int y);
void GlowSprite(byte set,int s,int x,int y,char bright);
void GhostSprite(byte set,int s,int x,int y,char bright);

// logo handler
void RenderLogo(void);
void RetractLogo(void);
void AdvanceLogo(void);

#endif