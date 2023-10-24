#include "winpch.h"
#include "tile.h"
#include "display.h"
#include "progress.h"
#include "shop.h"
#include "config.h"
#include "ioext.h"
#include "recolor.h"

static tile_t tiles[NUMTILES];
static MGLDraw *tileMGL;
static int numTiles;

#define SCRWID (tileMGL->GetWidth())
#define SCRHEI (tileMGL->GetHeight())

void InitTiles(MGLDraw *mgl)
{
	tileMGL=mgl;
}

void ExitTiles(void)
{
	// nothing to do
}

byte *GetTileData(int t)
{
	return tiles[t];
}

void SetTiles(byte *scrn)
{
	int i,j;
	int x,y;

	x=0;
	y=0;
	for(i=0;i<400;i++)
	{
		for(j=0;j<TILE_HEIGHT;j++)
			memcpy(&tiles[i][j*TILE_WIDTH],&scrn[x+(y+j)*SCRWID],TILE_WIDTH);
		x+=TILE_WIDTH;
		if(x>=SCRWID)
		{
			x=0;
			y+=TILE_HEIGHT;
		}
	}
}

void LoadOldTiles(FILE *f)
{
	numTiles=400;
	fread(tiles,numTiles,sizeof(tile_t),f);
}

void SetTile(int t,int x,int y,byte *src)
{
	int i;

	for(i=0;i<TILE_HEIGHT;i++)
		memcpy(&tiles[t][i*TILE_WIDTH],&src[x+(y+i)*SCRWID],TILE_WIDTH);
}

static void SaveTile(std::ostream& f, byte *t)
{
	int row,i;
	byte size,c;
	byte compress[3],curComp;
	dword curBit;

	curComp=0;
	compress[0]=compress[1]=compress[2]=0;
	curBit=1;
	for(row=0;row<24;row++)
	{
		// see how big it would be in RLE format
		size=2;
		c=t[row*TILE_WIDTH];
		for(i=1;i<32;i++)
		{
			if(t[row*TILE_WIDTH+i]==c)
				continue;
			else
			{
				c=t[row*TILE_WIDTH+i];
				size+=2;
			}
		}
		if(size<32)	// RLE is superior here
			compress[curComp]|=curBit;	// so set the compression bit
		curBit*=2;
		if(curBit==256)
		{
			curBit=1;
			curComp++;
		}
	}

	// now that all 24 have been checked, "compress" contains bits for whether to RLE each one
	f.write((char*) compress, 3);
	curComp=0;
	curBit=1;
	// now write out the rows themselves
	for(row=0;row<24;row++)
	{
		if(compress[curComp]&curBit)	// RLE format
		{
			size=1;
			c=t[row*TILE_WIDTH];
			for(i=1;i<32;i++)
			{
				if(t[row*TILE_WIDTH+i]==c)
				{
					size++;
					continue;
				}
				else
				{
					// write out the current run and start a new one
					f.put(size);
					f.put(c);
					c=t[row*TILE_WIDTH+i];
					size=1;
				}
			}
			// write out the final run
			f.put(size);
			f.put(c);
		}
		else	// straight format, simple
		{
			f.write((char*) &t[row*TILE_WIDTH], 32);	// just write the 32 bytes
		}

		curBit*=2;
		if(curBit==256)
		{
			curBit=1;
			curComp++;
		}
	}
	// took all that to write one lousy ass tile.
}


void SaveTiles(FILE *f)
{
	FilePtrStream stream(f);
	SaveTiles(stream);
}

void SaveTiles(std::ostream& f)
{
	for (int i = 0; i < numTiles; ++i)
		SaveTile(f, tiles[i]);
}

void SaveTilesToBMP(const char *fname)
{
	char tmp[64];
	int i,j;

	strcpy(tmp,fname);
	for(i=0;i<(int)strlen(tmp);i++)
		if(tmp[i]>='A' && tmp[i]<='Z')
			tmp[i]+='a'-'A';	// lowercase it

	if(strlen(tmp)==0)
	{
		strcpy(tmp,"mytiles.bmp");
	}
	else if(strlen(tmp)<5)
	{
		strcat(tmp,".bmp");
	}
	else
	{
		if(strcmp(&tmp[strlen(tmp)-4],".bmp"))
			strcat(tmp,".bmp");
	}

	GetDisplayMGL()->ClearScreen();
	for(j=0;j<20;j++)
		for(i=0;i<20;i++)
		{
			if(i+j*20>=numTiles)
				break;
			RenderFloorTileUnlit(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20);
		}

	//GetDisplayMGL()->Flip();
	GetDisplayMGL()->SaveBMP(tmp);

	if(numTiles>400)
	{
		strcpy(&tmp[strlen(tmp)-4],"2.bmp");	// make filename2.bmp
		GetDisplayMGL()->ClearScreen();
		for(j=0;j<20;j++)
			for(i=0;i<20;i++)
			{
				if(i+j*20+400>=numTiles)
					break;
				RenderFloorTileUnlit(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20+400);
			}

		//GetDisplayMGL()->Flip();
		GetDisplayMGL()->SaveBMP(tmp);
	}
	GetDisplayMGL()->ClearScreen();
}

static void LoadTile(std::istream &f, byte *t)
{
	int row,x;
	byte size,c;
	byte compress[3],curComp;
	dword curBit;

	f.read((char*) compress, 3);

	curComp=0;
	curBit=1;

	for(row=0;row<24;row++)
	{
		if(compress[curComp]&curBit)	// RLE format
		{
			x=0;
			while(x<32)
			{
				size = f.get();
				c = f.get();
				memset(&t[row*TILE_WIDTH+x],c,size);
				x+=size;
			}
		}
		else	// straight format, simple
		{
			f.read((char*) &t[row*TILE_WIDTH], 32);	// just write the 32 bytes
		}

		curBit*=2;
		if(curBit==256)
		{
			curBit=1;
			curComp++;
		}
	}
}

void LoadTiles(FILE *f)
{
	FilePtrStream stream(f);
	LoadTiles(stream);
}

void LoadTiles(std::istream &f)
{
	for (int i = 0; i < numTiles; ++i)
		LoadTile(f, tiles[i]);
}

void AppendTiles(int start,FILE *f)
{
	FilePtrStream stream(f);
	for(int i=start;i<numTiles;i++)
		LoadTile(stream, tiles[i]);
}

void SetNumTiles(int n)
{
	numTiles=n;
}

word GetNumTiles(void)
{
	return numTiles;
}

void RenderEmptyTile(int x,int y,char light)
{
	tileMGL->Box(x,y,x+TILE_WIDTH-1,y+TILE_HEIGHT-1,31);
	tileMGL->FillBox(x+1,y+1,x+TILE_WIDTH-2,y+TILE_HEIGHT-2,0);
	CenterPrint(x+TILE_WIDTH/2,y+4,"???",light,1);
}

void RenderWallTile(int x,int y,word w,word f,char light)
{
	RenderFloorTile(x,y,w,light);
	RenderFloorTile(x,y-TILE_HEIGHT,f,light);
}

void RenderWallTileTrans(int x,int y,word w,word f,char light)
{
	RenderFloorTile(x,y,w,light);
	RenderFloorTileTrans(x,y-TILE_HEIGHT,f,light);
}

void PlotStar(int x,int y,byte col,byte tx,byte ty,word tileNum)
{
	byte *dst;

	if(tiles[tileNum][tx+ty*TILE_WIDTH]==0)
	{
		dst=tileMGL->GetScreen()+x+y*tileMGL->GetWidth();
		*dst=col;
	}
}

// disco tiles ---------------------------------------

byte discoTab[]={1,3,4,5,6,7};

static inline byte PickDiscoColor(void)
{
	return discoTab[Random(6)]*32;
}

// --- RENDERING!

byte ModifyDiscoColor(byte color, byte disco)
{
	if (profile.progress.purchase[modeShopNum[MODE_DISCO]]&SIF_ACTIVE) return (color & 31) | disco;
	else return color;
}

// Rendering for real!

void RenderFloorTile(int x, int y, int t, char light)
{
	byte *dst, *src;
	int wid, hgt;
	byte disco = PickDiscoColor();

	if (t >= numTiles) {
		return RenderEmptyTile(x, y, 0);
	}

	if (light == 0 && !(profile.progress.purchase[modeShopNum[MODE_DISCO]]&SIF_ACTIVE))
	{
		return RenderFloorTileUnlit(x, y, t);
	}

	if (x < 0)
	{
		wid = TILE_WIDTH + x;
		if (wid < 1)
			return;

		dst = tileMGL->GetScreen() + y * SCRWID;
		src = tiles[t] - x;
	}
	else if (x > SCRWID - TILE_WIDTH)
	{
		wid = TILE_WIDTH - (x - (SCRWID - TILE_WIDTH));
		if (wid < 1)
			return;
		dst = tileMGL->GetScreen() + x + y * SCRWID;
		src = tiles[t];
	}
	else
	{
		wid = TILE_WIDTH;
		dst = tileMGL->GetScreen() + x + y * SCRWID;
		src = tiles[t];
	}

	if (y < 0)
	{
		dst -= y * SCRWID;
		src -= y*TILE_WIDTH;

		hgt = TILE_HEIGHT + y;
	}
	else if (y > SCRHEI - TILE_HEIGHT)
	{
		hgt = TILE_HEIGHT - (y - (SCRHEI - TILE_HEIGHT));
	}
	else
	{
		hgt = TILE_HEIGHT;
	}

	if (hgt <= 0)
		return;

	if (light<-28)
	{
		// just render a black box
		while (hgt > 0)
		{
			hgt--;
			memset(dst, 0, wid);
			dst += SCRWID;
		}
		return;
	}
	else
	{
		while (hgt > 0)
		{
			hgt--;
			for (int i = 0; i < wid; ++i)
			{
				dst[i] = SprModifyLight(ModifyDiscoColor(src[i], disco), light);
			}
			dst += SCRWID;
			src += 32;
		}
	}
}

void RenderFloorTileShadow(int x, int y, int t, char light)
{
	byte *dst, *src;
	int wid, hgt, darkpart;
	byte disco = PickDiscoColor();

	if (t >= numTiles) {
		return RenderEmptyTile(x, y, 0);
	}

	if (x < 0)
	{
		wid = TILE_WIDTH + x;
		if (wid < 1)
			return;

		darkpart = 8;
		dst = tileMGL->GetScreen() + y * SCRWID;
		src = tiles[t] - x;
	}
	else if (x > SCRWID - TILE_WIDTH)
	{
		wid = TILE_WIDTH - (x - (SCRWID - TILE_WIDTH));
		if (wid < 1)
			return;
		darkpart = 8 - (x - (SCRWID - TILE_WIDTH));
		dst = tileMGL->GetScreen() + x + y * SCRWID;
		src = tiles[t];
	}
	else
	{
		wid = TILE_WIDTH;
		dst = tileMGL->GetScreen() + x + y * SCRWID;
		src = tiles[t];
		darkpart = 8; // shadows are 8 pixels wide I guess
	}

	if (y < 0)
	{
		dst -= y * SCRWID;
		src -= y*TILE_WIDTH;

		hgt = TILE_HEIGHT + y;
	}
	else if (y > SCRHEI - TILE_HEIGHT)
	{
		hgt = TILE_HEIGHT - (y - (SCRHEI - TILE_HEIGHT));
	}
	else
		hgt = TILE_HEIGHT;

	if (hgt <= 0)
		return;

	while (hgt > 0)
	{
		hgt--;
		for (int i = 0; i < wid; ++i)
		{
			dst[i] = SprModifyLight(ModifyDiscoColor(src[i], disco), light - 4 * (i > wid - darkpart));
		}
		dst += SCRWID;
		src += 32;
	}
}

void RenderFloorTileUnlit(int x, int y, int t)
{
	byte *dst, *src;
	int wid, hgt;

	if (t >= numTiles) {
		return RenderEmptyTile(x, y, 0);
	}

	if (x < 0)
	{
		wid = TILE_WIDTH + x;
		if (wid < 1)
			return;

		dst = tileMGL->GetScreen() + y * SCRWID;
		src = tiles[t] - x;
	}
	else if (x > SCRWID - TILE_WIDTH)
	{
		wid = TILE_WIDTH - (x - (SCRWID - TILE_WIDTH));
		if (wid < 1)
			return;
		dst = tileMGL->GetScreen() + x + y * SCRWID;
		src = tiles[t];
	}
	else
	{
		wid = TILE_WIDTH;
		dst = tileMGL->GetScreen() + x + y * SCRWID;
		src = tiles[t];
	}

	if (y < 0)
	{
		dst -= y * SCRWID;
		src -= y*TILE_WIDTH;

		hgt = TILE_HEIGHT + y;
	}
	else if (y > SCRHEI - TILE_HEIGHT)
	{
		hgt = TILE_HEIGHT - (y - (SCRHEI - TILE_HEIGHT));
	}
	else
		hgt = TILE_HEIGHT;

	while (hgt > 0)
	{
		hgt--;
		memcpy(dst, src, wid);
		dst += SCRWID;
		src += 32;
	}
}

void RenderFloorTileTrans(int x, int y, int t, char light)
{
	byte *dst, *src;
	int wid, hgt;
	byte disco = PickDiscoColor();

	if (t >= numTiles) {
		return RenderEmptyTile(x, y, 0);
	}

	if (x < 0)
	{
		wid = TILE_WIDTH + x;
		if (wid < 1)
			return;

		dst = tileMGL->GetScreen() + y * SCRWID;
		src = tiles[t] - x;
	}
	else if (x > SCRWID - TILE_WIDTH)
	{
		wid = TILE_WIDTH - (x - (SCRWID - TILE_WIDTH));
		if (wid < 1)
			return;
		dst = tileMGL->GetScreen() + x + y * SCRWID;
		src = tiles[t];
	}
	else
	{
		wid = TILE_WIDTH;
		dst = tileMGL->GetScreen() + x + y * SCRWID;
		src = tiles[t];
	}

	if (y < 0)
	{
		dst -= y * SCRWID;
		src -= y*TILE_WIDTH;

		hgt = TILE_HEIGHT + y;
	}
	else if (y > SCRHEI - TILE_HEIGHT)
	{
		hgt = TILE_HEIGHT - (y - (SCRHEI - TILE_HEIGHT));
	}
	else
		hgt = TILE_HEIGHT;

	if (hgt <= 0)
		return;

	while (hgt > 0)
	{
		hgt--;
		for (int i = 0; i < wid; ++i)
		{
			if (src[i]) dst[i] = SprModifyLight(ModifyDiscoColor(src[i], disco), light);
		}
		dst += SCRWID;
		src += 32;
	}
}

#define GB_WID	(TILE_WIDTH/2)
#define GB_HEI	(TILE_HEIGHT/2)

inline void GouraudBox(int x,int y,byte *src,char light0,char light1,char light2,char light3)
{
	int i,j,tmp;
	byte *dst;
	int curLight,dlx,dly1,dly2,firstLight,lastLight;

	dst=tileMGL->GetScreen()+x+y*SCRWID;

	curLight=light0*FIXAMT;

	firstLight=light0*FIXAMT;
	lastLight=light1*FIXAMT;
	dly1=(light2-light0)*FIXAMT/GB_HEI;
	dly2=(light3-light1)*FIXAMT/GB_HEI;

	for(j=0;j<GB_HEI;j++)
	{
		dlx=(lastLight-firstLight)/GB_WID;
		curLight=firstLight;
		if(y+j>=SCRHEI)
			return;	// all done!
		if(y+j>=0)
		{
			for(i=0;i<GB_WID;i++)
			{
				if(x+i>=0 && x+i<SCRWID)
				{
					tmp=((*src)&31)+(curLight/FIXAMT);
					if(tmp<0)
						tmp=0;
					if(tmp>31)
						tmp=31;
					(*dst)=((*src)&(~31))+tmp;
				}
				dst++;
				src++;

				curLight+=dlx;
			}
		}
		else
		{
			dst+=GB_WID;
			src+=GB_WID;
		}
		dst+=(SCRWID-GB_WID);
		src+=GB_WID;

		firstLight+=dly1;
		lastLight+=dly2;
	}
}

inline void GouraudBoxTrans(int x,int y,byte *src,char light0,char light1,char light2,char light3)
{
	int i,j,tmp;
	byte *dst;
	int curLight,dlx,dly1,dly2,firstLight,lastLight;

	dst=tileMGL->GetScreen()+x+y*SCRWID;

	curLight=light0*FIXAMT;

	firstLight=light0*FIXAMT;
	lastLight=light1*FIXAMT;
	dly1=(light2-light0)*FIXAMT/GB_HEI;
	dly2=(light3-light1)*FIXAMT/GB_HEI;

	for(j=0;j<GB_HEI;j++)
	{
		dlx=(lastLight-firstLight)/GB_WID;
		curLight=firstLight;
		if(y+j>=SCRHEI)
			return;	// all done!
		if(y+j>=0)
		{
			for(i=0;i<GB_WID;i++)
			{
				if(x+i>=0 && x+i<SCRWID)
				{
					if((*src)!=0)
					{
						tmp=((*src)&31)+(curLight/FIXAMT);
						if(tmp<0)
							tmp=0;
						if(tmp>31)
							tmp=31;
						(*dst)=((*src)&(~31))+tmp;
					}
				}
				dst++;
				src++;

				curLight+=dlx;
			}
		}
		else
		{
			dst+=GB_WID;
			src+=GB_WID;
		}
		dst+=(SCRWID-GB_WID);
		src+=GB_WID;

		firstLight+=dly1;
		lastLight+=dly2;
	}
}

inline void GouraudBoxDiscoTrans(int x,int y,byte *src,char light0,char light1,char light2,char light3)
{
	int i,j,tmp;
	byte *dst;
	int curLight,dlx,dly1,dly2,firstLight,lastLight;
	byte color;

	dst=tileMGL->GetScreen()+x+y*SCRWID;

	curLight=light0*FIXAMT;

	firstLight=light0*FIXAMT;
	lastLight=light1*FIXAMT;
	dly1=(light2-light0)*FIXAMT/GB_HEI;
	dly2=(light3-light1)*FIXAMT/GB_HEI;

	color=PickDiscoColor();

	for(j=0;j<GB_HEI;j++)
	{
		dlx=(lastLight-firstLight)/GB_WID;
		curLight=firstLight;
		if(y+j>=SCRHEI)
			return;	// all done!
		if(y+j>=0)
		{
			for(i=0;i<GB_WID;i++)
			{
				if(x+i>=0 && x+i<SCRWID)
				{
					if((*src)!=0)
					{
						tmp=((*src)&31)+(curLight/FIXAMT);
						if(tmp<0)
							tmp=0;
						if(tmp>31)
							tmp=31;
						(*dst)=color+tmp;
					}
				}
				dst++;
				src++;

				curLight+=dlx;
			}
		}
		else
		{
			dst+=GB_WID;
			src+=GB_WID;
		}
		dst+=(SCRWID-GB_WID);
		src+=GB_WID;

		firstLight+=dly1;
		lastLight+=dly2;
	}
}

inline void GouraudBoxDisco(int x,int y,byte *src,char light0,char light1,char light2,char light3)
{
	int i,j,tmp;
	byte *dst;
	int curLight,dlx,dly1,dly2,firstLight,lastLight;
	byte color;

	dst=tileMGL->GetScreen()+x+y*SCRWID;

	curLight=light0*FIXAMT;

	firstLight=light0*FIXAMT;
	lastLight=light1*FIXAMT;
	dly1=(light2-light0)*FIXAMT/GB_HEI;
	dly2=(light3-light1)*FIXAMT/GB_HEI;

	color=PickDiscoColor();

	for(j=0;j<GB_HEI;j++)
	{
		dlx=(lastLight-firstLight)/GB_WID;
		curLight=firstLight;
		if(y+j>=SCRHEI)
			return;	// all done!
		if(y+j>=0)
		{
			for(i=0;i<GB_WID;i++)
			{
				if(x+i>=0 && x+i<SCRWID)
				{
					tmp=((*src)&31)+(curLight/FIXAMT);
					if(tmp<0)
						tmp=0;
					if(tmp>31)
						tmp=31;
					(*dst)=color+tmp;
				}
				dst++;
				src++;

				curLight+=dlx;
			}
		}
		else
		{
			dst+=GB_WID;
			src+=GB_WID;
		}
		dst+=(SCRWID-GB_WID);
		src+=GB_WID;

		firstLight+=dly1;
		lastLight+=dly2;
	}
}

void RenderFloorTileFancy(int x,int y,int t,byte shadow,const char *theLight)
{
	// 9 light values are passed in:
	//
	//   0  1  2
	//   3  4  5
	//   6  7  8

	int i,j;
	char light[9];

	if(x<=-TILE_WIDTH || y<=-TILE_HEIGHT || x>=SCRWID || y>=SCRHEI)
		return;	// no need to render


	if(config.shading==0)
	{
		if(shadow==1)
			RenderFloorTileShadow(x,y,t,theLight[4]);
		else
			RenderFloorTile(x,y,t,theLight[4]);
		return;
	}

	memcpy(light,theLight,9*sizeof(char));
	j=0;
	light[0]=(light[0]+light[4]+light[3]+light[1])/4;
	light[2]=(light[2]+light[4]+light[1]+light[5])/4;
	light[6]=(light[6]+light[4]+light[3]+light[7])/4;
	light[8]=(light[8]+light[4]+light[7]+light[5])/4;

	for(i=0;i<9;i++)
	{
		if(light[i]==0)
			j++;
		if(i==1 || i==3 || i==5 || i==7)
			light[i]=(light[4]+light[i])/2;	// average each one with this tile's central light
	}

	if(j==9 && !shadow && !(profile.progress.purchase[modeShopNum[MODE_DISCO]]&SIF_ACTIVE))
	{
		RenderFloorTileUnlit(x,y,t);
		return;
	}

	if(shadow==1)
	{
		light[2]-=8;
		light[5]-=8;
		light[8]-=8;
	}
	if(shadow==2)
	{
		light[2]-=8;
		light[5]-=8;
	}
	if(shadow==3)
	{
		light[8]-=8;
	}
	if(shadow==4)
	{
		light[6]-=8;
		light[7]-=8;
		light[8]-=8;
	}
	if(shadow==5)
	{
		light[5]=-8;
		light[6]-=8;
		light[7]-=8;
		light[8]-=8;
	}
	if(shadow==6)
	{
		light[2]=-8;
		light[5]=-8;
		light[6]-=8;
		light[7]-=8;
		light[8]-=8;
	}
	if(shadow==7)
	{
		light[6]-=8;
		light[7]-=8;
	}

	if(profile.progress.purchase[modeShopNum[MODE_DISCO]]&SIF_ACTIVE)
	{
		GouraudBoxDisco(x,y,tiles[t],light[0],light[1],light[3],light[4]);
		GouraudBoxDisco(x+GB_WID,y,tiles[t]+GB_WID,light[1],light[2],light[4],light[5]);
		GouraudBoxDisco(x,y+GB_HEI,tiles[t]+GB_HEI*TILE_WIDTH,light[3],light[4],light[6],light[7]);
		GouraudBoxDisco(x+GB_WID,y+GB_HEI,tiles[t]+GB_WID+GB_HEI*TILE_WIDTH,light[4],light[5],light[7],light[8]);
	}
	else
	{
		GouraudBox(x,y,tiles[t],light[0],light[1],light[3],light[4]);
		GouraudBox(x+GB_WID,y,tiles[t]+GB_WID,light[1],light[2],light[4],light[5]);
		GouraudBox(x,y+GB_HEI,tiles[t]+GB_HEI*TILE_WIDTH,light[3],light[4],light[6],light[7]);
		GouraudBox(x+GB_WID,y+GB_HEI,tiles[t]+GB_WID+GB_HEI*TILE_WIDTH,light[4],light[5],light[7],light[8]);
	}
}

void RenderWallTileFancy(int x,int y,int t,const char *theLight)
{
	// 9 light values are passed in:
	//
	//   0  1  2
	//   3  4  5
	//   6  7  8

	int i,j;
	char light[9];

	if(x<=-TILE_WIDTH || y<=-TILE_HEIGHT || x>=SCRWID || y>=SCRHEI)
		return;	// no need to render

	if(config.shading==0)
	{
		RenderFloorTile(x,y,t,theLight[4]);
		return;
	}

	memcpy(light,theLight,9*sizeof(char));
	j=0;
	light[6]=(light[6]+light[4]+light[3]+light[7])/4;
	light[8]=(light[8]+light[4]+light[7]+light[5])/4;

	for(i=0;i<9;i++)
	{
		if(light[i]==0)
			j++;
		if(i==1 || i==3 || i==5 || i==7)
			light[i]=(light[4]+light[i])/2;	// average each one with this tile's central light
	}

	light[3]=light[6];
	light[4]=light[7];
	light[5]=light[8];

	light[0]=light[3];
	light[2]=light[5];
	light[1]=light[4];

	if(j==9 && !(profile.progress.purchase[modeShopNum[MODE_DISCO]]&SIF_ACTIVE))
	{
		RenderFloorTileUnlit(x,y,t);
		return;
	}

	if(profile.progress.purchase[modeShopNum[MODE_DISCO]]&SIF_ACTIVE)
	{
		GouraudBoxDisco(x,y,tiles[t],light[0],light[1],light[3],light[4]);
		GouraudBoxDisco(x+GB_WID,y,tiles[t]+GB_WID,light[1],light[2],light[4],light[5]);
		GouraudBoxDisco(x,y+GB_HEI,tiles[t]+GB_HEI*TILE_WIDTH,light[3],light[4],light[6],light[7]);
		GouraudBoxDisco(x+GB_WID,y+GB_HEI,tiles[t]+GB_WID+GB_HEI*TILE_WIDTH,light[4],light[5],light[7],light[8]);
	}
	else
	{
		GouraudBox(x,y,tiles[t],light[0],light[1],light[3],light[4]);
		GouraudBox(x+GB_WID,y,tiles[t]+GB_WID,light[1],light[2],light[4],light[5]);
		GouraudBox(x,y+GB_HEI,tiles[t]+GB_HEI*TILE_WIDTH,light[3],light[4],light[6],light[7]);
		GouraudBox(x+GB_WID,y+GB_HEI,tiles[t]+GB_WID+GB_HEI*TILE_WIDTH,light[4],light[5],light[7],light[8]);
	}
}

void RenderRoofTileFancy(int x,int y,int t,byte trans,byte wallBelow,const char *theLight)
{
	// 9 light values are passed in:
	//
	//   0  1  2
	//   3  4  5
	//   6  7  8

	int i,j;
	char light[9];

	if(x<=-TILE_WIDTH || y<=-TILE_HEIGHT || x>=SCRWID || y>=SCRHEI)
		return;	// no need to render


	if(config.shading==0)
	{
		if(trans)
			RenderFloorTileTrans(x,y,t,theLight[4]);
		else
			RenderFloorTile(x,y,t,theLight[4]);
		return;
	}

	memcpy(light,theLight,9*sizeof(char));
	j=0;
	light[0]=(light[0]+light[4]+light[3]+light[1])/4;
	light[2]=(light[2]+light[4]+light[1]+light[5])/4;
	light[6]=(light[6]+light[4]+light[3]+light[7])/4;
	light[8]=(light[8]+light[4]+light[7]+light[5])/4;

	for(i=0;i<9;i++)
	{
		if(light[i]==0)
			j++;
		if(i==1 || i==3 || i==5 || i==7)
			light[i]=(light[4]+light[i])/2;	// average each one with this tile's central light
	}

	if(j==9 && !(profile.progress.purchase[modeShopNum[MODE_DISCO]]&SIF_ACTIVE))
	{
		if(trans)
			RenderFloorTileTrans(x,y,t,0);
		else
			RenderFloorTileUnlit(x,y,t);
		return;
	}

	/*
	if(config.shading==0)
	{
		if(trans)
			RenderFloorTileTrans(x,y,t,theLight[4]);
		else
			RenderFloorTile(x,y,t,theLight[4]);
		return;
	}

	memcpy(light,theLight,9*sizeof(char));
	j=0;
	light[0]=(light[0]+light[4]+light[3]+light[1])/4;
	light[2]=(light[2]+light[4]+light[1]+light[5])/4;
	if(wallBelow)
	{
		light[6]=(light[6]+light[4]+light[3]+light[7])/4;
		light[8]=(light[8]+light[4]+light[7]+light[5])/4;
	}
	for(i=0;i<9;i++)
	{
		if(light[i]==0)
			j++;
		if(i==1 || i==3 || i==5 || i==7)
			light[i]=(light[4]+light[i])/2;	// average each one with this tile's central light
	}
	if(!wallBelow)
	{
		light[6]=light[3];
		light[8]=light[5];
		light[7]=light[4];
	}
	*/

	if(j==9 && !(profile.progress.purchase[modeShopNum[MODE_DISCO]]&SIF_ACTIVE))
	{
		if(trans)
			RenderFloorTileTrans(x,y,t,0);
		else
			RenderFloorTileUnlit(x,y,t);
		return;
	}

	if(profile.progress.purchase[modeShopNum[MODE_DISCO]]&SIF_ACTIVE)
	{
		if(!trans)
		{
			GouraudBoxDisco(x,y,tiles[t],light[0],light[1],light[3],light[4]);
			GouraudBoxDisco(x+GB_WID,y,tiles[t]+GB_WID,light[1],light[2],light[4],light[5]);
			GouraudBoxDisco(x,y+GB_HEI,tiles[t]+GB_HEI*TILE_WIDTH,light[3],light[4],light[6],light[7]);
			GouraudBoxDisco(x+GB_WID,y+GB_HEI,tiles[t]+GB_WID+GB_HEI*TILE_WIDTH,light[4],light[5],light[7],light[8]);
		}
		else
		{
			GouraudBoxDiscoTrans(x,y,tiles[t],light[0],light[1],light[3],light[4]);
			GouraudBoxDiscoTrans(x+GB_WID,y,tiles[t]+GB_WID,light[1],light[2],light[4],light[5]);
			GouraudBoxDiscoTrans(x,y+GB_HEI,tiles[t]+GB_HEI*TILE_WIDTH,light[3],light[4],light[6],light[7]);
			GouraudBoxDiscoTrans(x+GB_WID,y+GB_HEI,tiles[t]+GB_WID+GB_HEI*TILE_WIDTH,light[4],light[5],light[7],light[8]);
		}
	}
	else
	{
		if(!trans)
		{
			GouraudBox(x,y,tiles[t],light[0],light[1],light[3],light[4]);
			GouraudBox(x+GB_WID,y,tiles[t]+GB_WID,light[1],light[2],light[4],light[5]);
			GouraudBox(x,y+GB_HEI,tiles[t]+GB_HEI*TILE_WIDTH,light[3],light[4],light[6],light[7]);
			GouraudBox(x+GB_WID,y+GB_HEI,tiles[t]+GB_WID+GB_HEI*TILE_WIDTH,light[4],light[5],light[7],light[8]);
		}
		else
		{
			GouraudBoxTrans(x,y,tiles[t],light[0],light[1],light[3],light[4]);
			GouraudBoxTrans(x+GB_WID,y,tiles[t]+GB_WID,light[1],light[2],light[4],light[5]);
			GouraudBoxTrans(x,y+GB_HEI,tiles[t]+GB_HEI*TILE_WIDTH,light[3],light[4],light[6],light[7]);
			GouraudBoxTrans(x+GB_WID,y+GB_HEI,tiles[t]+GB_WID+GB_HEI*TILE_WIDTH,light[4],light[5],light[7],light[8]);
		}
	}
}
