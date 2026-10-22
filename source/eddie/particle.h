#ifndef PARTICLE_H
#define PARTICLE_H

#include "jamultypes.h"

#define MAX_PARTICLES 512

#define PART_NONE   0
#define PART_PURPLE 1
#define PART_SMOKE  2
#define PART_LILBOOM 3
#define PART_BIGBOOM 4
#define PART_RED	5
#define PART_SPARKLY 6
#define PART_SHOCKWAVE 7
#define PART_SNOW	8
#define PART_YELLOW 9
#define PART_BLUE	10
#define PART_GREEN	11
#define PART_RIPPLE	12
#define PART_SMOKE2	13
#define PART_GREY	14
#define PART_SPUTTER 15	// the sputters from Eddie's jetpack
#define PART_SANDWORM 16	// ripples in the sand caused by sandworms
#define PART_ENGINEGLOW 17	// bright particle for rocket ship engine

void InitParticles(void);
void ExitParticles(void);
void ClearParticles(void);

void GravitySwitch(byte on);

void UpdateParticles(void);
void RenderParticles(void);
void PreRenderParticles(void);	// some particles want to be drawn BEFORE the sprites rather than after

void ExplodeParticles(byte type,int x,int y,byte force,byte count);
void ExplodeParticlesHigh(byte type,int x,int y,byte force,byte count);	// the particles start up off the ground
void AddParticle(byte type,int x,int y,int force,byte life);
void AddParticleExact(byte type,int x,int y,int z,int dx,int dy,int dz,byte life);

#endif