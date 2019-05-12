#include "jamulfmv.h"

void FLI_play(char *name,byte loop,word wait,MGLDraw *mgl,FlicCallBack callback)
{
}

#if 0  // TODO
// different kinds of flic chunks
#define FLI_COLOR		11
#define FLI_LC			12
#define FLI_BLACK		13
#define FLI_BRUN		15
#define FLI_COPY		16
#define FLI_DELTA		7
#define FLI_256_COLOR	4
#define FLI_MINI		18

typedef struct fliheader
{
  long size;
  word magic;
  word frames;
  word width,height;
  word flags;
  word speed;
  long next,frit;
  byte expand[104];
} fliheader;

typedef struct frmheader
{
  long size;
  word magic;    /* always $F1FA */
  word chunks;
  byte expand[8];
} frmheader;

// because of padding, the following 6-byte header is 8 bytes.
// therefore use this define instead of sizeof() to read from a FLIc
#define sizeofchunkheader 6

typedef struct chunkheader
{
  long size;
  word kind;
} chunkheader;

FILE *	FLI_file;
palette_t FLI_pal[256];
word	fliWidth,fliHeight;

//------------------------------------------------------------------------------

void PlotSolidRun(int x,int y,int len,byte *scrn,byte c)
{
	int i;
	int pos;

	x*=2;
	y*=2;
	pos=x+y*640;
	for(i=0;i<len;i++)
	{
		scrn[pos+640]=c;
		scrn[pos+641]=c;
		scrn[pos++]=c;
		scrn[pos++]=c;
	}
}

void PlotSolidWordRun(int x,int y,int len,byte *scrn,word c)
{
	int i;
	int pos;
	byte c2;

	x*=2;
	y*=2;
	pos=x+y*640;
	c2=(byte)(c>>8);
	for(i=0;i<len;i++)
	{
		scrn[pos+640]=(byte)c;
		scrn[pos+641]=(byte)c;
		scrn[pos++]=(byte)c;
		scrn[pos++]=(byte)c;
		scrn[pos+640]=c2;
		scrn[pos+641]=c2;
		scrn[pos++]=c2;
		scrn[pos++]=c2;
	}
}

void PlotDataRun(int x,int y,int len,byte *scrn,byte *data)
{
	int i;
	int pos;

	x*=2;
	y*=2;
	pos=x+y*640;
	for(i=0;i<len;i++)
	{
		scrn[pos+640]=*data;
		scrn[pos+641]=*data;
		scrn[pos++]=*data;
		scrn[pos++]=*data;
		data++;
	}
}


void FLI_docolor2(byte *p,MGLDraw *mgl)
{
  word numpak;
  word pos=0;
  byte palpos=0;
  byte numcol;
  byte b;

  memcpy(&numpak,p,2);
  pos=2;
  while(numpak>0) {
    numpak--;
    palpos+=p[pos++];
    numcol=p[pos++];
    do {
      memcpy(&FLI_pal[palpos],&p[pos],3);
	  b=FLI_pal[palpos].blue;
	  FLI_pal[palpos].blue=FLI_pal[palpos].red;
	  FLI_pal[palpos].red=b;
	  palpos++;
      pos+=3;
      --numcol;
    } while(numcol>0);
  }
  // apply the palette here
  mgl->SetPalette(FLI_pal);
  mgl->RealizePalette();
}

void FLI_docolor(byte *p,MGLDraw *mgl)
{
	// docolor2 and docolor are supposed to be different, but they aren't
	FLI_docolor2(p,mgl);
}

void FLI_doDelta(byte *scrn,int scrWidth,byte *p)
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
				PlotDataRun(x,y,sizeCount*2,scrn,&p[pos]);
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
				PlotSolidWordRun(x,y,-sizeCount,scrn,v);
				x-=2*sizeCount;
				pos+=2;
			}
			numPaks--;
		}
		y++;
	}
}

void FLI_doLC(byte *scrn,int scrWidth,byte *p)
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
		PlotSolidRun(x,y,-size,scrn,p[pos]);
        pos++;
        x-=size;
      }
      if(size>0) {
        //memcpy(&scrn[x+y*scrWidth],&p[pos],size);
		PlotDataRun(x,y,size,scrn,&p[pos]);
        pos+=size;
        x+=size;
      }
    }
    y++;
  }
}

void FLI_doBRUN(byte *scrn,int scrWidth,byte *p)
{
  byte numpak;
  word x,y=0;
  char size;
  word pos=0;

  do
  {
    x=0;
    numpak=p[pos++];
    while(numpak>0)
	{
      numpak--;
      size=p[pos++];
      if(size>0)
	  {
        //memset(&scrn[x+y*scrWidth],p[pos],size);
		PlotSolidRun(x,y,size,scrn,p[pos]);
        pos++;
        x+=size;
      }
      if(size<0)
	  {
        //memcpy(&scrn[x+y*scrWidth],&p[pos],-size);
		PlotDataRun(x,y,-size,scrn,&p[pos]);
        pos-=size;
        x-=size;
      }
    }
    ++y;
  }
  while(y<fliHeight);
}

void FLI_nextchunk(MGLDraw *mgl,int scrWidth)
{
  int i;
  chunkheader chead;
  byte *p;

  fread(&chead,1,sizeofchunkheader,FLI_file);
  p=(byte *)malloc(chead.size-sizeofchunkheader);
  fread(p,1,chead.size-sizeofchunkheader,FLI_file);
  switch(chead.kind)
  {
    case FLI_COPY:  for(i=0;i<fliHeight;i++)
						memcpy(&mgl->GetScreen()[i*scrWidth],&p[i*fliWidth],fliWidth);
                    break;
    case FLI_BLACK: mgl->ClearScreen();
                    break;
    case FLI_COLOR: FLI_docolor(p,mgl);
                    break;
    case FLI_LC:    FLI_doLC(mgl->GetScreen(),scrWidth,p);
                    break;
    case FLI_BRUN:  FLI_doBRUN(mgl->GetScreen(),scrWidth,p);
                    break;
	case FLI_MINI:  break; // ignore it
	case FLI_DELTA: FLI_doDelta(mgl->GetScreen(),scrWidth,p);
					break;
	case FLI_256_COLOR: FLI_docolor2(p,mgl);
						break;
  }
  free(p);
}

void FLI_nextfr(MGLDraw *mgl,int scrWidth)
{
  frmheader fhead;
  int i;

  fread(&fhead,1,sizeof(frmheader),FLI_file);

  // check to see if this is a FLC file's special frame... if it is, skip it
  if(fhead.magic==0x00A1)
  {
	fseek(FLI_file,fhead.size,SEEK_CUR);
	return;
  }

  for(i=0;i<fhead.chunks;i++)
	  FLI_nextchunk(mgl,scrWidth);
}

void FLI_skipfr(void)
{
  frmheader fhead;

  fread(&fhead,1,sizeof(frmheader),FLI_file);

  fseek(FLI_file,fhead.size-sizeof(frmheader),SEEK_CUR);
}

void FLI_play(char *name,byte loop,word wait,MGLDraw *mgl,FlicCallBack callback)
{
  int frmon=0;
  long frsize;
  fliheader FLI_hdr;
  int scrWidth;
  char k;
  dword now,then,wTime;

  FLI_file=fopen(name,"rb");
  if(!FLI_file)
	  return;

  fread(&FLI_hdr,1,sizeof(fliheader),FLI_file);
  fread(&frsize,1,4,FLI_file);
  fseek(FLI_file,-4,SEEK_CUR);
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
  do {
	then=timeGetTime();
    frmon++;
	scrWidth=mgl->GetWidth();
    FLI_nextfr(mgl,scrWidth);
	mgl->Flip();
	now=timeGetTime();
	if(now-then<wait)
	{
		wTime=wait-(now-then);
		Sleep(wTime);
	}
    if((loop)&&(frmon==FLI_hdr.frames+1)) {
      frmon=1;
      fseek(FLI_file,128+frsize,SEEK_SET);
    }
    if((!loop)&&(frmon==FLI_hdr.frames))
      frmon=FLI_hdr.frames+1;
	k=mgl->LastKeyPressed();
	// key #27 is escape
	if(callback!=NULL && callback(frmon)==0)
		k=27;
  } while((frmon<FLI_hdr.frames+1)&&(mgl->Process()) && (k!=27));
  fclose(FLI_file);
}

word FLI_numFrames(char *name)
{
  fliheader FLI_hdr;

  FLI_file=fopen(name,"rb");
  fread(&FLI_hdr,1,sizeof(fliheader),FLI_file);
  fclose(FLI_file);
  if((name[strlen(name)-1]=='c')||
	 (name[strlen(name)-1]=='C'))
	return FLI_hdr.frames;
  else
	return FLI_hdr.frames;
}

#endif
