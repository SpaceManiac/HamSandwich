#include "jamulfmv.h"
#include "mgldraw.h"
#include "clock.h"
#include "appdata.h"
#include <stdio.h>

// different kinds of flic chunks
enum
{
	FLI_COLOR       = 11,
	FLI_LC          = 12,
	FLI_BLACK       = 13,
	FLI_BRUN        = 15,
	FLI_COPY        = 16,
	FLI_DELTA       = 7,
	FLI_256_COLOR   = 4,
	FLI_MINI        = 18,
};

struct fliheader
{
	int32_t size;
	word magic;  // FLC=0xAF12 (FLI=0xAF11, but no games use this)
	word frames;
	word width,height;
	word depth;  // ignored, they're all 8-bit
	word flags;  // ignored, always 3, whatever that means
	word speed;  // milliseconds per frame
	int32_t next,frit;  // meaning unknown
};
static_assert(sizeof(fliheader) == 28, "sizeof(fliheader) is incorrect for direct i/o");
static_assert(SDL_BYTEORDER == SDL_LIL_ENDIAN, "HamSandwich currently only supports little-endian processors");

struct frmheader
{
	int32_t size;
	word magic;    /* always $F1FA */
	word chunks;
	byte expand[8];
};
static_assert(sizeof(frmheader) == 16, "sizeof(frmheader) is incorrect for direct i/o");

// because of padding, the following 6-byte header is 8 bytes.
// therefore use this define instead of sizeof() to read from a FLIc
constexpr int sizeofchunkheader = 6;

struct chunkheader
{
	int32_t size;
	word kind;
};

static RGB FLI_pal[256];
static word	fliWidth,fliHeight;

//------------------------------------------------------------------------------

static void FLI_docolor2(byte *p,MGLDraw *mgl)
{
	word numpak;
	word pos=0;
	byte palpos=0;
	byte numcol;

	memcpy(&numpak,p,2);
	pos=2;
	while(numpak>0) {
		numpak--;
		palpos+=p[pos++];
		numcol=p[pos++];
		do {
			FLI_pal[palpos].r = p[pos++];
			FLI_pal[palpos].g = p[pos++];
			FLI_pal[palpos].b = p[pos++];
			palpos++;
			--numcol;
		} while(numcol>0);
	}
	// apply the palette here
	mgl->SetPalette(FLI_pal);
	mgl->RealizePalette();
}

static void FLI_docolor(byte *p,MGLDraw *mgl)
{
	// docolor2 and docolor are supposed to be different, but they aren't
	FLI_docolor2(p,mgl);
}

static void FLI_doDelta(byte *scrn,int scrWidth,byte *p)
{
	short numLines,numPaks;
	int pos,x,y;
	char sizeCount;
	word v;

	/*
	   Bit 15   Bit 14   Description
	   0        0      Packet count for the line, it can be zero
	   0        1      Undefined
	   1        0      Store the opcode's low byte in the last pixel of the line
	   1        1      The absolute value of the opcode is the line skip count
	*/
	memcpy(&numLines,p,2);
	pos=2;
	y=0;
	while(numLines>0)
	{
		x=0;
		numLines--;
		numPaks=(word)65535;
		while(numPaks&0xC000)	// while it doesn't have 00 as the top two bits
		{
			memcpy(&numPaks,&p[pos],2);
			pos+=2;
			switch(numPaks&0xC000)
			{
				case 0xC000:	// both bits set, this is a skip
					y-=numPaks;
					break;
				case 0x4000:	// bottom bit, undefined
					//numPaks=numPaks;
					break;
				case 0x8000:	// top bit, supposedly last pixel thing
					//numPaks=numPaks;
					break;
			}
		}

		while(numPaks>0)
		{
			x+=p[pos++];
			sizeCount=(char)p[pos++];
			if(sizeCount>0)	// copy sizeCount words
			{
				memcpy(&scrn[scrWidth*y+x],&p[pos],sizeCount*2);
				pos+=sizeCount*2;
				x+=sizeCount*2;
			}
			else if(sizeCount<0) // copy the word value -sizeCount times
			{
				memcpy(&v,&p[pos],2);
				while(sizeCount<0)
				{
					sizeCount++;
					memcpy(&scrn[scrWidth*y+x],&v,2);
					x+=2;
				}
				x-=2*sizeCount;
				pos+=2;
			}
			numPaks--;
		}
		y++;
	}
}

static void FLI_doLC(byte *scrn,int scrWidth,byte *p)
{
	word numln;
	word x,y;
	byte packets,skip;
	char size;
	size_t pos=0;

	memcpy(&y,&p[pos++],2);
	pos++;
	memcpy(&numln,&p[pos++],2);
	pos++;
	while(numln>0) {
		numln--;
		packets=p[pos++];
		x=0;
		while(packets>0) {
			packets--;
			skip=p[pos++];
			size=p[pos++];
			x+=skip;
			if(size<0) {
				memset(&scrn[x+y*scrWidth],p[pos],-size);
				pos++;
				x-=size;
			}
			if(size>0) {
				memcpy(&scrn[x+y*scrWidth],&p[pos],size);
				pos+=size;
				x+=size;
			}
		}
		y++;
	}
}

static void FLI_doBRUN(byte *scrn,int scrWidth,byte *p)
{
	for (word y = 0; y < fliHeight; ++y) {
		word x = 0;
		// In some movies, this number of packets is correct, but in movies
		// like LL2/ending.flc and Factory.flc, it is 255, meaning "entire
		// screen width", so just do that always, because it always works.
		p++;
		while (x < scrWidth) {
			char size = *p++;
			if(size >= 0) {
				memset(&scrn[x+y*scrWidth], *p++, size);
				x += size;
			} else {
				memcpy(&scrn[x+y*scrWidth], p, -size);
				p += -size;
				x += -size;
			}
		}
	}
}

static void FLI_nextchunk(SDL_RWops* FLI_file, MGLDraw *mgl, int scrWidth)
{
	chunkheader chead;

	SDL_RWread(FLI_file, &chead,1,sizeofchunkheader);
	if(chead.kind==FLI_COPY)
		chead.size=fliWidth*fliHeight+sizeofchunkheader;	// a hack to make up for a bug in Animator?

	std::vector<byte> buffer(chead.size-sizeofchunkheader, 0);
	byte *p = buffer.data();
	SDL_RWread(FLI_file, p,1,chead.size-sizeofchunkheader);

	switch(chead.kind)
	{
		case FLI_COPY:
			memcpy(mgl->GetScreen(), p, fliHeight * fliWidth);
			break;
		case FLI_BLACK:
			mgl->ClearScreen();
			break;
		case FLI_COLOR:
			FLI_docolor(p,mgl);
			break;
		case FLI_LC:
			FLI_doLC(mgl->GetScreen(),scrWidth,p);
			break;
		case FLI_BRUN:
			FLI_doBRUN(mgl->GetScreen(),scrWidth,p);
			break;
		case FLI_MINI:
			break; // ignore it
		case FLI_DELTA:
			FLI_doDelta(mgl->GetScreen(),scrWidth,p);
			break;
		case FLI_256_COLOR:
			FLI_docolor2(p,mgl);
			break;
	}
}

static void FLI_nextfr(SDL_RWops* FLI_file, MGLDraw *mgl, int scrWidth)
{
	long start = SDL_RWtell(FLI_file);

	frmheader fhead;
	SDL_RWread(FLI_file,&fhead,1,sizeof(frmheader));

	// check to see if this is a FLC file's special frame... if it is, skip it
	if (fhead.magic == 0xF1FA)
	{
		for(int i=0; i < fhead.chunks; i++)
			FLI_nextchunk(FLI_file, mgl, scrWidth);
	}
	// Other possible value of "magic" is 0x00A1, indicating the FLC file's
	// special frame, but we already skip over that with the seek in FLI_play.

	// Some movies (TWOLCREDITS.flc) have padding at the end of the frame,
	// after its chunks, so it needs to be skipped.
	SDL_RWseek(FLI_file, start + fhead.size, RW_SEEK_SET);
}

TASK(byte) FLI_play(const char *name, byte loop, word wait, MGLDraw *mgl, FlicCallBack callback)
{
	int frmon=0;
	fliheader FLI_hdr;
	int scrWidth;
	char k;
	dword playbackTime, currentTime;

	owned::SDL_RWops FLI_file = AssetOpen_SDL_Owned(name);
	if (!FLI_file)
	{
		// Asset stack printed error already
		CO_RETURN 0;
	}

	// Read the main part of the header.
	SDL_RWread(FLI_file, &FLI_hdr, 1, sizeof(fliheader));
	fliWidth = FLI_hdr.width;
	fliHeight = FLI_hdr.height;

	// "wait" can be overridden, but defaults to the value from the file.
	if (!wait)
		wait = FLI_hdr.speed;

	// Resizing the buffer here means that MGLDraw will handle the upscaling.
	int oldWidth = mgl->GetWidth(), oldHeight = mgl->GetHeight();
	mgl->ResizeBuffer(fliWidth, fliHeight);

	// At this offset in the header is the offset of the first "real" frame.
	// In older FLCs (everything except LL2's ending.flc), there is a dummy
	// frame between the header and this location that must be skipped.
	int32_t ofs1;
	SDL_RWseek(FLI_file, 80, RW_SEEK_SET);
	SDL_RWread(FLI_file, &ofs1, 4, 1);
	// There is also the offset of the second frame following this, but all
	// released movies have it at its expected location.

	// The end of the header is at 128 bytes. In some movies, the real first
	// frame is there, but in others there's a dummy frame with information
	// we don't care about. Luckily ofs1 points to the real first frame.
	SDL_RWseek(FLI_file, ofs1, RW_SEEK_SET);

	mgl->LastKeyPressed();	// clear key buffer

	playbackTime = timeGetTime() - 1;
	do
	{
		currentTime = timeGetTime();

		// If vsync time is faster than FLC framerate, idle.
		while (currentTime < playbackTime)
		{
			SDL_Delay((playbackTime - currentTime) / 2);
			currentTime = timeGetTime();
		}

		// If the FLC's framerate is faster than vsync time, skip frames.
		while (playbackTime < currentTime)
		{
			frmon++;
			scrWidth=mgl->GetWidth();
			FLI_nextfr(FLI_file.get(), mgl, scrWidth);
			if (callback && !callback(frmon))
				break;
			if((loop)&&(frmon==FLI_hdr.frames+1))
			{
				frmon=1;
				SDL_RWseek(FLI_file, ofs1, RW_SEEK_SET);
			}
			if((!loop)&&(frmon==FLI_hdr.frames))
				frmon=FLI_hdr.frames+1;

			playbackTime += wait;
		}

		AWAIT mgl->Flip();
		k=mgl->LastKeyPressed();
		// key #27 is escape
	} while((frmon<FLI_hdr.frames+1)&&(mgl->Process()) && (k!=27));

	FLI_file.reset();
	mgl->ResizeBuffer(oldWidth, oldHeight);

	CO_RETURN k != 27;
}
