#include "tile.h"
#include "options.h"

tile_t tiles[NUMTILES];
MGLDraw *tileMGL;

void InitTiles(MGLDraw *mgl)
{
	tileMGL=mgl;
}

void ExitTiles(void)
{
	// nothing to do
}

void SetTiles(byte *scrn)
{
	int i,j;
	int x,y;

	x=0;
	y=0;
	for(i=0;i<NUMTILES;i++)
	{
		for(j=0;j<TILE_HEIGHT;j++)
			memcpy(&tiles[i][j*TILE_WIDTH],&scrn[x+(y+j)*640],TILE_WIDTH);
		x+=TILE_WIDTH;
		if(x>639)
		{
			x=0;
			y+=TILE_HEIGHT;
		}
	}
}

void SaveTiles(FILE *f)
{
	fwrite(tiles,NUMTILES,sizeof(tile_t),f);
}

void LoadTiles(FILE *f)
{
	fread(tiles,NUMTILES,sizeof(tile_t),f);
}

// --- RENDERING!
// Helper shenanigans for C stuff, see jamulspr.cpp
extern byte SprModifyColor(byte color, byte hue);
extern byte SprGetColor(byte color);
extern byte SprModifyLight(byte color, char bright);
extern byte SprModifyGhost(byte src, byte dst, char bright);
extern byte SprModifyGlow(byte src, byte dst, char bright);

void RenderFloorTile(int x,int y,int t,char light)
{
    byte *dst, *src;
    int wid, hgt;

	if(light==0) {
		return RenderFloorTileUnlit(x,y,t);
	}

	if(x<0) {
		wid=TILE_WIDTH+x;
		if(wid<1)
			return;

		dst=tileMGL->GetScreen()+y*640;
		src=tiles[t]-x;
	} else if (x>640-TILE_WIDTH) {
		wid=TILE_WIDTH-(x-(640-TILE_WIDTH));
		if(wid<1)
			return;
		dst=tileMGL->GetScreen()+x+y*640;
		src=tiles[t];
	} else {
		wid=TILE_WIDTH;
		dst=tileMGL->GetScreen()+x+y*640;
		src=tiles[t];
	}

	if(y<0) {
		dst-=y*640;
		src-=y*TILE_WIDTH;

		hgt=TILE_HEIGHT+y;
	} else if(y>480-TILE_HEIGHT) {
		hgt=TILE_HEIGHT-(y-(480-TILE_HEIGHT));
	} else {
		hgt=TILE_HEIGHT;
    }

	if(hgt<=0)
		return;

	if(light<-28) {
		// just render a black box
		while(hgt>0) {
			hgt--;
			memset(dst, 0, wid);
			dst += 640;
		}
		return;
	} else {
        while (hgt > 0) {
            hgt--;
            for (int i = 0; i < wid; ++i) {
                dst[i] = SprModifyLight(src[i], light);
            }
            dst += 640;
            src += 32;
        }
    }
}

void RenderFloorTileShadow(int x,int y,int t,char light)
{
    byte *dst, *src;
    int wid, hgt, darkpart;

	if(x<0)
	{
		wid=TILE_WIDTH+x;
		if(wid<1)
			return;

		darkpart=8;
		dst=tileMGL->GetScreen()+y*640;
		src=tiles[t]-x;
	}
	else if (x>640-TILE_WIDTH)
	{
		wid=TILE_WIDTH-(x-(640-TILE_WIDTH));
		if(wid<1)
			return;
		darkpart=8-(x-(640-TILE_WIDTH));
		dst=tileMGL->GetScreen()+x+y*640;
		src=tiles[t];
	}
	else
	{
		wid=TILE_WIDTH;
		dst=tileMGL->GetScreen()+x+y*640;
		src=tiles[t];
		darkpart=8;	// shadows are 8 pixels wide I guess
	}

	if(y<0)
	{
		dst-=y*640;
		src-=y*TILE_WIDTH;

		hgt=TILE_HEIGHT+y;
	}
	else if(y>480-TILE_HEIGHT)
	{
		hgt=TILE_HEIGHT-(y-(480-TILE_HEIGHT));
	}
	else
		hgt=TILE_HEIGHT;

	if(hgt<=0)
		return;

    while (hgt > 0) {
        hgt--;
        for (int i = 0; i < wid; ++i) {
            dst[i] = SprModifyLight(src[i], light - 4 * (i > wid-darkpart));
        }
        dst += 640;
        src += 32;
    }
}

void RenderFloorTileUnlit(int x,int y,int t)
{
    byte *dst, *src;
    int wid, hgt;

	if(x<0)
	{
		wid=TILE_WIDTH+x;
		if(wid<1)
			return;

		dst=tileMGL->GetScreen()+y*640;
		src=tiles[t]-x;
	}
	else if (x>640-TILE_WIDTH)
	{
		wid=TILE_WIDTH-(x-(640-TILE_WIDTH));
		if(wid<1)
			return;
		dst=tileMGL->GetScreen()+x+y*640;
		src=tiles[t];
	}
	else
	{
		wid=TILE_WIDTH;
		dst=tileMGL->GetScreen()+x+y*640;
		src=tiles[t];
	}

	if(y<0)
	{
		dst-=y*640;
		src-=y*TILE_WIDTH;

		hgt=TILE_HEIGHT+y;
	}
	else if(y>480-TILE_HEIGHT)
	{
		hgt=TILE_HEIGHT-(y-(480-TILE_HEIGHT));
	}
	else
		hgt=TILE_HEIGHT;

    while (hgt > 0) {
        hgt--;
        memcpy(dst, src, wid);
        dst += 640;
        src += 32;
    }
}

void RenderFloorTileTrans(int x,int y,int t,char light)
{
    byte *dst, *src;
    int wid, hgt;

	if(x<0)
	{
		wid=TILE_WIDTH+x;
		if(wid<1)
			return;

		dst=tileMGL->GetScreen()+y*640;
		src=tiles[t]-x;
	}
	else if (x>640-TILE_WIDTH)
	{
		wid=TILE_WIDTH-(x-(640-TILE_WIDTH));
		if(wid<1)
			return;
		dst=tileMGL->GetScreen()+x+y*640;
		src=tiles[t];
	}
	else
	{
		wid=TILE_WIDTH;
		dst=tileMGL->GetScreen()+x+y*640;
		src=tiles[t];
	}

	if(y<0)
	{
		dst-=y*640;
		src-=y*TILE_WIDTH;

		hgt=TILE_HEIGHT+y;
	}
	else if(y>480-TILE_HEIGHT)
	{
		hgt=TILE_HEIGHT-(y-(480-TILE_HEIGHT));
	}
	else
		hgt=TILE_HEIGHT;

	if(hgt<=0)
		return;

    while (hgt > 0) {
        hgt--;
        for (int i = 0; i < wid; ++i) {
            if (src[i]) dst[i] = SprModifyLight(src[i], light);
        }
        dst += 640;
        src += 32;
    }
}

void RenderWallTile(int x,int y,byte w,byte f,char light)
{
	RenderFloorTile(x,y,w+199,light);
	RenderFloorTile(x,y-TILE_HEIGHT,f,light);
}

void RenderWallTileTrans(int x,int y,byte w,byte f,char light)
{
	RenderFloorTile(x,y,w+199,light);
	RenderFloorTileTrans(x,y-TILE_HEIGHT,f,light);
}

void PlotStar(int x,int y,byte col,byte tx,byte ty,byte tileNum)
{
	byte *dst;

	if(tiles[tileNum][tx+ty*TILE_WIDTH]==0)
	{
		dst=tileMGL->GetScreen()+x+y*640;
		*dst=col;
	}
}

// -- Disco!

byte discoTab[]={1,3,4,5,6,7};

static inline byte PickDiscoColor() {
	return discoTab[rand() % 6]*32;
}

// -- Gourad!

#define GB_WID	(TILE_WIDTH/2)
#define GB_HEI	(TILE_HEIGHT/2)

void GouraudBox(int x,int y,byte *src,char light0,char light1,char light2,char light3)
{
	int i,j,tmp;
	byte *dst;
	int curLight,dlx,dly1,dly2,firstLight,lastLight;

	dst=tileMGL->GetScreen()+x+y*640;

	curLight=light0*FIXAMT;

	firstLight=light0*FIXAMT;
	lastLight=light1*FIXAMT;
	dly1=(light2-light0)*FIXAMT/GB_HEI;
	dly2=(light3-light1)*FIXAMT/GB_HEI;

	for(j=0;j<GB_HEI;j++)
	{
		dlx=(lastLight-firstLight)/GB_WID;
		curLight=firstLight;
		if(y+j>479)
			return;	// all done!
		if(y+j>=0)
		{
			for(i=0;i<GB_WID;i++)
			{
				if(x+i>=0 && x+i<640)
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
		dst+=(640-GB_WID);
		src+=GB_WID;

		firstLight+=dly1;
		lastLight+=dly2;
	}
}

void GouraudBoxTrans(int x,int y,byte *src,char light0,char light1,char light2,char light3)
{
	int i,j,tmp;
	byte *dst;
	int curLight,dlx,dly1,dly2,firstLight,lastLight;

	dst=tileMGL->GetScreen()+x+y*640;

	curLight=light0*FIXAMT;

	firstLight=light0*FIXAMT;
	lastLight=light1*FIXAMT;
	dly1=(light2-light0)*FIXAMT/GB_HEI;
	dly2=(light3-light1)*FIXAMT/GB_HEI;

	for(j=0;j<GB_HEI;j++)
	{
		dlx=(lastLight-firstLight)/GB_WID;
		curLight=firstLight;
		if(y+j>479)
			return;	// all done!
		if(y+j>=0)
		{
			for(i=0;i<GB_WID;i++)
			{
				if(x+i>=0 && x+i<640)
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
		dst+=(640-GB_WID);
		src+=GB_WID;

		firstLight+=dly1;
		lastLight+=dly2;
	}
}

void GouraudBoxDiscoTrans(int x,int y,byte *src,char light0,char light1,char light2,char light3)
{
	int i,j,tmp;
	byte *dst;
	int curLight,dlx,dly1,dly2,firstLight,lastLight;
	byte color;

	dst=tileMGL->GetScreen()+x+y*640;

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
		if(y+j>479)
			return;	// all done!
		if(y+j>=0)
		{
			for(i=0;i<GB_WID;i++)
			{
				if(x+i>=0 && x+i<640)
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
		dst+=(640-GB_WID);
		src+=GB_WID;

		firstLight+=dly1;
		lastLight+=dly2;
	}
}

void GouraudBoxDisco(int x,int y,byte *src,char light0,char light1,char light2,char light3)
{
	int i,j,tmp;
	byte *dst;
	int curLight,dlx,dly1,dly2,firstLight,lastLight;
	byte color;

	dst=tileMGL->GetScreen()+x+y*640;

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
		if(y+j>479)
			return;	// all done!
		if(y+j>=0)
		{
			for(i=0;i<GB_WID;i++)
			{
				if(x+i>=0 && x+i<640)
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
		dst+=(640-GB_WID);
		src+=GB_WID;

		firstLight+=dly1;
		lastLight+=dly2;
	}
}

void RenderFloorTileFancy(int x,int y,int t,byte shadow,char *theLight)
{
	// 9 light values are passed in:
	//
	//   0  1  2
	//   3  4  5
	//   6  7  8

	int i,j;
	char light[9];

	if(x<=-TILE_WIDTH || y<=-TILE_HEIGHT || x>639 || y>479)
		return;	// no need to render


	if(!opt.smoothLight)
	{
		if(shadow==1 || shadow==2)
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

	if(j==9 && !shadow && !opt.discoMode)
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

	if(opt.discoMode)
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

void RenderWallTileFancy(int x,int y,int t,char *theLight)
{
	// 9 light values are passed in:
	//
	//   0  1  2
	//   3  4  5
	//   6  7  8

	int i,j;
	char light[9];

	if(x<=-TILE_WIDTH || y<=-TILE_HEIGHT || x>639 || y>479)
		return;	// no need to render

	if(!opt.smoothLight)
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

	if(j==9 && !opt.discoMode)
	{
		RenderFloorTileUnlit(x,y,t);
		return;
	}

	if(opt.discoMode)
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

void RenderRoofTileFancy(int x,int y,int t,byte trans,byte wallBelow,char *theLight)
{
	// 9 light values are passed in:
	//
	//   0  1  2
	//   3  4  5
	//   6  7  8

	int i,j;
	char light[9];

	if(x<=-TILE_WIDTH || y<=-TILE_HEIGHT || x>639 || y>479)
		return;	// no need to render


	if(!opt.smoothLight)
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

	if(j==9 && !opt.discoMode)
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

	if(opt.discoMode)
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
