#include "guy.h"
#include "jamulspr.h"
#include "title.h"
#include "display.h"

enum class Direction
{
	East,
	South,
	West,
	North,
};

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
};

constexpr int MAX_GUYS = 16;

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

void TitleInitGuys(void)
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

const sprite_t* GetGuySprite(byte type, byte frame)
{
	return g_GuySprites[type]->GetSprite(frame);
}
