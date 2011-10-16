#include "particle.h"
#include "bullet.h"
#include "monster.h"

Particle **particleList;
int maxParticles;
static int snowCount = 0; // explicit int

Particle::Particle(void)
{
	this->life = 0;
}

Particle::~Particle(void)
{
}

void Particle::Go(byte type, int x, int y, int z, byte angle, byte force)
{
	byte fforce;

	if (force == 0)
		return;

	this->type = type;
	size = 2;
	fforce = force / 4;

	if (fforce == 0)
		fforce = 1;
	this->x = x + MGL_randoml(32 << FIXSHIFT)-(16 << FIXSHIFT);
	this->y = y + MGL_randoml(32 << FIXSHIFT)-(16 << FIXSHIFT);
	this->z = z;
	this->dx = Cosine(angle) * MGL_random(fforce);
	this->dy = Sine(angle) * MGL_random(fforce);
	this->dz = MGL_random(fforce * 2) << FIXSHIFT;
	this->life = MGL_random(force) + 10;
}

void Particle::GoLightning(int x, int y, int x2, int y2)
{
	this->type = PART_LIGHTNING;
	this->x = x;
	this->y = y;
	this->dx = x2;
	this->dy = y2;
	this->life = 4;
	this->color = 64;
	this->size = (abs((x2 - x) >> FIXSHIFT) + abs((y2 - y) >> FIXSHIFT)) / 8;
}

void Particle::GoRandom(byte type, int x, int y, int z, byte force)
{
	this->type = type;
	size = 2;
	if (force == 0)
		return;

	this->x = x + MGL_randoml(32 << FIXSHIFT)-(16 << FIXSHIFT);
	this->y = y + MGL_randoml(32 << FIXSHIFT)-(16 << FIXSHIFT);
	this->z = z;
	this->dx = (MGL_random(force) - force / 2) << FIXSHIFT;
	this->dy = (MGL_random(force) - force / 2) << FIXSHIFT;
	this->dz = MGL_random(force * 2) << FIXSHIFT;
	this->life = MGL_random(force) + 20;
}

void Particle::Update(Map *map)
{
	byte mapx, mapy;
	byte v;
	byte c1;
	char brt;

	if (life > 0)
	{
		if (type != PART_LIGHTNING)
		{
			dz -= FIXAMT;
			x += dx;
			y += dy;
			z += dz;
			if (z < 0)
			{
				z = 0;
				dz = -dz / 2;
			}
		}

		life--;
		switch (type) {
			case PART_COUNTESS:
				dz += FIXAMT; // no gravity
				break;
			case PART_SMOKE:
				dz += FIXAMT; // no gravity
				z += FIXAMT;
				size = (6 - life / 8);
				dx += MGL_random(65535) - FIXAMT / 2;
				dy += MGL_random(65535) - FIXAMT / 2;
				Dampen(&dx, FIXAMT / 8);
				Dampen(&dy, FIXAMT / 8);
				break;
			case PART_STINKY:
				dz += FIXAMT; // no gravity
				z += FIXAMT + FIXAMT / 2;
				size = ((life / 2)&3);
				if (size == 3)
					size = 1;
				dx += MGL_random(65535) - FIXAMT / 2;
				dy += MGL_random(65535) - FIXAMT / 2;
				Dampen(&dx, FIXAMT / 8);
				Dampen(&dy, FIXAMT / 8);
				break;
			case PART_BOOM:
				dz += FIXAMT;
				z += FIXAMT;
				size = 7 - life;
				break;
			case PART_HAMMER:
				v = life;
				if (v > 31 - 8)
					v = 31 - 8;
				color = 128 + v;
				if (life > 20)
					size = 2;
				else if (life < 10)
					size = 0;
				else
					size = 1;
				break;
			case PART_GLASS:
				if (life > 20)
					size = 2;
				else if (life < 10)
					size = 0;
				else
					size = 1;
				break;
			case PART_DIRT:
				v = life;
				if (v > 31 - 8)
					v = 31 - 8;
				color = 64 + v;
				if (life > 20)
					size = 2;
				else if (life < 10)
					size = 0;
				else
					size = 1;
				break;
			case PART_SNOW2:
				v = life * 2;
				if (v > 31)
					v = 31;
				if (v < 31 - 16)
					v = 31 - 16;
				color = v;
				if (life > 20)
					size = 2;
				else if (life < 10)
					size = 0;
				else
					size = 1;
				break;
			case PART_WATER:
				v = life;
				if (v > 31)
					v = 31;
				if (v < 8)
					v = 8;
				color = 96 + v;
				if (life > 20)
					size = 2;
				else if (life < 10)
					size = 0;
				else
					size = 1;
				break;
			case PART_SLIME:
				v = life;
				if (v > 31 - 8)
					v = 31 - 8;
				color = 32 + 4 + v;
				if (life > 10)
					size = 2;
				else if (life < 5)
					size = 0;
				else
					size = 1;
				break;
			case PART_SNOW:
				dx += MGL_random(65535) - FIXAMT / 2;
				dy += MGL_random(65535) - FIXAMT / 2;
				Dampen(&dx, FIXAMT / 8);
				Dampen(&dy, FIXAMT / 8);
				dz += FIXAMT - 256; // not as much gravity as other things
				color = 31;
				if (z == 0)
				{
					dx = 0;
					dy = 0;
					dz = 0;
					if (life < 50)
						size = 1;
					if (life < 25)
						size = 0;
					color = life / 2;
					if (color < 26)
						color = 26;
					if (color > 31)
						color = 31;
					snowCount++;
				}
				else
					life++; // can't die while airborne
				break;
			case PART_LIGHTNING:
				// get dimmer with each frame
				color /= 2;
				break;
		}


		if (x < 0 || y < 0 || x >= ((map->width * TILE_WIDTH) << FIXSHIFT) || y >= ((map->height * TILE_HEIGHT) << FIXSHIFT))
		{
			life = 0;
			return;
		}

		if (type == PART_SMOKE || type == PART_BOOM || type == PART_STINKY)
		{
			mapx = (x / TILE_WIDTH) >> FIXSHIFT;
			mapy = (y / TILE_HEIGHT) >> FIXSHIFT;
			color = 64 + map->map[mapx + mapy * map->width].templight;
		}
		else if (type == PART_LIGHTNING || type == PART_GLASS)
		{
			// nothing to do
		}
		else
		{
			mapx = (x / TILE_WIDTH) >> FIXSHIFT;
			mapy = (y / TILE_HEIGHT) >> FIXSHIFT;

			// brighten it appropriately
			brt = map->map[mapx + mapy * map->width].templight;
			c1 = (color & (~31)); // c1 is the color range
			color += brt;
			if (color > c1 + 31 || color < c1)
			{
				if (brt > 0)
					color = c1 + 31;
				else
					color = c1;
			}
		}
	}
}

bool Particle::Alive(void)
{
	return (life > 0);
}

//--------------------------------------------------------------------------

void InitParticles(int max)
{
	int i;

	maxParticles = max;

	particleList = (Particle **) malloc(sizeof (Particle *) * maxParticles);
	for (i = 0; i < maxParticles; i++)
		particleList[i] = new Particle();
}

void ExitParticles(void)
{
	int i;

	for (i = 0; i < maxParticles; i++)
		delete particleList[i];

	free(particleList);
}

void UpdateParticles(Map *map)
{
	int i;

	snowCount = 0;
	for (i = 0; i < maxParticles; i++)
		particleList[i]->Update(map);
}

void RenderParticle(int x, int y, byte *scrn, byte color, byte size)
{
	byte c1, c2;

	if (x < 0 || x > 639 || y < 0 || y > 479)
		return;

	switch (size) {
		case 2: // big particle
			if (x < 2 || x > 637 || y < 2 || y > 477)
				return;

			if ((color & 31) > 1)
				c1 = color - 2; // only do this if subtracting 2 keeps it in the same color group
			else
				c1 = color;
			if ((c1 & 31) > 1)
				c2 = c1 - 2; // only do this if subtracting 4 keeps it in the same color group
			else
				c2 = c1;

			scrn += (x + (y - 2)*640);
			*scrn = c2;
			scrn += 639;
			*scrn++ = c1;
			*scrn++ = color;
			*scrn = c1;
			scrn += 637;
			*scrn++ = c2;
			*scrn++ = c1;
			*scrn++ = color;
			*scrn++ = c1;
			*scrn = c2;
			scrn += 637;
			*scrn++ = c1;
			*scrn++ = color;
			*scrn = c1;
			*(scrn + 639) = c2;
			break;
		case 1: // normal particle
			if (x < 1 || x > 638 || y < 1 || y > 478)
				return;
			if (color & 31)
				c1 = color - 1; // only do this if subtracting 1 keeps it in the same color group
			else
				c1 = color;
			scrn += (x + (y - 1)*640);
			*scrn = c1;
			scrn += 639;
			*scrn++ = c1;
			*scrn++ = color;
			*scrn = c1;
			scrn += 639;
			*scrn = c1;
			break;
		case 0: // tiny particle (1 pixel)
			scrn[x + y * 640] = color;
			break;
	}
}

// this was going to be local to renderlightningparticle, but that would've seriously
// chomped up the stack, since that function's recursive.
byte ctab[] = {8, 3, 2, 3, 8,
	3, 2, 1, 2, 3,
	2, 1, 0, 1, 2,
	3, 2, 1, 2, 3,
	8, 3, 2, 3, 8};

void RenderLightningParticle(int x1, int y1, int x2, int y2, int range, byte bright, byte *scrn)
{
	int midx, midy;
	byte *ctptr;
	byte b, brt;

	// base case: draw the (x1,y1) pixel
	if ((x1 - x2 < 2 && x1 - x2>-2) && (y1 - y2 < 2 && y1 - y2>-2))
	{
		if (x1 >= 0 && x1 < 635 && y1 >= 0 && y1 < 475)
		{
			scrn += (x1 + y1 * 640);
			ctptr = &ctab[0];
			for (midy = y1; midy < y1 + 5; midy++)
			{
				for (midx = x1; midx < x1 + 5; midx++)
				{
					b = *scrn;
					brt = bright >> (*ctptr);
					if (((b + brt)&(~31)) != (b & (~31)))
						*scrn = (b & (~31)) + 31;
					else
						*scrn = b + brt;
					ctptr++;
					scrn++;
				}
				scrn += 640 - 5;
			}
		}
	}
	else // recursive case, find a (midx,midy) between the other two points
	{
		if (range < 1)
			range = 1;
		if (x1 < x2)
			midx = x1 + (x2 - x1) / 2;
		else
			midx = x2 + (x1 - x2) / 2;
		midx += MGL_random(range) - range / 2;
		if (y1 < y2)
			midy = y1 + (y2 - y1) / 2;
		else
			midy = y2 + (y1 - y2) / 2;
		midy += MGL_random(range) - range / 2;
		RenderLightningParticle(x1, y1, midx, midy, range * 3 / 4, bright, scrn);
		RenderLightningParticle(midx, midy, x2, y2, range * 3 / 4, bright, scrn);
	}
}

void RenderParticles(void)
{
	int i;

	for (i = 0; i < maxParticles; i++)
	{
		if (particleList[i]->Alive())
		{
			if (particleList[i]->type == PART_SMOKE)
				RenderSmoke(particleList[i]->x >> FIXSHIFT, particleList[i]->y >> FIXSHIFT,
					particleList[i]->z >> FIXSHIFT, (char) (particleList[i]->color),
					particleList[i]->size);
			else if (particleList[i]->type == PART_BOOM)
				RenderBoom(particleList[i]->x >> FIXSHIFT, particleList[i]->y >> FIXSHIFT,
					particleList[i]->z >> FIXSHIFT, (char) (particleList[i]->color),
					particleList[i]->size);
			else if (particleList[i]->type == PART_LIGHTNING)
				LightningDraw(particleList[i]->x >> FIXSHIFT, particleList[i]->y >> FIXSHIFT,
					particleList[i]->dx >> FIXSHIFT, particleList[i]->dy >> FIXSHIFT,
					particleList[i]->color, (char) particleList[i]->size);
			else if (particleList[i]->type == PART_STINKY)
				RenderStinky(particleList[i]->x >> FIXSHIFT, particleList[i]->y >> FIXSHIFT,
					particleList[i]->z >> FIXSHIFT, (char) (particleList[i]->color),
					particleList[i]->size);
			else if (particleList[i]->type == PART_COUNTESS)
				SprDraw(particleList[i]->x >> FIXSHIFT, particleList[i]->y >> FIXSHIFT,
					particleList[i]->z >> FIXSHIFT,
					255, particleList[i]->life * 4 - 8,
					GetMonsterSprite(MONS_COUNTESS, ANIM_IDLE, 0, 0), DISPLAY_DRAWME | DISPLAY_GLOW);
			else
				ParticleDraw(particleList[i]->x >> FIXSHIFT, particleList[i]->y >> FIXSHIFT,
					particleList[i]->z >> FIXSHIFT, particleList[i]->color, particleList[i]->size,
					DISPLAY_DRAWME | DISPLAY_PARTICLE);
		}
	}
}

void BlowSmoke(int x, int y, int z, int dz)
{
	int i;
	for (i = 0; i < maxParticles; i++)
	{
		if (!particleList[i]->Alive())
		{
			particleList[i]->x = x;
			particleList[i]->y = y;
			particleList[i]->z = z;
			particleList[i]->dx = 0;
			particleList[i]->dy = 0;
			particleList[i]->dz = dz;
			particleList[i]->life = 6 * 4 - MGL_random(8);
			particleList[i]->size = 6;
			particleList[i]->color = 64;
			particleList[i]->type = PART_SMOKE;
			break;
		}
	}
}

void StinkySteam(int x, int y, int z, int dz)
{
	int i;
	for (i = 0; i < maxParticles; i++)
	{
		if (!particleList[i]->Alive())
		{
			particleList[i]->x = x;
			particleList[i]->y = y;
			particleList[i]->z = z;
			particleList[i]->dx = 0;
			particleList[i]->dy = 0;
			particleList[i]->dz = dz;
			particleList[i]->life = 6 * 4 - MGL_random(8);
			particleList[i]->size = 0;
			particleList[i]->color = 64;
			particleList[i]->type = PART_STINKY;
			break;
		}
	}
}

void CountessGlow(int x, int y)
{
	int i;
	for (i = 0; i < maxParticles; i++)
	{
		if (!particleList[i]->Alive())
		{
			particleList[i]->x = x;
			particleList[i]->y = y;
			particleList[i]->z = 0;
			particleList[i]->dx = 0;
			particleList[i]->dy = 0;
			particleList[i]->dz = 0;
			particleList[i]->life = 4;
			particleList[i]->size = 0;
			particleList[i]->color = 64;
			particleList[i]->type = PART_COUNTESS;
			break;
		}
	}
}

void BlowUpGuy(int x, int y, int x2, int y2, int z, byte amt)
{
	int i;

	for (i = 0; i < maxParticles; i++)
	{
		if (!particleList[i]->Alive())
		{
			particleList[i]->x = (x + MGL_randoml(x2 - x)) << FIXSHIFT;
			particleList[i]->y = (y + MGL_randoml(y2 - y)) << FIXSHIFT;
			particleList[i]->z = z;
			particleList[i]->dx = 0;
			particleList[i]->dy = 0;
			particleList[i]->dz = 0;
			particleList[i]->life = 7;
			particleList[i]->size = 0;
			particleList[i]->color = 64;
			particleList[i]->type = PART_BOOM;
			MakeSound(SND_BOMBBOOM, particleList[i]->x, particleList[i]->y, SND_CUTOFF, 1800);
			if (!(--amt))
				break;
		}
	}
}

void GlassShatter(int x, int y, int x2, int y2, int z, byte amt)
{
	int i;

	for (i = 0; i < maxParticles; i++)
	{
		if (!particleList[i]->Alive())
		{
			particleList[i]->x = (x + MGL_randoml(x2 - x)) << FIXSHIFT;
			particleList[i]->y = (y + MGL_randoml(y2 - y)) << FIXSHIFT;
			particleList[i]->z = z;
			particleList[i]->GoRandom(PART_GLASS, (x + MGL_randoml(x2 - x)) << FIXSHIFT, (y + MGL_randoml(y2 - y)) << FIXSHIFT,
					MGL_randoml(10 * FIXAMT), 20);
			particleList[i]->color = MGL_random(8)*32 + 16;
			if (!(--amt))
				break;
		}
	}
}

void SpurtParticles(byte type, bool left, int x, int y, int z, byte angle, byte force)
{
	int i, amt;
	byte ang2;

	amt = force;

	x += Cosine(angle)*10;
	y += Sine(angle)*10;
	if (left)
		ang2 = angle + 64;
	else
		ang2 = angle + 128 + 64;

	x += Cosine(ang2)*20;
	y += Sine(ang2)*20;

	for (i = 0; i < maxParticles; i++)
	{
		if (!particleList[i]->Alive())
		{
			particleList[i]->Go(type, x, y, z, angle, force);
			if (!--amt)
				break;
		}
	}
}

void ExplodeParticles(byte type, int x, int y, int z, byte force)
{
	int i, amt;

	amt = force;

	for (i = 0; i < maxParticles; i++)
	{
		if (!particleList[i]->Alive())
		{
			particleList[i]->GoRandom(type, x, y, z, force);
			if (!--amt)
				break;
		}
	}
}

void ExplodeParticles2(byte type, int x, int y, int z, byte num, byte force)
{
	int i;

	if (num == 0)
		return;

	for (i = 0; i < maxParticles; i++)
	{
		if (!particleList[i]->Alive())
		{
			particleList[i]->GoRandom(type, x, y, z, force);
			if (!--num)
				break;
		}
	}
}

void MakeItSnow(Map *map)
{
	int i;
	int cx, cy;

	// only 25% of particles may be snowflakes
	if (MGL_random(100) > 30 || snowCount > maxParticles / 4)
		return;

	GetCamera(&cx, &cy);
	cx -= 320;
	cy -= 240;
	for (i = 0; i < maxParticles; i++)
	{
		if (!particleList[i]->Alive())
		{

			particleList[i]->x = (MGL_random(640) + cx) << FIXSHIFT;
			particleList[i]->y = (MGL_random(480) + cy) << FIXSHIFT;
			particleList[i]->z = (300 + MGL_random(300)) << FIXSHIFT;
			particleList[i]->dx = 0;
			particleList[i]->dy = 0;
			particleList[i]->dz = 0;
			particleList[i]->size = 2;
			particleList[i]->life = 50 + MGL_random(50);
			particleList[i]->type = PART_SNOW;
			particleList[i]->color = 31;
			break;
		}
	}
}

void SpecialSnow(int x, int y)
{
	int i;

	for (i = 0; i < maxParticles; i++)
	{
		if (!particleList[i]->Alive())
		{

			particleList[i]->x = x;
			particleList[i]->y = y;
			particleList[i]->z = (10 + MGL_random(20)) << FIXSHIFT;
			particleList[i]->dx = 0;
			particleList[i]->dy = 0;
			particleList[i]->dz = 0;
			particleList[i]->size = 2;
			particleList[i]->life = 20 + MGL_random(30);
			particleList[i]->type = PART_SNOW;
			break;
		}
	}
}

void LightningBolt(int x, int y, int x2, int y2)
{
	int i;

	for (i = 0; i < maxParticles; i++)
	{
		if (!particleList[i]->Alive())
		{
			particleList[i]->GoLightning(x, y, x2, y2);
			break;
		}
	}
}
