#include "jamulspr.h"
#include "mgldraw.h"
#include "appdata.h"
#include "log.h"
#include "recolor.h"
#include <limits.h>
#include <algorithm>

// the sprites are 12 bytes, not including the data itself
constexpr int SPRITE_INFO_SIZE = 16;

/*
Jamul Sprite - JSP

header:
count		1 word	how many frames in this sprite
data:
count structures:
	width	1 word		width of sprite in pixels
	height	1 word		height of sprite in pixels
	ofsX	1 short		x-coord of hotspot relative to left
	ofsY	1 short		y-coord of hotspot relative to top
	size	1 dword		how big the sprite data is in bytes
	data	size bytes	transparency RLE'd sprite data

	The RLE format is as follows:

	count	1 byte	if count is positive, this is considered
			a run of data, negative is a run of
			transparency.  If the run is data, it is
			followed by count bytes of data.  If
			it is transparent, the next RLE tag
			simply follows it.
			Runs do not cross line boundaries.
*/

// -------------------------------------------------------------------------
// Constraints

namespace
{
	int constrainX = 0;
	int constrainY = 0;
	// X2 and Y2 are inclusive.
	int constrainX2 = INT_MAX;
	int constrainY2 = INT_MAX;
}

void SetSpriteConstraints(int x,int y,int x2,int y2)
{
	constrainX = x;
	constrainY = y;
	constrainX2 = x2;
	constrainY2 = y2;
}

void ClearSpriteConstraints()
{
	constrainX = 0;
	constrainY = 0;
	constrainX2 = INT_MAX;
	constrainY2 = INT_MAX;
}

static inline int ResolveConstrainX2(MGLDraw* mgl)
{
	return std::min(mgl->GetWidth() - 1, constrainX2);
}

static inline int ResolveConstrainY2(MGLDraw* mgl)
{
	return std::min(mgl->GetHeight() - 1, constrainY2);
}

// -------------------------------------------------------------------------
// ****************************** SPRITE_T *********************************
// -------------------------------------------------------------------------

// CONSTRUCTORS & DESTRUCTORS
sprite_t::sprite_t(void)
	: width(0)
	, height(0)
	, ofsx(0)
	, ofsy(0)
	, size(0)
{
}

sprite_t::sprite_t(byte *info)
{
	memcpy(&width,&info[0],2);
	memcpy(&height,&info[2],2);
	memcpy(&ofsx,&info[4],2);
	memcpy(&ofsy,&info[6],2);
	memcpy(&size,&info[8],4);
}

// REGULAR MEMBER FUNCTIONS
bool sprite_t::LoadData(SDL_RWops *f)
{
	if(size==0)
		return true;

	data.resize(size);

	if(SDL_RWread(f,data.data(),1,size)!=size)
	{
		return false;
	}
	return true;
}

bool sprite_t::SaveData(FILE *f) const
{
	if(size==0)
		return true;

	if(data.empty())
		return true;

	if(fwrite(data.data(),1,size,f)!=size)
	{
		return false;
	}
	return true;
}

void sprite_t::GetHeader(byte *buffer) const
{
	memcpy(&buffer[0],&width,2);
	memcpy(&buffer[2],&height,2);
	memcpy(&buffer[4],&ofsx,2);
	memcpy(&buffer[6],&ofsy,2);
	memcpy(&buffer[8],&size,4);
}

void sprite_t::GetCoords(int x,int y,int *rx,int *ry,int *rx2,int *ry2) const
{
	*rx=x-ofsx;
	*ry=y-ofsy;
	*rx2=*rx+width;
	*ry2=*ry+height;
}

void sprite_t::Draw(int x, int y, MGLDraw *mgl) const
{
	const byte *src;
	byte *dst, b, skip;
	int pitch;
	int srcx, srcy;
	byte noDraw;

	x -= ofsx;
	y -= ofsy;
	int constrainX2 = ResolveConstrainX2(mgl);
	int constrainY2 = ResolveConstrainY2(mgl);
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data.data();
	dst = mgl->GetScreen() + x + y*pitch;

	srcx = x;
	srcy = y;
	if (srcy < constrainY)
		noDraw = 1;
	else
		noDraw = 0;
	while (srcy < height + y)
	{
		if ((*src)&128) // transparent run
		{
			b = (*src)&127;
			srcx += b;
			dst += b;
			src++;
		}
		else // solid run
		{
			b = *src;
			src++;
			if (srcx < constrainX - b || srcx > constrainX2)
			{
				// don't draw this line
				src += b;
				srcx += b;
				dst += b;
			}
			else if (srcx < constrainX)
			{
				// skip some of the beginning
				skip = (constrainX - srcx);
				src += skip;
				srcx += skip;
				dst += skip;
				b -= skip;
				if (srcx > constrainX2 - b)
				{
					skip = (b - (constrainX2 - srcx)) - 1;
					if (!noDraw)
						memcpy(dst, src, b - skip);
					src += b;
					srcx += b;
					dst += b;
				}
				else
				{
					if (!noDraw)
						memcpy(dst, src, b);
					src += b;
					srcx += b;
					dst += b;
				}
			}
			else if (srcx > constrainX2 - b)
			{
				// skip some of the end
				skip = (srcx - (constrainX2 - b)) - 1;
				if (!noDraw)
					memcpy(dst, src, b - skip);
				src += b;
				srcx += b;
				dst += b;
			}
			else
			{
				// do it all!
				if (!noDraw)
					memcpy(dst, src, b);
				srcx += b;
				src += b;
				dst += b;
			}
		}
		if (srcx >= width + x)
		{
			srcx = x;
			srcy++;
			dst += pitch;
			dst -= width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

//   bright: how much to darken or lighten the whole thing (-16 to +16 reasonable)

void sprite_t::DrawBright(int x, int y, MGLDraw *mgl, char bright) const
{
	const byte *src;
	byte *dst, b, skip;
	int pitch;
	int srcx, srcy;
	byte noDraw;
	int i;

	if (bright == 0)
	{ // don't waste time!
		Draw(x, y, mgl);
		return;
	}

	x -= ofsx;
	y -= ofsy;
	int constrainX2 = ResolveConstrainX2(mgl);
	int constrainY2 = ResolveConstrainY2(mgl);
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data.data();
	dst = mgl->GetScreen() + x + y*pitch;

	srcx = x;
	srcy = y;
	if (srcy < constrainY)
		noDraw = 1;
	else
		noDraw = 0;
	while (srcy < height + y)
	{
		if ((*src)&128) // transparent run
		{
			b = (*src)&127;
			srcx += b;
			dst += b;
			src++;
		}
		else // solid run
		{
			b = *src;
			src++;
			if (srcx < constrainX - b || srcx > constrainX2)
			{
				// don't draw this line
				src += b;
				srcx += b;
				dst += b;
			}
			else if (srcx < constrainX)
			{
				// skip some of the beginning
				skip = (constrainX - srcx);
				src += skip;
				srcx += skip;
				dst += skip;
				b -= skip;
				if (srcx > constrainX2 - b)
				{
					skip = (b - (constrainX2 - srcx)) - 1;
					if (!noDraw)
						for (i = 0; i < b - skip; ++i)
							dst[i] = SprModifyLight(src[i], bright);
					src += b;
					srcx += b;
					dst += b;
				}
				else
				{
					if (!noDraw)
						for (i = 0; i < b; ++i)
							dst[i] = SprModifyLight(src[i], bright);
					src += b;
					srcx += b;
					dst += b;
				}
			}
			else if (srcx > constrainX2 - b)
			{
				// skip some of the end
				skip = (srcx - (constrainX2 - b)) - 1;
				if (!noDraw)
					for (i = 0; i < b - skip; ++i)
						dst[i] = SprModifyLight(src[i], bright);
				src += b;
				srcx += b;
				dst += b;
			}
			else
			{
				// do it all!
				if (!noDraw)
					for (i = 0; i < b; ++i)
						dst[i] = SprModifyLight(src[i], bright);
				srcx += b;
				src += b;
				dst += b;
			}
		}
		if (srcx >= width + x)
		{
			srcx = x;
			srcy++;
			dst += pitch;
			dst -= width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

//	 color:  which hue (0-7) to use for the entire thing, ignoring its real hue
//   bright: how much to darken or lighten the whole thing (-16 to +16 reasonable)

void sprite_t::DrawColored(int x, int y, MGLDraw *mgl, byte color, char bright) const
{
	const byte *src;
	byte *dst, b, skip;
	int pitch;
	int srcx, srcy;
	byte noDraw;
	int i;

	x -= ofsx;
	y -= ofsy;
	int constrainX2 = ResolveConstrainX2(mgl);
	int constrainY2 = ResolveConstrainY2(mgl);
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data.data();
	dst = mgl->GetScreen() + x + y*pitch;

	srcx = x;
	srcy = y;
	if (srcy < constrainY)
		noDraw = 1;
	else
		noDraw = 0;
	while (srcy < height + y)
	{
		if ((*src)&128) // transparent run
		{
			b = (*src)&127;
			srcx += b;
			dst += b;
			src++;
		}
		else // solid run
		{
			b = *src;
			src++;
			if (srcx < constrainX - b || srcx > constrainX2)
			{
				// don't draw this line
				src += b;
				srcx += b;
				dst += b;
			}
			else if (srcx < constrainX)
			{
				// skip some of the beginning
				skip = (constrainX - srcx);
				src += skip;
				srcx += skip;
				dst += skip;
				b -= skip;
				if (srcx > constrainX2 - b)
				{
					skip = (b - (constrainX2 - srcx)) - 1;
					if (!noDraw)
						for (i = 0; i < b - skip; ++i)
							dst[i] = SprModifyLight(SprModifyColor(src[i], color), bright);
					src += b;
					srcx += b;
					dst += b;
				}
				else
				{
					if (!noDraw)
						for (i = 0; i < b; ++i)
							dst[i] = SprModifyLight(SprModifyColor(src[i], color), bright);
					src += b;
					srcx += b;
					dst += b;
				}
			}
			else if (srcx > constrainX2 - b)
			{
				// skip some of the end
				skip = (srcx - (constrainX2 - b)) - 1;
				if (!noDraw)
					for (i = 0; i < b - skip; ++i)
						dst[i] = SprModifyLight(SprModifyColor(src[i], color), bright);
				src += b;
				srcx += b;
				dst += b;
			}
			else
			{
				// do it all!
				if (!noDraw)
					for (i = 0; i < b; ++i)
						dst[i] = SprModifyLight(SprModifyColor(src[i], color), bright);
				srcx += b;
				src += b;
				dst += b;
			}
		}
		if (srcx >= width + x)
		{
			srcx = x;
			srcy++;
			dst += pitch;
			dst -= width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

void sprite_t::DrawOffColor(int x, int y, MGLDraw *mgl, byte fromColor, byte toColor, char bright) const
{
	const byte *src;
	byte *dst, b, skip;
	int pitch;
	int srcx, srcy;
	byte noDraw;
	int i;

	x -= ofsx;
	y -= ofsy;
	int constrainX2 = ResolveConstrainX2(mgl);
	int constrainY2 = ResolveConstrainY2(mgl);
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data.data();
	dst = mgl->GetScreen() + x + y*pitch;

	srcx = x;
	srcy = y;
	if (srcy < constrainY)
		noDraw = 1;
	else
		noDraw = 0;
	while (srcy < height + y)
	{
		if ((*src)&128) // transparent run
		{
			b = (*src)&127;
			srcx += b;
			dst += b;
			src++;
		}
		else // solid run
		{
			b = *src;
			src++;
			if (srcx < constrainX - b || srcx > constrainX2)
			{
				// don't draw this line
				src += b;
				srcx += b;
				dst += b;
			}
			else if (srcx < constrainX)
			{
				// skip some of the beginning
				skip = (constrainX - srcx);
				src += skip;
				srcx += skip;
				dst += skip;
				b -= skip;
				if (srcx > constrainX2 - b)
				{
					skip = (b - (constrainX2 - srcx)) - 1;
					if (!noDraw)
						for (i = 0; i < b - skip; ++i)
							dst[i] = SprModifyLight(SprGetColor(src[i]) == fromColor ? SprModifyColor(src[i], toColor) : src[i], bright);
					src += b;
					srcx += b;
					dst += b;
				}
				else
				{
					if (!noDraw)
						for (i = 0; i < b; ++i)
							dst[i] = SprModifyLight(SprGetColor(src[i]) == fromColor ? SprModifyColor(src[i], toColor) : src[i], bright);
					src += b;
					srcx += b;
					dst += b;
				}
			}
			else if (srcx > constrainX2 - b)
			{
				// skip some of the end
				skip = (srcx - (constrainX2 - b)) - 1;
				if (!noDraw)
					for (i = 0; i < b - skip; ++i)
						dst[i] = SprModifyLight(SprGetColor(src[i]) == fromColor ? SprModifyColor(src[i], toColor) : src[i], bright);
				src += b;
				srcx += b;
				dst += b;
			}
			else
			{
				// do it all!
				if (!noDraw)
					for (i = 0; i < b; ++i)
						dst[i] = SprModifyLight(SprGetColor(src[i]) == fromColor ? SprModifyColor(src[i], toColor) : src[i], bright);
				srcx += b;
				src += b;
				dst += b;
			}
		}
		if (srcx >= width + x)
		{
			srcx = x;
			srcy++;
			dst += pitch;
			dst -= width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

// a ghost sprite is rather special.  It is drawn normally (except lightened
// or darkened according to the brightness parameter), except where it is grey
// (color 1-31).  Wherever those colors occur, they are instead used as the
// degree to which the background should be brightened instead of drawn over.
//   bright: how much to darken or lighten the whole thing (-16 to +16 reasonable)
// Ghostly parts are NOT affected by brightness.
// Default in Spooky Castle, Lunatic, Supreme, Eddie, Mystic, Sleepless.
void sprite_t::DrawGhost(int x, int y, MGLDraw *mgl, char bright) const
{
	const byte *src;
	byte *dst, b, skip;
	int pitch;
	int srcx, srcy;
	byte noDraw;
	int i;

	x -= ofsx;
	y -= ofsy;
	int constrainX2 = ResolveConstrainX2(mgl);
	int constrainY2 = ResolveConstrainY2(mgl);
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data.data();
	dst = mgl->GetScreen() + x + y*pitch;

	srcx = x;
	srcy = y;
	if (srcy < constrainY)
		noDraw = 1;
	else
		noDraw = 0;
	while (srcy < height + y)
	{
		if ((*src)&128) // transparent run
		{
			b = (*src)&127;
			srcx += b;
			dst += b;
			src++;
		}
		else // solid run
		{
			b = *src;
			src++;
			if (srcx < constrainX - b || srcx > constrainX2)
			{
				// don't draw this line
				src += b;
				srcx += b;
				dst += b;
			}
			else if (srcx < constrainX)
			{
				// skip some of the beginning
				skip = (constrainX - srcx);
				src += skip;
				srcx += skip;
				dst += skip;
				b -= skip;
				if (srcx > constrainX2 - b)
				{
					skip = (b - (constrainX2 - srcx)) - 1;
					if (!noDraw)
						for (i = 0; i < b - skip; ++i)
							dst[i] = SprModifyGhost(src[i], dst[i], bright);
					src += b;
					srcx += b;
					dst += b;
				}
				else
				{
					if (!noDraw)
						for (i = 0; i < b; ++i)
							dst[i] = SprModifyGhost(src[i], dst[i], bright);
					src += b;
					srcx += b;
					dst += b;
				}
			}
			else if (srcx > constrainX2 - b)
			{
				// skip some of the end
				skip = (srcx - (constrainX2 - b)) - 1;
				if (!noDraw)
					for (i = 0; i < b - skip; ++i)
						dst[i] = SprModifyGhost(src[i], dst[i], bright);
				src += b;
				srcx += b;
				dst += b;
			}
			else
			{
				// do it all!
				if (!noDraw)
					for (i = 0; i < b; ++i)
						dst[i] = SprModifyGhost(src[i], dst[i], bright);
				srcx += b;
				src += b;
				dst += b;
			}
		}
		if (srcx >= width + x)
		{
			srcx = x;
			srcy++;
			dst += pitch;
			dst -= width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

// See DrawGhost above.
// Ghostly parts ARE affected by brightness.
// Default in Stockboy, Loonyland, Loonyland 2.
void sprite_t::DrawGhostBright(int x, int y, MGLDraw *mgl, char bright) const
{
	const byte *src;
	byte *dst, b, skip;
	int pitch;
	int srcx, srcy;
	byte noDraw;
	int i;

	x -= ofsx;
	y -= ofsy;
	int constrainX2 = ResolveConstrainX2(mgl);
	int constrainY2 = ResolveConstrainY2(mgl);
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data.data();
	dst = mgl->GetScreen() + x + y*pitch;

	srcx = x;
	srcy = y;
	if (srcy < constrainY)
		noDraw = 1;
	else
		noDraw = 0;
	while (srcy < height + y)
	{
		if ((*src)&128) // transparent run
		{
			b = (*src)&127;
			srcx += b;
			dst += b;
			src++;
		}
		else // solid run
		{
			b = *src;
			src++;
			if (srcx < constrainX - b || srcx > constrainX2)
			{
				// don't draw this line
				src += b;
				srcx += b;
				dst += b;
			}
			else if (srcx < constrainX)
			{
				// skip some of the beginning
				skip = (constrainX - srcx);
				src += skip;
				srcx += skip;
				dst += skip;
				b -= skip;
				if (srcx > constrainX2 - b)
				{
					skip = (b - (constrainX2 - srcx)) - 1;
					if (!noDraw)
						for (i = 0; i < b - skip; ++i)
							dst[i] = SprModifyGhostBright(src[i], dst[i], bright);
					src += b;
					srcx += b;
					dst += b;
				}
				else
				{
					if (!noDraw)
						for (i = 0; i < b; ++i)
							dst[i] = SprModifyGhostBright(src[i], dst[i], bright);
					src += b;
					srcx += b;
					dst += b;
				}
			}
			else if (srcx > constrainX2 - b)
			{
				// skip some of the end
				skip = (srcx - (constrainX2 - b)) - 1;
				if (!noDraw)
					for (i = 0; i < b - skip; ++i)
						dst[i] = SprModifyGhostBright(src[i], dst[i], bright);
				src += b;
				srcx += b;
				dst += b;
			}
			else
			{
				// do it all!
				if (!noDraw)
					for (i = 0; i < b; ++i)
						dst[i] = SprModifyGhostBright(src[i], dst[i], bright);
				srcx += b;
				src += b;
				dst += b;
			}
		}
		if (srcx >= width + x)
		{
			srcx = x;
			srcy++;
			dst += pitch;
			dst -= width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

void sprite_t::DrawGlow(int x, int y, MGLDraw *mgl, char bright) const
{
	const byte *src;
	byte *dst, b, skip;
	int pitch;
	int srcx, srcy;
	byte noDraw;
	int i;

	x -= ofsx;
	y -= ofsy;
	int constrainX2 = ResolveConstrainX2(mgl);
	int constrainY2 = ResolveConstrainY2(mgl);
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data.data();
	dst = mgl->GetScreen() + x + y*pitch;

	srcx = x;
	srcy = y;
	if (srcy < constrainY)
		noDraw = 1;
	else
		noDraw = 0;
	while (srcy < height + y)
	{
		if ((*src)&128) // transparent run
		{
			b = (*src)&127;
			srcx += b;
			dst += b;
			src++;
		}
		else // solid run
		{
			b = *src;
			src++;
			if (srcx < constrainX - b || srcx > constrainX2)
			{
				// don't draw this line
				src += b;
				srcx += b;
				dst += b;
			}
			else if (srcx < constrainX)
			{
				// skip some of the beginning
				skip = (constrainX - srcx);
				src += skip;
				srcx += skip;
				dst += skip;
				b -= skip;
				if (srcx > constrainX2 - b)
				{
					skip = (b - (constrainX2 - srcx)) - 1;
					if (!noDraw)
						for (i = 0; i < b - skip; ++i)
							dst[i] = SprModifyGlow(src[i], dst[i], bright);
					src += b;
					srcx += b;
					dst += b;
				}
				else
				{
					if (!noDraw)
						for (i = 0; i < b; ++i)
							dst[i] = SprModifyGlow(src[i], dst[i], bright);
					src += b;
					srcx += b;
					dst += b;
				}
			}
			else if (srcx > constrainX2 - b)
			{
				// skip some of the end
				skip = (srcx - (constrainX2 - b)) - 1;
				if (!noDraw)
					for (i = 0; i < b - skip; ++i)
						dst[i] = SprModifyGlow(src[i], dst[i], bright);
				src += b;
				srcx += b;
				dst += b;
			}
			else
			{
				// do it all!
				if (!noDraw)
					for (i = 0; i < b; ++i)
						dst[i] = SprModifyGlow(src[i], dst[i], bright);
				srcx += b;
				src += b;
				dst += b;
			}
		}
		if (srcx >= width + x)
		{
			srcx = x;
			srcy++;
			dst += pitch;
			dst -= width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

void sprite_t::DrawShadow(int x, int y, MGLDraw *mgl) const
{
	const byte *src;
	byte *dst, b, skip;
	int pitch;
	int srcx, srcy, x2;
	byte noDraw;
	byte alternate;
	int i;

	x -= ofsx + height / 2;
	y -= ofsy / 2;
	int constrainX2 = ResolveConstrainX2(mgl);
	int constrainY2 = ResolveConstrainY2(mgl);
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data.data();
	dst = mgl->GetScreen() + x + y*pitch;

	srcx = x;
	srcy = y;
	if (srcy < constrainY)
		noDraw = 1;
	else
		noDraw = 0;

	alternate = 1;
	x2 = x;

	while (srcy < height / 2 + y)
	{
		if ((*src)&128) // transparent run
		{
			b = (*src)&127;
			srcx += b;
			dst += b;
			src++;
		}
		else // solid run
		{
			b = *src;
			src++;
			if (srcx < constrainX - b || srcx > constrainX2)
			{
				// don't draw this line
				src += b;
				srcx += b;
				dst += b;
			}
			else if (srcx < constrainX)
			{
				// skip some of the beginning
				skip = (constrainX - srcx);
				src += skip;
				srcx += skip;
				dst += skip;
				b -= skip;
				if (srcx > constrainX2 - b)
				{
					skip = (b - (constrainX2 - srcx)) - 1;
					if (!noDraw && alternate)
						for (i = 0; i < b - skip; ++i)
							dst[i] = SprModifyLight(dst[i], -4);
					src += b;
					srcx += b;
					dst += b;
				}
				else
				{
					if (!noDraw && alternate)
						for (i = 0; i < b; ++i)
							dst[i] = SprModifyLight(dst[i], -4);
					src += b;
					srcx += b;
					dst += b;
				}
			}
			else if (srcx > constrainX2 - b)
			{
				// skip some of the end
				skip = (srcx - (constrainX2 - b)) - 1;
				if (!noDraw && alternate)
					for (i = 0; i < b - skip; ++i)
						dst[i] = SprModifyLight(dst[i], -4);
				src += b;
				srcx += b;
				dst += b;
			}
			else
			{
				// do it all!
				if (!noDraw && alternate)
					for (i = 0; i < b; ++i)
						dst[i] = SprModifyLight(dst[i], -4);
				srcx += b;
				src += b;
				dst += b;
			}
		}
		if (srcx >= width + x2)
		{
			alternate = 1 - alternate;
			x2 += alternate;
			srcx -= width - alternate;
			srcy += alternate;
			dst += (alternate ? pitch : 1);
			dst -= width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

// -------------------------------------------------------------------------
// ***************************** SPRITE_SET_T ******************************
// -------------------------------------------------------------------------

// CONSTRUCTORS & DESTRUCTORS
sprite_set_t::sprite_set_t(void)
{
}

sprite_set_t::sprite_set_t(const char *fname)
{
	Load(fname);
}

// REGULAR MEMBER FUNCTIONS
bool sprite_set_t::Load(const char *fname)
{
	int i;

	spr.clear();

	SDL_RWops *f=AssetOpen_SDL(fname);
	if(!f) {
		// Asset stack printed error already
		return false;
	}
	// read the count
	word count;
	SDL_RWread(f, &count, 2, 1);

	spr.resize(count);

	// allocate a buffer to load sprites into
	std::vector<byte> buffer(SPRITE_INFO_SIZE*count);

	// read in the sprite headers
	if(SDL_RWread(f,buffer.data(),SPRITE_INFO_SIZE,count)!=count)
	{
		SDL_RWclose(f);
		spr.clear();
		return false;
	}

	// allocate the sprites and read in the data for them
	for(i=0;i<count;i++)
	{
		spr[i] = std::make_unique<sprite_t>(&buffer[i*SPRITE_INFO_SIZE]);
		if(!spr[i]->LoadData(f))
		{
			SDL_RWclose(f);
			spr[i] = nullptr;
			return false;
		}
	}
	SDL_RWclose(f);
	return true;
}

bool sprite_set_t::Save(const char *fname) const
{
	FILE *f;
	word i;

	f=AssetOpen_Write(fname);
	if(!f)
		return false;
	// write the count
	word count = spr.size();
	fwrite(&count,2,1,f);

	// allocate a buffer to copy sprites into
	std::vector<byte> buffer(SPRITE_INFO_SIZE*count);

	for(i=0;i<count;i++)
		spr[i]->GetHeader(&buffer[i*SPRITE_INFO_SIZE]);

	// write the sprites out
	if(fwrite(buffer.data(),SPRITE_INFO_SIZE,count,f)!=count)
	{
		fclose(f);
		return false;
	}

	// write the sprite data
	for(i=0;i<count;i++)
	{
		if(!spr[i]->SaveData(f))
		{
			fclose(f);
			return false;
		}
	}
	fclose(f);
	AppdataSync();
	return true;
}

sprite_t* sprite_set_t::GetSprite(int which)
{
	if (which >= 0 && (size_t)which < spr.size() && spr[which])
		return spr[which].get();
	return nullptr;
}

const sprite_t* sprite_set_t::GetSprite(int which) const
{
	if (which >= 0 && (size_t)which < spr.size() && spr[which])
		return spr[which].get();
	return nullptr;
}

word sprite_set_t::GetCount(void) const
{
	return spr.size();
}

void NewComputerSpriteFix(char *fname)
{
	int i;

	sprite_set_t s { fname };

	for(i=0;i<s.GetCount();i++)
	{
		s.GetSprite(i)->ofsx+=1;
		s.GetSprite(i)->ofsy-=2;
	}
	s.Save(fname);
}
