#ifndef TILE_H
#define TILE_H

#include "winpch.h"
#include "mgldraw.h"

#define TILE_WIDTH  32
#define TILE_HEIGHT 24
#define NUMTILES	400

typedef byte tile_t[TILE_WIDTH*TILE_HEIGHT];

void InitTiles(MGLDraw *mgl);
void ExitTiles(void);
void SetTiles(byte *scrn);
void LoadTiles(FILE *f);
void SaveTiles(FILE *f);
void RenderFloorTile(int x,int y,int t,char light);
void RenderFloorTileTrans(int x,int y,int t,char light);
void RenderFloorTileShadow(int x,int y,int t,char light);
void RenderFloorTileUnlit(int x,int y,int t);
void RenderWallTile(int x,int y,byte w,byte f,char light);
void RenderWallTileTrans(int x,int y,byte w,byte f,char light);

#endif