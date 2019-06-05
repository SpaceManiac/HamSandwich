#ifndef DISPLAY_H
#define DISPLAY_H

#include "mgldraw.h"
#include "jamulspr.h"
#include "jamulfont.h"
#include "cossin.h"
#include "map.h"
#include "tile.h"
#pragma pack(1)

/* This file handles all screen displaying.  It's a go-between so that you
   don't have to pass the mgldraw object everywhere, and also handles the display
   list and camera, so everything is drawn in sorted order (or not drawn). */

#define MAX_DISPLAY_OBJS 1024

#define DISPLAY_XBORDER 256
#define DISPLAY_YBORDER 256

//display object flags
#define DISPLAY_DRAWME		1
#define DISPLAY_SHADOW		2
#define DISPLAY_WALLTILE	4
#define DISPLAY_ROOFTILE    8
#define DISPLAY_PARTICLE	16
#define DISPLAY_GHOST		32
#define DISPLAY_GLOW		64
#define DISPLAY_TRANSTILE	128
#define DISPLAY_LIGHTNING	256
#define DISPLAY_CIRCLEPART	512	// circle particle
#define DISPLAY_OFFCOLOR	1024
#define DISPLAY_RINGPART	2048	// ring particle
#define DISPLAY_BOTTOM		4096	// sort it to the bottom, in with the shadows (it's on the floor)
#define DISPLAY_NUMBER		8192	// render a printed number
#define DISPLAY_CLOCK		16384	// add clock hands!

typedef struct displayObj_t
{
	int x,y,z,z2;
	sprite_t *spr;
	word hue;
	int bright;
	word flags;
	int prev,next;
	char light[9];
} displayObj_t;

class DisplayList
{
	public:
		DisplayList(void);
		~DisplayList(void);

		bool DrawSprite(int x,int y,int z,int z2,word hue,int bright,sprite_t *spr,word flags);
		void ClearList(void);
		void Render(void);
	private:

		void HookIn(int me);
		int GetOpenSlot(void);


		displayObj_t dispObj[MAX_DISPLAY_OBJS];
		int head,nextfree;
};

bool InitDisplay(MGLDraw *mainmgl);
void ExitDisplay(void);

byte *GetDisplayScreen(void);

void DrawMouseCursor(int x,int y);

void PutCamera(int x,int y);
void GetCamera(int *x,int *y);
// call this once per gameloop, with the X and Y of the object you want the camera to track
void UpdateCamera(int x,int y,byte facing,Map *map);
void Print(int x,int y,const char *s,char bright,byte font);
void CenterPrint(int x,int y,const char *s,char bright,byte font);
void RightPrint(int x,int y,const char *s,char bright,byte font);
void PrintColor(int x,int y,const char *s,byte color,char bright,byte font);
void PrintGlow(int x,int y,const char *s,char bright,byte font);
void CenterPrintGlow(int x,int y,const char *s,char bright,byte font);
void CenterPrintColor(int x,int y,const char *s,byte color,char bright,byte font);
void CenterPrintDark(int x,int y,const char *s,byte font);
void RightPrintColor(int x,int y,const char *s,byte color,char bright,byte font);
void RightPrintGlow(int x,int y,const char *s,char bright,byte font);
void RightPrintDark(int x,int y,const char *s,byte font);
void PrintDark(int x,int y,const char *s,byte font);
void PrintRectBlack(int x,int y,int x2,int y2,const char *s,byte height,int bright,byte font);
void PrintDarkAdj(int x,int y,const char *s,char bright,byte font);
void PrintRectBlack2(int x,int y,int x2,int y2,const char *s,byte height,byte font);

void RenderItAll(world_t *world,Map *map,byte flags);
int GetStrLength(const char *s,byte f);

void SprDraw(int x,int y,int z,byte hue,char bright,sprite_t *spr,word flags);
void OffSprDraw(int x,int y,int z,byte hue,byte hue2,char bright,sprite_t *spr,word flags);
void WallDraw(int x,int y,word wall,word floor,const char *light,word flags);
void RoofDraw(int x,int y,word roof,const char *light,word flags);
void ParticleDraw(int x,int y,int z,byte type,byte size,word flags);
void LightningDraw(int x,int y,int x2,int y2,byte bright,char range);
void PlotPixel(int x,int y,byte c,byte *scrn,int pitch);
void NumberDraw(int x,int y,int z,byte type,byte color,short num,word flags);

void MakeItFlip(void);
void DrawBox(int x,int y,int x2,int y2,byte c);
void DrawFillBox(int x,int y,int x2,int y2,byte c);
void DrawDebugBox(int x,int y,int x2,int y2);
void ShakeScreen(byte howlong);
void ShowImageOrFlic(char *str);
void FilmWiggle(byte howlong);

MGLDraw *GetDisplayMGL(void);
byte GetGamma(void);
void SetGamma(byte g);
void DarkenScreen(byte dark);
void DrawLine(int x,int y,int x2,int y2,byte col);

#endif
