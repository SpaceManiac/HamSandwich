#ifndef DISPLAY_H
#define DISPLAY_H

#include "mgldraw.h"
#include "jamulspr.h"
#include "jamulfont.h"
#include "cossin.h"
#include "map.h"
#include "tile.h"

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
#define DISPLAY_TEXTPART	4096
#define DISPLAY_ARROW		8192
#define DISPLAY_COLORBLIND	16384

typedef struct displayObj_t
{
	int x,y,z,z2;
	sprite_t *spr;
	word hue;
	char bright;
	word flags;
	int prev,next;
} displayObj_t;

class DisplayList
{
	public:
		DisplayList(void);
		~DisplayList(void);

		bool DrawSprite(int x,int y,int z,int z2,word hue,char bright,sprite_t *spr,word flags);
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

void SetDisplayMGL(MGLDraw *mainmgl);

byte *GetDisplayScreen(void);

void DrawMouseCursor(int x,int y);

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
void RightPrintColor(int x,int y,const char *s,byte color,char bright,byte font);
void RightPrintGlow(int x,int y,const char *s,char bright,byte font);
void PrintLimited(int x,int y,int maxX,const char *s,char bright,byte font);
void PrintOutline(int x,int y,const char *s,char bright,byte font);
void CenterPrintOutline(int x,int y,const char *s,char bright,byte font);
void RvsCenterPrintOutline(int x,int y,const char *s,char bright,byte font);
void RightPrintOutline(int x,int y,const char *s,char bright,byte font);
void PrintUnGlow(int x,int y,const char *s,byte font);
void CenterPrintLimit(int x,int y,int maxX,const char *s,char bright,byte font);
void PrintAngleUnGlow(int x,int y,const char *s,byte font);
void CenterPrintUnGlow(int x,int y,const char *s,byte font);
void RightPrintUnGlow(int x,int y,const char *s,byte font);
void PrintAngle2(int x,int y,const char *s,byte font);
void PrintUnGlowSideways(int x,int y,int minY,const char *s,byte font);

void RenderItAll(world_t *world,Map *map,byte flags);
int GetStrLength(const char *s,byte f);

void SprDraw(int x,int y,int z,byte hue,char bright,sprite_t *spr,word flags);
void OffSprDraw(int x,int y,int z,byte hue,byte hue2,char bright,sprite_t *spr,word flags);
void WallDraw(int x,int y,word wall,word floor,char light,word flags);
void RoofDraw(int x,int y,word roof,char light,word flags);
void ParticleDraw(int x,int y,int z,byte type,byte size,word flags);
void LightningDraw(int x,int y,int x2,int y2,byte bright,char range);
void PlotPixel(int x,int y,byte c,byte *scrn,int pitch);

void MakeItFlip(void);
void DrawBox(int x,int y,int x2,int y2,byte c);
void DrawFillBox(int x,int y,int x2,int y2,byte c);
void DrawDebugBox(int x,int y,int x2,int y2);
void ShakeScreen(byte howlong);
void ShowVictoryAnim(byte world);
void ShowImageOrFlic(char *str);
void FilmWiggle(byte howlong);

MGLDraw *GetDisplayMGL(void);
byte GetGamma(void);
void SetGamma(byte g);
void DarkenScreen(byte dark);

#endif
