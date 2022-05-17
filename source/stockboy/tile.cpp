#include "tile.h"
#include "profile.h"
#include "display.h"

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

void SetTiles(byte *scrn,byte wall)
{
	int i,j;
	int x,y;
	int start;

	x=0;
	y=0;
	start=0;

	for(i=start;i<NUMTILES;i++)
	{
		for(j=0;j<TILE_HEIGHT;j++)
			memcpy(&tiles[i][j*TILE_WIDTH],&scrn[x+(y+j)*SCRWID],TILE_WIDTH);
		x+=TILE_WIDTH;
		if(x>SCRWID-1)
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

void RenderFloorTile(int x,int y,int t,char light)
{
	byte *dst,*src;
	int wid,hgt;

	if(light==0)
	{
		RenderFloorTileUnlit(x,y,t);
		return;
	}

	if(x<0)
	{
		wid=TILE_WIDTH+x;
		if(wid<1)
			return;

		dst=tileMGL->GetScreen()+y*SCRWID;
		src=tiles[t]-x;
	}
	else if (x>SCRWID-TILE_WIDTH)
	{
		wid=TILE_WIDTH-(x-(SCRWID-TILE_WIDTH));
		if(wid<1)
			return;
		dst=tileMGL->GetScreen()+x+y*SCRWID;
		src=tiles[t];
	}
	else
	{
		wid=TILE_WIDTH;
		dst=tileMGL->GetScreen()+x+y*SCRWID;
		src=tiles[t];
	}

	if(y<0)
	{
		dst-=y*SCRWID;
		src-=y*TILE_WIDTH;

		hgt=TILE_HEIGHT+y;
	}
	else if(y>SCRHEI-TILE_HEIGHT)
	{
		hgt=TILE_HEIGHT-(y-(SCRHEI-TILE_HEIGHT));
	}
	else
		hgt=TILE_HEIGHT;

	if(hgt<=0)
		return;

	if(light<-28)
	{
		// just render a black box
		while(hgt>0)
		{
			hgt--;
			memset(dst,0,wid);
			dst+=SCRWID;
		}
		return;
	}

	while (hgt > 0)
	{
		hgt--;
		for (int i = 0; i < wid; ++i)
		{
			dst[i] = SprModifyLight(src[i], light);
		}
		dst += 640;
		src += 32;
	}

	if(profile.colorblind)
		RenderColorblindTileInfo(x+TILE_WIDTH/2,y+TILE_HEIGHT/2-8,t);
}

void RenderFloorTileShadow(int x,int y,int t,char light)
{
	byte *dst,*src;
	int wid,hgt;
	int darkpart;

	if(x<0)
	{
		wid=TILE_WIDTH+x;
		if(wid<1)
			return;

		darkpart=8;
		dst=tileMGL->GetScreen()+y*SCRWID;
		src=tiles[t]-x;
	}
	else if (x>SCRWID-TILE_WIDTH)
	{
		wid=TILE_WIDTH-(x-(SCRWID-TILE_WIDTH));
		if(wid<1)
			return;
		darkpart=8-(x-(SCRWID-TILE_WIDTH));
		dst=tileMGL->GetScreen()+x+y*SCRWID;
		src=tiles[t];
	}
	else
	{
		wid=TILE_WIDTH;
		dst=tileMGL->GetScreen()+x+y*SCRWID;
		src=tiles[t];
		darkpart=8;	// shadows are 8 pixels wide I guess
	}

	if(y<0)
	{
		dst-=y*SCRWID;
		src-=y*TILE_WIDTH;

		hgt=TILE_HEIGHT+y;
	}
	else if(y>SCRHEI-TILE_HEIGHT)
	{
		hgt=TILE_HEIGHT-(y-(SCRHEI-TILE_HEIGHT));
	}
	else
		hgt=TILE_HEIGHT;

	if(hgt<=0)
		return;

	// if the whole thing is in shadow, deal
	if(darkpart>wid)
		light-=4;

	while (hgt > 0)
	{
		hgt--;
		for (int i = 0; i < wid; ++i)
		{
			dst[i] = SprModifyLight(src[i], light - 4 * (i > wid - darkpart));
		}
		dst += 640;
		src += 32;
	}
}

void RenderFloorTileUnlit(int x,int y,int t)
{
	byte *dst,*src;
	int wid,hgt;

	if(x<0)
	{
		wid=TILE_WIDTH+x;
		if(wid<1)
			return;

		dst=tileMGL->GetScreen()+y*SCRWID;
		src=tiles[t]-x;
	}
	else if (x>SCRWID-TILE_WIDTH)
	{
		wid=TILE_WIDTH-(x-(SCRWID-TILE_WIDTH));
		if(wid<1)
			return;
		dst=tileMGL->GetScreen()+x+y*SCRWID;
		src=tiles[t];
	}
	else
	{
		wid=TILE_WIDTH;
		dst=tileMGL->GetScreen()+x+y*SCRWID;
		src=tiles[t];
	}

	if(y<0)
	{
		dst-=y*SCRWID;
		src-=y*TILE_WIDTH;

		hgt=TILE_HEIGHT+y;
	}
	else if(y>SCRHEI-TILE_HEIGHT)
	{
		hgt=TILE_HEIGHT-(y-(SCRHEI-TILE_HEIGHT));
	}
	else
		hgt=TILE_HEIGHT;

	if(hgt<=0)
		return;

	while (hgt > 0)
	{
		hgt--;
		memcpy(dst, src, wid);
		dst += 640;
		src += 32;
	}

	if(profile.colorblind)
		RenderColorblindTileInfo(x+TILE_WIDTH/2,y+TILE_HEIGHT/2-8,t);
}

void RenderColorblindTileInfo(int x,int y,int t)
{
	int colTab[]=
		{5,25,45,65,
		 13,33,53,73,
		 83,103,123,143,
		 91,111,131,151,
		 161,181,201,221,
		 169,189,209,229,
		 241,262,281,301,
		 249,269,289,309,
		};
	char ch[][2]={"G","O","B","R","Y","P","A"};

	int i,j;

	for(i=0;i<7;i++)
	{
		for(j=0;j<32;j++)
			if(t==colTab[j]+i)
				CenterPrintOutline(x,y,ch[i],0,1);
	}
}

void RenderFloorTileTrans(int x,int y,int t,char light)
{
	byte *dst,*src;
	int wid,hgt;

	if(x<0)
	{
		wid=TILE_WIDTH+x;
		if(wid<1)
			return;

		dst=tileMGL->GetScreen()+y*SCRWID;
		src=tiles[t]-x;
	}
	else if (x>SCRWID-TILE_WIDTH)
	{
		wid=TILE_WIDTH-(x-(SCRWID-TILE_WIDTH));
		if(wid<1)
			return;
		dst=tileMGL->GetScreen()+x+y*SCRWID;
		src=tiles[t];
	}
	else
	{
		wid=TILE_WIDTH;
		dst=tileMGL->GetScreen()+x+y*SCRWID;
		src=tiles[t];
	}

	if(y<0)
	{
		dst-=y*SCRWID;
		src-=y*TILE_WIDTH;

		hgt=TILE_HEIGHT+y;
	}
	else if(y>SCRHEI-TILE_HEIGHT)
	{
		hgt=TILE_HEIGHT-(y-(SCRHEI-TILE_HEIGHT));
	}
	else
		hgt=TILE_HEIGHT;

	if(hgt<=0)
		return;

	while (hgt > 0)
	{
		hgt--;
		for (int i = 0; i < wid; ++i)
		{
			if (src[i]) dst[i] = SprModifyLight(src[i], light);
		}
		dst += 640;
		src += 32;
	}
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
		dst=tileMGL->GetScreen()+x+y*SCRWID;
		*dst=col;
	}
}

void RenderFloorTileSml(int x,int y,int t)
{
	byte *dst,*src;
	int wid,hgt;
	int i,j;

	wid=TILE_WIDTH;
	dst=tileMGL->GetScreen()+x+y*SCRWID;
	src=tiles[t];
	hgt=TILE_HEIGHT;

	for(j=0;j<hgt/2;j++)
	{
		for(i=0;i<wid/2;i++)
		{
			*dst=*src;
			src+=2;
			dst++;
		}
		dst+=SCRWID-wid/2;
		src+=wid;
	}
}
