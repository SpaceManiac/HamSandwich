#include "jamulspr.h"
#include "mgldraw.h"
#include "appdata.h"
#include "log.h"

// the sprites are 12 bytes, not including the data itself
#define SPRITE_INFO_SIZE 16

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
// ****************************** SPRITE_T *********************************
// -------------------------------------------------------------------------

// Helper shenanigans for C stuff

byte SprModifyColor(byte color, byte hue)
{
	return (hue << 5) | (color & 31);
}

byte SprGetColor(byte color)
{
	return (color >> 5);
}

byte SprModifyLight(byte color, char bright)
{
	byte value = (color & 31) + bright;
	if (value > 128) value = 0; // since byte is unsigned...
	else if (value > 31) value = 31;
	return (color & ~31) | value;
}

byte SprModifyGhost(byte src, byte dst, char bright)
{
	if (src >> 5 == 0)
	{
		return SprModifyLight(dst, src);
	}
	else
	{
		return SprModifyLight(src, bright);
	}
}

byte SprModifyGlow(byte src, byte dst, char bright)
{
	return SprModifyLight(src, (dst & 31) + bright);
}

static int constrainX=0,constrainY=0,constrainX2=639,constrainY2=479;

// CONSTRUCTORS & DESTRUCTORS
sprite_t::sprite_t(void)
{
	width=0;
	height=0;
	ofsx=0;
	ofsy=0;
	size=0;
	data=NULL;
}

sprite_t::sprite_t(byte *info)
{
	memcpy(&width,&info[0],2);
	memcpy(&height,&info[2],2);
	memcpy(&ofsx,&info[4],2);
	memcpy(&ofsy,&info[6],2);
	memcpy(&size,&info[8],4);
}

sprite_t::~sprite_t(void)
{
	if(data)
		free(data);
}

// REGULAR MEMBER FUNCTIONS
bool sprite_t::LoadData(SDL_RWops *f)
{
	if(size==0)
		return true;

	data=(byte *)malloc(size);
	if(!data)
		return false;

	if(SDL_RWread(f,data,1,size)!=size)
	{
		return false;
	}
	return true;
}

bool sprite_t::SaveData(FILE *f)
{
	if(size==0)
		return true;

	if(!data)
		return true;

	if(fwrite(data,1,size,f)!=size)
	{
		return false;
	}
	return true;
}

void sprite_t::GetHeader(byte *buffer)
{
	memcpy(&buffer[0],&width,2);
	memcpy(&buffer[2],&height,2);
	memcpy(&buffer[4],&ofsx,2);
	memcpy(&buffer[6],&ofsy,2);
	memcpy(&buffer[8],&size,4);
}

void sprite_t::GetCoords(int x,int y,int *rx,int *ry,int *rx2,int *ry2)
{
	*rx=x-ofsx;
	*ry=y-ofsy;
	*rx2=*rx+width;
	*ry2=*ry+height;
}

void sprite_t::Draw(int x, int y, MGLDraw *mgl)
{
	byte *src, *dst, b, skip;
	dword pitch;
	int srcx, srcy;
	byte noDraw;

	x -= ofsx;
	y -= ofsy;
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data;
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
			dst += pitch - width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

//   bright: how much to darken or lighten the whole thing (-16 to +16 reasonable)

void sprite_t::DrawBright(int x, int y, MGLDraw *mgl, char bright)
{
	byte *src, *dst, b, skip;
	dword pitch;
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
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data;
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
			dst += pitch - width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

//	 color:  which hue (0-7) to use for the entire thing, ignoring its real hue
//   bright: how much to darken or lighten the whole thing (-16 to +16 reasonable)

void sprite_t::DrawColored(int x, int y, MGLDraw *mgl, byte color, char bright)
{
	byte *src, *dst, b, skip;
	dword pitch;
	int srcx, srcy;
	byte noDraw;
	int i;

	x -= ofsx;
	y -= ofsy;
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data;
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
			dst += pitch - width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

void sprite_t::DrawOffColor(int x, int y, MGLDraw *mgl, byte fromColor, byte toColor, char bright)
{
	byte *src, *dst, b, skip;
	dword pitch;
	int srcx, srcy;
	byte noDraw;
	int i;

	x -= ofsx;
	y -= ofsy;
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data;
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
			dst += pitch - width;
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

void sprite_t::DrawGhost(int x, int y, MGLDraw *mgl, char bright)
{
	byte *src, *dst, b, skip;
	dword pitch;
	int srcx, srcy;
	byte noDraw;
	int i;

	x -= ofsx;
	y -= ofsy;
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data;
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
			dst += pitch - width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

void sprite_t::DrawGlow(int x, int y, MGLDraw *mgl, char bright)
{
	byte *src, *dst, b, skip;
	dword pitch;
	int srcx, srcy;
	byte noDraw;
	int i;

	x -= ofsx;
	y -= ofsy;
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data;
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
			dst += pitch - width;
			if (srcy >= constrainY)
				noDraw = 0;
			if (srcy > constrainY2)
				return;
		}
	}
}

void sprite_t::DrawShadow(int x, int y, MGLDraw *mgl)
{
	byte *src, *dst, b, skip;
	dword pitch;
	int srcx, srcy, x2;
	byte noDraw;
	byte alternate;
	int i;

	x -= ofsx + height / 2;
	y -= ofsy / 2;
	if (x > constrainX2 || y > constrainY2)
		return; // whole sprite is offscreen

	pitch = mgl->GetWidth();
	src = data;
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
			dst += (alternate ? pitch : 1) - width;
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
	count=0;
	spr=NULL;
}

sprite_set_t::sprite_set_t(const char *fname)
{
	count=0;
	spr=NULL;
	Load(fname);
}

sprite_set_t::~sprite_set_t(void)
{
	Free();
}

// REGULAR MEMBER FUNCTIONS
bool sprite_set_t::Load(const char *fname)
{
	int i;
	byte *buffer;

	if(spr)
		Free();

	SDL_RWops *f=AssetOpen_SDL(fname,"rb");
	if(!f) {
		LogError("%s: %s", fname, SDL_GetError());
		return false;
	}
	// read the count
	SDL_RWread(f, &count, 2, 1);

	spr=(sprite_t **)malloc(sizeof(sprite_t *)*count);
	if(!spr)
	{
		SDL_RWclose(f);
		return false;
	}

	// allocate a buffer to load sprites into
	buffer=(byte *)malloc(SPRITE_INFO_SIZE*count);
	if(!buffer)
	{
		SDL_RWclose(f);
		free(spr);
		spr=NULL;
		return false;
	}

	// read in the sprite headers
	if(SDL_RWread(f,buffer,SPRITE_INFO_SIZE,count)!=count)
	{
		SDL_RWclose(f);
		free(spr);
		free(buffer);
		spr=NULL;
		return false;
	}

	// allocate the sprites and read in the data for them
	for(i=0;i<count;i++)
	{
		spr[i]=new sprite_t(&buffer[i*SPRITE_INFO_SIZE]);
		if(!spr[i])
		{
			SDL_RWclose(f);
			return false;
		}
		if(!spr[i]->LoadData(f))
		{
			SDL_RWclose(f);
			spr[i]=NULL;
			return false;
		}
	}
	free(buffer);
	SDL_RWclose(f);
	return true;
}

bool sprite_set_t::Save(const char *fname)
{
	FILE *f;
	int i;
	byte *buffer;

	f=AssetOpen(fname,"wb");
	if(!f)
		return false;
	// write the count
	fwrite(&count,2,1,f);

	// allocate a buffer to copy sprites into
	buffer=(byte *)malloc(SPRITE_INFO_SIZE*count);
	if(!buffer)
	{
		fclose(f);
		return false;
	}

	for(i=0;i<count;i++)
		spr[i]->GetHeader(&buffer[i*SPRITE_INFO_SIZE]);

	// write the sprites out
	if(fwrite(buffer,SPRITE_INFO_SIZE,count,f)!=count)
	{
		fclose(f);
		free(buffer);
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

void sprite_set_t::Free(void)
{
	int i;
	if (!spr) return;

	for(i=0;i<count;i++)
		if (spr[i])
			delete spr[i];
	free(spr);
	spr=NULL;
}

sprite_t *sprite_set_t::GetSprite(int which)
{
	if(spr && which<count && spr[which])
		return spr[which];
	return NULL;
}

word sprite_set_t::GetCount(void)
{
	return count;
}

void SetSpriteConstraints(int x,int y,int x2,int y2)
{
	constrainX=x;
	constrainX2=x2;
	constrainY=y;
	constrainY2=y2;
}

void sprite_t::DrawC(int x,int y,MGLDraw *mgl)
{
	byte *src,*dst,b,skip;
	dword pitch;
	int srcx,srcy;
	byte noDraw;

	x-=ofsx;
	y-=ofsy;
	if(x>constrainX2 || y>constrainY2)
		return;	// whole sprite is offscreen

	pitch=mgl->GetWidth();
	src=data;
	dst=mgl->GetScreen()+x+y*pitch;

	srcx=x;
	srcy=y;
	if(srcy<constrainY)
		noDraw=1;
	else
		noDraw=0;
	while(srcy<height+y)
	{
		if((*src)&128)	// transparent run
		{
			b=(*src)&127;
			srcx+=b;
			dst+=b;
			src++;
		}
		else	// solid run
		{
			b=*src;
			src++;
			if(srcx<constrainX-b || srcx>constrainX2)
			{
				// don't draw this line
				src+=b;
				srcx+=b;
				dst+=b;
			}
			else if(srcx<constrainX)
			{
				// skip some of the beginning
				skip=(constrainX-srcx)-1;
				src+=skip;
				srcx+=skip;
				dst+=skip;
				b-=skip;
				if(srcx>constrainX2-b)
				{
					skip=(b-(constrainX2-srcx))-1;
					if(!noDraw)
						memcpy(dst,src,b-skip);
					src+=b;
					srcx+=b;
					dst+=b;
				}
				else
				{
					if(!noDraw)
						memcpy(dst,src,b);
					src+=b;
					srcx+=b;
					dst+=b;
				}
			}
			else if(srcx>constrainX2-b)
			{
				// skip some of the end
				skip=(srcx-(constrainX2-b))-1;
				if(!noDraw)
					memcpy(dst,src,b-skip);
				src+=b;
				srcx+=b;
				dst+=b;
			}
			else
			{
				// do it all!
				if(!noDraw)
					memcpy(dst,src,b);
				srcx+=b;
				src+=b;
				dst+=b;
			}
		}
		if(srcx>=width+x)
		{
			srcx=x;
			srcy++;
			dst+=pitch-width;
			if(srcy>=constrainY)
				noDraw=0;
			if(srcy>constrainY2)
				return;
		}
	}
}

void NewComputerSpriteFix(char *fname)
{
	int i;
	sprite_set_t *s;

	s=new sprite_set_t(fname);

	for(i=0;i<s->GetCount();i++)
	{
		s->GetSprite(i)->ofsx+=1;
		s->GetSprite(i)->ofsy-=2;
	}
	s->Save(fname);
	delete s;
}
