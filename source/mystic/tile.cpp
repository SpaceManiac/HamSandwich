#include "tile.h"
#include "water.h"
#include "game.h"
#include "options.h"

tile_t tiles[NUMTILES];
MGLDraw *tileMGL;

void InitTiles(MGLDraw *mgl)
{
	tileMGL=mgl;
	InitWater();
}

void ExitTiles(void)
{
	ExitWater();
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
	if(light==0)
	{
		RenderFloorTileUnlit(x,y,t);
		return;
	}
	else if(light<-28)
	{
		// just render a black box
		int i,j;
		byte *dst;
		int pitch;

		dst=tileMGL->GetScreen();
		pitch=tileMGL->GetWidth();

		for(i=0;i<TILE_WIDTH;i++)
			for(j=0;j<TILE_HEIGHT;j++)
				if(x+i>=0 && x+i<640 && y+j>=0 && y+j<480)
				{
					dst[(x+i)+(y+j)*pitch]=0;
				}
		return;
	}
	else
	{
		int i,j;
		byte *dst,b;
		int pitch;

		dst=tileMGL->GetScreen();
		pitch=tileMGL->GetWidth();

		for(i=0;i<TILE_WIDTH;i++)
			for(j=0;j<TILE_HEIGHT;j++)
				if(x+i>=0 && x+i<640 && y+j>=0 && y+j<480)
				{
					b=tiles[t][i+j*TILE_WIDTH];
					if((b&(~31))==3*32)
						b=WaterPixel(x+i,y+j);
					if((b&31)+light>31)
						b=(b&(~31))+31;
					else if((b&31)+light<0)
						b=(b&(~31));
					else
						b+=light;
					dst[(x+i)+(y+j)*pitch]=b;
				}
		return;
	}
}

void RenderFloorTileShadow(int x,int y,int t,char light)
{
	if(light<-28)
	{
		// just render a black box
		int i,j;
		byte *dst;
		int pitch;

		dst=tileMGL->GetScreen();
		pitch=tileMGL->GetWidth();

		for(i=0;i<TILE_WIDTH;i++)
			for(j=0;j<TILE_HEIGHT;j++)
				if(x+i>=0 && x+i<640 && y+j>=0 && y+j<480)
				{
					dst[(x+i)+(y+j)*pitch]=0;
				}
		return;
	}
	else
	{
		int i,j;
		byte *dst,b;
		int pitch;

		dst=tileMGL->GetScreen();
		pitch=tileMGL->GetWidth();

		for(i=0;i<TILE_WIDTH;i++)
			for(j=0;j<TILE_HEIGHT;j++)
				if(x+i>=0 && x+i<640 && y+j>=0 && y+j<480)
				{
					b=tiles[t][i+j*TILE_WIDTH];
					if((b&(~31))==3*32)
						b=WaterPixel(x+i,y+j);
					if(i<TILE_WIDTH-8)
					{
						if((b&31)+light>31)
							b=(b&(~31))+31;
						else if((b&31)+light<0)
							b=(b&(~31));
						else
							b+=light;
					}
					else
					{
						if((b&31)+light-4>31)
							b=(b&(~31))+31;
						else if((b&31)+light-4<0)
							b=(b&(~31));
						else
							b+=light-4;
					}
					dst[(x+i)+(y+j)*pitch]=b;
				}
		return;
	}

	/*byte *dst,*src;
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
	*/
}

void RenderFloorTileUnlit(int x,int y,int t)
{
	int i,j;
	byte *dst,b;
	int pitch;

	dst=tileMGL->GetScreen();
	pitch=tileMGL->GetWidth();

	for(i=0;i<TILE_WIDTH;i++)
		for(j=0;j<TILE_HEIGHT;j++)
			if(x+i>=0 && x+i<640 && y+j>=0 && y+j<480)
			{
				b=tiles[t][i+j*TILE_WIDTH];

				if((b&(~31))==3*32)
					b=WaterPixel(x+i,y+j);
				dst[(x+i)+(y+j)*pitch]=b;
			}
	/*
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
	}*/
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

#define GB_WID	(TILE_WIDTH/2)
#define GB_HEI	(TILE_HEIGHT/2)

inline void GouraudBox(int x,int y,byte *src,char light0,char light1,char light2,char light3)
{
	int i,j,tmp;
	byte *dst,b;
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
					b=*src;

					if((b&(~31))==3*32)
						b=WaterPixel(x+i,y+j);

					tmp=(b&31)+(curLight/FIXAMT);
					if(tmp<0)
						tmp=0;
					if(tmp>31)
						tmp=31;
					(*dst)=(b&(~31))+tmp;
						/*
					tmp=((*src)&31)+(curLight/FIXAMT);
					if(tmp<0)
						tmp=0;
					if(tmp>31)
						tmp=31;
					(*dst)=((*src)&(~31))+tmp;
					*/
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

inline void GouraudBoxTrans(int x,int y,byte *src,char light0,char light1,char light2,char light3)
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

	if(!opt.lightFX)
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

	if(j==9 && !shadow)
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

	GouraudBox(x,y,tiles[t],light[0],light[1],light[3],light[4]);
	GouraudBox(x+GB_WID,y,tiles[t]+GB_WID,light[1],light[2],light[4],light[5]);
	GouraudBox(x,y+GB_HEI,tiles[t]+GB_HEI*TILE_WIDTH,light[3],light[4],light[6],light[7]);
	GouraudBox(x+GB_WID,y+GB_HEI,tiles[t]+GB_WID+GB_HEI*TILE_WIDTH,light[4],light[5],light[7],light[8]);
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

	if(!opt.lightFX)
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

	if(j==9)
	{
		RenderFloorTileUnlit(x,y,t);
		return;
	}

	GouraudBox(x,y,tiles[t],light[0],light[1],light[3],light[4]);
	GouraudBox(x+GB_WID,y,tiles[t]+GB_WID,light[1],light[2],light[4],light[5]);
	GouraudBox(x,y+GB_HEI,tiles[t]+GB_HEI*TILE_WIDTH,light[3],light[4],light[6],light[7]);
	GouraudBox(x+GB_WID,y+GB_HEI,tiles[t]+GB_WID+GB_HEI*TILE_WIDTH,light[4],light[5],light[7],light[8]);
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

	if(!opt.lightFX)
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

	if(j==9)
	{
		if(trans)
			RenderFloorTileTrans(x,y,t,0);
		else
			RenderFloorTileUnlit(x,y,t);
		return;
	}

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

void GoldUpPixel(int t,int pix,int amt)
{
	if(amt>0)
	{
		if(tiles[t][pix]<32)	// grey
			tiles[t][pix]+=2*32;	// change to brown
		else if(tiles[t][pix]<3*32)	// brown
			tiles[t][pix]+=3*32;	// change to yellow
	}
	else
	{
		if(tiles[t][pix]<32)	// grey
		{
			// is good
		}
		else if(tiles[t][pix]<3*32)	// brown
			tiles[t][pix]-=2*32;	// change to grey
		else	// yellow
			tiles[t][pix]-=3*32;	// change to brown
	}
}

void GoldUpTile(int t,int amt)
{
	int s,i;
	byte all;

	if(amt==2 || amt==-2)
		all=1;
	else
		all=0;

	if(all)
	{
		for(s=0;s<32*24;s++)
		{
			GoldUpPixel(t,s,amt);
		}
	}
	else
	{
		for(i=0;i<16;i++)
		{
			s=MGL_random(32*24);
			GoldUpPixel(t,s,amt);
		}
	}
}
