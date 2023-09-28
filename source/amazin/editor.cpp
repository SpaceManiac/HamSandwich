#include "editor.h"
#include "display.h"
#include "game.h"
#include "loop.h"
#include "title.h"
#include "map.h"

byte g_EditorFlip;
int g_EditorX, g_EditorY;
GameMode g_EditorTheme;
byte g_EditorCurrentTile;
int g_EditorMouseX, g_EditorMouseY;

bool EditorUpdate();
void EditorRender();

void EditorInit()
{
	SetUpdate(EditorUpdate);
	SetRender(EditorRender);
	g_EditorMouseX = 0;
	g_EditorMouseY = 0;
	g_EditorX = 0;
	g_EditorY = 0;
	g_EditorCurrentTile = 0;
	g_EditorTheme = GameMode::HappyFields;
	DisplayListClear();
	MapClear(g_EditorTheme);
	MapRenderToBackground(g_EditorTheme);
}

void EditorExit()
{
}

bool EditorUpdate()
{
	char k = mgl->LastKeyPressed();
	if (k == '\x1b')
	{
		EditorExit();
		TitleInit();
	}
	else
	{
		if (k == 'S')
		{
			MapSave("temp.map");
		}
		if (k == 'L')
		{
			MapLoad("temp.map");
			MapRenderToBackground(g_EditorTheme);
		}
		if ((k == 'q') && (0 < g_EditorX))
		{
			if ((g_Map[g_EditorY][g_EditorX].flags & TileFlags::WallV) == 0)
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags | TileFlags::WallV;
			}
			else
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags & ~TileFlags::WallV;
			}
		}
		if ((k == 'w') && (g_EditorY < 0xd))
		{
			if ((g_Map[g_EditorY][g_EditorX].flags & TileFlags::WallH) == 0)
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags | TileFlags::WallH;
			}
			else
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags & ~TileFlags::WallH;
			}
		}
		if (k == 'e')
		{
			if ((g_Map[g_EditorY][g_EditorX].flags & TileFlags::Candle) == 0)
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags | 4;
				g_Map[g_EditorY][g_EditorX].item = ItemType::Candle;
				g_Map[g_EditorY][g_EditorX].itemAnim = 0;
			}
			else
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags & 0xfffb;
				g_Map[g_EditorY][g_EditorX].item = ItemType::None;
				g_Map[g_EditorY][g_EditorX].itemAnim = 0;
			}
		}
		if (k == 'r')
		{
			if ((g_Map[g_EditorY][g_EditorX].flags & TileFlags::HammerUp) == 0)
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags | 8;
				g_Map[g_EditorY][g_EditorX].item = ItemType::HammerUp;
				g_Map[g_EditorY][g_EditorX].itemAnim = 0;
			}
			else
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags & 0xfff7;
				g_Map[g_EditorY][g_EditorX].item = ItemType::None;
				g_Map[g_EditorY][g_EditorX].itemAnim = 0;
			}
		}
		if (k == 't')
		{
			if ((g_Map[g_EditorY][g_EditorX].flags & TileFlags::PK) == 0)
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags | TileFlags::PK;
			}
			else
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags & ~TileFlags::PK;
			}
		}
		if (k == 'y')
		{
			if ((g_Map[g_EditorY][g_EditorX].flags & TileFlags::P) == 0)
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags | TileFlags::P;
			}
			else
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags & ~TileFlags::P;
			}
		}
		if (k == 'a')
		{
			if ((g_Map[g_EditorY][g_EditorX].flags & TileFlags::PU) == 0)
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags | TileFlags::PU;
			}
			else
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags & ~TileFlags::PU;
			}
		}
		if (k == 's')
		{
			if ((g_Map[g_EditorY][g_EditorX].flags & TileFlags::P1) == 0)
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags | TileFlags::P1;
			}
			else
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags & ~TileFlags::P1;
			}
		}
		if (k == 'd')
		{
			if ((g_Map[g_EditorY][g_EditorX].flags & TileFlags::P2) == 0)
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags | TileFlags::P2;
			}
			else
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags & ~TileFlags::P2;
			}
		}
		if (k == 'f')
		{
			if ((g_Map[g_EditorY][g_EditorX].flags & TileFlags::TP) == 0)
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags | TileFlags::TP;
			}
			else
			{
				g_Map[g_EditorY][g_EditorX].flags = g_Map[g_EditorY][g_EditorX].flags & ~TileFlags::TP;
			}
		}
		if (k == '-')
		{
			g_EditorCurrentTile = g_EditorCurrentTile - 1;
			if (0x14 < g_EditorCurrentTile)
			{
				g_EditorCurrentTile = 0x14;
			}
		}
		if (k == '=')
		{
			g_EditorCurrentTile = g_EditorCurrentTile + 1;
			if (0x14 < g_EditorCurrentTile)
			{
				g_EditorCurrentTile = 0;
			}
		}
		if (k == 'I')
		{
			g_EditorTheme = GameMode::HappyFields;
			MapClear(GameMode::HappyFields);
			MapRenderToBackground(g_EditorTheme);
		}
		if (k == 'O')
		{
			g_EditorTheme = GameMode::DankDungeons;
			MapClear(GameMode::DankDungeons);
			MapRenderToBackground(g_EditorTheme);
		}
		if (k == 'P')
		{
			g_EditorTheme = GameMode::SiblingRivalry;
			MapClear(GameMode::SiblingRivalry);
			MapRenderToBackground(g_EditorTheme);
		}
		mgl->GetMouse(&g_EditorMouseX, &g_EditorMouseY);
		g_EditorX = (int)(g_EditorMouseX + -0x10 + (g_EditorMouseX + -0x10 >> 0x1f & 0x1fU)) >> 5;
		g_EditorY = (int)(g_EditorMouseY + -0x20 + (g_EditorMouseY + -0x20 >> 0x1f & 0x1fU)) >> 5;
		if (g_EditorX < 0)
		{
			g_EditorX = 0;
		}
		if (0x12 < g_EditorX)
		{
			g_EditorX = 0x12;
		}
		if (g_EditorY < 0)
		{
			g_EditorY = 0;
		}
		if (0xd < g_EditorY)
		{
			g_EditorY = 0xd;
		}
		if (mgl->MouseDown())
		{
			g_Map[g_EditorY][g_EditorX].tile = g_EditorCurrentTile;
			MapRedrawTile((byte)g_EditorX, (byte)g_EditorY);
		}
		g_EditorFlip = 1 - g_EditorFlip;
	}
	return true;
}

void EditorRender(void)
{
	memcpy(mgl->GetScreen(), g_Background, 0x4b000);
	MapRender(true);
	DisplayListRender();
	if (g_EditorFlip == 1)
	{
		mgl->FillBox(g_EditorX * 0x20 + 0x10, g_EditorY * 0x20 + 0x20, g_EditorX * 0x20 + 0x2f, g_EditorY * 0x20 + 0x3f, 0);
	}
	return;
}
