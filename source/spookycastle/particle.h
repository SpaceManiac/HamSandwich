#ifndef PARTICLE_H
#define PARTICLE_H

#include "map.h"
#include "jamultypes.h"
#include "display.h"

#define PART_NONE  0
#define PART_SNOW2 1	// speedy snow particles, just like hammer particles but white
#define PART_SNOW  2
#define PART_DIRT  3
#define PART_HAMMER 5
#define PART_SLIME 6
#define PART_SMOKE 7
#define PART_BOOM  8
#define PART_WATER 9

class Particle
{
	public:
		Particle(void);
		~Particle(void);

		void Go(byte type,int x,int y,int z,byte angle,byte force);
		void GoRandom(byte type,int x,int y,int z,byte force);
		void Update(Map *map);
		bool Alive(void);

		byte size;
		byte type;
		byte color;
		int x,y,z;
		int dx,dy,dz;
		int life;
};

void InitParticles(int max);
void ExitParticles(void);
void UpdateParticles(Map *map);
void RenderParticles(void);

void RenderParticle(int x,int y,byte *scrn,byte color,byte size);

void BlowSmoke(int x,int y,int z,int dz);
void SpurtParticles(byte type,bool left,int x,int y,int z,byte angle,byte force);
void BlowUpGuy(int x,int y,int x2,int y2,int z,byte amt);
void ExplodeParticles(byte type,int x,int y,int z,byte force);
void ExplodeParticles2(byte type,int x,int y,int z,byte num,byte force);
void MakeItSnow(Map *map);
void SpecialSnow(int x,int y);

#endif