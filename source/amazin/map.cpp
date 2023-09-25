#include "map.h"
#include "display.h"
#include "jamulspr.h"
#include "jamultypes.h"

struct mapTile
{
	uint16_t flags;
	byte reachability;
	byte tileAnim;
	byte tile;
	byte item;
	byte itemAnim;
	byte field6_0x7;
};

mapTile g_Map[14][19];
byte g_Tiles[21][32][32];

sprite_set_t *g_WallsJsp;
sprite_set_t *g_ItemsJsp;

void MapInit(void)
{
	bool ok;
	int y;
	int x;
	int t;
	int sy;
	int sx;

	mapTile *map = &g_Map[0][0];
	for (t = 0; t < 0x10a; t = t + 1)
	{
		map[t].flags = 0;
		map[t].tile = 0;
		map[t].reachability = 0;
		map[t].tileAnim = 0;
	}
	if (!mgl->LoadBMP("graphics/tiles.bmp"))
	{
		FatalError("Missing file: graphics/tiles.bmp");
		exit(1);
	}
	sx = 0;
	sy = 0;
	for (t = 0; t < 0x15; t = t + 1)
	{
		for (x = 0; x < 0x20; x = x + 1)
		{
			for (y = 0; y < 0x20; y = y + 1)
			{
				g_Tiles[t][y][x] = mgl->GetScreen()[sx + x + (sy + y) * mgl->GetWidth()];
			}
		}
		sx = sx + 0x20;
		if (0x260 < sx)
		{
			sx = 0;
			sy = sy + 0x20;
		}
	}
	g_WallsJsp = new sprite_set_t("graphics/walls.jsp");
	g_ItemsJsp = new sprite_set_t("graphics/items.jsp");
}

void MapExit()
{
	delete g_WallsJsp;
	delete g_ItemsJsp;
}

void TileRender(byte tile, int x, int y)
{
	for (int px = 0; px < 0x20; px = px + 1)
	{
		for (int py = 0; py < 0x20; py = py + 1)
		{
			mgl->GetScreen()[px + x + (y + py) * mgl->GetWidth()] = g_Tiles[tile][py][px];
		}
	}
}
