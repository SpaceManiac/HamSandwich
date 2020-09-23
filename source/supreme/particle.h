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
#define PART_LIGHTNING 10
#define PART_STINKY 11	// stinky lines for garlic
#define PART_GLASS	12	// multicolored stained glass
#define PART_COUNTESS 13	// glowing image of the countess for when she charges
#define PART_BUBBLE	14
#define PART_MINDCONTROL 15
#define PART_COLOR	16	// colored particles
#define PART_RAIN	17	// raindrops, keep falling on my head
#define PART_LUNA	18
#define PART_RADAR	19	// used for the radars
#define PART_FX		20	// accelerates upward, shrinks as it dies, for effects
//cool
#define PART_RLHAMMER 21	// used by hammer, not always red (not working atm)
#define PART_SUCK	22	// particles that get sucked in by black hole
#define PART_YELLOW 23
#define PART_SHORTYELLOW 24
#define PART_FLOATER 25	// colored particles that float up in a ring
#define PART_LAVA 26
//from loonyland!
#define PART_FIRE	27	// an expanding ring for bonkula
#define PART_COLDFIRE 28	// same as fire, but white

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
void SlimeTime(Map *map);
void SpecialSnow(int x,int y);
void LightningBolt(int x,int y,int x2,int y2);
void StinkySteam(int x,int y,int z,int dz);
void GlassShatter(int x,int y,int x2,int y2,int z,byte amt);
void MonsterGlow(int x,int y,dword m); //edited
void MindControlSparks(int x,int y,int z,int dz);
void BlowBubble(int x,int y,int z,int dz);
void WeathermanWeather(int x,int y);
void ColorRing(byte color,int x,int y,int z,byte num,byte force);
void ColorDrop(byte color,int x,int y,int z);
void ColorDropSize(byte color,int x,int y,int z, int s);
void TrackParticle(byte color,int x,int y,int tx,int ty);
void HealRing(byte color,int x,int y,int z,byte num,byte force);
void TeamChangeRing(byte color,int x,int y,int z,byte num,byte force);

void SmokeTile(int x,int y);
void JackFrostWeather(int x,int y);
void SuckParticle(int x,int y,int z);
void MakeRingParticle(int x,int y, int z,byte size,byte cnt);
void MakeColdRingParticle(int x,int y, int z,byte size);

int CountParticles();

#endif
