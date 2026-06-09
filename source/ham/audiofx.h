#ifndef AUDIOFX_H
#define AUDIOFX_H

#include "owned_mixer.h"

owned::MIX_Audio FxRandomPitch(MIX_Audio* sample);
owned::MIX_Audio FxBackwards(MIX_Audio* sample);
owned::MIX_Audio FxDoubleSpeed(MIX_Audio* sample);

#endif  // AUDIOFX_H
