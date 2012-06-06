#include "map.h"
#include "display.h"
#include "world.h"
#include "game.h"
#include "guy.h"
#include "options.h"

const int NUM_STARS = 400;

int totalBrains;
static world_t *world;
byte specialClock[MAX_SPECIAL];
byte allSpecialClock;

word starX[NUM_STARS], starY[NUM_STARS];
byte starCol[NUM_STARS];

Map::Map(FILE *f)
{
	fread(&width, 1, sizeof (int), f);
	fread(&height, 1, sizeof (int), f);

	fread(name, 32, sizeof (char), f);
	fread(badguy, MAX_MAPMONS, sizeof (mapBadguy_t), f);
	fread(special, MAX_SPECIAL, sizeof (special_t), f);
	fread(&song, 1, 1, f);
	fread(&flags, 1, 1, f);

	map = (mapTile_t *) calloc(sizeof (mapTile_t) * width*height, 1);

	fread(map, width*height, sizeof (mapTile_t), f);

#ifdef LOG
	int i;

	fprintf(logFile, "LOADMAP %s\n", name);
	for (i = 0; i < MAX_MAPMONS; i++)
	{
		if (badguy[i].type)
			fprintf(logFile, "M#%03d. %d. %s - (%d,%d)\n", i, badguy[i].type, MonsterName(badguy[i].type), badguy[i].x, badguy[i].y);
	}
	fflush(logFile);
#endif
}

Map::Map(byte size, const char *name)
{
	switch (size) {
		case 0:
			width = 20;
			height = 20;
			break;
		case 1:
			width = 64;
			height = 64;
			break;
		case 2:
			width = 128;
			height = 128;
			break;
	}

	strcpy(this->name, name);

	map = (mapTile_t *) calloc(sizeof (mapTile_t) * width*height, 1);
	song = 2;
	flags = 0;
	memset(badguy, 0, sizeof (mapBadguy_t) * MAX_MAPMONS);
	memset(special, 0, sizeof (special_t) * MAX_SPECIAL);
}

Map::Map(Map *m)
{
	width = m->width;
	height = m->height;
	song = m->song;
	flags = m->flags;
	strcpy(this->name, m->name);

	map = (mapTile_t *) calloc(sizeof (mapTile_t) * width*height, 1);
	memcpy(map, m->map, sizeof (mapTile_t) * width * height);
	memcpy(badguy, m->badguy, sizeof (mapBadguy_t) * MAX_MAPMONS);
	memcpy(special, m->special, sizeof (special_t) * MAX_SPECIAL);
}

Map::~Map(void)
{
	free(map);
}

byte Map::Save(FILE *f)
{
	fwrite(&width, 1, sizeof (int), f);
	fwrite(&height, 1, sizeof (int), f);
	fwrite(name, 32, sizeof (char), f);
	fwrite(badguy, MAX_MAPMONS, sizeof (mapBadguy_t), f);
	fwrite(special, MAX_SPECIAL, sizeof (special_t), f);
	fwrite(&song, 1, 1, f);
	fwrite(&flags, 1, 1, f);

	fwrite(map, width*height, sizeof (mapTile_t), f);
	return 1;
}

void Map::Init(world_t *wrld)
{
	int i;

	totalBrains = 0;

	for (i = 0; i < MAX_MAPMONS; i++)
	{
		if (badguy[i].type == MONS_ZOMBIE || badguy[i].type == MONS_ZOMBONI)
			totalBrains++;
		if (badguy[i].type == MONS_SUPERZOMBIE)
			totalBrains += 2;
	}

	for (i = 0; i < NUM_STARS; i++)
	{
		starX[i] = MGL_random(640);
		starY[i] = MGL_random(480);
		starCol[i] = (byte) MGL_random(32);
	}

	for (i = 0; i < width * height; i++)
	{
		if (map[i].item == ITM_BRAIN)
			totalBrains++;
		if (map[i].item == ITM_KEYCH1 && PlayerKeyChain(0)) // keychains only appear once
			map[i].item = 0;
		if (map[i].item == ITM_KEYCH2 && PlayerKeyChain(1)) // keychains only appear once
			map[i].item = 0;
		if (map[i].item == ITM_KEYCH3 && PlayerKeyChain(2)) // keychains only appear once
			map[i].item = 0;
		if (map[i].item == ITM_KEYCH4 && PlayerKeyChain(3)) // keychains only appear once
			map[i].item = 0;
		if (map[i].item == ITM_LOONYKEY && PlayerHasLunacyKey(player.worldNum)) // loonykeys only appear once
			map[i].item = 0;
		map[i].templight = -32; // make it all black so it'll fade in
	}

	world = wrld;
	// pop in all the badguys
	AddMapGuys(this);

	// reset all special clocks
	for (i = 0; i < MAX_SPECIAL; i++)
	{
		specialClock[i] = 0;
		if (special[i].trigger & TRG_TIMED)
		{
			// start it at a different time
			if (special[i].msg[0] == '@')
				specialClock[i] = atoi(&special[i].msg[1]);
		}
	}
	allSpecialClock = 0; // this one just counts up to 30 so you know when a second occurs
}

void Map::SmoothLight(int x, int y)
{
	int v, c;

	c = 1;
	v = map[x + y * width].light;
	if (x > 0)
	{
		c++;
		v += map[x - 1 + y * width].light;
	}
	if (x < width - 1)
	{
		c++;
		v += map[x + 1 + y * width].light;
	}
	if (y > 0)
	{
		c++;
		v += map[x + (y - 1) * width].light;
	}
	if (y < width - 1)
	{
		c++;
		v += map[x + (y + 1) * width].light;
	}
	v /= c;
	map[x + y * width].light = v;
}

void Map::SmoothLights(void)
{
	int i, j;

	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
			SmoothLight(i, j);
}

void Map::Update(byte mode, world_t *world)
{
	int i;
	static byte timeToReset = 0;
	static byte timeToAnim = 0;

	timeToReset++;
	if (timeToReset < 2)
		return;

	timeToAnim++;

	timeToReset = 0;

	for (i = 0; i < width * height; i++)
	{
		if (mode == UPDATE_FADEIN)
		{
			// make the lights approach what they are supposed to be
			if (map[i].templight > map[i].light)
				map[i].templight--;
			else if (map[i].templight < map[i].light - 1)
				map[i].templight += 2;
		}
		else if (mode == UPDATE_GAME)
		{
			// make the lights approach what they are supposed to be
			if (map[i].templight > map[i].light)
				map[i].templight--;
			else if (map[i].templight < map[i].light)
				map[i].templight++;
		}
		else if (mode == UPDATE_EDIT)
		{
			// make the lights right immediately
			map[i].templight = map[i].light;
		}
		else if (mode == UPDATE_FADE)
		{
			// just plain continuously darken
			if (map[i].templight>-32)
				map[i].templight -= 2;
		}

		if (mode != UPDATE_EDIT) // edit mode doesn't animate since it would screw it up
		{
			// check for animation
			if (timeToAnim == 2)
				if (world->terrain[map[i].floor].flags & TF_ANIM)
					map[i].floor = world->terrain[map[i].floor].next;
		}
	}

	if (timeToAnim == 2)
		timeToAnim = 0;
}

void Map::LOSPoints(int x, int y, int curx, int cury, int *p1x, int *p1y, int *p2x, int *p2y)
{
	int xdist, ydist;

	xdist = curx - x;
	ydist = cury - y;

	if (xdist < 0)
	{
		xdist = -xdist; // for abs. value needed later
		*p1x = curx + 1;
	}
	else if (xdist > 0)
		*p1x = curx - 1;
	else
		*p1x = curx;

	if (ydist < 0)
	{
		ydist = -ydist; // for abs. value needed later
		*p1y = cury + 1;
	}
	else if (ydist > 0)
		*p1y = cury - 1;
	else
		*p1y = cury;

	if (ydist > xdist) // need abs. value here
	{
		*p2x = curx;
		*p2y = *p1y;
	}
	else if (ydist < xdist) // and here
	{
		*p2x = *p1x;
		*p2y = cury;
	}
	else
	{
		*p2x = *p1x;
		*p2y = *p1y;
	}
}

byte Map::LOS(int x, int y, int radius, int value, byte(*DoIt)(int, int, int, int, int, Map *))
{
	int p1x, p1y, p2x, p2y;
	int i, curx, cury;

	if (x < 0 || x >= width || y < 0 || y >= height)
		return 0;

	// do whatever you are doing at x,y
	if (!DoIt(x, y, x, y, value, this))
		return 1; // DoIt returns zero if it wants you to quit

	map[x + y * width].opaque = 0;

	for (i = 1; i < radius; i++) // i is the radius of the square you are working with
	{
		for (cury = y - i; cury <= y + i; cury += i * 2)
			for (curx = x - i; curx <= x + i; curx++)
			{
				// check to be sure point is legal
				if (curx < 0 || curx >= width || cury < 0 || cury >= height)
					continue;
				LOSPoints(x, y, curx, cury, &p1x, &p1y, &p2x, &p2y);
				if (map[p1x + p1y * width].opaque +
						map[p2x + p2y * width].opaque >= 2)
				{
					map[curx + cury * width].opaque = 1;
				}
				else
				{
					if (map[curx + cury * width].wall) // there's a wall here, so opaque
						map[curx + cury * width].opaque = 1;
					else
						map[curx + cury * width].opaque = 0;
					// do what you have to, it's in sight
					if (!DoIt(curx, cury, x, y, value, this))
						return 1; // DoIt returns zero if it wants you to quit
				}
			}
		for (curx = x - i; curx <= x + i; curx += i * 2)
			for (cury = y - i + 1; cury <= y + i - 1; cury++)
			{
				// check to be sure point is legal
				if (curx < 0 || curx >= width || cury < 0 || cury >= height)
					continue;
				LOSPoints(x, y, curx, cury, &p1x, &p1y, &p2x, &p2y);
				if (map[p1x + p1y * width].opaque +
						map[p2x + p2y * width].opaque >= 2)
				{
					map[curx + cury * width].opaque = 1;
				}
				else
				{
					if (map[curx + cury * width].wall) // there's a wall here, so opaque
						map[curx + cury * width].opaque = 1;
					else
						map[curx + cury * width].opaque = 0;
					// do what you have to, it's in sight
					if (!DoIt(curx, cury, x, y, value, this))
						return 1;
				}
			}
	}
	return 0;
}

byte PlaceItemCallback(int x, int y, int cx, int cy, int value, Map *map)
{
	if (map->map[x + y * map->width].item)
		return 1;
	if (map->map[x + y * map->width].wall)
		return 1;
	if (world->terrain[map->map[x + y * map->width].floor].flags & (TF_WATER | TF_LAVA | TF_SOLID))
		return 1;

	map->map[x + y * map->width].item = (byte) value;
	if (value != ITM_BRAIN)
		MakeSound(SND_ITEMDROP, (x * TILE_WIDTH) << FIXSHIFT, (y * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF, 500);
	return 0; // all done, you placed the item
}

byte TorchCallback(int x, int y, int cx, int cy, int value, Map *map)
{
	int b;

	b = ((cx - x)*(cx - x)+(cy - y)*(cy - y)) / 2;
	b = value - b;

	if (b > 16)
	{
		b /= 4;
	}
	else if (b >= 0)
		b = 0;

	if (b < 0)
		return 1; // not bright enough

	if (map->map[x + y * map->width].light < b)
		map->map[x + y * map->width].light = b;
	if (map->map[x + y * map->width].light > MAX_LIGHT)
		map->map[x + y * map->width].light = MAX_LIGHT;
	return 1;
}

byte TempTorchCallback(int x, int y, int cx, int cy, int value, Map *map)
{
	int b, desiredLight;

	b = ((cx - x)*(cx - x)+(cy - y)*(cy - y));
	b = value - b;

	if (b < 0)
		return 1;

	if (b < 16)
	{
		desiredLight = b - 17;
	}
	else if (b < value - 4)
		desiredLight = 0;
	else
		desiredLight = b - (value - 4);

	if (map->map[x + y * map->width].templight < desiredLight - 1)
		map->map[x + y * map->width].templight += 2;
	else if (map->map[x + y * map->width].templight == desiredLight - 1)
		map->map[x + y * map->width].templight = desiredLight;

	return 1;
}

byte TempBrightCallback(int x, int y, int cx, int cy, int value, Map *map)
{
	int b;

	b = ((cx - x)*(cx - x)+(cy - y)*(cy - y))*3;
	b = value - b;

	if (b < 0)
		return 1;
	b += map->map[x + y * map->width].light;
	if (b > MAX_LIGHT)
		b = MAX_LIGHT;

	if (map->map[x + y * map->width].templight < b - 1)
		map->map[x + y * map->width].templight += 2;
	else if (map->map[x + y * map->width].templight == b - 1)
		map->map[x + y * map->width].templight = b;

	return 1;
}

byte Map::DropItem(int x, int y, byte itm)
{
	return LOS(x, y, 10, itm, PlaceItemCallback);
}

void Map::PermaTorch(int x, int y, char brt)
{
	LOS(x, y, brt / 2, brt, TorchCallback);
}

void Map::TempTorch(int x, int y, char brt)
{
	LOS(x, y, brt / 4, brt, TempTorchCallback);
}

void Map::BrightTorch(int x, int y, char brt, byte size)
{
	LOS(x, y, size, brt, TempBrightCallback);
}

void Map::LightSpecial(int x, int y, char brt, byte size)
{
	LOS(x, y, size, brt, TorchCallback);
}

void Map::Swap(int sx, int sy, int blkwidth, int blkheight, int dx, int dy)
{
	mapTile_t *tempMap;
	int i;

	if (sx < 0 || sx >= width || sy < 0 || sy >= height ||
			(sx + blkwidth) < sx || (sx + blkwidth) > width ||
			(sy + blkheight) < sy || (sy + blkheight) > height ||
			dx < 0 || dx >= width || dy < 0 || dy >= height ||
			(dx + blkwidth) < dx || (dx + blkwidth) > width ||
			(dy + blkheight) < dy || (dy + blkheight) > height)
		return; // illegal swap

	tempMap = (mapTile_t *) malloc(blkwidth * blkheight * sizeof (mapTile_t));
	if (!tempMap)
		return; // can't swap, not enough memory

	// copy destination to temp
	for (i = 0; i < blkheight; i++)
	{
		memcpy(&tempMap[i * blkwidth], &map[(i + dy) * width + dx], sizeof (mapTile_t) * blkwidth);
	}

	// copy source to dest
	for (i = 0; i < blkheight; i++)
	{
		memcpy(&map[(i + dy) * width + dx], &map[(i + sy) * width + sx], sizeof (mapTile_t) * blkwidth);
	}

	// and copy temp to source
	for (i = 0; i < blkheight; i++)
	{
		memcpy(&map[(i + sy) * width + sx], &tempMap[i * blkwidth], sizeof (mapTile_t) * blkwidth);
	}

	free(tempMap);

	// move all specials that are in the target zone
	for (i = 0; i < MAX_SPECIAL; i++)
	{
		if (special[i].x >= sx && special[i].y >= sy && special[i].x < sx + blkwidth && special[i].y < sy + blkheight)
		{
			special[i].x = special[i].x - sx + dx;
			special[i].y = special[i].y - sy + dy;
		}
		else if (special[i].x >= dx && special[i].y >= dy && special[i].x < dx + blkwidth && special[i].y < dy + blkheight)
		{
			special[i].x = special[i].x + sx - dx;
			special[i].y = special[i].y + sy - dy;
		}
	}

	// and move all guys in the target
	GuySwap(sx, sy, blkwidth, blkheight, dx, dy);
	// don't forget bullets
	BulletSwap(sx, sy, blkwidth, blkheight, dx, dy);
}

void Map::Copy(int sx, int sy, int blkwidth, int blkheight, int dx, int dy)
{
	int i;

	if (sx < 0 || sx >= width || sy < 0 || sy >= height ||
			(sx + blkwidth) < sx || (sx + blkwidth) > width ||
			(sy + blkheight) < sy || (sy + blkheight) > height ||
			dx < 0 || dx >= width || dy < 0 || dy >= height ||
			(dx + blkwidth) < dx || (dx + blkwidth) > width ||
			(dy + blkheight) < dy || (dy + blkheight) > height)
		return; // illegal coords

	// copy source to dest
	for (i = 0; i < blkheight; i++)
	{
		memcpy(&map[(i + dy) * width + dx], &map[(i + sy) * width + sx], sizeof (mapTile_t) * blkwidth);
	}

	// move all specials that are in the target zone
	for (i = 0; i < MAX_SPECIAL; i++)
	{
		if (special[i].x >= sx && special[i].y >= sy && special[i].x < sx + blkwidth && special[i].y < sy + blkheight)
		{
			special[i].x = special[i].x - sx + dx;
			special[i].y = special[i].y - sy + dy;
		}
	}
}

char Map::LightOf(int x, int y)
{
	x = (x < 0) ? 0 : (x > width - 1) ? width - 1 : x;
	y = (y < 0) ? 0 : (y > height - 1) ? height - 1 : y;
	return map[x + y * width].templight;
}

bool Map::Wall(int x, int y)
{
	x = (x < 0) ? 0 : (x > width - 1) ? width - 1 : x;
	y = (y < 0) ? 0 : (y > height - 1) ? height - 1 : y;
	return map[x + y * width].wall;
}

byte Map::MakeSmoothShadow(int x, int y)
{
	if (Wall(x, y + 1))
	{
		if (Wall(x + 1, y))
		{
			return 6;
		}
		else
		{
			if (Wall(x + 1, y + 1))
			{
				return 4;
			}
			else
			{
				return 7;
			}
		}
	}
	else
	{
		if (Wall(x + 1, y))
		{
			if (Wall(x + 1, y + 1))
			{
				return 1;
			}
			else
			{
				return 2;
			}
		}
		else
		{
			if (Wall(x + 1, y + 1))
			{
				return 3;
			}
			else
			{
				return 0;
			}
		}
	}
}

char* Map::MakeSmoothLighting(bool beZero, int x, int y)
{
	if (beZero)
	{
		memset(smoothLight, 0, 9);
	}
	else
	{
		smoothLight[0] = LightOf(x - 1, y - 1);
		smoothLight[1] = LightOf(x, y - 1);
		smoothLight[2] = LightOf(x + 1, y - 1);
		smoothLight[3] = LightOf(x - 1, y);
		smoothLight[4] = LightOf(x, y);
		smoothLight[5] = LightOf(x + 1, y);
		smoothLight[6] = LightOf(x - 1, y + 1);
		smoothLight[7] = LightOf(x, y + 1);
		smoothLight[8] = LightOf(x + 1, y + 1);
	}
	return smoothLight;
}

void Map::Render(world_t *world, int camX, int camY, byte flags)
{
	int i, j;

	int tileX, tileY;
	int ofsX, ofsY;
	int scrX, scrY;
	mapTile_t *m;

	camX -= 320;
	camY -= 240;

	tileX = (camX / TILE_WIDTH) - 1;
	tileY = (camY / TILE_HEIGHT) - 1;
	ofsX = camX % TILE_WIDTH;
	ofsY = camY % TILE_HEIGHT;

	int lite;
	if (!(flags & MAP_SHOWLIGHTS))
		lite = DISPLAY_GHOST; // let's pretend ghost means we're ignoring lighting
	else
		lite = 0;

	scrX = -ofsX - TILE_WIDTH;
	for (i = tileX; i < tileX + (640 / TILE_WIDTH + 4); i++)
	{
		scrY = -ofsY - TILE_HEIGHT;
		for (j = tileY; j < tileY + (480 / TILE_HEIGHT + 6); j++)
		{
			if (i >= 0 && i < width && j >= 0 && j < height)
			{
				m = &map[i + j * width];

				if (m->item && (flags & MAP_SHOWITEMS))
					RenderItem(scrX + camX + (TILE_WIDTH / 2), scrY + camY + (TILE_HEIGHT / 2),
						m->item, lite ? 0 : m->templight);

				if (m->wall && (flags & MAP_SHOWWALLS)) // there is a wall on this tile
				{
					if (j < height - 1)
					{
						// if the tile below this one is also a wall, don't waste the
						// time of drawing the front of this wall
						if (map[i + (j + 1) * width].wall &&
								(!(world->terrain[map[i + (j + 1) * width].floor].flags & TF_TRANS)))
						{
							if (world->terrain[m->floor].flags & TF_TRANS)
								RoofDraw(scrX + camX, scrY + camY, m->floor, this,
									DISPLAY_DRAWME | DISPLAY_ROOFTILE | DISPLAY_TRANSTILE | lite);
							else
								RoofDraw(scrX + camX, scrY + camY, m->floor, this, DISPLAY_DRAWME | DISPLAY_ROOFTILE | lite);
						}
						else
							if (world->terrain[m->floor].flags & TF_TRANS)
							WallDraw(scrX + camX, scrY + camY, m->wall, m->floor, this,
								DISPLAY_DRAWME | DISPLAY_WALLTILE | DISPLAY_TRANSTILE | lite);
						else
							WallDraw(scrX + camX, scrY + camY, m->wall, m->floor, this,
								DISPLAY_DRAWME | DISPLAY_WALLTILE | lite);
					}
						// make wall tiles get drawn in sorted order unlike the floor tiles
					else
					{
						if (world->terrain[m->floor].flags & TF_TRANS)
							WallDraw(scrX + camX, scrY + camY, m->wall, m->floor, this,
								DISPLAY_DRAWME | DISPLAY_WALLTILE | DISPLAY_TRANSTILE | lite);
						else
							WallDraw(scrX + camX, scrY + camY, m->wall, m->floor, this,
								DISPLAY_DRAWME | DISPLAY_WALLTILE | lite);
					}
				}
				else
				{
					if (j < height - 1)
					{
						// if there is a wall on the tile below this one, no
						// point in rendering this floor (unless it is transparent
						if ((!map[i + (j + 1) * width].wall) || (!(flags & MAP_SHOWWALLS) ||
								(world->terrain[map[i + (j + 1) * width].floor].flags & TF_TRANS)))
						{
							RenderFloorTileFancy(scrX, scrY, m->floor, MakeSmoothShadow(i, j), MakeSmoothLighting(lite, i, j));
						}
					}
					else
					{
						RenderFloorTileFancy(scrX, scrY, m->floor, MakeSmoothShadow(i, j), MakeSmoothLighting(lite, i, j));
					}
				}
			}
			else
			{
				// put black in empty spaces
				DrawFillBox(scrX, scrY, scrX + TILE_WIDTH - 1, scrY + TILE_HEIGHT - 1, 0);
			}
			scrY += TILE_HEIGHT;
		}
		scrX += TILE_WIDTH;
	}

	if (this->flags & MAP_STARS)
	{
		int tx, ty;

		for (i = 0; i < NUM_STARS; i++)
		{
			tx = (starX[i] + camX) / TILE_WIDTH;
			ty = (starY[i] + camY) / TILE_HEIGHT;

			if (tx < 0 || ty < 0 || tx >= width || ty >= height)
				continue;

			m = &map[tx + ty * width];
			if (m->wall)
				continue;

			PlotStar(starX[i], starY[i], starCol[i], (starX[i] + camX - tx * TILE_WIDTH), (starY[i] + camY - ty * TILE_HEIGHT), m->floor);
		}
	}
}

void ZapWall(Map *map, int x, int y, byte newFloor)
{
	byte flr, wall;

	if (map->map[x + y * map->width].wall == 0) // no wall to zap
		return;

	// store these for looking for neighbors
	flr = map->map[x + y * map->width].floor;
	wall = map->map[x + y * map->width].wall;

	// turn this square into floor
	map->map[x + y * map->width].floor = newFloor;
	map->map[x + y * map->width].wall = 0;

	// recurse to all neighbors that match
	if (x > 0 && map->map[x - 1 + y * map->width].wall == wall &&
			map->map[x - 1 + y * map->width].floor == flr)
		ZapWall(map, x - 1, y, newFloor);
	if (x < map->width - 1 && map->map[x + 1 + y * map->width].wall == wall &&
			map->map[x + 1 + y * map->width].floor == flr)
		ZapWall(map, x + 1, y, newFloor);
	if (y > 0 && map->map[x + (y - 1) * map->width].wall == wall &&
			map->map[x + (y - 1) * map->width].floor == flr)
		ZapWall(map, x, y - 1, newFloor);
	if (y < map->height - 1 && map->map[x + (y + 1) * map->width].wall == wall &&
			map->map[x + (y + 1) * map->width].floor == flr)
		ZapWall(map, x, y + 1, newFloor);
}

void RaiseWall(Map *map, int x, int y, byte newWall)
{
	byte flr;

	if (map->map[x + y * map->width].wall) // there's a wall already, can't raise one
		return;
	if (newWall == 0) // can't raise a non-wall (wall value of 0 indicates absence of wall)
		return;

	// store these for looking for neighbors
	flr = map->map[x + y * map->width].floor;

	// turn this square into a wall
	map->map[x + y * map->width].wall = newWall;

	// recurse to all neighbors that match
	if (x > 0 && map->map[x - 1 + y * map->width].wall == 0 &&
			map->map[x - 1 + y * map->width].floor == flr)
		RaiseWall(map, x - 1, y, newWall);
	if (x < map->width - 1 && map->map[x + 1 + y * map->width].wall == 0 &&
			map->map[x + 1 + y * map->width].floor == flr)
		RaiseWall(map, x + 1, y, newWall);
	if (y > 0 && map->map[x + (y - 1) * map->width].wall == 0 &&
			map->map[x + (y - 1) * map->width].floor == flr)
		RaiseWall(map, x, y - 1, newWall);
	if (y < map->height - 1 && map->map[x + (y + 1) * map->width].wall == 0 &&
			map->map[x + (y + 1) * map->width].floor == flr)
		RaiseWall(map, x, y + 1, newWall);
}

void ChangeFloor(Map *map, int x, int y, byte newFloor)
{
	byte flr;

	if (map->map[x + y * map->width].wall) // there's a wall, no good
		return;

	// store these for looking for neighbors
	flr = map->map[x + y * map->width].floor;

	if (flr == newFloor)
		return; // can't change to itself!

	// change the floor
	map->map[x + y * map->width].floor = newFloor;

	// recurse to all neighbors that match
	if (x > 0 && map->map[x - 1 + y * map->width].wall == 0 &&
			map->map[x - 1 + y * map->width].floor == flr)
		ChangeFloor(map, x - 1, y, newFloor);
	if (x < map->width - 1 && map->map[x + 1 + y * map->width].wall == 0 &&
			map->map[x + 1 + y * map->width].floor == flr)
		ChangeFloor(map, x + 1, y, newFloor);
	if (y > 0 && map->map[x + (y - 1) * map->width].wall == 0 &&
			map->map[x + (y - 1) * map->width].floor == flr)
		ChangeFloor(map, x, y - 1, newFloor);
	if (y < map->height - 1 && map->map[x + (y + 1) * map->width].wall == 0 &&
			map->map[x + (y + 1) * map->width].floor == flr)
		ChangeFloor(map, x, y + 1, newFloor);
}

void SpecialStepCheck(Map *map, int x, int y, Guy *me)
{
	int i;

	for (i = 0; i < MAX_SPECIAL; i++)
	{
		if ((map->special[i].trigger & (TRG_STEP | TRG_ENEMYSTEP)) &&
				map->special[i].x == x && map->special[i].y == y)
		{
			if ((me->type == MONS_BOUAPHA && (map->special[i].trigger & TRG_STEP)) ||
					(me->type != MONS_BOUAPHA && (map->special[i].trigger & TRG_ENEMYSTEP) &&
					(map->special[i].trigValue == 0 || map->special[i].trigValue == me->type)))
				SpecialTakeEffect(i, map, &map->special[i], me);
		}
		else if ((map->special[i].trigger & TRG_NEAR) && (me->type == MONS_BOUAPHA) &&
				(abs(map->special[i].x - x) + abs(map->special[i].y - y) <= map->special[i].trigValue))
		{
			SpecialTakeEffect(i, map, &map->special[i], me);
		}
	}
}

void SpecialShootCheck(Map *map, int x, int y)
{
	int i;

	for (i = 0; i < MAX_SPECIAL; i++)
		if ((map->special[i].trigger & TRG_SHOOT) &&
				map->special[i].x >= x - 1 && map->special[i].y >= y - 1 &&
				map->special[i].x <= x + 1 && map->special[i].y <= y + 1)
		{
			SpecialTakeEffect(i, map, &map->special[i], NULL);
		}
}

void SpecialKillCheck(Map *map, byte type)
{
	int i;

	for (i = 0; i < MAX_SPECIAL; i++)
		if ((map->special[i].trigger & TRG_KILLONE) &&
				map->special[i].trigValue == type)
		{
			SpecialTakeEffect(i, map, &map->special[i], NULL);
		}
}

void SpecialAnytimeCheck(Map *map)
{
	int i;

	allSpecialClock++;
	if (allSpecialClock == 30)
	{
		allSpecialClock = 0;
		for (i = 0; i < MAX_SPECIAL; i++)
			specialClock[i]++;
	}

	for (i = 0; i < MAX_SPECIAL; i++)
		if ((map->special[i].trigger) &&
				(!(map->special[i].trigger & (TRG_SHOOT | TRG_STEP | TRG_ENEMYSTEP | TRG_NEAR | TRG_CHAIN | TRG_KILLONE))))
		{
			SpecialTakeEffect(i, map, &map->special[i], NULL);
		}
}

void RenderSpecialXes(MGLDraw *mgl, Map *map, byte world)
{
	int i;
	int camx, camy;

	GetCamera(&camx, &camy);
	camx -= 320;
	camy -= 240;
	for (i = 0; i < MAX_SPECIAL; i++)
		if (map->special[i].effect == SPC_GOTOMAP && PlayerPassedLevel(world, map->special[i].value))
			DrawRedX(map->special[i].x * TILE_WIDTH - camx + TILE_WIDTH / 2,
				map->special[i].y * TILE_HEIGHT - camy + TILE_HEIGHT / 2, mgl);
}

void SpecialNeighborCheck(Map *map, special_t *spcl)
{
	int i, j, k;

	for (i = spcl->x - 1; i <= spcl->x + 1; i++)
		for (j = spcl->y - 1; j <= spcl->y + 1; j++)
		{
			if (i != spcl->x || j != spcl->y)
				for (k = 0; k < MAX_SPECIAL; k++)
				{
					if (map->special[k].x == i && map->special[k].y == j && (map->special[k].trigger & TRG_CHAIN))
						SpecialTakeEffect(i, map, &map->special[k], NULL);
				}
		}
}

void SpecialTakeEffect(byte num, Map *map, special_t *spcl, Guy *victim)
{
	char s[64];
	char *tok;
	byte fail;
	int sx, sy, sx2, sy2;
	dword start, end;

	// first check to be sure all conditions are met
	if (spcl->trigger & TRG_KILLALL)
	{
		if (MonsterExists(spcl->trigValue))
			return; // they ain't all dead yet
	}
	if (spcl->trigger & TRG_HAVEBRAINS)
	{
		if (PlayerBrains() < spcl->trigValue)
			return;
	}
	if (spcl->trigger & TRG_KEYCHAINS)
	{
		if ((!PlayerKeyChain(0)) ||
				(!PlayerKeyChain(1)) ||
				(!PlayerKeyChain(2)) ||
				(!PlayerKeyChain(3)))
			return; // must have them all
	}
	if (spcl->trigger & TRG_PASSLEVELS)
	{
		if (PlayerLevelsPassed() < spcl->trigValue)
			return;
	}
	if (spcl->trigger & TRG_TIMED)
	{
		if (specialClock[num] != spcl->trigValue)
			return;
		specialClock[num] = 0; // repeat
	}
	if (spcl->trigger & TRG_RANDOM)
	{
		if (allSpecialClock != 0) // only can happen once a second
			return;
		if (MGL_random(100) >= spcl->trigValue)
			return;
	}
	if (spcl->trigger & TRG_HAVELOONY)
	{
		if (!player.lunacyKey[player.worldNum]) // have the right loony key
			return;
	}
	if (spcl->trigger & TRG_FLOORHERE) // the floor is the right type
	{
		if (map->map[spcl->x + spcl->y * map->width].floor != spcl->trigValue)
			return;
	}

	if (!(spcl->trigger & TRG_CHAIN))
		SpecialNeighborCheck(map, spcl);
#ifdef LOG
	fprintf(logFile, "Special #%d, effect %d\n", num, spcl->effect);
	fflush(logFile);
#endif
	switch (spcl->effect) {
		case SPC_GOTOMAP:
			MakeNormalSound(SND_GOTOMAP);
			SendMessageToGame(MSG_GOTOMAP, spcl->value);
			break;
		case SPC_EXIT:
#ifdef LOG
			MonsterTally();
			BulletTally();
#endif
			if (PlayerBrains() == totalBrains)
			{
				SendMessageToGame(MSG_WINLEVEL, 0);
				MakeNormalSound(SND_WINLEVEL);
			}
			else
				return; // to avoid triggering a message or shutting it off if repeatable
			break;
		case SPC_ZAPWALL:
			if (map->map[spcl->effectX + spcl->effectY * map->width].wall)
			{
				MakeSound(SND_WALLDOWN, (spcl->effectX * TILE_WIDTH) << FIXSHIFT,
						(spcl->effectY * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF | SND_ONE, 1500);
				ZapWall(map, spcl->effectX, spcl->effectY, spcl->value);
			}
			break;
		case SPC_RAISEWALL:
			if (!map->map[spcl->effectX + spcl->effectY * map->width].wall)
			{
				MakeSound(SND_WALLUP, (spcl->effectX * TILE_WIDTH) << FIXSHIFT,
						(spcl->effectY * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF | SND_ONE, 1500);
				RaiseWall(map, spcl->effectX, spcl->effectY, spcl->value);
			}
			break;
		case SPC_TOGGLEWALL:
			if (map->map[spcl->effectX + spcl->effectY * map->width].wall)
			{
				MakeSound(SND_WALLDOWN, (spcl->effectX * TILE_WIDTH) << FIXSHIFT,
						(spcl->effectY * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF | SND_ONE, 1500);
				// store what the wall looked like for later popping it back up
				sx = map->map[spcl->effectX + spcl->effectY * map->width].wall;
				ZapWall(map, spcl->effectX, spcl->effectY,
						spcl->value);
				spcl->value = sx;
			}
			else
			{
				MakeSound(SND_WALLUP, (spcl->effectX * TILE_WIDTH) << FIXSHIFT,
						(spcl->effectY * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF | SND_ONE, 1500);
				sx = map->map[spcl->effectX + spcl->effectY * map->width].floor;
				RaiseWall(map, spcl->effectX, spcl->effectY, spcl->value);
				spcl->value = sx;
			}
			break;
		case SPC_LIGHT:
			MakeSound(SND_LIGHTSON, (spcl->effectX * TILE_WIDTH) << FIXSHIFT,
					(spcl->effectY * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF | SND_ONE, 1300);
			map->LightSpecial(spcl->effectX, spcl->effectY, spcl->value, abs(spcl->value) / 2);
			break;
		case SPC_TELEPORT:
			if (!victim)
				victim = goodguy;

			if (victim)
			{
				if (victim->type == MONS_BOUAPHA)
				{
					if (GetTportClock())
						return; // can't teleport yet
				}
				MakeSound(SND_TELEPORT, victim->x, victim->y, SND_CUTOFF, 1500);
				victim->x = (spcl->effectX * TILE_WIDTH + (TILE_WIDTH / 2)) << FIXSHIFT;
				victim->y = (spcl->effectY * TILE_HEIGHT + (TILE_HEIGHT / 2)) << FIXSHIFT;
				victim->dx = 0;
				victim->dy = 0;
				if (victim->type == MONS_BOUAPHA)
				{
					PutCamera(victim->x, victim->y);
					UpdateCamera(victim->x, victim->y, victim->facing, map);
					SetTportClock(30);
				}
				map->map[spcl->effectX + spcl->effectY * map->width].templight = 34;
				map->map[spcl->effectX - 1 + spcl->effectY * map->width].templight = 20;
				map->map[spcl->effectX + 1 + spcl->effectY * map->width].templight = 20;
				map->map[spcl->effectX + (spcl->effectY - 1) * map->width].templight = 20;
				map->map[spcl->effectX + (spcl->effectY + 1) * map->width].templight = 20;
			}
			break;
		case SPC_SUMMON:
			MakeSound(SND_TELEPORT, (spcl->effectX * TILE_WIDTH + (TILE_WIDTH / 2)) << FIXSHIFT,
					(spcl->effectY * TILE_HEIGHT + (TILE_HEIGHT / 2)) << FIXSHIFT, SND_CUTOFF, 1500);
			AddGuy((spcl->effectX * TILE_WIDTH + (TILE_WIDTH / 2)) << FIXSHIFT,
					(spcl->effectY * TILE_HEIGHT + (TILE_HEIGHT / 2)) << FIXSHIFT,
					64 * FIXAMT, spcl->value);
			break;
		case SPC_PICTURE:
			ShowImageOrFlic(spcl->msg);
			break;
		case SPC_PLAYSONG:
			// play the chosen song, but ONLY if music is set to On
			// if it's set to random, the player wants random
			start = timeGetTime();
			if (PlayerGetMusicSettings() == MUSIC_ON)
				CDPlay(spcl->value);
			end = timeGetTime();
			AddGarbageTime(end - start);
			break;
		case SPC_PLAYSOUND:
			if (spcl->effectX == 0 && spcl->effectY == 0)
				MakeNormalSound(spcl->value);
			else
				MakeSound(spcl->value, (spcl->effectX * TILE_WIDTH) << FIXSHIFT,
					(spcl->effectY * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF, 10000);
			break;
		case SPC_PLAYSOUND2:
			if (spcl->effectX == 0 && spcl->effectY == 0)
				MakeNormalSound(spcl->value + 256);
			else
				MakeSound(spcl->value + 256, (spcl->effectX * TILE_WIDTH) << FIXSHIFT,
					(spcl->effectY * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF, 10000);
			break;
		case SPC_DROPITEM:
			map->map[spcl->effectX + spcl->effectY * map->width].item = spcl->value;
			break;
		case SPC_TEMPLIGHT:
			map->BrightTorch(spcl->effectX, spcl->effectY, spcl->value, spcl->value / 2);
			break;
		case SPC_CHGTILE:
			ChangeFloor(map, spcl->effectX, spcl->effectY, spcl->value);
			break;
		case SPC_SWAPMAP:
			// must parse coords from message text
			strcpy(s, spcl->msg);
			fail = 0;
			tok = strtok(s, ", ");
			if (tok)
			{
				sx = atoi(tok);
				tok = strtok(NULL, "- ");
				if (tok)
				{
					sy = atoi(tok);
					tok = strtok(NULL, ", ");
					if (tok)
					{
						sx2 = atoi(tok);
						tok = strtok(NULL, " ");
						if (tok)
							sy2 = atoi(tok);
						else fail = 1;
					}
					else fail = 1;
				}
				else fail = 1;
			}
			else fail = 1;

			if (!fail)
			{
				map->Swap(sx, sy, (sx2 - sx + 1), (sy2 - sy + 1), spcl->effectX, spcl->effectY);
			}
			break;
		case SPC_COPYMAP:
			// must parse coords from message text
			strcpy(s, spcl->msg);
			fail = 0;
			tok = strtok(s, ", ");
			if (tok)
			{
				sx = atoi(tok);
				tok = strtok(NULL, "- ");
				if (tok)
				{
					sy = atoi(tok);
					tok = strtok(NULL, ", ");
					if (tok)
					{
						sx2 = atoi(tok);
						tok = strtok(NULL, " ");
						if (tok)
							sy2 = atoi(tok);
						else fail = 1;
					}
					else fail = 1;
				}
				else fail = 1;
			}
			else fail = 1;

			if (!fail)
			{
				map->Copy(sx, sy, (sx2 - sx + 1), (sy2 - sy + 1), spcl->effectX, spcl->effectY);
			}
			break;
		case SPC_WINANDGO:
			if (PlayerBrains() == totalBrains)
			{
				SendMessageToGame(MSG_WINLEVEL, spcl->value);
				MakeNormalSound(SND_WINLEVEL);
			}
			else
				return; // to avoid triggering a message or shutting it off if repeatable
			break;
		case SPC_KILLMONS:
			if (spcl->effectX != 0 || spcl->effectY != 0)
				KillMonster(spcl->effectX, spcl->effectY);
			else
				KillAllMonsters(spcl->value);
			break;
		case SPC_CHGMONS:
			sx = atoi(spcl->msg);

			if (spcl->effectX != 0 || spcl->effectY != 0)
				ChangeMonster(spcl->effectX, spcl->effectY, spcl->value);
			else
				ChangeAllMonsters(sx, spcl->value);
			break;
		case SPC_RMVSPCL:
			for (sx = 0; sx < MAX_SPECIAL; sx++)
				if (map->special[sx].x == spcl->effectX && map->special[sx].y == spcl->effectY)
					map->special[sx].trigger = 0;
			break;
		case SPC_TOGGLEITEM:
			if (spcl->effectX >= 0 && spcl->effectX < map->width && spcl->effectY >= 0 &&
					spcl->effectY < map->height)
			{
				sx = map->map[spcl->effectX + spcl->effectY * map->width].item;
				map->map[spcl->effectX + spcl->effectY * map->width].item = spcl->value;
				spcl->value = sx;
			}
			break;
	}
	if (spcl->trigger & TRG_MESSAGE)
		NoRepeatNewMessage(spcl->msg, 75, 1);
	if (!(spcl->trigger & TRG_REPEATABLE))
		spcl->trigger = 0; // can't trigger anymore
}

int TotalBrains(void)
{
	return totalBrains;
}
