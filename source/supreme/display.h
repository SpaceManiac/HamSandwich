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

#define DISPLAY_XBORDER 128
#define DISPLAY_YBORDER 128

//display object flags
#define DISPLAY_DRAWME		1
#define DISPLAY_SHADOW		2
#define DISPLAY_WALLTILE	4
#define DISPLAY_ROOFTILE	8
#define DISPLAY_PARTICLE	16
#define DISPLAY_GHOST		32
#define DISPLAY_GLOW		64
#define DISPLAY_TRANSTILE	128
#define DISPLAY_LIGHTNING	256
#define DISPLAY_OFFCOLOR	512
#define DISPLAY_TILESPRITE	1024
#define DISPLAY_CIRCLEPART	2048 // circle particle


// text file display modes
#define TEXTFILE_NORMAL		0
#define TEXTFILE_YERFDOG	1
#define TEXTFILE_COMPUTER	2
#define TEXTFILE_MAX		3

typedef struct displayObj_t
{
	int x,y,z,z2;
	sprite_t *spr;
	word hue;
	char bright;
	word flags;
	char light[9];
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

byte *GetDisplayScreen(void);

void DrawMouseCursor(int x,int y);

void PutCamera(int x,int y);
void GetCamera(int *x,int *y);
// call this once per gameloop, with the X and Y of the object you want the camera to track
void UpdateCamera(int x,int y,int dx,int dy,Map *map);
void Print(int x,int y,const char *s,char bright,byte font);
void PrintGlow(int x,int y,const char *s,char bright,byte font);
void PrintUnGlow(int x,int y,const char *s,byte font);
void PrintProgressiveGlow(int x,int y,const char *s,int bright,byte font);
void PrintRect(int x,int y,int x2,int y2,int height,const char *s,byte font);
void PrintGlowRect(int x,int y,int x2,int y2,int height,const char *s,byte font);
void PrintGlowRectBright(int x,int y,int x2,int y2,int height,const char *s,char bright,byte font);
void PrintUnGlowRect(int x,int y,int x2,int y2,int height,const char *s,byte font);
void PrintLimited(int x,int y,int maxX,const char *s,char bright,byte font);
void PrintGlowLimited(int x,int y,int maxX,const char *s,char bright,byte font);
void CenterPrint(int x,int y,const char *s,char bright,byte font);
void RenderItAll(world_t *world,Map *map,byte flags);
int GetStrLength(const char *s,byte font);

void SprDraw(int x,int y,int z,byte hue,char bright,sprite_t *spr,word flags);
void SprDrawOff(int x,int y,int z,byte fromHue,byte hue,char bright,sprite_t *spr,word flags);
void SprDrawTile(int x,int y,word tile,char light,word flags);	// use a tile as a sprite

void WallDraw(int x,int y,word wall,word floor,const char *light,word flags);
void RoofDraw(int x,int y,word roof,const char *light,word flags);
void ParticleDraw(int x,int y,int z,byte type,byte size,word flags);
void LightningDraw(int x,int y,int x2,int y2,byte bright,char range);

void DrawBox(int x,int y,int x2,int y2,byte c);
void DrawFillBox(int x,int y,int x2,int y2,byte c);
void DrawDebugBox(int x,int y,int x2,int y2);
void ShakeScreen(byte howlong);
void ShowImageOrFlic(char *str,byte nosnd,byte mode);

MGLDraw *GetDisplayMGL(void);
byte GetGamma(void);
void SetGamma(byte g);
void DrawLine(int x,int y,int x2,int y2,byte col);

#endif
