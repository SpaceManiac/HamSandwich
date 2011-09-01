#include "tile.h"

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


	__asm
	{
		pusha
		push ds
		pop	 es
		mov  esi,src
		mov  edi,dst
		mov  edx,hgt
		mov  ecx,wid
		mov  bh,light
loop1:
		mov  al,[esi]
		mov  bl,al
		and  bl,~31
		add  al,bh
		cmp  al,bl
		jae	 okay1
		cmp  bh,0
		jl	 fine
		mov  al,bl
		add  al,31
		jmp okay2
fine:
		mov  al,bl
		jmp okay2
okay1:
		add  bl,31
		cmp  al,bl
		jb	 okay2
		cmp  bh,0
		jl   fine2
		mov  al,bl
		jmp  okay2
fine2:
		mov  al,bl
		and  al,(~31)
okay2:
		mov  [edi],al
		inc  esi
		inc  edi
		dec  ecx
		jnz	 loop1
		mov  ecx,wid
		add  esi,TILE_WIDTH
		sub	 esi,wid
		add  edi,640
		sub  edi,wid
		dec  edx
		jnz  loop1
		popa
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

	__asm
	{
		pusha
		push ds
		pop	 es
		mov  esi,src
		mov  edi,dst
		mov  edx,hgt
		mov  ecx,wid
		mov  bh,light
loop1:
		mov  al,[esi]
		mov  bl,al
		and  bl,~31
		add  al,bh
		cmp  al,bl
		jae	 okay1
		cmp  bh,0
		jl	 fine
		mov  al,bl
		add  al,31
		jmp okay2
fine:
		mov  al,bl
		jmp okay2
okay1:
		add  bl,31
		cmp  al,bl
		jb	 okay2
		cmp  bh,0
		jl   fine2
		mov  al,bl
		jmp  okay2
darkenit:
		sub  bh,4
		jmp  donedarken
fine2:
		mov  al,bl
		and  al,(~31)
okay2:
		mov  [edi],al
		inc  esi
		inc  edi
		cmp  ecx,darkpart
		je   darkenit
donedarken:
		dec  ecx
		jnz	 loop1
		mov  bh,light
		mov  ecx,wid
		add  esi,TILE_WIDTH
		sub	 esi,wid
		add  edi,640
		sub  edi,wid
		dec  edx
		jnz  loop1
		popa
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

	if(hgt<=0)
		return;

	__asm
	{
		pusha
		push ds
		pop	 es
		mov  esi,src
		mov  edi,dst
		mov  edx,hgt
		mov  ecx,wid
loop1:
		rep  movsb
		mov  ecx,wid
		add  esi,TILE_WIDTH
		sub	 esi,wid
		add  edi,640
		sub  edi,wid
		dec  edx
		jnz  loop1
		popa
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

	__asm
	{
		pusha
		push ds
		pop	 es
		mov  esi,src
		mov  edi,dst
		mov  edx,hgt
		mov  ecx,wid
		mov  bh,light
loop1:
		mov  al,[esi]
		cmp  al,0
		je   trans
		mov  bl,al
		and  bl,~31
		add  al,bh
		cmp  al,bl
		jae	 okay1
		cmp  bh,0
		jl	 fine
		mov  al,bl
		add  al,31
		jmp okay2
fine:
		mov  al,bl
		jmp okay2
okay1:
		add  bl,31
		cmp  al,bl
		jb	 okay2
		cmp  bh,0
		jl   fine2
		mov  al,bl
		jmp  okay2
fine2:
		mov  al,bl
		and  al,(~31)
okay2:
		mov  [edi],al
trans:
		inc  esi
		inc  edi
		dec  ecx
		jnz	 loop1
		mov  ecx,wid
		add  esi,TILE_WIDTH
		sub	 esi,wid
		add  edi,640
		sub  edi,wid
		dec  edx
		jnz  loop1
		popa
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
