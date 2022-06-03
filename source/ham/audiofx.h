#ifndef AUDIOFX_H
#define AUDIOFX_H

struct Mix_Chunk;

Mix_Chunk* FxRandomPitch(Mix_Chunk* sample);
Mix_Chunk* FxBackwards(Mix_Chunk* sample);
Mix_Chunk* FxDoubleSpeed(Mix_Chunk* sample);

#endif  // AUDIOFX_H
