#include "tile.h"
#include "jamulspr.h"

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

	if(light<-28)
	{
		// just render a black box
		while(hgt>0)
		{
			hgt--;
			memset(dst,0,wid);
			dst+=640;
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

	while (hgt > 0)
	{
		hgt--;
		memcpy(dst, src, wid);
		dst += 640;
		src += 32;
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
