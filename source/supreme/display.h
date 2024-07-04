#ifndef DISPLAY_H
#define DISPLAY_H

#include <string_view>
#include "mgldraw.h"
#include "jamulspr.h"
#include "jamulfont.h"
#include "cossin.h"
#include "map.h"
#include "tile.h"

/* This file handles all screen displaying.  It's a go-between so that you
   don't have to pass the mgldraw object everywhere, and also handles the display
   list and camera, so everything is drawn in sorted order (or not drawn). */

constexpr int MAX_DISPLAY_OBJS = 1024 * 4;

constexpr int DISPLAY_XBORDER = 128;
constexpr int DISPLAY_YBORDER = 128;

// Display object flags.
enum : word
{
	DISPLAY_DRAWME      = 1 << 0,
	DISPLAY_SHADOW      = 1 << 1,
	DISPLAY_WALLTILE    = 1 << 2,
	DISPLAY_ROOFTILE    = 1 << 3,
	DISPLAY_PARTICLE    = 1 << 4,
	DISPLAY_GHOST       = 1 << 5,
	DISPLAY_GLOW        = 1 << 6,
	DISPLAY_TRANSTILE   = 1 << 7,
	DISPLAY_LIGHTNING   = 1 << 8,
	DISPLAY_OFFCOLOR    = 1 << 9,
	DISPLAY_TILESPRITE  = 1 << 10,
};

// Text file display modes.
// SERIALIZED in special effect Show Pic/Movie.
enum : byte
{
	TEXTFILE_NORMAL     = 0,
	TEXTFILE_YERFDOG    = 1,
	TEXTFILE_COMPUTER   = 2,
	TEXTFILE_MAX
};

struct displayObj_t
{
	int x,y,z,z2;
	const sprite_t *spr;
	word hue;
	char bright;
	word flags;
	char light[9];
	int prev,next;
};

class DisplayList
{
	public:
		DisplayList(void);
		~DisplayList(void);

		bool DrawSprite(int x,int y,int z,int z2,word hue,char bright,const sprite_t *spr,word flags);
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
void Print(int x,int y,std::string_view s,char bright,byte font);
void PrintGlow(int x,int y,std::string_view s,char bright,byte font);
void PrintUnGlow(int x,int y,std::string_view s,byte font);
void PrintProgressiveGlow(int x,int y,std::string_view s,int bright,byte font);
void PrintRect(int x,int y,int x2,int y2,int height,std::string_view s,byte font);
void PrintGlowRect(int x,int y,int x2,int y2,int height,std::string_view s,byte font);
void PrintGlowRectBright(int x,int y,int x2,int y2,int height,std::string_view s,char bright,byte font);
void PrintUnGlowRect(int x,int y,int x2,int y2,int height,std::string_view s,byte font);
void PrintLimited(int x,int y,int maxX,std::string_view s,char bright,byte font);
void PrintGlowLimited(int x,int y,int maxX,std::string_view s,char bright,byte font);
void CenterPrint(int x,int y,std::string_view s,char bright,byte font);
void RenderItAll(world_t *world,Map *map,byte flags);
int GetStrLength(std::string_view s,byte font);

void SprDraw(int x,int y,int z,byte hue,char bright,const sprite_t *spr,word flags);
void SprDrawOff(int x,int y,int z,byte fromHue,byte hue,char bright,const sprite_t *spr,word flags);
void SprDrawTile(int x,int y,word tile,char light,word flags);	// use a tile as a sprite

void WallDraw(int x,int y,word wall,word floor,const char *light,word flags);
void RoofDraw(int x,int y,word roof,const char *light,word flags);
void ParticleDraw(int x,int y,int z,byte type,byte size,word flags);
void LightningDraw(int x,int y,int x2,int y2,byte bright,char range);

void DrawBox(int x,int y,int x2,int y2,byte c);
void DrawFillBox(int x,int y,int x2,int y2,byte c);
void DrawDebugBox(int x,int y,int x2,int y2);
void ShakeScreen(byte howlong);
TASK(void) ShowImageOrFlic(const char *str,byte nosnd,byte mode);

MGLDraw *GetDisplayMGL(void);
byte GetGamma(void);
void SetGamma(byte g);
void DrawLine(int x,int y,int x2,int y2,byte col);

#endif
