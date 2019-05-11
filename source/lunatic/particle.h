#ifndef PARTICLE_H
#define PARTICLE_H

#include "map.h"
#include "jamultypes.h"
#include "display.h"

enum {
	PART_NONE = 0,
	PART_SNOW2,		// speedy snow particles, just like hammer particles but white
	PART_SNOW,
	PART_DIRT,
	PART_HAMMER,
	PART_SLIME,
	PART_SMOKE,
	PART_BOOM,
	PART_WATER,
	PART_LIGHTNING,
	PART_STINKY,	// stinky lines for garlic
	PART_GLASS,		// multicolored stained glass
	PART_COUNTESS	// glowing image of the countess for when she charges
};

class Particle
{
public:
	Particle(void);
	~Particle(void);

	void Go(byte type, int x, int y, int z, byte angle, byte force);
	void GoLightning(int x, int y, int x2, int y2);
	void GoRandom(byte type, int x, int y, int z, byte force);
	void Update(Map *map);
	bool Alive(void);

	byte size;
	byte type;
	byte color;
	int x, y, z;
	int dx, dy, dz;
	int life;
};

void InitParticles(int max);
void ExitParticles(void);
void UpdateParticles(Map *map);
void RenderParticles(void);

void RenderParticle(int x, int y, byte *scrn, byte color, byte size);
void RenderLightningParticle(int x1, int y1, int x2, int y2, int range, byte bright, byte *scrn);

void BlowSmoke(int x, int y, int z, int dz);
void SpurtParticles(byte type, bool left, int x, int y, int z, byte angle, byte force);
void BlowUpGuy(int x, int y, int x2, int y2, int z, byte amt);
void ExplodeParticles(byte type, int x, int y, int z, byte force);
void ExplodeParticles2(byte type, int x, int y, int z, byte num, byte force);
void MakeItSnow(Map *map);
void SpecialSnow(int x, int y);
void LightningBolt(int x, int y, int x2, int y2);
void StinkySteam(int x, int y, int z, int dz);
void GlassShatter(int x, int y, int x2, int y2, int z, byte amt);
void CountessGlow(int x, int y);

#endif
