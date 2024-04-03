#include "guy.h"
#include "control.h"
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
	unsigned short uVar1;
	bool bVar2;
	unsigned int uVar3;
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
	g_Guys[i].x = ((unsigned int)x * 0x20 + 0x10) * 0x100;
	g_Guys[i].y = ((unsigned int)y * 0x20 + 0x10) * 0x100;
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
			uVar1 = (unsigned short)g_MapNum;
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

void GuyScarePumpkins()
{
	int i;

	for (i = 0; i < 0x10; i = i + 1)
	{
		if (((g_Guys[i].type != GuyType::None) && ((g_Guys[i].flags & GuyFlags::Pumpkin) != 0)) &&
		    ((g_Guys[i].flags & GuyFlags::Retreating) == 0))
		{
			switch (g_Guys[i].direction)
			{
			case Direction::East:
				g_Guys[i].destX = g_Guys[i].destX + -0x2000;
				g_Guys[i].direction = Direction::West;
				break;
			case Direction::South:
				g_Guys[i].destY = g_Guys[i].destY + -0x2000;
				g_Guys[i].direction = Direction::North;
				break;
			case Direction::West:
				g_Guys[i].destX = g_Guys[i].destX + 0x2000;
				g_Guys[i].direction = Direction::East;
				break;
			case Direction::North:
				g_Guys[i].destY = g_Guys[i].destY + 0x2000;
				g_Guys[i].direction = Direction::South;
			}
		}
	}
	return;
}

void GuysPutToSleep()
{
	int i;

	for (i = 0; i < 0x10; i = i + 1)
	{
		if ((g_Guys[i].flags & GuyFlags::Pumpkin) != 0)
		{
			g_Guys[i].sleepTime = 0x96;
		}
	}
	return;
}

void GetItem(Guy *me)
{
	ItemType item;
	byte x;
	byte y;

	x = me->gridX;
	y = me->gridY;
	item = g_Map[y][x].item;
	if ((ItemType::HammerUp < item) && (item < ItemType::FireTrail))
	{
		ErasePowerup();
	}
	switch (item)
	{
	case ItemType::None:
	case ItemType::FireTrail:
	case ItemType::IceTrail:
		PlaySound(1, 100);
		break;
	case ItemType::Candle:
		g_Map[y][x].flags = g_Map[y][x].flags & ~TileFlags::Candle;
		PlaySound(0, 2000);
		g_Map[y][x].item = ItemType::None;
		g_Player[me->player - 1].candles = g_Player[me->player - 1].candles + 1;
		break;
	case ItemType::HammerUp:
	case ItemType::HammerUpRandom:
		g_Map[y][x].flags = g_Map[y][x].flags & ~TileFlags::HammerUp;
		PlaySound(4, 2000);
		if (me->hammerTime == 0)
		{
			g_PlayersWithHammers = g_PlayersWithHammers + 1;
		}
		if (g_MapNum < 0x14)
		{
			me->hammerTime = (unsigned short)g_MapNum * -10 + 300;
		}
		else
		{
			me->hammerTime = 100;
		}
		g_Map[y][x].item = ItemType::None;
		GuyScarePumpkins();
		break;
	case ItemType::Points100:
		PlayerGetPoints(me->player - 1, 100);
		PlaySound(0x1b, 1000);
		break;
	case ItemType::Points250:
		PlayerGetPoints(me->player - 1, 200);
		PlaySound(0x1b, 1000);
		break;
	case ItemType::Points500:
		PlayerGetPoints(me->player - 1, 500);
		PlaySound(0x1b, 1000);
		break;
	case ItemType::Points1000:
		PlayerGetPoints(me->player - 1, 1000);
		PlaySound(0x1b, 1000);
		break;
	case ItemType::SpeedyBoots:
		me->speedTime = me->speedTime + 150;
		PlaySound(0x14, 0x5dc);
		break;
	case ItemType::Ghost:
		me->ghostTime = me->ghostTime + 150;
		PlaySound(0x1c, 1000);
		break;
	case ItemType::IceCream:
		g_SnowTimer = 150;
		ParticleAddSnow(100);
		PlaySound(22, 0x5dc);
		break;
	case ItemType::Zzz:
		GuysPutToSleep();
		PlaySound(21, 0x5dc);
		break;
	case ItemType::Shield:
		me->invincibleTime = me->invincibleTime + 90;
		PlaySound(0x19, 1000);
		break;
	case ItemType::OneUp:
		g_Player[me->player - 1].lives = g_Player[me->player - 1].lives + 1;
		PlaySound(0xf, 1000);
		break;
	case ItemType::FirePower:
		me->fireTime = me->fireTime + 0x96;
		PlaySound(0x17, 0x5dc);
		break;
	case ItemType::IcePower:
		me->iceTime = me->iceTime + 0xd2;
		PlaySound(0x1a, 1000);
		break;
	case ItemType::EvilEye:
		me->evilEyeTime = me->evilEyeTime + 300;
		PlaySound(0x18, 0x5dc);
		break;
	case ItemType::BigCandle:
		g_Player[me->player - 1].candles = g_Player[me->player - 1].candles + 5;
		PlaySound(0x1d, 0x5dc);
	}
	switch (item)
	{
	case ItemType::None:
	case ItemType::Points100:
	case ItemType::Points250:
	case ItemType::Points500:
	case ItemType::Points1000:
	case ItemType::FireTrail:
	case ItemType::IceTrail:
		break;
	case ItemType::Candle:
		PlayerGetPoints(me->player - 1, 5);
		break;
	case ItemType::HammerUp:
		PlayerGetPoints(me->player - 1, 250);
		break;
	default:
		PlayerGetPoints(me->player - 1, 50);
	}
	return;
}

bool GuyStartWalking(byte x, byte y, Direction dir, Guy *me)
{
	bool bVar1;
	unsigned int rivalY;
	unsigned int rivalX;
	int i;

	rivalX = 200;
	rivalY = 200;
	if ((me->type == GuyType::Bouapha) || (me->type == GuyType::Bouaphetta))
	{
		for (i = 0; i < 0x10; i = i + 1)
		{
			if (byte(g_Guys[i].type) == 3 - byte(me->type))
			{
				rivalX = (unsigned int)g_Guys[i].gridX;
				rivalY = (unsigned int)g_Guys[i].gridY;
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

void GuyGetHurt(Guy *me, Guy *enemy, bool sound)
{
	me->anim = 4;
	me->frame = 0;
	if (sound)
	{
		PlaySound(byte(me->type) + 0xc, 0x898);
	}
	if (g_GameMode == GameMode::SiblingRivalry)
	{
		if (g_Player[me->player - 1].candles != 0)
		{
			g_Player[me->player - 1].candles = g_Player[me->player - 1].candles - 1;
		}
		if ((enemy == nullptr) || ((unsigned int)enemy->type == 3 - byte(me->type)))
		{
			g_Player[1 - (me->player - 1)].candles = g_Player[1 - (me->player - 1)].candles + 1;
		}
	}
	else if (g_IsTimeAttack == false)
	{
		if (PlayerLoseLife(0) && (g_LevelWon == 0))
		{
			g_LevelWon = 0x78;
			g_EndAnim[0] = 4;
			g_EndAnim[1] = 4;
			g_EndAnimPumpkins = 3;
		}
	}
	else
	{
		g_RecordTimeFrames = g_RecordTimeFrames + 300;
	}
}

Guy *FindGuy1(Guy *me)
{
	int i;

	if ((g_LevelWon == 0) && ((me->invincibleTime == 0 || (me->hammerTime != 0))))
	{
		for (i = 0; i < 0x10; i = i + 1)
		{
			if ((((g_Guys[i].type != GuyType::None) && (g_Guys + i != me)) &&
			     ((g_Guys[i].flags & GuyFlags::Retreating) == 0)) &&
			    ((g_Guys[i].anim != 4 && (g_Guys[i].invincibleTime == 0))))
			{
				if (((me->x + -0xc00 < g_Guys[i].x + 0xc00) &&
				     ((me->y + -0xc00 < g_Guys[i].y + 0xc00 && (g_Guys[i].x + -0xc00 < me->x + 0xc00)))) &&
				    (g_Guys[i].y + -0xc00 < me->y + 0xc00))
				{
					return g_Guys + i;
				}
			}
		}
	}
	return (Guy *)0x0;
}

Guy *FindGuy2(Guy *me)
{
	GuyType GVar1;
	int i;

	if (g_LevelWon == 0)
	{
		if (me->type == GuyType::Bouapha)
		{
			GVar1 = GuyType::Bouaphetta;
		}
		else
		{
			GVar1 = GuyType::Bouapha;
		}
		for (i = 0; i < 0x10; i = i + 1)
		{
			if (g_Guys[i].type == GVar1)
			{
				if ((((me->x + -0xf00 < g_Guys[i].x + 0xf00) && (me->y + -0xf00 < g_Guys[i].y + 0xf00)) &&
				     (g_Guys[i].x + -0xf00 < me->x + 0xf00)) &&
				    (g_Guys[i].y + -0xf00 < me->y + 0xf00))
				{
					return g_Guys + i;
				}
			}
		}
	}
	return (Guy *)0x0;
}

void GuyRespawn(Guy *me)
{
	unsigned int uVar1;
	unsigned int uVar2;
	byte y;
	byte x;
	Guy *enemy;

	do
	{
		do
		{
			uVar1 = MGL_random(0x13);
			uVar1 = uVar1 & 0xffff;
			uVar2 = MGL_random(0xe);
			uVar2 = uVar2 & 0xffff;
		} while ((g_Map[uVar2][uVar1].flags & TileFlags::TP) == 0);
	} while ((uVar1 == me->gridX) && (uVar2 == me->gridY));
	x = (byte)uVar1;
	me->gridX = x;
	y = (byte)uVar2;
	me->gridY = y;
	me->x = ((unsigned int)me->gridX * 0x20 + 0x10) * 0x100;
	me->y = ((unsigned int)me->gridY * 0x20 + 0x10) * 0x100;
	ParticleAddRespawn(me->x, me->y);
	enemy = FindGuy1(me);
	while (enemy != (Guy *)0x0)
	{
		switch (enemy->type)
		{
		case GuyType::Bouapha:
		case GuyType::Bouaphetta:
			GuyGetHurt(enemy, me, false);
			PlaySound(5, 2000);
			break;
		case GuyType::Smoove:
		case GuyType::Chuckles:
		case GuyType::Helga:
		case GuyType::Pete:
			enemy->anim = 4;
			enemy->frame = 0;
			if ((me->type == GuyType::Bouapha) || (me->type == GuyType::Bouaphetta))
			{
				PlayerGetPoints(me->player - 1, 100);
			}
			PlaySound(5, 2000);
		}
		enemy = FindGuy1(me);
	}
}

static void GuyUpdatePlayer(Guy *me)
{
	static const byte anim1[12] = {
		1,
		2,
		3,
		2,
		1,
		0,
		4,
		5,
		6,
		5,
		4,
		0,
	};
	static const byte anim3[20] = {
		7,
		8,
		9,
		8,
		7,
		0,
		0,
		0,
		7,
		8,
		9,
		8,
		7,
		8,
		9,
		8,
		7,
		0,
		0,
		0,
	};

	bool bVar1;
	byte bVar2;
	unsigned int uVar3;
	Guy *pGVar4;
	int iVar5;
	int iVar6;
	sprite_t *spr;
	bool bVar7;
	int speed;
	Guy *local_1c;
	byte local_8;

	if ((g_LevelWon != 0) && (me->anim != g_EndAnim[me->player - 1]))
	{
		me->anim = g_EndAnim[me->player - 1];
		me->frame = 0;
		me->direction = Direction::South;
	}
	if (((me->hammerTime != 0) && (me->anim != 2)) &&
	    (me->hammerTime = me->hammerTime + -1, me->hammerTime == 0))
	{
		g_PlayersWithHammers = g_PlayersWithHammers - 1;
	}
	if (me->invincibleTime != 0)
	{
		me->invincibleTime = me->invincibleTime - 1;
	}
	if (me->speedTime != 0)
	{
		me->speedTime = me->speedTime - 1;
	}
	if (me->ghostTime != 0)
	{
		me->ghostTime = me->ghostTime + -1;
	}
	if (me->fireTime != 0)
	{
		me->fireTime = me->fireTime + -1;
	}
	if (me->iceTime != 0)
	{
		me->iceTime = me->iceTime + -1;
	}
	if (me->evilEyeTime == 0)
	{
		if (((g_Map[me->gridY][me->gridX].tile == 0x14) && (me->anim != 4)) && (me->invincibleTime == 0))
		{
			GuyGetHurt(me, (Guy *)0x0, true);
			PlaySound(0x10, 2000);
		}
	}
	else
	{
		me->evilEyeTime = me->evilEyeTime + -1;
	}
	if (((g_Map[me->gridY][me->gridX].item == ItemType::FireTrail) && (me->anim != 4)) &&
	    (me->invincibleTime == 0))
	{
		GuyGetHurt(me, (Guy *)0x0, true);
		PlaySound(0x11, 2000);
	}
	g_RivalsColliding = false;
	bVar7 = false;
	local_8 = 0;
	speed = 0x400;
	if (me->speedTime != 0)
	{
		speed = 0x800;
	}
	if (me->anim == 1)
	{
		me->frame = me->frame + 1;
		if (0xb < me->frame)
		{
			me->frame = me->frame - 0xc;
		}
		uVar3 = GetPlayerControls(me->player - 1);
		if (me->direction == Direction::East)
		{
			me->x = me->x + speed;
			pGVar4 = FindGuy2(me);
			bVar7 = pGVar4 != (Guy *)0x0;
			if (bVar7)
			{
				me->x = me->x - speed;
			}
			if (me->destX < me->x)
			{
				me->x = me->destX;
			}
			if ((uVar3 & 4) != 0)
			{
				me->destX = me->destX + -0x2000;
				me->direction = Direction::West;
			}
		}
		else if (me->direction == Direction::South)
		{
			me->y = me->y + speed;
			pGVar4 = FindGuy2(me);
			bVar7 = pGVar4 != (Guy *)0x0;
			if (bVar7)
			{
				me->y = me->y - speed;
			}
			if (me->destY < me->y)
			{
				me->y = me->destY;
			}
			if ((uVar3 & 1) != 0)
			{
				me->destY = me->destY + -0x2000;
				me->direction = Direction::North;
			}
		}
		else if (me->direction == Direction::West)
		{
			me->x = me->x - speed;
			pGVar4 = FindGuy2(me);
			bVar7 = pGVar4 != (Guy *)0x0;
			if (bVar7)
			{
				me->x = me->x + speed;
			}
			if (me->x < me->destX)
			{
				me->x = me->destX;
			}
			if ((uVar3 & 8) != 0)
			{
				me->destX = me->destX + 0x2000;
				me->direction = Direction::East;
			}
		}
		else if (me->direction == Direction::North)
		{
			me->y = me->y - speed;
			pGVar4 = FindGuy2(me);
			bVar7 = pGVar4 != (Guy *)0x0;
			if (bVar7)
			{
				me->y = me->y + speed;
			}
			if (me->y < me->destY)
			{
				me->y = me->destY;
			}
			if ((uVar3 & 2) != 0)
			{
				me->destY = me->destY + 0x2000;
				me->direction = Direction::South;
			}
		}
		if (((int)(me->x + (me->x >> 0x1f & 0x1fU)) >> 0xd != (unsigned int)me->gridX) ||
		    ((int)(me->y + (me->y >> 0x1f & 0x1fU)) >> 0xd != (unsigned int)me->gridY))
		{
			if (me->fireTime == 0)
			{
				if ((me->iceTime != 0) && (g_Map[me->gridY][me->gridX].item == ItemType::None))
				{
					g_Map[me->gridY][me->gridX].item = ItemType::IceTrail;
					g_Map[me->gridY][me->gridX].itemAnim = 0;
				}
			}
			else if (g_Map[me->gridY][me->gridX].item == ItemType::None)
			{
				g_Map[me->gridY][me->gridX].item = ItemType::FireTrail;
				g_Map[me->gridY][me->gridX].itemAnim = 0;
			}
			me->gridX = (byte)((unsigned int)((int)(me->x + (me->x >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->gridY = (byte)((unsigned int)((int)(me->y + (me->y >> 0x1f & 0x1fU)) >> 5) >> 8);
			GetItem(me);
		}
		if ((me->x == me->destX) && (me->y == me->destY))
		{
			me->anim = 0;
			local_8 = me->frame;
			me->frame = 0;
			me->gridX = (byte)((unsigned int)((int)(me->x + (me->x >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->gridY = (byte)((unsigned int)((int)(me->y + (me->y >> 0x1f & 0x1fU)) >> 5) >> 8);
			g_Map[me->gridY][me->gridX].tileAnim = 0;
			if ((g_Map[me->gridY][me->gridX].flags & TileFlags::TP) != 0)
			{
				me->anim = 5;
				me->frame = 0;
				ParticleAddTeleport(me->x, me->y);
				PlaySound(0x13, 2000);
			}
		}
	}
	if (me->anim == 0)
	{
		uVar3 = GetPlayerControls(me->player - 1);
		if ((uVar3 & 1) == 0)
		{
			if ((uVar3 & 2) == 0)
			{
				if ((uVar3 & 4) == 0)
				{
					if ((uVar3 & 8) != 0)
					{
						me->direction = Direction::East;
						bVar1 = GuyStartWalking(me->gridX, me->gridY, me->direction, me);
						if (bVar1)
						{
							me->anim = 1;
							me->frame = local_8;
							me->destX = me->x + 0x2000;
							me->destY = me->y;
						}
					}
				}
				else
				{
					me->direction = Direction::West;
					bVar1 = GuyStartWalking(me->gridX, me->gridY, me->direction, me);
					if (bVar1)
					{
						me->anim = 1;
						me->frame = local_8;
						me->destX = me->x + -0x2000;
						me->destY = me->y;
					}
				}
			}
			else
			{
				me->direction = Direction::South;
				bVar1 = GuyStartWalking(me->gridX, me->gridY, me->direction, me);
				if (bVar1)
				{
					me->anim = 1;
					me->frame = local_8;
					me->destX = me->x;
					me->destY = me->y + 0x2000;
				}
			}
		}
		else
		{
			me->direction = Direction::North;
			bVar1 = GuyStartWalking(me->gridX, me->gridY, me->direction, me);
			if (bVar1)
			{
				me->anim = 1;
				me->frame = local_8;
				me->destX = me->x;
				me->destY = me->y + -0x2000;
			}
		}
	}
	if ((g_GameMode == GameMode::SiblingRivalry) && ((me->anim == 0 || (me->anim == 1))))
	{
		local_1c = (Guy *)0x0;
		for (speed = 0; speed < 0x10; speed = speed + 1)
		{
			if ((unsigned int)g_Guys[speed].type == 3 - byte(me->type))
			{
				local_1c = g_Guys + speed;
			}
		}
		if (((local_1c != (Guy *)0x0) && ((local_1c->anim == 0 || (local_1c->anim == 1)))) &&
		    ((bVar7 || (g_RivalsColliding != false))))
		{
			if ((me->hammerTime != 0) && (local_1c->invincibleTime == 0))
			{
				me->anim = 2;
				me->frame = 0;
				GuyGetHurt(local_1c, me, true);
				PlaySound(byte(me->type) + 5, 2200);
				iVar5 = abs(local_1c->x - me->x);
				iVar6 = abs(local_1c->y - me->y);
				if (iVar5 < iVar6)
				{
					if (local_1c->y < me->y)
					{
						me->direction = Direction::North;
					}
					else if (me->y < local_1c->y)
					{
						me->direction = Direction::South;
					}
				}
				else if (local_1c->x < me->x)
				{
					me->direction = Direction::West;
				}
				else if (me->x < local_1c->x)
				{
					me->direction = Direction::East;
				}
			}
			if ((local_1c->hammerTime != 0) && (me->invincibleTime == 0))
			{
				if (local_1c->anim != 4)
				{
					local_1c->anim = 2;
					local_1c->frame = 0;
				}
				GuyGetHurt(me, local_1c, true);
				PlaySound(byte(local_1c->type) + 5, 0x898);
				iVar5 = abs(local_1c->x - me->x);
				iVar6 = abs(local_1c->y - me->y);
				if (iVar5 < iVar6)
				{
					if (me->y < local_1c->y)
					{
						local_1c->direction = Direction::North;
					}
					else if (local_1c->y < me->y)
					{
						local_1c->direction = Direction::South;
					}
				}
				else if (me->x < local_1c->x)
				{
					local_1c->direction = Direction::West;
				}
				else if (local_1c->x < me->x)
				{
					local_1c->direction = Direction::East;
				}
			}
		}
	}
	if (me->anim == 2)
	{
		me->frame = me->frame + 1;
		if (me->frame < 10)
		{
			if (me->frame == 4)
			{
				PlaySound(5, 0x640);
			}
		}
		else
		{
			me->anim = 0;
			me->frame = 0;
			me->gridX = (byte)((unsigned int)((int)(me->x + (me->x >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->gridY = (byte)((unsigned int)((int)(me->y + (me->y >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->x = ((unsigned int)me->gridX * 0x20 + 0x10) * 0x100;
			me->y = ((unsigned int)me->gridY * 0x20 + 0x10) * 0x100;
			if ((g_Map[me->gridY][me->gridX].flags & TileFlags::TP) != 0)
			{
				me->anim = 5;
				me->frame = 0;
				ParticleAddTeleport(me->x, me->y);
				PlaySound(0x13, 2000);
			}
		}
	}
	if (me->anim == 4)
	{
		me->frame = me->frame + 1;
		if (me->frame < 0x12)
		{
			if ((g_LevelWon != 0) && (9 < me->frame))
			{
				me->frame = 9;
			}
		}
		else
		{
			GetPlayerStart(me->player, &me->x, &me->y);
			me->gridX = (byte)((unsigned int)((int)(me->x + (me->x >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->gridY = (byte)((unsigned int)((int)(me->y + (me->y >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->anim = 0;
			me->frame = 0;
			me->invincibleTime = 0x3c;
			me->hammerTime = 0;
			me->ghostTime = 0;
			me->speedTime = 0;
			me->fireTime = 0;
			me->iceTime = 0;
			me->evilEyeTime = 0;
			pGVar4 = FindGuy2(me);
			if (pGVar4 != (Guy *)0x0)
			{
				GetPlayerStart(3 - me->player, &me->x, &me->y);
				me->gridX = (byte)((unsigned int)((int)(me->x + (me->x >> 0x1f & 0x1fU)) >> 5) >> 8);
				me->gridY = (byte)((unsigned int)((int)(me->y + (me->y >> 0x1f & 0x1fU)) >> 5) >> 8);
			}
		}
	}
	if ((me->anim == 3) && (me->frame = me->frame + 1, 0x13 < me->frame))
	{
		me->frame = 0;
	}
	if (me->anim == 5)
	{
		me->frame = me->frame + 1;
		if (me->frame == 9)
		{
			GuyRespawn(me);
		}
		else if (0x12 < me->frame)
		{
			me->anim = 0;
			me->frame = 0;
		}
	}
	if ((((me->anim != 2) && (me->anim != 4)) && (me->anim != 3)) &&
	    ((pGVar4 = FindGuy1(me), pGVar4 != (Guy *)0x0 && ((pGVar4->flags & GuyFlags::Pumpkin) != 0))))
	{
		if (me->hammerTime == 0)
		{
			GuyGetHurt(me, pGVar4, true);
			pGVar4->anim = 3;
			pGVar4->frame = 0;
			pGVar4->sleepTime = 0;
		}
		else
		{
			me->anim = 2;
			me->frame = 0;
			pGVar4->anim = 4;
			pGVar4->frame = 0;
			pGVar4->sleepTime = 0;
			PlaySound(byte(me->type) + 5, 0x898);
			PlayerGetPoints(me->player - 1, 100);
		}
		iVar5 = abs(pGVar4->x - me->x);
		iVar6 = abs(pGVar4->y - me->y);
		if (iVar5 < iVar6)
		{
			if (pGVar4->y < me->y)
			{
				me->direction = Direction::North;
			}
			else if (me->y < pGVar4->y)
			{
				me->direction = Direction::South;
			}
		}
		else if (pGVar4->x < me->x)
		{
			me->direction = Direction::West;
		}
		else if (me->x < pGVar4->x)
		{
			me->direction = Direction::East;
		}
	}
	if (((unsigned short)me->hammerTime < 0x1f) && ((me->hammerTime == 0 || ((me->hammerTime & 1U) != 0))))
	{
		bVar2 = byte(me->direction) * 0x13;
		switch (me->anim)
		{
		case 0:
			spr = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)bVar2);
			me->spr = spr;
			break;
		case 1:
			spr = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)anim1[me->frame] + (unsigned int)bVar2);
			me->spr = spr;
			if (me->speedTime != 0)
			{
				ParticleAddSpeed(me->type, anim1[me->frame] + bVar2, me->x, me->y);
			}
			break;
		case 2:
			bVar2 = byte(me->direction) * 0x11 + 0x4c;
			spr = g_GuySprites[byte(me->type)]->GetSprite(me->frame + 7 + (unsigned int)bVar2);
			me->spr = spr;
			if (me->speedTime != 0)
			{
				ParticleAddSpeed(me->type, me->frame + 7 + bVar2, me->x, me->y);
			}
			break;
		case 3:
			spr = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)anim3[me->frame] + (unsigned int)bVar2);
			me->spr = spr;
			break;
		case 4:
			spr = g_GuySprites[byte(me->type)]->GetSprite(((int)(unsigned int)me->frame >> 1) + 10 + (unsigned int)bVar2);
			me->spr = spr;
			if (me->speedTime != 0)
			{
				ParticleAddSpeed(me->type, (char)((int)(unsigned int)me->frame >> 1) + 10 + bVar2, me->x, me->y);
			}
			break;
		case 5:
			spr = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)bVar2);
			me->spr = spr;
		}
	}
	else
	{
		bVar2 = byte(me->direction) * 0x11 + 0x4c;
		switch (me->anim)
		{
		case 0:
			spr = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)bVar2);
			me->spr = spr;
			break;
		case 1:
			spr = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)anim1[me->frame] + (unsigned int)bVar2);
			me->spr = spr;
			if (me->speedTime != 0)
			{
				ParticleAddSpeed(me->type, anim1[byte(me->frame)] + bVar2, me->x, me->y);
			}
			break;
		case 2:
			spr = g_GuySprites[byte(me->type)]->GetSprite(me->frame + 7 + (unsigned int)bVar2);
			me->spr = spr;
			if (me->speedTime != 0)
			{
				ParticleAddSpeed(me->type, me->frame + 7 + bVar2, me->x, me->y);
			}
			break;
		case 3:
			spr = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)anim3[me->frame] + (unsigned int)(byte)(byte(me->direction) * 0x13));
			me->spr = spr;
			break;
		case 4:
			bVar2 = byte(me->direction) * 0x13;
			spr = g_GuySprites[byte(me->type)]->GetSprite(((int)(unsigned int)me->frame >> 1) + 10 + (unsigned int)bVar2);
			me->spr = spr;
			if (me->speedTime != 0)
			{
				ParticleAddSpeed(me->type, (char)((int)(unsigned int)me->frame >> 1) + '\n' + bVar2, me->x, me->y);
			}
			break;
		case 5:
			spr = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)bVar2);
			me->spr = spr;
		}
	}
	return;
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
			me->gridX = (byte)((unsigned int)((int)(me->x + (me->x >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->gridY = (byte)((unsigned int)((int)(me->y + (me->y >> 0x1f & 0x1fU)) >> 5) >> 8);
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
			hammerTime = (unsigned int)g_MapNum * -0xf + 0xb4;
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
		me->spr = g_GuySprites[int(me->type)]->GetSprite((unsigned int)me->direction * 0xd);
	}
	else
	{
		bVar1 = byte(me->direction) * 13;
		if (me->anim == 3)
		{
			me->spr = g_GuySprites[int(me->type)]->GetSprite((unsigned int)anim[me->frame] + (unsigned int)bVar1);
		}
		else
		{
			me->spr = g_GuySprites[int(me->type)]->GetSprite(bVar1 + 10);
		}
	}
}

static void GuyUpdatePumpkin(Guy *me)
{
	static const byte anim3a[30] = {
		0xb,
		0xb,
		0xc,
		0xc,
		0xb,
		0xb,
		0,
		0,
		0xb,
		0xb,
		0xc,
		0xc,
		0xb,
		0xb,
		0,
		0,
		0xb,
		0xb,
		0xc,
		0xc,
		0xb,
		0xb,
		0,
		0,
		0xb,
		0xb,
		0xc,
		0xc,
		0xb,
		0xb,
	};
	static const byte anim3b[30] = {
		0xb,
		0xb,
		0,
		0,
		0xc,
		0xc,
		0,
		0,
		0xb,
		0xb,
		0,
		0,
		0xc,
		0xc,
		0,
		0,
		0xb,
		0xb,
		0,
		0,
		0xc,
		0xc,
		0,
		0,
		0xb,
		0xb,
		0,
		0,
		0,
		0,
	};
	static const byte anim4[11] = {
		0,
		0,
		0,
		0,
		0,
		0,
		7,
		8,
		9,
		10,
	};

	unsigned int uVar1;
	int iVar2;
	int iVar3;
	sprite_t *psVar4;
	byte bVar5;
	int local_2c;
	int speed;
	bool canGo[4];
	byte i;
	byte local_c[4];
	byte frame;

	if ((g_LevelWon != 0) && (me->anim != g_EndAnimPumpkins))
	{
		me->anim = g_EndAnimPumpkins;
		me->frame = 0;
	}
	if (me->hammerTime != 0)
	{
		me->hammerTime = me->hammerTime + -1;
	}
	if (me->speedTime != 0)
	{
		me->speedTime = me->speedTime - 1;
	}
	if (me->sleepTime != 0)
	{
		me->sleepTime = me->sleepTime + -1;
		return;
	}
	me->iceTime = (unsigned short)g_SnowTimer;
	if ((g_Map[me->gridY][me->gridX].tile == 0x14) && (me->anim != 4))
	{
		me->anim = 4;
		me->frame = 7;
		PlaySound(0x10, 0x640);
	}
	if ((g_Map[me->gridY][me->gridX].item == ItemType::FireTrail) && (me->anim != 4))
	{
		me->anim = 4;
		me->frame = 7;
		PlaySound(0x11, 2000);
	}
	frame = 0;
	if (me->anim == 1)
	{
		switch (me->type)
		{
		case GuyType::Smoove:
			speed = (unsigned int)g_MapNum * 0x20 + 0x180;
			break;
		case GuyType::Chuckles:
			speed = (unsigned int)g_MapNum * 0x20 + 0x100;
			break;
		case GuyType::Helga:
			speed = (unsigned int)g_MapNum * 0x20 + 0x200;
			break;
		case GuyType::Pete:
			speed = (unsigned int)g_MapNum * 0x20 + 0x300;
		}
		if (g_SnowTimer != 0)
		{
			speed = (int)(speed + (speed >> 0x1f & 3U)) >> 2;
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
		else if (me->direction == Direction::South)
		{
			me->y = me->y + speed;
			if (me->destY < me->y)
			{
				me->y = me->destY;
			}
		}
		else if (me->direction == Direction::West)
		{
			me->x = me->x - speed;
			if (me->x < me->destX)
			{
				me->x = me->destX;
			}
		}
		else if ((me->direction == Direction::North) && (me->y = me->y - speed, me->y < me->destY))
		{
			me->y = me->destY;
		}
		if ((me->x == me->destX) && (me->y == me->destY))
		{
			me->anim = 0;
			frame = me->frame;
			me->frame = 0;
			me->gridX = (byte)((unsigned int)((int)(me->x + (me->x >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->gridY = (byte)((unsigned int)((int)(me->y + (me->y >> 0x1f & 0x1fU)) >> 5) >> 8);
			if ((g_Map[me->gridY][me->gridX].flags & TileFlags::P) == 0)
			{
				me->flags = me->flags | GuyFlags::Player;
			}
			if ((g_Map[me->gridY][me->gridX].flags & TileFlags::TP) != 0)
			{
				me->anim = 5;
				me->frame = 0;
				ParticleAddTeleport(me->x, me->y);
				PlaySound(0x13, 2000);
			}
		}
	}
	if (me->anim == 0)
	{
		canGo[0] = GuyStartWalking(me->gridX, me->gridY, Direction::East, me);
		canGo[1] = GuyStartWalking(me->gridX, me->gridY, Direction::South, me);
		canGo[2] = GuyStartWalking(me->gridX, me->gridY, Direction::West, me);
		canGo[3] = GuyStartWalking(me->gridX, me->gridY, Direction::North, me);
		i = 0;
		if (((me->hammerTime == 0) && (me->type != GuyType::Pete)) && (g_PlayersWithHammers == 0))
		{
			if ((me->type == GuyType::Helga) || (me->type == GuyType::Smoove))
			{
				bVar5 = canGo[0] + canGo[1] + canGo[2] + canGo[3];
				if (bVar5 == 0)
				{
					return;
				}
				if (1 < bVar5)
				{
					canGo[byte(me->direction) + 2 & 3] = false;
				}
				i = 0xff;
				for (int _j = 0; _j < 4; _j = _j + 1)
				{
					if ((canGo[_j] != false) &&
					    (g_Map[(unsigned int)me->gridY + (int)g_DirectionY[_j]]
					          [(unsigned int)me->gridX + (int)g_DirectionX[_j]]
					              .tileAnim < i))
					{
						me->direction = Direction(_j);
						i = g_Map[(unsigned int)me->gridY + (int)g_DirectionY[_j]]
						         [(unsigned int)me->gridX + (int)g_DirectionX[_j]]
						             .tileAnim;
					}
				}
				if ((i == 0xff) && (me->type == GuyType::Smoove))
				{
					do
					{
						uVar1 = MGL_random(4);
						me->direction = (Direction)uVar1;
					} while (canGo[byte(me->direction)] == false);
				}
			}
		}
		else
		{
			bVar5 = canGo[0] + canGo[1] + canGo[2] + canGo[3];
			if (bVar5 == 0)
			{
				return;
			}
			if (1 < bVar5)
			{
				canGo[byte(me->direction) + 2 & 3] = false;
			}
			do
			{
				uVar1 = MGL_random(4);
				i = (byte)uVar1;
			} while (canGo[uVar1 & 0xff] == false);
			me->direction = Direction(i);
		}
		if ((((me->type == GuyType::Chuckles) && (me->hammerTime == 0)) && (g_PlayersWithHammers == 0)) || ((me->type == GuyType::Helga && (i == 0xff))))
		{
			bVar5 = canGo[0] + canGo[1] + canGo[2] + canGo[3];
			if (bVar5 == 0)
			{
				return;
			}
			if (1 < bVar5)
			{
				canGo[byte(me->direction) + 2 & 3] = false;
			}
			i = 0xff;
			for (int _j = 0; _j < 0x10; _j = _j + 1)
			{
				if ((g_Guys[_j].type == GuyType::Bouapha) || (g_Guys[_j].type == GuyType::Bouaphetta))
				{
					if (i == 0xff)
					{
						i = _j;
						iVar2 = abs(me->x - g_Guys[_j].x);
						iVar3 = abs(me->y - g_Guys[_j].y);
						local_2c = iVar2 + iVar3;
					}
					else
					{
						iVar2 = abs(me->x - g_Guys[_j].x);
						iVar3 = abs(me->y - g_Guys[_j].y);
						if (iVar2 + iVar3 < local_2c)
						{
							i = _j;
							iVar2 = abs(me->x - g_Guys[_j].x);
							iVar3 = abs(me->y - g_Guys[_j].y);
							local_2c = iVar2 + iVar3;
						}
					}
				}
			}
			if (i == 0xff)
			{
				return;
			}
			iVar2 = abs(g_Guys[i].x - me->x);
			iVar3 = abs(g_Guys[i].y - me->y);
			if (iVar3 < iVar2)
			{
				if (g_Guys[i].x < me->x)
				{
					local_c[2] = 4;
					local_c[0] = 0;
				}
				else
				{
					local_c[0] = 4;
					local_c[2] = 0;
				}
				if (g_Guys[i].y < me->y)
				{
					local_c[3] = 3;
					local_c[1] = 2;
				}
				else
				{
					local_c[1] = 3;
					local_c[3] = 2;
				}
			}
			else
			{
				if (g_Guys[i].y < me->y)
				{
					local_c[3] = 4;
					local_c[1] = 0;
				}
				else
				{
					local_c[1] = 4;
					local_c[3] = 0;
				}
				if (g_Guys[i].x < me->x)
				{
					local_c[2] = 3;
					local_c[0] = 2;
				}
				else
				{
					local_c[0] = 3;
					local_c[2] = 2;
				}
			}
			me->direction = Direction(0xff);
			for (i = 0; i < 4; i = i + 1)
			{
				if ((canGo[i] != false) &&
				    ((me->direction == Direction(0xff) || (local_c[byte(me->direction)] < local_c[i]))))
				{
					me->direction = Direction(i);
				}
			}
		}
		me->anim = 1;
		me->frame = frame;
		me->destX = me->x + g_DirectionX[byte(me->direction)] * 0x2000;
		me->destY = me->y + g_DirectionY[byte(me->direction)] * 0x2000;
	}
	if (me->anim == 3)
	{
		me->frame = me->frame + 1;
		if (me->frame < 0x1e)
		{
			if (((me->frame == 10) && ((me->flags & GuyFlags::Player) != 0)) && (g_LevelWon == 0))
			{
				PlaySound(byte(me->type) + 5, 0x708);
			}
		}
		else
		{
			if (g_LevelWon == 0)
			{
				me->anim = 0;
			}
			me->gridX = (byte)((unsigned int)((int)(me->x + (me->x >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->gridY = (byte)((unsigned int)((int)(me->y + (me->y >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->x = ((unsigned int)me->gridX * 0x20 + 0x10) * 0x100;
			me->y = ((unsigned int)me->gridY * 0x20 + 0x10) * 0x100;
			me->frame = 0;
		}
	}
	if ((me->anim == 4) && (me->frame = me->frame + 1, 9 < me->frame))
	{
		if (g_LevelWon == 0)
		{
			me->anim = 0;
			me->frame = 0;
			me->flags = me->flags | GuyFlags::Retreating;
			me->gridX = (byte)((unsigned int)((int)(me->x + (me->x >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->gridY = (byte)((unsigned int)((int)(me->y + (me->y >> 0x1f & 0x1fU)) >> 5) >> 8);
			me->x = ((unsigned int)me->gridX * 0x20 + 0x10) * 0x100;
			me->y = ((unsigned int)me->gridY * 0x20 + 0x10) * 0x100;
			GuyUpdatePumpkinRetreat(me);
			return;
		}
		me->frame = 9;
	}
	if (me->anim == 5)
	{
		me->frame = me->frame + 1;
		if (me->frame == 9)
		{
			GuyRespawn(me);
		}
		else if (0x12 < me->frame)
		{
			me->anim = 0;
			me->frame = 0;
		}
	}
	bVar5 = byte(me->direction) * 13;
	switch (me->anim)
	{
	case 0:
		psVar4 = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)bVar5);
		me->spr = psVar4;
		break;
	case 1:
		psVar4 = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)me->frame + (unsigned int)bVar5);
		me->spr = psVar4;
		break;
	case 3:
		if ((me->type == GuyType::Smoove) || (me->type == GuyType::Chuckles))
		{
			psVar4 = g_GuySprites[byte(me->type)]->GetSprite(anim3a[me->frame] + 0xd);
			me->spr = psVar4;
		}
		else
		{
			psVar4 = g_GuySprites[byte(me->type)]->GetSprite(anim3b[me->frame] + 0xd);
			me->spr = psVar4;
		}
		break;
	case 4:
		psVar4 = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)anim4[me->frame] + (unsigned int)bVar5);
		me->spr = psVar4;
		break;
	case 5:
		psVar4 = g_GuySprites[byte(me->type)]->GetSprite((unsigned int)bVar5);
		me->spr = psVar4;
	}
}

static void TitleUpdateGuyPumpkin(Guy *me)
{
	static const byte smooveChucklesAnim[30] = {
		0xb, 0xb, 0xc, 0xc, 0xb, 0xb, 0, 0,
		0xb, 0xb, 0xc, 0xc, 0xb, 0xb, 0, 0,
		0xb, 0xb, 0xc, 0xc, 0xb, 0xb, 0, 0,
		0xb, 0xb, 0xc, 0xc, 0xb, 0xb
	};
	static const byte peteHelgaAnim[30] = {
		0xb, 0xb, 0, 0, 0xc, 0xc, 0, 0,
		0xb, 0xb, 0, 0, 0xc, 0xc, 0, 0,
		0xb, 0xb, 0, 0, 0xc, 0xc, 0, 0,
		0xb, 0xb, 0, 0, 0, 0
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
	static const byte walkAnim[12] = {
		1, 2, 3, 2, 1, 0, 4, 5, 6, 5, 4, 0
	};
	static const byte jumpAnim[13] = {
		7, 7, 8, 8, 9, 8, 8, 7, 7, 0, 0, 0, 0
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
			ShootEvilEye((unsigned int)g_Guys[i].gridX, (unsigned int)g_Guys[i].gridY, g_Guys[i].direction);
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
			DisplayListAdd(me->spr, (me->x >> 8) + 0x10, (me->y >> 8) + 0x20, 0, (me->y >> 8) + 0x20, 0, DisplayEffect::Glow);
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
