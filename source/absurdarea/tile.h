#ifndef TILE_H
#define TILE_H

#include "winpch.h"
#include "mgldraw.h"

#define TILE_WIDTH  32
#define TILE_HEIGHT 24
#define NUMTILES	1200
#define TILEPAGESIZE 400

#define WATER_WID	(96)
#define WATER_HEI	(72)

typedef byte tile_t[TILE_WIDTH*TILE_HEIGHT];

void InitTiles(MGLDraw *mgl);
void ExitTiles(void);
void SetTiles(byte *scrn,byte wall);
void LoadTiles(FILE *f);
void SaveTiles(FILE *f);
void RenderFloorTile(int x,int y,int t,char light);
void RenderFloorTileTrans(int x,int y,int t,char light);
void RenderFloorTileShadow(int x,int y,int t,char light);
void RenderFloorTileUnlit(int x,int y,int t);
void RenderWallTile(int x,int y,word w,word f,char light);
void RenderWallTileTrans(int x,int y,word w,word f,char light);
void PlotStar(int x,int y,byte col,byte tx,byte ty,word tileNum);

void RenderFloorTileFancy(int x,int y,int t,byte shadow,char *light);
void RenderRoofTileFancy(int x,int y,int t,byte trans,byte wallBelow,char *theLight);
void RenderWallTileFancy(int x,int y,int t,char *light);
void RenderFloorTileFancyWater(int x,int y,int t,byte water,byte shadow,char *theLight);

#endif
