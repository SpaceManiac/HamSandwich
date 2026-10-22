#include "mapscreen.h"
#include "editor.h"

static byte mapNum;
static world_t* world;

void InitMapScreen(world_t* wrld, byte currentMap)
{
	world = wrld;
	mapNum = currentMap;
}

void ExitMapScreen(void)
{
}

int CoordinateUL(int center, int scrw, int lvlw, int low, int high)
{
	int maxlvlw = scrw / 2 - 10;

	if (lvlw <= maxlvlw)
		return center - lvlw;	// small level, just center

	int windw = high - low + 1;

	if (low < (maxlvlw - windw) / 2)
		return center - (scrw / 2 - 10);	// low end, snap to low
	if (high > lvlw - (maxlvlw - windw) / 2 - 1)
		return center + (scrw / 2 - 10) - lvlw * 2;	// high end, snap to high

	int windc = (low + high + 1) / 2;
	return center - windc * 2;	// in the center, so center on the window
}

void CalcMapArea(int& xl, int& xr, int& yt, int& yb, int& ulx, int& uly, int& lvlwx, int& lvlwy)
{
	int sx, sy, ycent;

	GetCamera(&sx, &sy);
	lvlwx = world->map[mapNum]->width;
	lvlwy = world->map[mapNum]->height;
	xl = (sx - 320) / TILE_WIDTH;
	xr = (sx + 319) / TILE_WIDTH;
	yt = (sy - 240) / TILE_HEIGHT;
	yb = (sy + 179) / TILE_HEIGHT;
	if (xl < 0)
		xl = 0;
	if (xl > lvlwx)
		xl = lvlwx;
	if (xr < -1)
		xr = -1;
	if (xr > lvlwx - 1)
		xr = lvlwx - 1;
	if (yt < 0)
		yt = 0;
	if (yt > lvlwy)
		yt = lvlwy;
	if (yb < -1)
		yb = -1;
	if (yb > lvlwy - 1)
		yb = lvlwy - 1;

	ulx = CoordinateUL(320, 640, lvlwx, xl, xr);
	if (lvlwy <= 200)
		ycent = 210;
	else if (lvlwy >= 230)
		ycent = 240;
	else
		ycent = lvlwy + 10;
	uly = CoordinateUL(ycent, 2 * ycent, lvlwy, yt, yb);
}

void RenderMapScreen(int msx, int msy, MGLDraw* mgl)
{
	int x, y, i, ulx, uly, lvlwx, lvlwy;
	int xl, xr, yt, yb;
	byte c;

	CalcMapArea(xl, xr, yt, yb, ulx, uly, lvlwx, lvlwy);

	// box for the whole dialog
	DrawBox(ulx - 4, uly - 4, ulx + 2 * lvlwx + 3, uly + 2 * lvlwy + 3, 16);
	DrawBox(ulx - 3, uly - 3, ulx + 2 * lvlwx + 2, uly + 2 * lvlwy + 2, 16);
	DrawBox(ulx - 2, uly - 2, ulx + 2 * lvlwx + 1, uly + 2 * lvlwy + 1, 0);
	DrawBox(ulx - 1, uly - 1, ulx + 2 * lvlwx + 0, uly + 2 * lvlwy + 0, 0);

	for (x = 0; x < lvlwx; ++x)
		for (y = 0; y < lvlwy; ++y)
		{
			i = x + y * lvlwx;
			if (world->map[mapNum]->map[i].wall)
				c = 0;
			else if (byte it = world->map[mapNum]->map[i].item; it)
			{
				if (itemInfo[it].flags & IF_GET || (it >= IT_CHEST1 && it <= IT_CHEST5))
					c = 5 * 32 + 16;
				else if (itemInfo[it].flags & IF_OBSTACLE)
					c = 2 * 32 + 12;
				else
					c = 1 * 32 + 14;
			}
			else if (world->terrain[world->map[mapNum]->map[i].floor].flags & TF_SOLID)
				c = 8;
			else if (world->terrain[world->map[mapNum]->map[i].floor].flags & TF_WATER)
				c = 3 * 32 + 15;
			else if (world->terrain[world->map[mapNum]->map[i].floor].flags & TF_LAVA)
				c = 4 * 32 + 15;
			else if (world->terrain[world->map[mapNum]->map[i].floor].flags & TF_CLIFF)
				c = 2 * 32 + 18;
			else if (world->terrain[world->map[mapNum]->map[i].floor].flags & TF_SKY)
				c = 3 * 32 + 19;
			else if (world->terrain[world->map[mapNum]->map[i].floor].flags & TF_ICE)
				c = 7 * 32 + 15;
			else
				c = 24;

			DrawBox(ulx + 2 * x, uly + 2 * y, ulx + 2 * x + 1, uly + 2 * y + 1, c);
		}

	DrawBox(ulx - 1 + xl * 2, uly - 1 + yt * 2, ulx + 2 + xr * 2, uly + 2 + yb * 2, 31);
}

byte MapScreenKey(char key)
{
	if (key == 27)//esc
		return 0;

	return 1;
}

byte MapScreenClick(int msx, int msy)
{
	int x, y;
	int xl, xr, yt, yb, ulx, uly, lvlwx, lvlwy;

	CalcMapArea(xl, xr, yt, yb, ulx, uly, lvlwx, lvlwy);

	if (msx >= ulx && msx < (ulx+2*lvlwx) && msy >= uly && msy < (uly + 2 * lvlwy))
	{
		x = (msx - ulx) / 2 * TILE_WIDTH;
		y = (msy - uly) / 2 * TILE_HEIGHT;
		PutCamera(x << FIXSHIFT, y << FIXSHIFT);
	}

	return 1;
}
