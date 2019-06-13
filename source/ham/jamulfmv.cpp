#include "jamulfmv.h"
#include "mgldraw.h"
#include "clock.h"
#include "appdata.h"
#include <stdio.h>

// different kinds of flic chunks
#define FLI_COLOR		11
#define FLI_LC			12
#define FLI_BLACK		13
#define FLI_BRUN		15
#define FLI_COPY		16
#define FLI_DELTA		7
#define FLI_256_COLOR	4
#define FLI_MINI		18

struct fliheader
{
	long size;
	word magic;
	word frames;
	word width,height;
	word flags;
	word speed;
	long next,frit;
	byte expand[104];
};

struct frmheader
{
	long size;
	word magic;    /* always $F1FA */
	word chunks;
	byte expand[8];
};

// because of padding, the following 6-byte header is 8 bytes.
// therefore use this define instead of sizeof() to read from a FLIc
#define sizeofchunkheader 6

struct chunkheader
{
	long size;
	word kind;
};

static SDL_RWops* FLI_file;
static RGB FLI_pal[256];
static word	fliWidth,fliHeight;

//------------------------------------------------------------------------------

static void PlotSolidRun(int x,int y,int len,byte *scrn,int scrWidth,byte c)
{
	int i;
	int pos;

	x*=2;
	y*=2;
	pos=x+y*scrWidth;
	for(i=0;i<len;i++)
	{
		scrn[pos+scrWidth]=c;
		scrn[pos+scrWidth+1]=c;
		scrn[pos++]=c;
		scrn[pos++]=c;
	}
}

static void PlotSolidWordRun(int x,int y,int len,byte *scrn,int scrWidth,word c)
{
	int i;
	int pos;
	byte c2;

	x*=2;
	y*=2;
	pos=x+y*scrWidth;
	c2=(byte)(c>>8);
	for(i=0;i<len;i++)
	{
		scrn[pos+scrWidth]=(byte)c;
		scrn[pos+scrWidth+1]=(byte)c;
		scrn[pos++]=(byte)c;
		scrn[pos++]=(byte)c;
		scrn[pos+scrWidth]=c2;
		scrn[pos+scrWidth+1]=c2;
		scrn[pos++]=c2;
		scrn[pos++]=c2;
	}
}

static void PlotDataRun(int x,int y,int len,byte *scrn,int scrWidth,byte *data)
{
	int i;
	int pos;

	x*=2;
	y*=2;
	pos=x+y*scrWidth;
	for(i=0;i<len;i++)
	{
		scrn[pos+scrWidth]=*data;
		scrn[pos+scrWidth+1]=*data;
		scrn[pos++]=*data;
		scrn[pos++]=*data;
		data++;
	}
}


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
					numPaks=numPaks;
					break;
				case 0x8000:	// top bit, supposedly last pixel thing
					numPaks=numPaks;
					break;
			}
		}

		while(numPaks>0)
		{
			x+=p[pos++];
			sizeCount=(char)p[pos++];
			if(sizeCount>0)	// copy sizeCount words
			{
				//memcpy(&scrn[scrWidth*y+x],&p[pos],sizeCount*2);
				PlotDataRun(x,y,sizeCount*2,scrn,scrWidth,&p[pos]);
				pos+=sizeCount*2;
				x+=sizeCount*2;
			}
			else if(sizeCount<0) // copy the word value -sizeCount times
			{
				memcpy(&v,&p[pos],2);
				/*
				while(sizeCount<0)
				{
					sizeCount++;
					memcpy(&scrn[scrWidth*y+x],&v,2);
					x+=2;
				}
				*/
				PlotSolidWordRun(x,y,-sizeCount,scrn,scrWidth,v);
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
	word pos=0;

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
				//memset(&scrn[x+y*scrWidth],p[pos],-size);
				PlotSolidRun(x,y,-size,scrn,scrWidth,p[pos]);
				pos++;
				x-=size;
			}
			if(size>0) {
				//memcpy(&scrn[x+y*scrWidth],&p[pos],size);
				PlotDataRun(x,y,size,scrn,scrWidth,&p[pos]);
				pos+=size;
				x+=size;
			}
		}
		y++;
	}
}

static void FLI_doBRUN(byte *scrn,int scrWidth,byte *p)
{
	byte numpak;
	word x,y=0;
	char size;
	word pos=0;

	do {
		x=0;
		numpak=p[pos++];
		while(numpak>0) {
			numpak--;
			size=p[pos++];
			if(size>0) {
				//memset(&scrn[x+y*scrWidth],p[pos],size);
				PlotSolidRun(x,y,size,scrn,scrWidth,p[pos]);
				pos++;
				x+=size;
			}
			if(size<0) {
				//memcpy(&scrn[x+y*scrWidth],&p[pos],-size);
				PlotDataRun(x,y,-size,scrn,scrWidth,&p[pos]);
				pos-=size;
				x-=size;
			}
		}
		++y;
	} while(y<fliHeight);
}

static void FLI_nextchunk(MGLDraw *mgl,int scrWidth)
{
	int i,j;
	chunkheader chead;
	byte *p,*src,*dst;

	SDL_RWread(FLI_file, &chead,1,sizeofchunkheader);
	if(chead.kind==FLI_COPY)
		chead.size=fliWidth*fliHeight+sizeofchunkheader;	// a hack to make up for a bug in Animator?
	p=(byte *)malloc(chead.size-sizeofchunkheader);
	SDL_RWread(FLI_file, p,1,chead.size-sizeofchunkheader);
	switch(chead.kind)
	{
		case FLI_COPY:
			dst=mgl->GetScreen();
			src=p;
			for(j=0;j<fliHeight;j++)
			{
				for(i=0;i<fliWidth;i++)
				{
					*dst=*src;
					*(dst+1)=*src;
					*(dst+scrWidth)=*src;
					*(dst+scrWidth+1)=*src;
					dst+=2;
					src++;
				}
				dst+=scrWidth;
			}
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
	free(p);
}

static void FLI_nextfr(MGLDraw *mgl,int scrWidth)
{
	frmheader fhead;
	int i;

	SDL_RWread(FLI_file,&fhead,1,sizeof(frmheader));

	// check to see if this is a FLC file's special frame... if it is, skip it
	if(fhead.magic==0x00A1)
	{
		SDL_RWseek(FLI_file,fhead.size,RW_SEEK_CUR);
		return;
	}

	for(i=0;i<fhead.chunks;i++)
		FLI_nextchunk(mgl,scrWidth);
}

static void FLI_skipfr(void)
{
	frmheader fhead;

	SDL_RWread(FLI_file,&fhead,1,sizeof(frmheader));

	SDL_RWseek(FLI_file,fhead.size-sizeof(frmheader),RW_SEEK_CUR);
}

byte FLI_play(const char *name, byte loop, word wait, MGLDraw *mgl, FlicCallBack callback)
{
	int frmon=0;
	long frsize;
	fliheader FLI_hdr;
	int scrWidth;
	char k;
	dword startTime,endTime;

	FLI_file=AssetOpen_SDL(name,"rb");
	if (!FLI_file)
	{
		perror(name);
		return 0;
	}
	SDL_RWread(FLI_file, &FLI_hdr,1,sizeof(fliheader));
	SDL_RWread(FLI_file, &frsize,1,4);
	SDL_RWseek(FLI_file, -4, RW_SEEK_CUR);
	fliWidth=FLI_hdr.width;
	fliHeight=FLI_hdr.height;

	mgl->LastKeyPressed();	// clear key buffer

	// if this is a FLC, skip the first frame
	if((name[strlen(name)-1]=='c')||
			(name[strlen(name)-1]=='C'))
	{
		FLI_skipfr();
		frmon++;
		FLI_hdr.frames++;	// a confusion issue
	}
	do
	{
		startTime=timeGetTime();
		frmon++;
		scrWidth=mgl->GetWidth();
		FLI_nextfr(mgl,scrWidth);
		if (callback && !callback(frmon))
			break;
		mgl->Flip();
		if((loop)&&(frmon==FLI_hdr.frames+1))
		{
			frmon=1;
			SDL_RWseek(FLI_file, 128 + frsize, RW_SEEK_SET);
		}
		if((!loop)&&(frmon==FLI_hdr.frames))
			frmon=FLI_hdr.frames+1;
		k=mgl->LastKeyPressed();
		// key #27 is escape

		endTime=timeGetTime();
		while((endTime-startTime)<wait)
			endTime=timeGetTime();
	} while((frmon<FLI_hdr.frames+1)&&(mgl->Process()) && (k!=27));
	SDL_RWclose(FLI_file);
	return k != 27;
}

word FLI_numFrames(char *name)
{
	fliheader FLI_hdr;

	SDL_RWops *f=AssetOpen_SDL(name,"rb");
	SDL_RWread(f, &FLI_hdr,1,sizeof(fliheader));
	SDL_RWclose(f);
	if((name[strlen(name)-1]=='c')||
			(name[strlen(name)-1]=='C'))
		return FLI_hdr.frames;
	else
		return FLI_hdr.frames;
}
