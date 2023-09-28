#include "map.h"
#include "appdata.h"
#include "display.h"
#include "jamulspr.h"
#include "jamultypes.h"

MapTile g_Map[14][19];
byte g_Tiles[21][32][32];
sprite_set_t *g_ItemsJsp;
GameMode g_MapTheme;
sprite_set_t *g_WallsJsp;
word g_TimeUntilPowerupExpires;

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
	uint r;
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

void MapRedrawTile(byte tx, byte ty)
{
	int py;
	int px;

	for (px = 0; px < 0x20; px = px + 1)
	{
		for (py = 0; py < 0x20; py = py + 1)
		{
			g_Background[px + (uint)tx * 0x20 + 0x10 + (py + (uint)ty * 0x20 + 0x20) * mgl->GetWidth()] =
				g_Tiles[g_Map[ty][tx].tile][py][px];
		}
	}
	return;
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
			DisplayListAdd(g_ItemsJsp->GetSprite(((int)(uint)itemAnim >> 2) + 3), x, y, 0, y, 0, DisplayEffect::Normal);
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
				DisplayListAdd(g_ItemsJsp->GetSprite(19), x, y, 0, y, 4, DisplayEffect::Bright);
			}
			else
			{
				DisplayListAdd(g_ItemsJsp->GetSprite(19), x, y, 0, y, 0, DisplayEffect::Normal);
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
			DisplayListAdd(g_ItemsJsp->GetSprite(((int)(uint)itemAnim >> 2 & 1U) + 7), x, y, 0, y, 0, DisplayEffect::Normal);
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
			if ((int)(uint)itemAnim >> 2 == 0)
			{
				sprIdx = 14;
			}
			else if (((int)(uint)itemAnim >> 2 == 1) || ((int)(uint)itemAnim >> 2 == 3))
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
			DisplayListAdd(g_ItemsJsp->GetSprite(((int)(uint)itemAnim >> 2) + 17), x, y, 0, y, 0, DisplayEffect::Normal);
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
