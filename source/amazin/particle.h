#ifndef PARTICLE_H
#define PARTICLE_H

#include "jamultypes.h"
#include "guy.h"

void ParticlesInit();
void ParticlesUpdate();
void ParticlesRender();
void ParticleAddTeleport(int x, int y);
void ParticleAddRespawn(int x, int y);
void ParticleAddSpeed(GuyType player, byte life, int x, int y);
void ParticleAddSnow(byte amount);

#endif  // PARTICLE_H
