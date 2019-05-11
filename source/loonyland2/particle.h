#ifndef PARTICLE_H
#define PARTICLE_H

#include "map.h"
#include "jamultypes.h"
#include "display.h"
#pragma pack(1)

#define PART_NONE  0
#define PART_SNOW2 1	// speedy snow particles, just like hammer particles but white
#define PART_SNOW  2
#define PART_DIRT  3
#define PART_HAMMER 4
#define PART_SLIME 5
#define PART_WATER 6
#define PART_LIGHTNING 7
#define PART_GLASS	8	// multicolored stained glass
#define PART_CIRCLE	9	// circle, used for loony attack
#define PART_FIRE	10	// particle fire for torches
#define PART_RING	11	// an expanding ring for bonkula
#define PART_COLDFIRE 12	// same as fire, but white
#define PART_YELLOW 13
#define PART_NUM	14	// a number floating up
#define PART_XP		15	// a number floating up with "XP" written after it
#define PART_FLOATY	16	// sparkles that float up from items you can get
#define PART_FX		17	// accelerates upward, for special effects
#define PART_GHOST  18	// a ghost for the drain spell
#define PART_SMOKE  19
#define PART_CLOUD	20	// white FX things that just shrink away and stay up in the air
#define PART_BAN	21

// these are particle booms, definitions of entire explosions of other particle types
#define PART_WHITEPOP	50	// a pop of white particles
#define PART_FIREPOP	51	// a pop of fire bits
#define PART_SPARKPOP	52	// a pop of yellow sparks
#define PART_FLOATYPOP  53	// just used to make floaties come up in a random radius instead of from the center
#define PART_GREENPOP	54	// a pop of green particles
#define PART_REDPOP		55	// a pop of red stuff
#define PART_SMOKESINGLE 56	// a single puff of smoke instead of a pile (which you get if you ParticleBoom(PART_SMOKE)
#define PART_STEAM		57	// smoke particles shot upwards at high velocity

class Particle
{
	public:
		Particle(void);
		~Particle(void);

		void Go(byte type,int x,int y,int z,byte angle,byte force);
		void GoLightning(int x,int y,int x2,int y2);
		void GoRandom(byte type,int x,int y,int z,byte force);
		void GoExact(byte type,int x,int y,int z,byte angle,byte force);

		void Update(Map *map);
		bool Alive(void);

		byte size;
		byte type;
		byte color;
		int tx,ty;
		int x,y,z;
		int dx,dy,dz;
		int life;
		short num;
};

void InitParticles(int max);
void ExitParticles(void);
void UpdateParticles(Map *map);
void RenderParticles(void);

void RenderParticle(int x,int y,byte *scrn,byte color,byte size);
void RenderLightningParticle(int x1,int y1,int x2,int y2,int range,byte bright,byte *scrn);
void RenderCircleParticle(int x,int y,int radius,byte c,byte *scrn);
void RenderGlowCircleParticle(int x,int y,int radius,byte c,byte *scrn);
void RenderRingParticle(int x,int y,int radius,byte c,byte *scrn);

void SpurtParticles(byte type,bool left,int x,int y,int z,byte angle,byte force);
void ExplodeParticles(byte type,int x,int y,int z,byte force);
void ExplodeParticles2(byte type,int x,int y,int z,byte num,byte force);
void MakeItSnow(Map *map);
void SpecialSnow(int x,int y);
void LightningBolt(int x,int y,int x2,int y2);
void GlassShatter(int x,int y,int x2,int y2,int z,byte amt);
void MakeCircleParticle(int x,int y, int z,byte size);
void MakeCircleParticle2(int x,int y, int z,byte size);
void MakeRingParticle(int x,int y, int z,byte size,byte cnt);
void MakeColdRingParticle(int x,int y, int z,byte size);
void MakePlagueRingParticle(int x,int y, int z,int size);
void Burn(int x,int y,int z);
void ParticleBoom(byte type,int x,int y,int z);
void NumberParticle(short num,int x,int y,byte color);
void NumberParticle2(short num,int x,int y,byte color,byte type);
void FXRing(byte color,int x,int y,int z,byte num,byte force);
void Smoke(int x,int y,int z);

#endif
