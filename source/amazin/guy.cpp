#include "guy.h"
#include "display.h"
#include "game.h"
#include "jamulspr.h"
#include "map.h"
#include "options.h"
#include "particle.h"
#include "player.h"
#include "sound.h"
#include "title.h"

namespace GuyFlags
{
	enum : byte
	{
		Player = 0x1,
		Retreating = 0x2,
		Pumpkin = 0x4,
	};
}

struct Guy
{
	byte player;
	GuyType type;
	int x, y;
	int destX, destY;
	byte gridX, gridY;
	Direction direction;
	byte anim;
	byte frame;
	const sprite_t *spr;
	word hammerTime;
	byte invincibleTime;
	word sleepTime;
	word speedTime;
	word ghostTime;
	word fireTime;
	word iceTime;
	word evilEyeTime;
	byte flags;
};

constexpr int MAX_GUYS = 16;

const int8_t g_DirectionX[4] = {1, 0, -1, 0};
const int8_t g_DirectionY[4] = {0, 1, 0, -1};

sprite_set_t *g_GuySprites[8];
Guy g_Guys[16];
byte g_SnowTimer;
bool g_RivalsColliding;
byte g_PlayersWithHammers;

void GuysInit()
{
	for (int i = 0; i < MAX_GUYS; i = i + 1)
	{
		g_Guys[i].type = GuyType::None;
	}
	g_PlayersWithHammers = 0;
	g_SnowTimer = 0;
}

void GuysExit()
{
}

void GuysInitGfx()
{
	g_GuySprites[0] = nullptr;
	g_GuySprites[1] = new sprite_set_t("graphics/bouapha.jsp");
	g_GuySprites[2] = new sprite_set_t("graphics/bphetta.jsp");
	g_GuySprites[3] = new sprite_set_t("graphics/smoove.jsp");
	g_GuySprites[4] = new sprite_set_t("graphics/chuckles.jsp");
	g_GuySprites[5] = new sprite_set_t("graphics/helga.jsp");
	g_GuySprites[6] = new sprite_set_t("graphics/pete.jsp");
	g_ButtonsJsp = new sprite_set_t("graphics/buttons.jsp");
}

void GuysExitGfx()
{
	delete g_GuySprites[1];
	delete g_GuySprites[2];
	delete g_GuySprites[3];
	delete g_GuySprites[4];
	delete g_GuySprites[5];
	delete g_GuySprites[6];
	delete g_ButtonsJsp;
}

void GuyUpdate(Guy *me);

void GuyAdd(byte x, byte y, GuyType type)
{
	ushort uVar1;
	bool bVar2;
	uint uVar3;
	int i;

	i = 0;
	while (true)
	{
		if (0xf < i)
		{
			return;
		}
		if (g_Guys[i].type == GuyType::None)
			break;
		i = i + 1;
	}
	g_Guys[i].type = type;
	if (g_Guys[i].type == GuyType::Bouapha)
	{
		bVar2 = ConfigGetBouaphettaMode();
		g_Guys[i].player = bVar2 + 1;
	}
	else if (g_Guys[i].type == GuyType::Bouaphetta)
	{
		bVar2 = ConfigGetBouaphettaMode();
		g_Guys[i].player = 2 - bVar2;
	}
	else
	{
		g_Guys[i].player = 0;
	}
	g_Guys[i].x = ((uint)x * 0x20 + 0x10) * 0x100;
	g_Guys[i].y = ((uint)y * 0x20 + 0x10) * 0x100;
	g_Guys[i].gridX = x;
	g_Guys[i].gridY = y;
	g_Guys[i].direction = Direction::South;
	g_Guys[i].anim = 0;
	g_Guys[i].frame = 0;
	if ((g_Guys[i].type == GuyType::Bouapha) || (g_Guys[i].type == GuyType::Bouaphetta))
	{
		g_Guys[i].hammerTime = 0;
	}
	else if ((((g_Guys[i].type == GuyType::Smoove) || (g_Guys[i].type == GuyType::Chuckles)) || (g_Guys[i].type == GuyType::Helga)) || (g_Guys[i].type == GuyType::Pete))
	{
		if (g_MapNum < 0x15)
		{
			uVar1 = (ushort)g_MapNum;
			uVar3 = MGL_random(0x28);
			g_Guys[i].hammerTime = uVar1 * -5 + 0x78 + (short)uVar3;
		}
		else
		{
			uVar3 = MGL_random(0x28);
			g_Guys[i].hammerTime = (short)uVar3 + 10;
		}
	}
	g_Guys[i].destX = g_Guys[i].x;
	g_Guys[i].destY = g_Guys[i].y;
	if ((g_Guys[i].type == GuyType::Bouapha) || (g_Guys[i].type == GuyType::Bouaphetta))
	{
		g_Guys[i].flags = GuyFlags::Player;
	}
	else
	{
		g_Guys[i].flags = GuyFlags::Pumpkin;
	}
	g_Guys[i].sleepTime = 0;
	g_Guys[i].speedTime = 0;
	g_Guys[i].ghostTime = 0;
	g_Guys[i].fireTime = 0;
	g_Guys[i].iceTime = 0;
	g_Guys[i].evilEyeTime = 0;
	GuyUpdate(g_Guys + i);
}

bool GuyStartWalking(byte x, byte y, Direction dir, Guy *me)
{
	bool bVar1;
	uint rivalY;
	uint rivalX;
	int i;

	rivalX = 200;
	rivalY = 200;
	if ((me->type == GuyType::Bouapha) || (me->type == GuyType::Bouaphetta))
	{
		for (i = 0; i < 0x10; i = i + 1)
		{
			if (byte(g_Guys[i].type) == 3 - byte(me->type))
			{
				rivalX = (uint)g_Guys[i].gridX;
				rivalY = (uint)g_Guys[i].gridY;
			}
		}
	}
	if (dir == Direction::East)
	{
		if (x < 0x12)
		{
			if ((me->ghostTime == 0) && ((g_Map[y][x + 1].flags & TileFlags::WallV) != 0))
			{
				bVar1 = false;
			}
			else if (((me->flags & GuyFlags::Player) == 0) || ((g_Map[y][x + 1].flags & TileFlags::P) == 0))
			{
				if ((((me->flags & GuyFlags::Pumpkin) == 0) || (me->hammerTime == 0)) ||
				    ((g_Map[y][x + 1].flags & TileFlags::P) != 0))
				{
					if ((me->ghostTime == 0) && (g_Map[y][x + 1].item == ItemType::IceTrail))
					{
						bVar1 = false;
					}
					else if ((x + 1 == rivalX) && (y == rivalY))
					{
						g_RivalsColliding = true;
						bVar1 = false;
					}
					else if (((g_Map[y][x + 1].tile == 3) || (g_Map[y][x + 1].tile == 10)) || (g_Map[y][x + 1].tile == 0xd))
					{
						bVar1 = false;
					}
					else
					{
						bVar1 = true;
					}
				}
				else
				{
					bVar1 = false;
				}
			}
			else
			{
				bVar1 = false;
			}
		}
		else
		{
			bVar1 = false;
		}
	}
	else if (dir == Direction::South)
	{
		if (y < 0xd)
		{
			if ((me->ghostTime == 0) && ((g_Map[y][x].flags & TileFlags::WallH) != 0))
			{
				bVar1 = false;
			}
			else if (((me->flags & GuyFlags::Player) == 0) || ((g_Map[y + 1][x].flags & TileFlags::P) == 0))
			{
				if ((((me->flags & GuyFlags::Pumpkin) == 0) || (me->hammerTime == 0)) ||
				    ((g_Map[y + 1][x].flags & TileFlags::P) != 0))
				{
					if ((me->ghostTime == 0) && (g_Map[y + 1][x].item == ItemType::IceTrail))
					{
						bVar1 = false;
					}
					else if ((x == rivalX) && (y + 1 == rivalY))
					{
						g_RivalsColliding = true;
						bVar1 = false;
					}
					else if (((g_Map[y + 1][x].tile == 3) || (g_Map[y + 1][x].tile == 10)) || (g_Map[y + 1][x].tile == 0xd))
					{
						bVar1 = false;
					}
					else
					{
						bVar1 = true;
					}
				}
				else
				{
					bVar1 = false;
				}
			}
			else
			{
				bVar1 = false;
			}
		}
		else
		{
			bVar1 = false;
		}
	}
	else if (dir == Direction::West)
	{
		if (x == 0)
		{
			bVar1 = false;
		}
		else if ((me->ghostTime == 0) && ((g_Map[y][x].flags & TileFlags::WallV) != 0))
		{
			bVar1 = false;
		}
		else if (((me->flags & GuyFlags::Player) == 0) || ((g_Map[y - 1][x + 0x12].flags & TileFlags::P) == 0))
		{
			if ((((me->flags & GuyFlags::Pumpkin) == 0) || (me->hammerTime == 0)) ||
			    ((g_Map[y - 1][x + 0x12].flags & TileFlags::P) != 0))
			{
				if ((me->ghostTime == 0) && (g_Map[y - 1][x + 0x12].item == ItemType::IceTrail))
				{
					bVar1 = false;
				}
				else if ((x - 1 == rivalX) && (y == rivalY))
				{
					g_RivalsColliding = true;
					bVar1 = false;
				}
				else if (((g_Map[y - 1][x + 0x12].tile == 3) || (g_Map[y - 1][x + 0x12].tile == 10)) || (g_Map[y - 1][x + 0x12].tile == 0xd))
				{
					bVar1 = false;
				}
				else
				{
					bVar1 = true;
				}
			}
			else
			{
				bVar1 = false;
			}
		}
		else
		{
			bVar1 = false;
		}
	}
	else if (dir == Direction::North)
	{
		if (y == 0)
		{
			bVar1 = false;
		}
		else if ((me->ghostTime == 0) && ((g_Map[y - 1][x].flags & TileFlags::WallH) != 0))
		{
			bVar1 = false;
		}
		else if (((me->flags & GuyFlags::Player) == 0) || ((g_Map[y - 1][x].flags & TileFlags::P) == 0))
		{
			if ((((me->flags & GuyFlags::Pumpkin) == 0) || (me->hammerTime == 0)) ||
			    ((g_Map[y - 1][x].flags & TileFlags::P) != 0))
			{
				if ((me->ghostTime == 0) && (g_Map[y - 1][x].item == ItemType::IceTrail))
				{
					bVar1 = false;
				}
				else if ((x == rivalX) && (y - 1 == rivalY))
				{
					g_RivalsColliding = true;
					bVar1 = false;
				}
				else if (((g_Map[y - 1][x].tile == 3) || (g_Map[y - 1][x].tile == 10)) || (g_Map[y - 1][x].tile == 0xd))
				{
					bVar1 = false;
				}
				else
				{
					bVar1 = true;
				}
			}
			else
			{
				bVar1 = false;
			}
		}
		else
		{
			bVar1 = false;
		}
	}
	else
	{
		bVar1 = false;
	}
	return bVar1;
}

static void GuyUpdatePlayer(Guy *me)
{
	// TODO
}

static void GuyUpdatePumpkinRetreat(Guy *me)
{
	static const byte anim[11] = {
		10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 0
	};

	bool east;
	bool south;
	bool west;
	bool north;
	byte bVar1;
	int hammerTime;
	byte distanceFromSpawn;

	if (me->anim == 1)
	{
		if (me->direction == Direction::East)
		{
			me->x = me->x + 0x600;
			if (me->destX < me->x)
			{
				me->x = me->destX;
			}
		}
		else if (me->direction == Direction::South)
		{
			me->y = me->y + 0x600;
			if (me->destY < me->y)
			{
				me->y = me->destY;
			}
		}
		else if (me->direction == Direction::West)
		{
			me->x = me->x + -0x600;
			if (me->x < me->destX)
			{
				me->x = me->destX;
			}
		}
		else if ((me->direction == Direction::North) && (me->y = me->y + -0x600, me->y < me->destY))
		{
			me->y = me->destY;
		}
		if ((me->x == me->destX) && (me->y == me->destY))
		{
			me->anim = 0;
			me->gridX = (byte)((uint)((int)(me->x + (me->x >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->gridY = (byte)((uint)((int)(me->y + (me->y >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->flags = me->flags & ~GuyFlags::Player;
			if ((g_Map[me->gridY][me->gridX].flags & TileFlags::PK) != 0)
			{
				me->anim = 3;
				me->frame = 0;
				PlaySound(0xc, 0x4b0);
			}
		}
	}
	if (me->anim == 0)
	{
		east = GuyStartWalking(me->gridX, me->gridY, Direction::East, me);
		south = GuyStartWalking(me->gridX, me->gridY, Direction::South, me);
		west = GuyStartWalking(me->gridX, me->gridY, Direction::West, me);
		north = GuyStartWalking(me->gridX, me->gridY, Direction::North, me);
		distanceFromSpawn = 0xff;
		if ((east) && (g_Map[me->gridY][me->gridX + 1].distanceFromPumpkinSpawn != 0xff))
		{
			distanceFromSpawn = g_Map[me->gridY][me->gridX + 1].distanceFromPumpkinSpawn;
			me->direction = Direction::East;
		}
		if ((south) && (g_Map[me->gridY + 1][me->gridX].distanceFromPumpkinSpawn < distanceFromSpawn))
		{
			distanceFromSpawn = g_Map[me->gridY + 1][me->gridX].distanceFromPumpkinSpawn;
			me->direction = Direction::South;
		}
		if ((west) &&
		    (g_Map[me->gridY - 1][me->gridX + 0x12].distanceFromPumpkinSpawn < distanceFromSpawn))
		{
			distanceFromSpawn = g_Map[me->gridY - 1][me->gridX + 0x12].distanceFromPumpkinSpawn;
			me->direction = Direction::West;
		}
		if ((north) && (g_Map[me->gridY - 1][me->gridX].distanceFromPumpkinSpawn < distanceFromSpawn))
		{
			me->direction = Direction::North;
		}
		me->anim = 1;
		me->destX = me->x + g_DirectionX[byte(me->direction)] * 0x2000;
		me->destY = me->y + g_DirectionY[byte(me->direction)] * 0x2000;
	}
	if ((me->anim == 3) && (me->frame = me->frame + 1, 10 < me->frame))
	{
		me->flags = me->flags & ~GuyFlags::Retreating;
		if (g_MapNum < 0x14)
		{
			hammerTime = (uint)g_MapNum * -0xf + 0xb4;
		}
		else
		{
			hammerTime = -0x78;
		}
		if (hammerTime < 1)
		{
			hammerTime = 1;
		}
		me->hammerTime = (short)hammerTime;
		me->frame = 0;
		me->direction = Direction::South;
		me->spr = g_GuySprites[int(me->type)]->GetSprite((uint)me->direction * 0xd);
	}
	else
	{
		bVar1 = byte(me->direction) * 13;
		if (me->anim == 3)
		{
			me->spr = g_GuySprites[int(me->type)]->GetSprite((uint)anim[me->frame] + (uint)bVar1);
		}
		else
		{
			me->spr = g_GuySprites[int(me->type)]->GetSprite(bVar1 + 10);
		}
	}
}

static void GuyUpdatePumpkin(Guy *me)
{
	// TODO
}

static void TitleUpdateGuyPumpkin(Guy *me)
{
	static const byte smooveChucklesAnim[29] = {
		0xb, 0xb, 0xc, 0xc, 0xb, 0xb, 0, 0,
		0xb, 0xb, 0xc, 0xc, 0xb, 0xb, 0, 0,
		0xb, 0xb, 0xc, 0xc, 0xb, 0xb, 0, 0,
		0xb, 0xb, 0xc, 0xc, 0xb
	};
	static const byte peteHelgaAnim[29] = {
		0xb, 0xb, 0, 0, 0xc, 0xc, 0, 0,
		0xb, 0xb, 0, 0, 0xc, 0xc, 0, 0,
		0xb, 0xb, 0, 0, 0xc, 0xc, 0, 0,
		0xb, 0xb, 0, 0, 0
	};
	int speed;

	if (me->anim == 1)
	{
		switch (me->type)
		{
		case GuyType::Smoove:
			speed = 0x200;
			break;
		case GuyType::Chuckles:
			speed = 0x300;
			break;
		case GuyType::Helga:
			speed = 0x300;
			break;
		case GuyType::Pete:
			speed = 0x200;
		}
		me->frame = me->frame + 1;
		if (6 < me->frame)
		{
			me->frame = me->frame - 7;
		}
		if (me->direction == Direction::East)
		{
			me->x = me->x + speed;
			if (me->destX < me->x)
			{
				me->x = me->destX;
			}
		}
		else if ((me->direction == Direction::West) && (me->x = me->x - speed, me->x < me->destX))
		{
			me->x = me->destX;
		}
		if (me->x == me->destX)
		{
			me->anim = 3;
			me->frame = 0;
			me->direction = Direction::South;
		}
	}
	if (me->anim == 3)
	{
		me->frame = me->frame + 1;
		if (29 < me->frame)
		{
			me->frame = 0;
		}
		me->direction = Direction::South;
	}
	if (me->anim == 1)
	{
		me->spr = g_GuySprites[int(me->type)]->GetSprite(me->frame + int(me->direction) * 13);
	}
	else if (me->anim == 3)
	{
		if ((me->type == GuyType::Smoove) || (me->type == GuyType::Chuckles))
		{
			me->spr = g_GuySprites[int(me->type)]->GetSprite(smooveChucklesAnim[me->frame] + 0xd);
		}
		else
		{
			me->spr = g_GuySprites[int(me->type)]->GetSprite(peteHelgaAnim[me->frame] + 0xd);
		}
	}
}

static void TitleUpdateGuyPlayer(Guy *me)
{
	static const byte walkAnim[11] = {
		1, 2, 3, 2, 1, 0, 4, 5, 6, 5, 4
	};
	static const byte jumpAnim[11] = {
		7, 7, 8, 8, 9, 8, 8, 7, 7, 0, 0
	};

	if (me->anim == 1)
	{
		me->frame = me->frame + 1;
		if (0xb < me->frame)
		{
			me->frame = me->frame - 0xc;
		}
		if (me->direction == Direction::East)
		{
			me->x = me->x + 0x400;
			if (me->destX < me->x)
			{
				me->x = me->destX;
			}
		}
		else if ((me->direction == Direction::West) && (me->x = me->x + -0x400, me->x < me->destX))
		{
			me->x = me->destX;
		}
		if (me->x == me->destX)
		{
			me->anim = 3;
			me->frame = 0;
		}
	}
	if (me->anim == 3)
	{
		me->frame = me->frame + 1;
		if (12 < me->frame)
		{
			me->frame = 0;
		}
		me->direction = Direction::South;
	}
	if (me->anim == 1)
	{
		me->spr = g_GuySprites[int(me->type)]->GetSprite(walkAnim[me->frame] + int(me->direction) * 19);
	}
	else if (me->anim == 3)
	{
		me->spr = g_GuySprites[int(me->type)]->GetSprite(jumpAnim[me->frame] + int(me->direction) * 19);
	}
}

void GuyUpdate(Guy *me)
{
	switch (me->type)
	{
	case GuyType::Bouapha:
	case GuyType::Bouaphetta:
		GuyUpdatePlayer(me);
		break;
	case GuyType::Smoove:
	case GuyType::Chuckles:
	case GuyType::Helga:
	case GuyType::Pete:
		if ((me->flags & GuyFlags::Retreating) == 0)
		{
			GuyUpdatePumpkin(me);
		}
		else
		{
			GuyUpdatePumpkinRetreat(me);
		}
	}
	return;
}

void GuysUpdate()
{
	int i;

	if (g_SnowTimer != 0)
	{
		ParticleAddSnow(10);
		g_SnowTimer = g_SnowTimer - 1;
	}
	for (i = 0; i < 0x10; i = i + 1)
	{
		if (g_Guys[i].evilEyeTime != 0)
		{
			ShootEvilEye((uint)g_Guys[i].gridX, (uint)g_Guys[i].gridY, g_Guys[i].direction);
		}
	}
	for (i = 0; i < 0x10; i = i + 1)
	{
		if (g_Guys[i].type != GuyType::None)
		{
			GuyUpdate(g_Guys + i);
		}
	}
	return;
}

void TitleUpdateGuys(void)
{
	int i;

	for (i = 0; i < 0x10; i = i + 1)
	{
		if ((g_Guys[i].type == GuyType::Bouapha) || (g_Guys[i].type == GuyType::Bouaphetta))
		{
			TitleUpdateGuyPlayer(g_Guys + i);
		}
		if ((((g_Guys[i].type == GuyType::Smoove) || (g_Guys[i].type == GuyType::Chuckles)) ||
		     (g_Guys[i].type == GuyType::Pete)) ||
		    (g_Guys[i].type == GuyType::Helga))
		{
			TitleUpdateGuyPumpkin(g_Guys + i);
		}
	}
}

void TitleInitGuys()
{
	int i;

	GuysInit();
	for (i = 0; i < 6; i = i + 1)
	{
		g_Guys[i].invincibleTime = 0;
		g_Guys[i].anim = 1;
		g_Guys[i].frame = 0;
		g_Guys[i].sleepTime = 0;
		g_Guys[i].speedTime = 0;
		g_Guys[i].ghostTime = 0;
		g_Guys[i].fireTime = 0;
		g_Guys[i].iceTime = 0;
		g_Guys[i].evilEyeTime = 0;
	}
	g_Guys[0].type = GuyType::Bouapha;
	g_Guys[0].x = -0x7400;
	g_Guys[0].y = 0x7600;
	g_Guys[0].direction = Direction::East;
	g_Guys[0].destX = 0x4000;
	g_Guys[1].type = GuyType::Bouaphetta;
	g_Guys[1].x = 0x2d400;
	g_Guys[1].y = 0x7600;
	g_Guys[1].direction = Direction::West;
	g_Guys[1].destX = 0x22000;
	g_Guys[2].type = GuyType::Smoove;
	g_Guys[2].x = -0x7400;
	g_Guys[2].y = 0x9e00;
	g_Guys[2].direction = Direction::East;
	g_Guys[2].destX = 0x6800;
	g_Guys[3].type = GuyType::Chuckles;
	g_Guys[3].x = -0x7400;
	g_Guys[3].y = 0x9e00;
	g_Guys[3].direction = Direction::East;
	g_Guys[3].destX = 0xf400;
	g_Guys[4].type = GuyType::Helga;
	g_Guys[4].x = 0x2d400;
	g_Guys[4].y = 0x9e00;
	g_Guys[4].direction = Direction::West;
	g_Guys[4].destX = 0x16c00;
	g_Guys[5].type = GuyType::Pete;
	g_Guys[5].x = 0x2d400;
	g_Guys[5].y = 0x9e00;
	g_Guys[5].direction = Direction::West;
	g_Guys[5].destX = 0x1f800;
	TitleUpdateGuys();
}

void CongratsInitGuys()
{
	int i;

	GuysInit();
	for (i = 0; i < 2; i = i + 1)
	{
		g_Guys[i].invincibleTime = 0;
		g_Guys[i].anim = 1;
		g_Guys[i].frame = 0;
		g_Guys[i].sleepTime = 0;
		g_Guys[i].speedTime = 0;
		g_Guys[i].ghostTime = 0;
		g_Guys[i].fireTime = 0;
		g_Guys[i].iceTime = 0;
		g_Guys[i].evilEyeTime = 0;
	}
	g_Guys[0].type = GuyType::Bouapha;
	g_Guys[0].x = 0x6800;
	g_Guys[0].y = 0x13e00;
	g_Guys[0].direction = Direction::East;
	g_Guys[0].destX = 0x6800;
	g_Guys[1].type = GuyType::Bouaphetta;
	g_Guys[1].x = 0x1f800;
	g_Guys[1].y = 0x13e00;
	g_Guys[1].direction = Direction::West;
	g_Guys[1].destX = 0x1f800;
	TitleUpdateGuys();
}

static void GuyRender(Guy *me)
{
	if ((me->invincibleTime & 1) == 0)
	{
		if (me->anim == 5)
		{
			if (me->frame < 9)
			{
				DisplayListAdd(me->spr, (me->x >> 8) + 0x10, (me->y >> 8) + 0x20, 0, (me->y >> 8) + 0x20, me->frame * 3, DisplayEffect::Bright);
			}
			else
			{
				DisplayListAdd(me->spr, (me->x >> 8) + 0x10, (me->y >> 8) + 0x20, 0, (me->y >> 8) + 0x20, (18 - me->frame) * 3, DisplayEffect::Bright);
			}
		}
		else if (me->ghostTime != 0)
		{
			DisplayListAdd(me->spr, (me->x >> 8) + 0x10, (me->y >> 8) + 0x20, 0, (me->y >> 8) + 0x20, 0, DisplayEffect::Ghost);
		}
		else if (me->evilEyeTime != 0 || me->sleepTime != 0)
		{
			DisplayListAdd(me->spr, (me->x >> 8) + 0x10, (me->y >> 8) + 0x20, 0, (me->y >> 8) + 0x20, 0, DisplayEffect::Grey);
		}
		else if (me->fireTime != 0)
		{
			DisplayListAdd(me->spr, (me->x >> 8) + 0x10, (me->y >> 8) + 0x20, 0, (me->y >> 8) + 0x20, MGL_random(16) - 4, DisplayEffect::Red);
		}
		else if (me->iceTime != 0)
		{
			DisplayListAdd(me->spr, (me->x >> 8) + 0x10, (me->y >> 8) + 0x20, 0, (me->y >> 8) + 0x20, 0, DisplayEffect::Blue);
		}
		else
		{
			DisplayListAdd(me->spr, (me->x >> 8) + 0x10, (me->y >> 8) + 0x20, 0, (me->y >> 8) + 0x20, 0, DisplayEffect::Normal);
		}
	}
}

void GuysRender()
{
	for (int i = 0; i < MAX_GUYS; i = i + 1)
	{
		if (g_Guys[i].type != GuyType::None)
		{
			GuyRender(&g_Guys[i]);
		}
	}
}

const sprite_t *GetGuySprite(byte type, byte frame)
{
	return g_GuySprites[type]->GetSprite(frame);
}
