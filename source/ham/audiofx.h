#ifndef AUDIOFX_H
#define AUDIOFX_H

#include "owned_mixer.h"

owned::MIX_Audio FxBackwards(SDL_IOStream *file, MIX_Mixer *mixer);

#endif  // AUDIOFX_H
