#ifndef TILE_H
#define TILE_H

#include "mgldraw.h"
#include <iostream>
#include <stdio.h>

constexpr int TILE_WIDTH  = 32;
constexpr int TILE_HEIGHT = 24;
constexpr int NUMTILES    = 1000;

typedef byte tile_t[TILE_WIDTH*TILE_HEIGHT];

void InitTiles(MGLDraw *mgl);
void ExitTiles(void);
void SetNumTiles(int n);
word GetNumTiles(void);
void SetTiles(byte *scrn);
void SetTile(int t,int x,int y,byte *src);
void LoadTiles(FILE *f);
void LoadTiles(std::istream& f);
void SaveTiles(FILE *f);
void SaveTiles(std::ostream& f);
void SaveTilesToBMP(const char *fname);
void RenderFloorTile(int x,int y,int t,char light);
void RenderFloorTileTrans(int x,int y,int t,char light);
void RenderFloorTileShadow(int x,int y,int t,char light);
void RenderFloorTileUnlit(int x,int y,int t);
void RenderWallTile(int x,int y,word w,word f,char light);
void RenderWallTileTrans(int x,int y,word w,word f,char light);
void PlotStar(int x,int y,byte col,byte tx,byte ty,word tileNum);

byte *GetTileData(int t);

void AppendTiles(int start,FILE *f);

void LoadOldTiles(FILE *f);	// for legacy loading

// super shading!
void RenderFloorTileFancy(int x,int y,int t,byte shadow,const char *light);
void RenderRoofTileFancy(int x,int y,int t,byte trans,byte wallBelow,const char *theLight);
void RenderWallTileFancy(int x,int y,int t,const char *light);

#endif
