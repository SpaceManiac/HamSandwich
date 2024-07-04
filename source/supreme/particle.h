#ifndef PARTICLE_H
#define PARTICLE_H

#include "map.h"
#include "jamultypes.h"
#include "display.h"

enum : byte
{
	PART_NONE,
	PART_SNOW2, // speedy snow particles, just like hammer particles but white
	PART_SNOW,
	PART_DIRT,
	PART_HAMMER,
	PART_SLIME,
	PART_SMOKE,
	PART_BOOM,
	PART_WATER,
	PART_LIGHTNING,
	PART_STINKY, // stinky lines for garlic
	PART_GLASS, // multicolored stained glass
	PART_COUNTESS, // glowing image of the countess for when she charges
	PART_BUBBLE,
	PART_MINDCONTROL,
	PART_COLOR, // colored particles
	PART_RAIN, // raindrops, keep falling on my head
	PART_LUNA,
	PART_RADAR, // used for the radars
	PART_FX, // accelerates upward, shrinks as it dies, for effects
};

class Particle
{
	public:
		Particle(void);
		~Particle(void);

		void SpurtGo(byte type,int x,int y,int z,byte angle,byte force);
		void Go(byte type,int x,int y,int z,byte angle,byte force);
		void GoExact(byte type,int x,int y,int z,byte angle,byte force);
		void GoLightning(int x,int y,int x2,int y2);
		void GoRandom(byte type,int x,int y,int z,byte force);
		void GoRandomColor(byte color,int x,int y,int z,byte force);
		void Update(Map *map);
		bool Alive(void);

		byte size;
		byte type;
		byte color;
		int tx,ty;
		int x,y,z;
		int dx,dy,dz;
		int life;
};

void InitParticles(int max);
void ExitParticles(void);
void UpdateParticles(Map *map);
void RenderParticles(void);

void RenderParticle(int x,int y,byte *scrn,byte color,byte size);
void RenderLightningParticle(int x1,int y1,int x2,int y2,int range,byte bright,byte *scrn);
void RenderCircleParticle(int x,int y,int radius,byte c,byte *scrn);
void RenderGlowCircleParticle(int x,int y,int radius,byte c,byte *scrn);

int  BlowSmoke(int x,int y,int z,int dz);
void SpurtParticles(byte type,bool left,int x,int y,int z,byte angle,byte force);
void BlowUpGuy(int x,int y,int x2,int y2,int z,byte amt);
void ExplodeParticles(byte type,int x,int y,int z,byte force);
void ExplodeParticles2(byte type,int x,int y,int z,byte num,byte force);
void ExplodeParticlesColor(byte color,int x,int y,int z,byte num,byte force);
void MakeItSnow(Map *map);
void MakeItRain(Map *map);
void SpecialSnow(int x,int y);
void LightningBolt(int x,int y,int x2,int y2);
void StinkySteam(int x,int y,int z,int dz);
void GlassShatter(int x,int y,int x2,int y2,int z,byte amt);
void CountessGlow(int x,int y);
void MindControlSparks(int x,int y,int z,int dz);
void BlowBubble(int x,int y,int z,int dz);
void WeathermanWeather(int x,int y);
void ColorRing(byte color,int x,int y,int z,byte num,byte force);
void ColorDrop(byte color,int x,int y,int z);
void TrackParticle(byte color,int x,int y,int tx,int ty);
void HealRing(byte color,int x,int y,int z,byte num,byte force);
void TeamChangeRing(byte color,int x,int y,int z,byte num,byte force);

void SmokeTile(int x,int y);
void JackFrostWeather(int x,int y);

int CountParticles();

#endif
