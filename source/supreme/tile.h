#ifndef TILE_H
#define TILE_H

#include "mgldraw.h"
#include <iosfwd>
#include <stdio.h>

constexpr int TILE_WIDTH  = 32;
constexpr int TILE_HEIGHT = 24;
constexpr int NUMTILES    = 1000;

struct Tilegfx
{
	int numTiles = 0;
	std::array<std::array<byte, TILE_WIDTH * TILE_HEIGHT>, NUMTILES> tiles;

	byte *GetTileData(int t) { return &tiles[t][0]; }
	const byte *GetTileData(int t) const { return &tiles[t][0]; }

	void SetTiles(const byte *src, int width, int height);
	void SetTile(int t, const byte *src, int pitch);

	void LoadTiles(SDL_RWops *f, int numTiles);
	void LoadTiles(std::istream& f, int numTiles);
	void SaveTiles(SDL_RWops *f) const;
	void SaveTiles(std::ostream& f) const;
	void SaveTilesToBMP(const char *fname) const;
};

void InitTiles(MGLDraw *mgl);
void ExitTiles();
void SetCurrentTilegfx(const Tilegfx *gfx);

void RenderFloorTile(int x,int y,int t,char light);
void RenderFloorTileTrans(int x,int y,int t,char light);
void RenderFloorTileShadow(int x,int y,int t,char light);
void RenderFloorTileUnlit(int x,int y,int t);
void RenderWallTile(int x,int y,word w,word f,char light);
void RenderWallTileTrans(int x,int y,word w,word f,char light);
void PlotStar(int x,int y,byte col,byte tx,byte ty,word tileNum);

// super shading!
void RenderFloorTileFancy(int x,int y,int t,byte shadow,const char *light);
void RenderRoofTileFancy(int x,int y,int t,byte trans,byte wallBelow,const char *theLight);
void RenderWallTileFancy(int x,int y,int t,const char *light);

#endif
