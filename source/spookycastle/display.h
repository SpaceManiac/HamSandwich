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

#define MAX_DISPLAY_OBJS 512

#define DISPLAY_XBORDER 128
#define DISPLAY_YBORDER 128

//display object flags
#define DISPLAY_DRAWME		1
#define DISPLAY_SHADOW		2
#define DISPLAY_WALLTILE	4
#define DISPLAY_ROOFTILE    8
#define DISPLAY_PARTICLE	16
#define DISPLAY_GHOST		32
#define DISPLAY_GLOW		64
#define DISPLAY_TRANSTILE	128

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

bool InitDisplay(MGLDraw *mainmgl);
void ExitDisplay(void);

byte *GetDisplayScreen(void);

void DrawMouseCursor(int x,int y);

void PutCamera(int x,int y);
void GetCamera(int *x,int *y);
// call this once per gameloop, with the X and Y of the object you want the camera to track
void UpdateCamera(int x,int y,byte facing,Map *map);
void Print(int x,int y,const char *s,char bright,byte font);
void CenterPrint(int y,const char *s,char bright,byte font);
void RenderItAll(world_t *world,Map *map,byte flags);
int GetStrLength(const char *s);

void SprDraw(int x,int y,int z,byte hue,char bright,sprite_t *spr,byte flags);
void WallDraw(int x,int y,byte wall,byte floor,char light,byte flags);
void RoofDraw(int x,int y,byte roof,char light,byte flags);
void ParticleDraw(int x,int y,int z,byte type,byte size,byte flags);

void DrawBox(int x,int y,int x2,int y2,byte c);
void DrawFillBox(int x,int y,int x2,int y2,byte c);
void DrawDebugBox(int x,int y,int x2,int y2);
void ShakeScreen(byte howlong);
TASK(void) ShowVictoryAnim(byte world);
MGLDraw *GetDisplayMGL(void);
byte GetGamma(void);
void SetGamma(byte g);

#endif
