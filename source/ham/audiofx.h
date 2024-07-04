#ifndef AUDIOFX_H
#define AUDIOFX_H

#include "owned_mixer.h"

owned::Mix_Chunk FxRandomPitch(Mix_Chunk* sample);
owned::Mix_Chunk FxBackwards(Mix_Chunk* sample);
owned::Mix_Chunk FxDoubleSpeed(Mix_Chunk* sample);

#endif  // AUDIOFX_H
