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

#define DISPLAY_XBORDER 300
#define DISPLAY_YBORDER 300

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
	word hue;
	char bright;
	byte flags;
	int prev,next;
	char light[9];
} displayObj_t;

class DisplayList
{
	public:
		DisplayList(void);
		~DisplayList(void);

		bool DrawSprite(int x,int y,int z,word hue,char bright,sprite_t *spr,byte flags);
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
void CenterPrint(int midx,int y,const char *s,char bright,byte font);
void PrintGlow(int x,int y,const char *s,byte font);
void RightPrintGlow(int x,int y,const char *s,byte font);
void CenterPrintGlow(int y,const char *s,byte font);
void PrintBright(int x,int y,const char *s,char bright,byte font);
void PrintBrightGlow(int x,int y,const char *s,char brt,byte font);
void RenderItAll(world_t *world,Map *map,byte flags);
int GetStrLength(const char *s,byte fnt);

void SprDraw(int x,int y,int z,byte hue,char bright,sprite_t *spr,byte flags);
void WallDraw(int x,int y,word wall,word floor,const char *light,byte flags);
void RoofDraw(int x,int y,word roof,const char *light,byte flags);

void ParticleDraw(int x,int y,int z,byte type,byte size,byte flags);

void MakeItFlip(void);
void DrawBox(int x,int y,int x2,int y2,byte c);
void DrawFillBox(int x,int y,int x2,int y2,byte c);
void DrawDebugBox(int x,int y,int x2,int y2);
void ShakeScreen(byte howlong);
void ShowVictoryAnim(byte world);
MGLDraw *GetDisplayMGL(void);
byte GetGamma(void);
void SetGamma(byte g);

void EndingMovie(int frm);

#endif
