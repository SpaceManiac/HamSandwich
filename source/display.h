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

const int MAX_DISPLAY_OBJS = 1024;

const int DISPLAY_XBORDER = 128;
const int DISPLAY_YBORDER = 128;

// display object flags
enum {
    DISPLAY_DRAWME = 1,
    DISPLAY_SHADOW = 2,
    DISPLAY_WALLTILE = 4,
    DISPLAY_ROOFTILE = 8,
    DISPLAY_PARTICLE = 16,
    DISPLAY_GHOST = 32,
    DISPLAY_GLOW = 64,
    DISPLAY_TRANSTILE = 128,
    DISPLAY_LIGHTNING = 256,
    DISPLAY_OFFCOLOR = 512
};

typedef struct displayObj_t
{
	int x, y, z, z2;
	sprite_t *spr;
	byte hue;
	char bright;
	word flags;
	int prev, next;
} displayObj_t;

class DisplayList
{
public:
	DisplayList(void);
	~DisplayList(void);

	bool DrawSprite(int x, int y, int z, int z2, byte hue, char bright, sprite_t *spr, word flags);
	void ClearList(void);
	void Render(void);
private:

	void HookIn(int me);
	int GetOpenSlot(void);


	displayObj_t dispObj[MAX_DISPLAY_OBJS];
	int head, nextfree;
};

bool InitDisplay(MGLDraw *mainmgl);
void ExitDisplay(void);

byte *GetDisplayScreen(void);

void DrawMouseCursor(int x, int y);

void PutCamera(int x, int y);
void GetCamera(int *x, int *y);
// call this once per gameloop, with the X and Y of the object you want the camera to track
void UpdateCamera(int x, int y, byte facing, Map *map);
void Print(int x, int y, const char *s, char bright, byte font);
void CenterPrint(int x, int y, const char *s, char bright, byte font);
void RenderItAll(world_t *world, Map *map, byte flags);
int GetStrLength(const char *s);

void SprDraw(int x, int y, int z, byte hue, char bright, sprite_t *spr, word flags);
void SprDrawOff(int x, int y, int z, byte fromHue, byte hue, char bright, sprite_t *spr, word flags);

void WallDraw(int x, int y, byte wall, byte floor, Map* map, word flags);
void RoofDraw(int x, int y, byte roof, Map* map, word flags);
void ParticleDraw(int x, int y, int z, byte type, byte size, word flags);
void LightningDraw(int x, int y, int x2, int y2, byte bright, char range);

void MakeItFlip(void);
void DrawBox(int x, int y, int x2, int y2, byte c);
void DrawFillBox(int x, int y, int x2, int y2, byte c);
void DrawDebugBox(int x, int y, int x2, int y2);
void ShakeScreen(byte howlong);
void ShowVictoryAnim(byte world);
void ShowImageOrFlic(char *str);

MGLDraw *GetDisplayMGL(void);
byte GetGamma(void);
void SetGamma(byte g);

#endif
