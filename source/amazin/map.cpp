#include "map.h"
#include "appdata.h"
#include "display.h"
#include "jamulspr.h"
#include "jamultypes.h"
#include "options.h"
#include "particle.h"
#include "sound.h"

byte g_MapTimer;
int g_Player1StartX, g_Player1StartY;
byte g_MapChirpTimer;
int g_Player2StartX, g_Player2StartY;
MapTile g_Map[14][19];
sprite_set_t *g_ItemsJsp;
GameMode g_MapTheme;
sprite_set_t *g_WallsJsp;
word g_TimeUntilPowerupExpires;
byte g_PowerupOnMap;
byte g_Tiles[21][32][32];
byte g_PowerupX, g_PowerupY;

void MapInit(void)
{
	bool ok;
	int y;
	int x;
	int t;
	int sy;
	int sx;

	MapTile *map = &g_Map[0][0];
	for (t = 0; t < 0x10a; t = t + 1)
	{
		map[t].flags = 0;
		map[t].tile = 0;
		map[t].distanceFromPumpkinSpawn = 0;
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

void MapLoad(const char *fname)
{
	auto f = AssetOpen_SDL_Owned(fname);
	if (!f)
	{
		FatalError("Can't load map!");
		exit(1);
	}
	SDL_RWread(f, g_Map, 0x10a, 8);
}

void MapSave(const char *fname)
{
	auto f = AppdataOpen_Write_SDL(fname);
	SDL_RWwrite(f, g_Map, 0x10a, 8);
}

void MapClear(GameMode theme)
{
	unsigned int r;
	int y;
	int x;

	if (theme == GameMode::HappyFields)
	{
		for (x = 0; x < 0x13; x = x + 1)
		{
			for (y = 0; y < 0xe; y = y + 1)
			{
				g_Map[y][x].tile = (char)x + (char)y & 1;
			}
		}
	}
	else if (theme == GameMode::DankDungeons)
	{
		for (x = 0; x < 0x13; x = x + 1)
		{
			for (y = 0; y < 0xe; y = y + 1)
			{
				r = MGL_random(5);
				g_Map[y][x].tile = (char)r + 4;
			}
		}
	}
	else if (theme == GameMode::SiblingRivalry)
	{
		for (x = 0; x < 0x13; x = x + 1)
		{
			for (y = 0; y < 0xe; y = y + 1)
			{
				g_Map[y][x].tile = 0xb;
			}
		}
	}
	return;
}

void MapRenderToBackground(GameMode theme)
{
	int ty;
	int tx;
	int py;
	int px;

	g_MapTheme = theme;
	if (!mgl->LoadBMP("graphics/backgd.bmp"))
	{
		FatalError("Missing file: graphics/backgd.bmp");
		exit(1);
	}
	memcpy(g_Background, mgl->GetScreen(), 0x4b000);
	for (tx = 0; tx < 0x13; tx = tx + 1)
	{
		for (ty = 0; ty < 0xe; ty = ty + 1)
		{
			for (px = 0; px < 0x20; px = px + 1)
			{
				for (py = 0; py < 0x20; py = py + 1)
				{
					g_Background[px + tx * 0x20 + 0x10 + (py + ty * 0x20 + 0x20) * mgl->GetWidth()] =
						g_Tiles[g_Map[ty][tx].tile][py][px];
				}
			}
		}
	}
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

void MapFloodFillReachable(int x, int y)
{
	if (((0 < y) && ((g_Map[y + -1][x].flags & TileFlags::WallH) == 0)) &&
	    (g_Map[y][x].distanceFromPumpkinSpawn + 1 < (unsigned int)g_Map[y + -1][x].distanceFromPumpkinSpawn))
	{
		g_Map[y + -1][x].distanceFromPumpkinSpawn = g_Map[y][x].distanceFromPumpkinSpawn + 1;
		MapFloodFillReachable(x, y + -1);
	}
	if (((0 < x) && ((g_Map[y][x].flags & TileFlags::WallV) == 0)) &&
	    (g_Map[y][x].distanceFromPumpkinSpawn + 1 < (unsigned int)g_Map[y][x - 1].distanceFromPumpkinSpawn))
	{
		g_Map[y][x - 1].distanceFromPumpkinSpawn = g_Map[y][x].distanceFromPumpkinSpawn + 1;
		MapFloodFillReachable(x + -1, y);
	}
	if (((x < 0x12) && ((g_Map[y][x + 1].flags & TileFlags::WallV) == 0)) &&
	    (g_Map[y][x].distanceFromPumpkinSpawn + 1 < (unsigned int)g_Map[y][x + 1].distanceFromPumpkinSpawn))
	{
		g_Map[y][x + 1].distanceFromPumpkinSpawn = g_Map[y][x].distanceFromPumpkinSpawn + 1;
		MapFloodFillReachable(x + 1, y);
	}
	if (((y < 0xd) && ((g_Map[y][x].flags & TileFlags::WallH) == 0)) &&
	    (g_Map[y][x].distanceFromPumpkinSpawn + 1 < (unsigned int)g_Map[y + 1][x].distanceFromPumpkinSpawn))
	{
		g_Map[y + 1][x].distanceFromPumpkinSpawn = g_Map[y][x].distanceFromPumpkinSpawn + 1;
		MapFloodFillReachable(x, y + 1);
	}
}

void MapSetupStart(void)
{
	int startY;
	int startX;
	int y;
	int x;

	for (y = 0; y < 0xe; y = y + 1)
	{
		for (x = 0; x < 0x13; x = x + 1)
		{
			if ((g_Map[y][x].flags & TileFlags::PK) == 0)
			{
				g_Map[y][x].distanceFromPumpkinSpawn = 0xff;
			}
			else
			{
				g_Map[y][x].distanceFromPumpkinSpawn = 0;
				startX = x;
				startY = y;
			}
			if ((g_Map[y][x].flags & TileFlags::P1) != 0)
			{
				g_Player1StartX = (x * 0x20 + 0x10) * 0x100;
				g_Player1StartY = (y * 0x20 + 0x10) * 0x100;
			}
			if ((g_Map[y][x].flags & TileFlags::P2) != 0)
			{
				g_Player2StartX = (x * 0x20 + 0x10) * 0x100;
				g_Player2StartY = (y * 0x20 + 0x10) * 0x100;
			}
			g_Map[y][x].tileAnim = 0xff;
		}
	}
	MapFloodFillReachable(startX, startY);
}

void MapRandomize(int w, int h)
{
	byte bVar1;
	byte bVar2;
	bool bVar3;
	ItemType IVar4;
	byte bVar5;
	unsigned int x;
	unsigned int y;
	unsigned int uVar6;
	int iVar7;
	int iVar8;
	unsigned int y2;
	unsigned int x2;
	int local_870;
	int j;
	unsigned int i;
	int centerY;
	int centerX;
	int local_85c;
	MapTile tempMap[14][19];
	MapTile *tile;

	auto map = &g_Map[0][0];
	for (i = 0; (int)i < 0x10a; i = i + 1)
	{
		map[i].flags = 0;
		map[i].tile = 0;
		map[i].distanceFromPumpkinSpawn = 0;
		map[i].tileAnim = 0;
	}
	for (i = 0; (int)i < w; i = i + 1)
	{
		for (j = 0; j < h; j = j + 1)
		{
			g_Map[j][i].flags = 0;
			if (0 < (int)i)
			{
				g_Map[j][i].flags = g_Map[j][i].flags | TileFlags::WallV;
			}
			if (j < h + -1)
			{
				g_Map[j][i].flags = g_Map[j][i].flags | TileFlags::WallH;
			}
			g_Map[j][i].distanceFromPumpkinSpawn = (byte)i;
			g_Map[j][i].tileAnim = (byte)j;
		}
	}
	x = MGL_random(2);
	if ((x & 0xffff) == 0)
	{
		x = MGL_random((unsigned short)w - 2);
		x = x & 0xffff;
		i = x + 1;
		y = MGL_random((unsigned short)h);
		y = y & 0xffff;
		g_Map[y][x + 1].flags = TileFlags::WallH | TileFlags::PK | TileFlags::P;
		g_Map[y][x].flags = TileFlags::WallV | TileFlags::WallH | TileFlags::P;
		g_Map[y][x].distanceFromPumpkinSpawn = (byte)i;
		g_Map[y][x + 2].flags = TileFlags::WallH | TileFlags::P;
		g_Map[y][x + 2].distanceFromPumpkinSpawn = (byte)i;
	}
	else
	{
		x = MGL_random((unsigned short)w);
		x = x & 0xffff;
		y = MGL_random((unsigned short)h - 2);
		y = y & 0xffff;
		g_Map[y + 1][x].flags = TileFlags::WallV | TileFlags::PK | TileFlags::P;
		g_Map[y][x].flags = TileFlags::WallV | TileFlags::P;
		j = y + 1;
		g_Map[y][x].tileAnim = (byte)j;
		g_Map[y + 2][x].flags = TileFlags::WallV | TileFlags::WallH | TileFlags::P;
		g_Map[y + 2][x].tileAnim = (byte)j;
	}
	bVar3 = false;
LAB_0040f9c3:
	do
	{
		do
		{
			x = MGL_random((unsigned short)w);
			x = x & 0xffff;
			y = MGL_random((unsigned short)h);
			y = y & 0xffff;
		} while ((g_Map[y][x].flags & TileFlags::P) != 0);
		uVar6 = MGL_random(4);
		switch (uVar6 & 0xffff)
		{
		case 0:
			if (((x == w - 1U) || (((g_Map[y][x + 1].flags & TileFlags::P) != 0 && (bVar3)))) ||
			    (((g_Map[y][x + 1].distanceFromPumpkinSpawn == g_Map[y][x].distanceFromPumpkinSpawn &&
			       (g_Map[y][x + 1].tileAnim == g_Map[y][x].tileAnim)) ||
			      ((g_Map[y][x + 1].flags & TileFlags::WallV) == 0))))
				goto LAB_0040f9c3;
			g_Map[y][x + 1].flags = g_Map[y][x + 1].flags & ~TileFlags::WallV;
			if ((g_Map[y][x + 1].flags & TileFlags::P) != 0)
			{
				bVar3 = true;
			}
			x2 = (unsigned int)g_Map[y][x + 1].distanceFromPumpkinSpawn;
			y2 = (unsigned int)g_Map[y][x + 1].tileAnim;
			break;
		case 1:
			if ((((y == h - 1U) || (((g_Map[y + 1][x].flags & TileFlags::P) != 0 && (bVar3)))) ||
			     ((g_Map[y + 1][x].distanceFromPumpkinSpawn == g_Map[y][x].distanceFromPumpkinSpawn &&
			       (g_Map[y + 1][x].tileAnim == g_Map[y][x].tileAnim)))) ||
			    ((g_Map[y][x].flags & TileFlags::WallH) == 0))
				goto LAB_0040f9c3;
			g_Map[y][x].flags = g_Map[y][x].flags & ~TileFlags::WallH;
			if ((g_Map[y + 1][x].flags & TileFlags::P) != 0)
			{
				bVar3 = true;
			}
			x2 = (unsigned int)g_Map[y + 1][x].distanceFromPumpkinSpawn;
			y2 = (unsigned int)g_Map[y + 1][x].tileAnim;
			break;
		case 2:
			if (((x == 0) || (((g_Map[y][x - 1].flags & TileFlags::P) != 0 && (bVar3)))) ||
			    (((g_Map[y][x - 1].distanceFromPumpkinSpawn == g_Map[y][x].distanceFromPumpkinSpawn && (g_Map[y][x - 1].tileAnim == g_Map[y][x].tileAnim)) ||
			      ((g_Map[y][x].flags & TileFlags::WallV) == 0))))
				goto LAB_0040f9c3;
			g_Map[y][x].flags = g_Map[y][x].flags & ~TileFlags::WallV;
			if ((g_Map[y][x - 1].flags & TileFlags::P) != 0)
			{
				bVar3 = true;
			}
			x2 = (unsigned int)g_Map[y][x - 1].distanceFromPumpkinSpawn;
			y2 = (unsigned int)g_Map[y][x - 1].tileAnim;
			break;
		case 3:
			if ((((y == 0) || (((g_Map[y - 1][x].flags & TileFlags::P) != 0 && (bVar3)))) ||
			     ((g_Map[y - 1][x].distanceFromPumpkinSpawn == g_Map[y][x].distanceFromPumpkinSpawn &&
			       (g_Map[y - 1][x].tileAnim == g_Map[y][x].tileAnim)))) ||
			    ((g_Map[y - 1][x].flags & TileFlags::WallH) == 0))
				goto LAB_0040f9c3;
			g_Map[y - 1][x].flags = g_Map[y - 1][x].flags & ~TileFlags::WallH;
			if ((g_Map[y - 1][x].flags & TileFlags::P) != 0)
			{
				bVar3 = true;
			}
			x2 = (unsigned int)g_Map[y - 1][x].distanceFromPumpkinSpawn;
			y2 = (unsigned int)g_Map[y - 1][x].tileAnim;
		}
		local_85c = 0;
		bVar1 = g_Map[y][x].distanceFromPumpkinSpawn;
		bVar2 = g_Map[y][x].tileAnim;
		for (i = 0; (int)i < w; i = i + 1)
		{
			for (j = 0; j < h; j = j + 1)
			{
				if ((g_Map[j][i].distanceFromPumpkinSpawn == x2) && (g_Map[j][i].tileAnim == y2))
				{
					g_Map[j][i].distanceFromPumpkinSpawn = bVar1;
					g_Map[j][i].tileAnim = bVar2;
				}
				if ((g_Map[j][i].distanceFromPumpkinSpawn == bVar1) && (g_Map[j][i].tileAnim == bVar2))
				{
					local_85c = local_85c + 1;
				}
			}
		}
		if (local_85c == w * h)
		{
			local_870 = 1000;
			while (iVar7 = local_870 + -1, 0 < local_870)
			{
				x = MGL_random((unsigned short)w);
				x = x & 0xffff;
				y = MGL_random((unsigned short)h);
				y = y & 0xffff;
				local_870 = iVar7;
				if ((g_Map[y][x].flags & TileFlags::P) == 0)
				{
					i = 0;
					if ((x == 0) || ((g_Map[y][x].flags & TileFlags::WallV) != 0))
					{
						i = 1;
					}
					if ((x == w - 1U) || ((g_Map[y][x + 1].flags & TileFlags::WallV) != 0))
					{
						i = i + 1;
					}
					if ((y == 0) || ((g_Map[y - 1][x].flags & TileFlags::WallH) != 0))
					{
						i = i + 1;
					}
					if ((y == h - 1U) || ((g_Map[y][x].flags & TileFlags::WallH) != 0))
					{
						i = i + 1;
					}
					if (2 < i)
					{
						uVar6 = MGL_random(4);
						switch (uVar6 & 0xffff)
						{
						case 0:
							if (((x != w - 1U) && ((g_Map[y][x + 1].flags & TileFlags::P) == 0)) &&
							    ((g_Map[y][x + 1].flags & TileFlags::WallV) != 0))
							{
								g_Map[y][x + 1].flags = g_Map[y][x + 1].flags & ~TileFlags::WallV;
							}
							break;
						case 1:
							if (((y != h - 1U) && ((g_Map[y + 1][x].flags & TileFlags::P) == 0)) &&
							    ((g_Map[y][x].flags & TileFlags::WallH) != 0))
							{
								g_Map[y][x].flags = g_Map[y][x].flags & ~TileFlags::WallH;
							}
							break;
						case 2:
							if (((x != 0) && ((g_Map[y][x - 1].flags & TileFlags::P) == 0)) &&
							    ((g_Map[y][x].flags & TileFlags::WallV) != 0))
							{
								g_Map[y][x].flags = g_Map[y][x].flags & ~TileFlags::WallV;
							}
							break;
						case 3:
							if (((y != 0) && ((g_Map[y - 1][x].flags & TileFlags::P) == 0)) &&
							    ((g_Map[y - 1][x].flags & TileFlags::WallH) != 0))
							{
								g_Map[y - 1][x].flags = g_Map[y - 1][x].flags & ~TileFlags::WallH;
							}
						}
					}
				}
			}
			do
			{
				x = MGL_random((unsigned short)(w / 2));
				x = x & 0xffff;
				y = MGL_random((unsigned short)(h / 2));
				y = y & 0xffff;
			} while ((g_Map[y][x].flags & TileFlags::P) != 0);
			g_Map[y][x].flags = g_Map[y][x].flags | TileFlags::HammerUp;
			do
			{
				x = MGL_random((unsigned short)(w / 2));
				iVar7 = (x & 0xffff) + w / 2;
				x = MGL_random((unsigned short)(h / 2));
				x = x & 0xffff;
			} while ((g_Map[x][iVar7].flags & TileFlags::P) != 0);
			g_Map[x][iVar7].flags = g_Map[x][iVar7].flags | TileFlags::HammerUp;
			do
			{
				x = MGL_random((unsigned short)(w / 2));
				x = x & 0xffff;
				y = MGL_random((unsigned short)(h / 2));
				iVar7 = (y & 0xffff) + h / 2;
			} while ((g_Map[iVar7][x].flags & TileFlags::P) != 0);
			g_Map[iVar7][x].flags = g_Map[iVar7][x].flags | TileFlags::HammerUp;
			do
			{
				x = MGL_random((unsigned short)(w / 2));
				iVar7 = (x & 0xffff) + w / 2;
				x = MGL_random((unsigned short)(h / 2));
				iVar8 = (x & 0xffff) + h / 2;
			} while ((g_Map[iVar8][iVar7].flags & TileFlags::P) != 0);
			g_Map[iVar8][iVar7].flags = g_Map[iVar8][iVar7].flags | TileFlags::HammerUp;
			do
			{
				x = MGL_random((unsigned short)w);
				x = x & 0xffff;
				y = MGL_random((unsigned short)h);
				y = y & 0xffff;
			} while ((g_Map[y][x].flags & (TileFlags::HammerUp | TileFlags::P)) != 0);
			g_Map[y][x].flags = g_Map[y][x].flags | (TileFlags::PU | TileFlags::P1);
			if (g_NumPlayers == 2)
			{
				do
				{
					x = MGL_random((unsigned short)w);
					x = x & 0xffff;
					y = MGL_random((unsigned short)h);
					y = y & 0xffff;
				} while ((g_Map[y][x].flags & (TileFlags::HammerUp | TileFlags::P1 | TileFlags::P)) != 0);
				g_Map[y][x].flags = g_Map[y][x].flags | (TileFlags::PU | TileFlags::P2);
			}
			x = MGL_random((short)((w * h) / 10) + 1);
			for (x = x & 0xffff; x != 0; x = x - 1)
			{
				do
				{
					y = MGL_random((unsigned short)w);
					y = y & 0xffff;
					uVar6 = MGL_random((unsigned short)h);
					uVar6 = uVar6 & 0xffff;
				} while ((g_Map[uVar6][y].flags &
				          (TileFlags::HammerUp | TileFlags::P1 | TileFlags::P2 | TileFlags::P)) != 0);
				g_Map[uVar6][y].flags = g_Map[uVar6][y].flags | TileFlags::PU;
			}
			iVar7 = w + -0xc + h;
			if (0 < iVar7)
			{
				x = MGL_random((unsigned short)(iVar7 & 0xffff));
				if (1 < (x & 0xffff))
				{
					x = MGL_random((short)(iVar7 / 6) + 2);
					i = (x & 0xffff) + 2;
					do
					{
						do
						{
							x = MGL_random((unsigned short)w);
							x = x & 0xffff;
							y = MGL_random((unsigned short)h);
							y = y & 0xffff;
						} while ((g_Map[y][x].flags &
						          (TileFlags::HammerUp | TileFlags::PU | TileFlags::P1 | TileFlags::P2 | TileFlags::P)) != 0);
						g_Map[y][x].flags = g_Map[y][x].flags | TileFlags::TP;
						i = i + -1;
					} while (i != 0);
				}
			}
			for (i = 0; (int)i < w; i = i + 1)
			{
				for (j = 0; j < h; j = j + 1)
				{
					if ((g_Map[j][i].flags & (TileFlags::HammerUp | TileFlags::PU | TileFlags::P | TileFlags::TP)) == 0)
					{
						g_Map[j][i].flags = g_Map[j][i].flags | TileFlags::Candle;
					}
				}
			}
			if ((w < 0x13) || (h < 0xe))
			{
				memcpy(tempMap, g_Map, 0x850);
				centerX = (0x13 - w) / 2;
				centerY = (0xe - h) / 2;
				for (i = 0; (int)i < 0x13; i = i + 1)
				{
					for (j = 0; j < 0xe; j = j + 1)
					{
						x2 = i - centerX;
						y2 = j - centerY;
						if ((int)x2 < 0)
						{
							x2 = x2 + 0x13;
						}
						if ((int)y2 < 0)
						{
							y2 = y2 + 0xe;
						}
						g_Map[j][i] = tempMap[y2][x2];
					}
				}
				i = centerY;
				if (w < 0x13)
				{
					for (; (int)i < centerY + h; i = i + 1)
					{
						g_Map[i][centerX + w].flags = g_Map[i][centerX + w].flags | TileFlags::WallV;
					}
				}
				i = centerX;
				if (0 < centerY)
				{
					for (; (int)i < centerX + w; i = i + 1)
					{
						g_Map[centerY + -1][i].flags = g_Map[centerY + -1][i].flags | TileFlags::WallH;
					}
				}
				i = centerY;
				if (0 < centerX)
				{
					for (; (int)i < centerY + h; i = i + 1)
					{
						g_Map[i][centerX].flags = g_Map[i][centerX].flags | TileFlags::WallV;
					}
				}
				i = centerX;
				if (h < 0xe)
				{
					for (; (int)i < centerX + w; i = i + 1)
					{
						g_Map[centerY + -1 + h][i].flags = g_Map[centerY + -1 + h][i].flags | TileFlags::WallH;
					}
				}
			}
			else
			{
				centerX = 0;
				centerY = 0;
			}
			for (i = 0; (int)i < 0x13; i = i + 1)
			{
				for (j = 0; j < 0xe; j = j + 1)
				{
					if ((((int)i < centerX) || (centerX + -1 + w < (int)i)) ||
					    ((j < centerY || (centerY + -1 + h < j))))
					{
						g_Map[j][i].tile = 10;
					}
					else
					{
						x = MGL_random(5);
						g_Map[j][i].tile = (char)x + 4;
					}
					if ((g_Map[j][i].flags & TileFlags::P) != 0)
					{
						g_Map[j][i].tile = 9;
					}
					if ((g_Map[j][i].flags & TileFlags::TP) != 0)
					{
						g_Map[j][i].tile = 0x12;
					}
				}
			}
			return;
		}
	} while (true);
}

void MapFilterForRivalry()
{
	byte bVar1;
	int y;
	int x;

	for (x = 0; x < 0x13; x = x + 1)
	{
		for (y = 0; y < 0xe; y = y + 1)
		{
			bVar1 = g_Map[y][x].tile;
			if ((bVar1 < 2) || ((3 < bVar1 && (bVar1 < 9))))
			{
				bVar1 = 0xb;
			}
			if ((bVar1 == 2) || (bVar1 == 9))
			{
				bVar1 = 0xc;
			}
			if ((bVar1 == 3) || (bVar1 == 10))
			{
				bVar1 = 0xd;
			}
			if ((bVar1 == 0xe) || (bVar1 == 0x12))
			{
				bVar1 = 0x10;
			}
			if ((bVar1 == 0xf) || (bVar1 == 0x13))
			{
				bVar1 = 0x11;
			}
			g_Map[y][x].tile = bVar1;
			if (((g_Map[y][x].flags & TileFlags::P) == 0) &&
			    ((g_Map[y][x].flags & (TileFlags::Candle | TileFlags::HammerUp | TileFlags::P1 | TileFlags::P2)) != 0))
			{
				g_Map[y][x].flags = g_Map[y][x].flags | TileFlags::PU;
			}
		}
	}
}

void GetPlayerStart(byte player, int *x, int *y)
{
	if (player == 1)
	{
		*x = g_Player1StartX;
		*y = g_Player1StartY;
	}
	else
	{
		*x = g_Player2StartX;
		*y = g_Player2StartY;
	}
}

void MapRedrawTile(byte tx, byte ty)
{
	int py;
	int px;

	for (px = 0; px < 0x20; px = px + 1)
	{
		for (py = 0; py < 0x20; py = py + 1)
		{
			g_Background[px + (unsigned int)tx * 0x20 + 0x10 + (py + (unsigned int)ty * 0x20 + 0x20) * mgl->GetWidth()] =
				g_Tiles[g_Map[ty][tx].tile][py][px];
		}
	}
	return;
}

void MapSpawnItems()
{
	byte bVar1;
	unsigned int uVar2;
	int i;

	auto map = &g_Map[0][0];
	for (i = 0; i < 0x10a; i = i + 1)
	{
		if ((map[i].flags & TileFlags::Candle) == 0)
		{
			if ((map[i].flags & TileFlags::HammerUp) == 0)
			{
				map[i].item = ItemType::None;
			}
			else
			{
				map[i].item = ItemType::HammerUp;
			}
		}
		else
		{
			map[i].item = ItemType::Candle;
			uVar2 = MGL_random(0xc);
			map[i].itemAnim = (byte)uVar2;
		}
	}
	g_PowerupOnMap = 0;
	if (g_GameMode == GameMode::HappyFields)
	{
		g_TimeUntilPowerupExpires = 0x1c2;
	}
	else if (g_GameMode == GameMode::DankDungeons)
	{
		uVar2 = MGL_random(0x14);
		g_TimeUntilPowerupExpires = ((short)uVar2 + 5) * 0x1e;
	}
	else if (g_GameMode == GameMode::SiblingRivalry)
	{
		bVar1 = ConfigGetRivalryPowerups();
		g_TimeUntilPowerupExpires = (10 - (unsigned short)bVar1) * 0x3c + 0x3c;
		bVar1 = ConfigGetRivalryPowerups();
		if (bVar1 == 0)
		{
			g_PowerupOnMap = 2;
		}
	}
}

void PlacePowerup()
{
	static const byte normalOdds[16] = {
		30,  // Points100
		20,  // Points250
		20,  // Points500
		10,  // Points1000
		4,   // SpeedyBoots
		3,   // Ghost
		3,   // IceCream
		3,   // HammerUpRandom
		3,   // Zzz
		3,   // Shield
		0,
		1,   // OneUp
		0,
		0,
		0,
		0,
	};
	static const byte rivalryOdds[16] = {
		0,
		0,
		0,
		0,
		12,  // SpeedyBoots
		12,  // Ghost
		5,   // IceCream
		12,  // HammerUpRandom
		5,   // Zzz
		10,  // Shield
		0,
		0,
		10,  // FirePower
		10,  // IcePower
		12,  // EvilEye
		12,  // BigCandle
	};

	unsigned int x;
	unsigned int y;
	unsigned int roll;
	int totalOdds;
	unsigned int powerup;
	byte odds;

	do
	{
		do
		{
			x = MGL_random(0x13);
			x = x & 0xffff;
			y = MGL_random(0xe);
			y = y & 0xffff;
		} while ((g_Map[y][x].flags & TileFlags::PU) == 0);
	} while (g_Map[y][x].item != ItemType::None);

	roll = MGL_random(100);
	totalOdds = 0;

	for (powerup = 0; powerup < 16; ++powerup)
	{
		if (g_GameMode == GameMode::SiblingRivalry)
		{
			odds = rivalryOdds[powerup];
		}
		else
		{
			odds = normalOdds[powerup];
		}
		totalOdds = totalOdds + odds;

		if (odds && roll < totalOdds)
		{
			g_PowerupX = x;
			g_PowerupY = y;
			g_Map[y][x].item = ItemType(powerup + byte(ItemType::Points100));
			g_Map[y][x].itemAnim = 0;
			break;
		}
	}

	ParticleAddTeleport((x * 0x20 + 0x10) * 0x100, (y * 0x20 + 0x10) * 0x100);
	PlaySound(0x1e, 2000);
	g_PowerupOnMap = 1;
	if (g_GameMode == GameMode::HappyFields)
	{
		g_TimeUntilPowerupExpires = (short)((int)(unsigned int)g_MapNum >> 1) * -0x19 + 0x1c2;
	}
	else if (g_GameMode == GameMode::DankDungeons)
	{
		powerup = (unsigned int)g_MapNum;
		if (0x14 < powerup)
		{
			powerup = 0x14;
		}
		g_TimeUntilPowerupExpires = (short)(powerup / 2) * -0x14 + 0x1c2;
		g_PowerupOnMap = 1;
	}
	else if (g_GameMode != GameMode::SiblingRivalry)
	{
		g_PowerupOnMap = 1;
	}
}

void ErasePowerup()
{
	byte bVar1;
	unsigned int uVar2;

	g_Map[g_PowerupY][g_PowerupX].item = ItemType::None;
	g_PowerupOnMap = 0;
	if (g_GameMode == GameMode::HappyFields)
	{
		g_TimeUntilPowerupExpires = (unsigned short)g_MapNum * -10 + 0x1c2;
	}
	else if (g_GameMode == GameMode::DankDungeons)
	{
		uVar2 = MGL_random(0x14);
		g_TimeUntilPowerupExpires = ((short)uVar2 + 5) * 0x1e;
	}
	else if (g_GameMode == GameMode::SiblingRivalry)
	{
		bVar1 = ConfigGetRivalryPowerups();
		g_TimeUntilPowerupExpires = (10 - (unsigned short)bVar1) * 0x3c + 1;
		bVar1 = ConfigGetRivalryPowerups();
		if (bVar1 == 0)
		{
			g_PowerupOnMap = 1;
		}
	}
	return;
}

void ItemAnimate(byte p)
{
	auto map = &g_Map[0][0];
	switch (map[p].item)
	{
	case ItemType::Candle:
	case ItemType::BigCandle:
		map[p].itemAnim = map[p].itemAnim + 1;
		if (0xb < map[p].itemAnim)
		{
			map[p].itemAnim = map[p].itemAnim - 0xc;
		}
		break;
	case ItemType::HammerUp:
		break;
	default:
		map[p].itemAnim = map[p].itemAnim + 1;
		break;
	case ItemType::EvilEye:
		map[p].itemAnim = map[p].itemAnim + 1;
		if (0xf < map[p].itemAnim)
		{
			map[p].itemAnim = map[p].itemAnim - 0x10;
		}
		break;
	case ItemType::FireTrail:
		map[p].itemAnim = map[p].itemAnim + 1;
		if (map[p].itemAnim == 0x96)
		{
			map[p].item = ItemType::None;
		}
		break;
	case ItemType::IceTrail:
		map[p].itemAnim = map[p].itemAnim + 1;
		if (map[p].itemAnim == 0x96)
		{
			map[p].item = ItemType::None;
		}
	}
	return;
}

void ShootEvilEye(int x, unsigned int y, Direction direction)
{
	unsigned int i;

	switch (direction)
	{
	case Direction::East:
		for (i = x; (int)i < 0x13; i = i + 1)
		{
			if ((g_Map[y][i].tile == 0xb) && (i != x))
			{
				g_Map[y][i].tile = 0x14;
				MapRedrawTile((byte)i, (byte)y);
			}
			if (((int)i < 0x12) && ((g_Map[y][i + 1].flags & TileFlags::WallV) != 0))
			{
				return;
			}
		}
		break;
	case Direction::South:
		for (i = y; (int)i < 0xe; i = i + 1)
		{
			if ((g_Map[i][x].tile == 0xb) && (i != y))
			{
				g_Map[i][x].tile = 0x14;
				MapRedrawTile((byte)x, (byte)i);
			}
			if ((g_Map[i][x].flags & TileFlags::WallH) != 0)
			{
				return;
			}
		}
		break;
	case Direction::West:
		for (i = x; -1 < (int)i; i = i + -1)
		{
			if ((g_Map[y][i].tile == 0xb) && (i != x))
			{
				g_Map[y][i].tile = 0x14;
				MapRedrawTile((byte)i, (byte)y);
			}
			if ((g_Map[y][i].flags & TileFlags::WallV) != 0)
			{
				return;
			}
		}
		break;
	case Direction::North:
		for (i = y; 0 < (int)i; i = i - 1)
		{
			if ((g_Map[i][x].tile == 0xb) && (i != y))
			{
				g_Map[i][x].tile = 0x14;
				MapRedrawTile((byte)x, (byte)i);
			}
			if ((0 < (int)i) && ((g_Map[i - 1][x].flags & TileFlags::WallH) != 0))
			{
				return;
			}
		}
	}
	return;
}

bool MapUpdate(byte)
{
	unsigned int uVar1;
	byte ty;
	int i;
	int tx;
	int candlesLeft;

	g_MapTimer = g_MapTimer + 1;
	g_MapChirpTimer = g_MapChirpTimer - 1;
	if (g_MapChirpTimer == 0)
	{
		if (g_MapTheme == GameMode::HappyFields)
		{
			uVar1 = MGL_random(3);
			if ((uVar1 & 0xffff) == 1)
			{
				PlaySound(2, 2);
			}
			else if ((uVar1 & 0xffff) == 2)
			{
				PlaySound(3, 2);
			}
		}
		uVar1 = MGL_random(200);
		g_MapChirpTimer = (char)uVar1 + 3;
	}
	candlesLeft = 0;
	tx = 0;
	ty = 0;
	auto map = &g_Map[0][0];
	for (i = 0; i < 0x10a; i = i + 1)
	{
		if (map[i].item != ItemType::None)
		{
			ItemAnimate((byte)i);
		}
		if ((map[i].flags & TileFlags::Candle) != 0)
		{
			candlesLeft = candlesLeft + 1;
		}
		if (map[i].tileAnim != 0xff)
		{
			map[i].tileAnim = map[i].tileAnim + 1;
		}
		if (((0xd < map[i].tile) && (map[i].tile < 0x14)) && ((g_MapTimer & 7) == 0))
		{
			if ((map[i].tile & 1) == 0)
			{
				map[i].tile = map[i].tile | 1;
			}
			else
			{
				map[i].tile = map[i].tile & 0xfe;
			}
			MapRedrawTile((byte)tx, ty);
		}
		if (map[i].tile == 0x14)
		{
			map[i].tile = 0xb;
			MapRedrawTile((byte)tx, ty);
		}
		tx = tx + 1;
		if (tx == 0x13)
		{
			tx = 0;
			ty = ty + 1;
		}
	}
	if ((g_PowerupOnMap < 2) &&
	    (g_TimeUntilPowerupExpires = g_TimeUntilPowerupExpires + -1, g_TimeUntilPowerupExpires == 0))
	{
		if (g_PowerupOnMap == 1)
		{
			ErasePowerup();
		}
		else
		{
			PlacePowerup();
		}
	}
	return candlesLeft == 0;
}

void CheatYippee(void)
{
	if (g_PowerupOnMap == 1)
	{
		ErasePowerup();
	}
	g_TimeUntilPowerupExpires = 1;
}

void ItemRender(ItemType item, byte itemAnim, int x, int y)
{
	sprite_t *spr;
	int sprIdx;
	int sort;

	if ((((item < ItemType::Points100) || (ItemType::BigCandle < item)) ||
	     (30 < g_TimeUntilPowerupExpires)) ||
	    ((g_TimeUntilPowerupExpires & 1) != 0))
	{
		switch (item)
		{
		case ItemType::Candle:
			DisplayListAdd(g_ItemsJsp->GetSprite(((int)(unsigned int)itemAnim >> 2) + 3), x, y, 0, y, 0, DisplayEffect::Normal);
			break;
		case ItemType::HammerUp:
			DisplayListAdd(g_ItemsJsp->GetSprite(6), x, y, 0, y, 0, DisplayEffect::Normal);
			break;
		case ItemType::Points100:
		case ItemType::Points250:
		case ItemType::Points500:
		case ItemType::Points1000:
			if ((itemAnim & 7) == 0)
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(byte(item) + 19), x, y, 0, y, 4, DisplayEffect::Bright);
			}
			else
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(byte(item) + 19), x, y, 0, y, 0, DisplayEffect::Normal);
			}
			break;
		case ItemType::SpeedyBoots:
			if ((itemAnim & 7) == 0)
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(21), x, y, 0, y, 4, DisplayEffect::Bright);
			}
			else
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(21), x, y, 0, y, 0, DisplayEffect::Normal);
			}
			break;
		case ItemType::Ghost:
			DisplayListAdd(g_ItemsJsp->GetSprite(((int)(unsigned int)itemAnim >> 2 & 1U) + 7), x, y, 0, y, 0, DisplayEffect::Normal);
			break;
		case ItemType::IceCream:
			if ((itemAnim & 7) == 0)
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(9), x, y, 0, y, 4, DisplayEffect::Bright);
			}
			else
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(9), x, y, 0, y, 0, DisplayEffect::Normal);
			}
			break;
		case ItemType::HammerUpRandom:
			if ((itemAnim & 7) == 0)
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(6), x, y, 0, y, 4, DisplayEffect::Bright);
			}
			else
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(6), x, y, 0, y, 0, DisplayEffect::Normal);
			}
			break;
		case ItemType::Zzz:
			if ((itemAnim & 7) == 0)
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(10), x, y, 0, y, 4, DisplayEffect::Bright);
			}
			else
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(10), x, y, 0, y, 0, DisplayEffect::Normal);
			}
			break;
		case ItemType::Shield:
			if ((itemAnim & 7) == 0)
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(11), x, y, 0, y, 4, DisplayEffect::Bright);
			}
			else
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(11), x, y, 0, y, 0, DisplayEffect::Normal);
			}
			break;
		case ItemType::OneUp:
			if ((itemAnim & 7) == 0)
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(20), x, y, 0, y, 4, DisplayEffect::Bright);
			}
			else
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(20), x, y, 0, y, 0, DisplayEffect::Normal);
			}
			break;
		case ItemType::FirePower:
			if ((itemAnim & 7) == 0)
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(12), x, y, 0, y, 4, DisplayEffect::Bright);
			}
			else
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(12), x, y, 0, y, 0, DisplayEffect::Normal);
			}
			break;
		case ItemType::IcePower:
			if ((itemAnim & 7) == 0)
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(13), x, y, 0, y, 4, DisplayEffect::Bright);
			}
			else
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(13), x, y, 0, y, 0, DisplayEffect::Normal);
			}
			break;
		case ItemType::EvilEye:
			if ((int)(unsigned int)itemAnim >> 2 == 0)
			{
				sprIdx = 14;
			}
			else if (((int)(unsigned int)itemAnim >> 2 == 1) || ((int)(unsigned int)itemAnim >> 2 == 3))
			{
				sprIdx = 15;
			}
			else
			{
				sprIdx = 16;
			}
			DisplayListAdd(g_ItemsJsp->GetSprite(sprIdx), x, y, 0, y, 0, DisplayEffect::Normal);
			break;
		case ItemType::BigCandle:
			DisplayListAdd(g_ItemsJsp->GetSprite(((int)(unsigned int)itemAnim >> 2) + 17), x, y, 0, y, 0, DisplayEffect::Normal);
			break;
		case ItemType::FireTrail:
			DisplayListAdd(spr, x, y, 0, y + -0x2000, 0, DisplayEffect::Normal);
			break;
		case ItemType::IceTrail:
			if (itemAnim < 0x8a)
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(30), x, y, 0, y, 0, DisplayEffect::Normal);
			}
			else
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(((int)((itemAnim - 138) + ((int)(itemAnim - 138) >> 0x1f & 3U)) >> 2) + 31), x, y, 0, y, 0, DisplayEffect::Normal);
			}
		}
	}
}

void MapRender(bool editor)
{
	byte wallBase;
	sprite_t *spr;
	int ty;
	int tx;
	int y;
	int x;
	MapTile *tile;

	wallBase = byte(g_MapTheme) * 2 - 2;
	tile = &g_Map[0][0];
	y = 0x3f;
	for (ty = 0; ty < 0xe; ty = ty + 1)
	{
		x = 0x10;
		for (tx = 0; tx < 0x13; tx = tx + 1)
		{
			if ((tile->flags & TileFlags::WallV) != 0)
			{
				DisplayListAdd(g_WallsJsp->GetSprite(wallBase + 1), x, y, 0, y - 0x20, 0, DisplayEffect::Normal);
			}
			if ((tile->flags & TileFlags::WallH) != 0)
			{
				DisplayListAdd(g_WallsJsp->GetSprite(wallBase), x, y, 0, y, 0, DisplayEffect::Normal);
			}
			if (tile->item != ItemType::None)
			{
				ItemRender(tile->item, tile->itemAnim, x, y + -0x1f);
			}
			if (editor)
			{
				if ((tile->flags & TileFlags::PK) != 0)
				{
					PrintMultiline(x + 8, y + -0x18, "PK");
				}
				if ((tile->flags & TileFlags::P) != 0)
				{
					PrintMultiline(x + 2, y + -0x20, "P");
				}
				if ((tile->flags & TileFlags::PU) != 0)
				{
					PrintMultiline(x + 8, y + -0x18, "PU");
				}
				if ((tile->flags & TileFlags::P1) != 0)
				{
					PrintMultiline(x + 8, y + -0x18, "P1");
				}
				if ((tile->flags & TileFlags::P2) != 0)
				{
					PrintMultiline(x + 8, y + -0x18, "P2");
				}
				if ((tile->flags & TileFlags::TP) != 0)
				{
					PrintMultiline(x + 8, y + -0x18, "TP");
				}
			}
			++tile;
			x = x + 0x20;
		}
		y = y + 0x20;
	}
}
