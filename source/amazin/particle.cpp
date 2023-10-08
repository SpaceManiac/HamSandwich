#include "particle.h"
#include "display.h"
#include "guy.h"
#include "jamulspr.h"
#include "jamultypes.h"

enum class ParticleType : byte
{
	None,
	Teleport,
	_2,
	Snow,
	SpeedP1,
	SpeedP2,
	Respawn,
};

struct Particle
{
	ParticleType type;
	int x, y, z;
	int dx, dy, dz;
	int tx, ty, tz;
	byte life, life2;
};

Particle g_Particles[200];

void ParticlesInit()
{
	int i;
	for (i = 0; i < 200; i = i + 1)
	{
		g_Particles[i].type = ParticleType::None;
	}
}

// This doesn't accomplish anything, but it matches the disassembly!
void Dampen(int *value, int amt)
{
/*
	if (value > 0)
	{
		value -= amt;
		if (value < 0)
			value = 0;
	}
	else if (value < 0)
	{
		value += amt;
		if (value > 0)
			value = 0;
	}
*/
}

void Approach(int *val, int target, int incr)
{
	if (*val < target)
	{
		*val = *val + incr;
		if (target < *val)
		{
			*val = target;
		}
	}
	else if (target < *val)
	{
		*val = *val + incr;
		if (*val < target)
		{
			*val = target;
		}
	}
}

void ParticleUpdate(Particle *me)
{
	switch (me->type)
	{
	case ParticleType::Teleport:
		Dampen(&me->dx, 0x80);
		Dampen(&me->dy, 0x80);
		me->dz = me->dz + 0x80;
		me->x = me->x + me->dx;
		me->y = me->y + me->dy;
		me->z = me->z + me->dz;
		if (me->z < 0)
		{
			me->z = 0;
		}
		me->life = me->life + 1;
		if (0x1e < me->life)
		{
			me->type = ParticleType::None;
		}
		break;
	case ParticleType::Snow:
		me->x = me->x + me->dx;
		me->y = me->y + me->dy;
		me->life = me->life - 1;
		if (me->life == 0)
		{
			me->type = ParticleType::None;
		}
		break;
	case ParticleType::SpeedP1:
	case ParticleType::SpeedP2:
		me->life2 = me->life2 - 1;
		if (me->life2 == 0)
		{
			me->type = ParticleType::None;
		}
		break;
	case ParticleType::Respawn:
		if (me->x < me->tx)
		{
			me->dx = me->dx + 0x40;
		}
		else
		{
			me->dx = me->dx + -0x40;
		}
		if (me->y < me->ty)
		{
			me->dy = me->dy + 0x40;
		}
		else
		{
			me->dy = me->dy + -0x40;
		}
		if (me->z < me->tz)
		{
			me->dz = me->dz + 0x40;
		}
		else
		{
			me->dz = me->dz + -0x40;
		}
		Approach(&me->x, me->tx, me->dx);
		Approach(&me->y, me->ty, me->dy);
		Approach(&me->z, me->tz, me->dz);
		if (me->z < 0)
		{
			me->z = 0;
		}
		me->life = me->life + 1;
		if (0x14 < me->life)
		{
			me->type = ParticleType::None;
		}
	}
}

void ParticlesUpdate()
{
	int i;

	for (i = 0; i < 200; i = i + 1)
	{
		if (g_Particles[i].type != ParticleType::None)
		{
			ParticleUpdate(g_Particles + i);
		}
	}
}

void ParticleDrawGlow(int x, int y, byte bright)
{
	byte bVar1;
	unsigned int local_8;
	byte under;

	if ((((0 < x) && (x < mgl->GetWidth() + -1)) && (0 < y)) && ((y < mgl->GetHeight() + -1 && (bright != 0))))
	{
		if (bright == 1)
		{
			bVar1 = 0;
		}
		else
		{
			bVar1 = bright - 2;
		}
		under = mgl->GetScreen()[x + y * mgl->GetWidth()];
		local_8 = (unsigned int)under;
		if ((local_8 + bright & 0xffffffe0) == (local_8 & 0xe0))
		{
			mgl->GetScreen()[x + y * mgl->GetWidth()] = under + bright;
		}
		else
		{
			mgl->GetScreen()[x + y * mgl->GetWidth()] = (under & 0xe0) + 0x1f;
		}
		under = mgl->GetScreen()[x + y * mgl->GetWidth() + -1];
		local_8 = (unsigned int)under;
		if ((local_8 + bVar1 & 0xffffffe0) == (local_8 & 0xe0))
		{
			mgl->GetScreen()[x + y * mgl->GetWidth() + -1] = under + bVar1;
		}
		else
		{
			mgl->GetScreen()[x + y * mgl->GetWidth() + -1] = (under & 0xe0) + 0x1f;
		}
		under = mgl->GetScreen()[x + y * mgl->GetWidth() + 1];
		if (((unsigned int)under + (unsigned int)bVar1 & 0xffffffe0) == (unsigned int)(under & 0xe0))
		{
			mgl->GetScreen()[x + y * mgl->GetWidth() + 1] = under + bVar1;
		}
		else
		{
			mgl->GetScreen()[x + y * mgl->GetWidth() + 1] = (under & 0xe0) + 0x1f;
		}
		under = mgl->GetScreen()[x + (y + -1) * mgl->GetWidth()];
		if (((unsigned int)under + (unsigned int)bVar1 & 0xffffffe0) == (unsigned int)(under & 0xe0))
		{
			mgl->GetScreen()[x + (y + -1) * mgl->GetWidth()] = under + bVar1;
		}
		else
		{
			mgl->GetScreen()[x + (y + -1) * mgl->GetWidth()] = (under & 0xe0) + 0x1f;
		}
		under = mgl->GetScreen()[x + (y + 1) * mgl->GetWidth()];
		if (((unsigned int)under + (unsigned int)bVar1 & 0xffffffe0) == (unsigned int)(under & 0xe0))
		{
			mgl->GetScreen()[x + (y + 1) * mgl->GetWidth()] = under + bVar1;
		}
		else
		{
			mgl->GetScreen()[x + (y + 1) * mgl->GetWidth()] = (under & 0xe0) + 0x1f;
		}
	}
}

void ParticleRender(Particle *me)
{
	switch (me->type)
	{
	case ParticleType::Teleport:
		ParticleDrawGlow(me->x >> 8, me->y - me->z >> 8, 0x1e - me->life);
		break;
	case ParticleType::Snow:
		ParticleDrawGlow(me->x >> 8, me->y >> 8, 0x12);
		break;
	case ParticleType::SpeedP1:
	case ParticleType::SpeedP2:
		GetGuySprite(byte(me->type) - 3, me->life)->DrawGhost(me->x >> 8, me->y >> 8, mgl, -8);
		break;
	case ParticleType::Respawn:
		ParticleDrawGlow(me->x >> 8, me->y - me->z >> 8, me->life);
	}
}

void ParticlesRender()
{
	int i;

	for (i = 0; i < 200; i = i + 1)
	{
		if (g_Particles[i].type != ParticleType::None)
		{
			ParticleRender(g_Particles + i);
		}
	}
}

void ParticleAddTeleport(int x, int y)
{
	unsigned int uVar1;
	int local_c;
	int i;

	local_c = 0x32;
	i = 0;
	do
	{
		if (199 < i)
		{
			return;
		}
		if (g_Particles[i].type == ParticleType::None)
		{
			g_Particles[i].type = ParticleType::Teleport;
			g_Particles[i].x = x + 0x1000;
			g_Particles[i].y = y + 0x2000;
			uVar1 = MGL_random(0x100);
			g_Particles[i].z = uVar1 & 0xffff;
			uVar1 = MGL_random(0x400);
			g_Particles[i].dx = (uVar1 & 0xffff) - 0x200;
			uVar1 = MGL_random(0x400);
			g_Particles[i].dy = (uVar1 & 0xffff) - 0x200;
			uVar1 = MGL_random(0x100);
			g_Particles[i].dz = -(uVar1 & 0xffff);
			g_Particles[i].life = 0;
			local_c = local_c + -1;
			if (local_c == 0)
			{
				return;
			}
		}
		i = i + 1;
	} while (true);
}

void ParticleAddRespawn(int x, int y)
{
	unsigned int uVar1;
	int local_c;
	int i;

	local_c = 0x32;
	i = 0;
	do
	{
		if (199 < i)
		{
			return;
		}
		if (g_Particles[i].type == ParticleType::None)
		{
			g_Particles[i].type = ParticleType::Respawn;
			uVar1 = MGL_random(0x5000);
			g_Particles[i].x = x + -0x1800 + (uVar1 & 0xffff);
			uVar1 = MGL_random(0x5000);
			g_Particles[i].y = y + -0x800 + (uVar1 & 0xffff);
			uVar1 = MGL_random(0x1e00);
			g_Particles[i].z = (uVar1 & 0xffff) + 0x1400;
			g_Particles[i].dx = 0;
			g_Particles[i].dy = 0;
			g_Particles[i].dz = 0;
			g_Particles[i].tx = x + 0x1000;
			g_Particles[i].ty = y + 0x2000;
			g_Particles[i].tz = 0;
			g_Particles[i].life = 0;
			local_c = local_c + -1;
			if (local_c == 0)
			{
				return;
			}
		}
		i = i + 1;
	} while (true);
}

void ParticleAddSpeed(GuyType player, byte life, int x, int y)
{
	int i;

	i = 0;
	while (true)
	{
		if (199 < i)
		{
			return;
		}
		if (g_Particles[i].type == ParticleType::None)
			break;
		i = i + 1;
	}
	g_Particles[i].type = ParticleType(byte(player) + byte(ParticleType::Snow));
	g_Particles[i].life = life;
	g_Particles[i].x = x + 0x1000;
	g_Particles[i].y = y + 0x2000;
	g_Particles[i].z = 0;
	g_Particles[i].dx = 0;
	g_Particles[i].dy = 0;
	g_Particles[i].dz = 0;
	g_Particles[i].life2 = 4;
	return;
}

void ParticleAddSnow(byte amount)
{
	unsigned int uVar1;
	int local_8;

	local_8 = 0;
	do
	{
		if (0xb3 < local_8)
		{
			return;
		}
		if (g_Particles[local_8].type == ParticleType::None)
		{
			g_Particles[local_8].type = ParticleType::Snow;
			uVar1 = MGL_random((unsigned short)mgl->GetWidth());
			g_Particles[local_8].x = (uVar1 & 0xffff) << 8;
			uVar1 = MGL_random((unsigned short)mgl->GetHeight());
			g_Particles[local_8].y = (uVar1 & 0xffff) << 8;
			g_Particles[local_8].z = 0;
			uVar1 = MGL_random(0xa00);
			g_Particles[local_8].dx = -0x100 - (uVar1 & 0xffff);
			uVar1 = MGL_random(0x600);
			g_Particles[local_8].dy = (uVar1 & 0xffff) - 0x100;
			g_Particles[local_8].dz = 0;
			g_Particles[local_8].life = 0x1e;
			amount = amount - 1;
			if (amount == 0)
			{
				return;
			}
		}
		local_8 = local_8 + 1;
	} while (true);
}
