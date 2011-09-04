#include "tile.h"
#include "asm.h"

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

void RenderFloorTile(int x,int y,int t,char light_)
{
    ASM_VAR(char, plain, light, light_);
    ASM_VAR(byte*, plain, dst, NULL);
    ASM_VAR(byte*, plain, src, NULL);
    ASM_VAR(int, plain, wid, 0);
    ASM_VAR(int, plain, hgt, 0);

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

    ASM_START()
_(          pusha                )
_(          push ds              )
_(          pop     es           )
_(          mov  esi,plain_src   )
_(          mov  edi,plain_dst   )
_(          mov  edx,plain_hgt   )
_(          mov  ecx,plain_wid   )
_(          mov  bh,plain_light  )
_(  plain_loop1:                 )
_(          mov  al,[esi]        )
_(          mov  bl,al           )
_(          and  bl,~31          )
_(          add  al,bh           )
_(          cmp  al,bl           )
_(          jae  plain_okay1     )
_(          cmp  bh,0            )
_(          jl   plain_fine      )
_(          mov  al,bl           )
_(          add  al,31           )
_(          jmp  plain_okay2     )
_(  plain_fine:                  )
_(          mov  al,bl           )
_(          jmp plain_okay2      )
_(  plain_okay1:                 )
_(          add  bl,31           )
_(          cmp  al,bl           )
_(          jb   plain_okay2     )
_(          cmp  bh,0            )
_(          jl   plain_fine2     )
_(          mov  al,bl           )
_(          jmp  plain_okay2     )
_(  plain_fine2:                 )
_(          mov  al,bl           )
_(          and  al,(~31)        )
_(  plain_okay2:                 )
_(          mov  [edi],al        )
_(          inc  esi             )
_(          inc  edi             )
_(          dec  ecx             )
_(          jnz  plain_loop1     )
_(          mov  ecx,plain_wid   )
_(          add  esi,32          ) // TILE_WIDTH -> 32
_(          sub  esi,plain_wid   )
_(          add  edi,640         )
_(          sub  edi,plain_wid   )
_(          dec  edx             )
_(          jnz  plain_loop1     )
_(          popa                 )
    ASM_END()
}

void RenderFloorTileShadow(int x,int y,int t,char light_)
{
    ASM_VAR(char, shadow, light, light_);
    ASM_VAR(byte*, shadow, dst, NULL);
    ASM_VAR(byte*, shadow, src, NULL);
    ASM_VAR(int, shadow, wid, 0);
    ASM_VAR(int, shadow, hgt, 0);
    ASM_VAR(int, shadow, darkpart, 0);

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

    ASM_START()
_(          pusha                )
_(          push ds              )
_(          pop     es           )
_(          mov  esi,shadow_src   )
_(          mov  edi,shadow_dst   )
_(          mov  edx,shadow_hgt   )
_(          mov  ecx,shadow_wid   )
_(          mov  bh,shadow_light  )
_(  shadow_loop1:                )
_(          mov  al,[esi]        )
_(          mov  bl,al           )
_(          and  bl,~31          )
_(          add  al,bh           )
_(          cmp  al,bl           )
_(          jae  shadow_okay1    )
_(          cmp  bh,0            )
_(          jl   shadow_fine     )
_(          mov  al,bl           )
_(          add  al,31           )
_(          jmp shadow_okay2     )
_(  shadow_fine:                 )
_(          mov  al,bl           )
_(          jmp shadow_okay2     )
_(  shadow_okay1:                )
_(          add  bl,31           )
_(          cmp  al,bl           )
_(          jb   shadow_okay2    )
_(          cmp  bh,0            )
_(          jl   shadow_fine2    )
_(          mov  al,bl           )
_(          jmp  shadow_okay2    )
_(  shadow_darkenit:             )
_(          sub  bh,4            )
_(          jmp  shadow_donedarken)
_(  shadow_fine2:                )
_(          mov  al,bl           )
_(          and  al,(~31)        )
_(  shadow_okay2:                )
_(          mov  [edi],al        )
_(          inc  esi             )
_(          inc  edi             )
_(          cmp  ecx,shadow_darkpart)
_(          je   shadow_darkenit )
_(  shadow_donedarken:           )
_(          dec  ecx             )
_(          jnz  shadow_loop1    )
_(          mov  bh,shadow_light )
_(          mov  ecx,shadow_wid  )
_(          add  esi,32          ) // TILE_WIDTH -> 32
_(          sub  esi,shadow_wid  )
_(          add  edi,640         )
_(          sub  edi,shadow_wid  )
_(          dec  edx             )
_(          jnz  shadow_loop1    )
_(          popa                 )
    ASM_END()
}

void RenderFloorTileUnlit(int x,int y,int t)
{
    ASM_VAR(byte*, unlit, dst, NULL);
    ASM_VAR(byte*, unlit, src, NULL);
    ASM_VAR(int, unlit, wid, 0);
    ASM_VAR(int, unlit, hgt, 0);

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

    ASM_START()
_(          pusha                )
_(          push ds              )
_(          pop     es           )
_(          mov  esi,unlit_src   )
_(          mov  edi,unlit_dst   )
_(          mov  edx,unlit_hgt   )
_(          mov  ecx,unlit_wid   )
_(  unlit_loop1:                 )
_(          rep  movsb           )
_(          mov  ecx,unlit_wid   )
_(          add  esi,32          ) // TILE_WIDTH -> 32
_(          sub  esi,unlit_wid   )
_(          add  edi,640         )
_(          sub  edi,unlit_wid   )
_(          dec  edx             )
_(          jnz  unlit_loop1     )
_(          popa                 )
    ASM_END()
}

void RenderFloorTileTrans(int x,int y,int t,char light_)
{
    ASM_VAR(char, trans, light, light_);
    ASM_VAR(byte*, trans, dst, NULL);
    ASM_VAR(byte*, trans, src, NULL);
    ASM_VAR(int, trans, wid, 0);
    ASM_VAR(int, trans, hgt, 0);

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

    ASM_START()
_(          pusha                )
_(          push ds              )
_(          pop     es           )
_(          mov  esi,trans_src   )
_(          mov  edi,trans_dst   )
_(          mov  edx,trans_hgt   )
_(          mov  ecx,trans_wid   )
_(          mov  bh,trans_light  )
_(  trans_loop1:                 )
_(          mov  al,[esi]        )
_(          cmp  al,0            )
_(          je   trans           )
_(          mov  bl,al           )
_(          and  bl,~31          )
_(          add  al,bh           )
_(          cmp  al,bl           )
_(          jae  trans_okay1     )
_(          cmp  bh,0            )
_(          jl   trans_fine      )
_(          mov  al,bl           )
_(          add  al,31           )
_(          jmp  trans_okay2     )
_(  trans_fine:                  )
_(          mov  al,bl           )
_(          jmp trans_okay2      )
_(  trans_okay1:                 )
_(          add  bl,31           )
_(          cmp  al,bl           )
_(          jb   trans_okay2     )
_(          cmp  bh,0            )
_(          jl   trans_fine2     )
_(          mov  al,bl           )
_(          jmp  trans_okay2     )
_(  trans_fine2:                 )
_(          mov  al,bl           )
_(          and  al,(~31)        )
_(  trans_okay2:                 )
_(          mov  [edi],al        )
_(  trans:                       )
_(          inc  esi             )
_(          inc  edi             )
_(          dec  ecx             )
_(          jnz  trans_loop1     )
_(          mov  ecx,trans_wid   )
_(          add  esi,32          ) // TILE_WIDTH -> 32
_(          sub  esi,trans_wid   )
_(          add  edi,640         )
_(          sub  edi,trans_wid   )
_(          dec  edx             )
_(          jnz  trans_loop1     )
_(          popa                 )
	ASM_END()
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
