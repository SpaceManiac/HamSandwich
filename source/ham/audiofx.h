#ifndef AUDIOFX_H
#define AUDIOFX_H

#include "jamultypes.h"
#include "owned_mixer.h"

HAM_API owned::MIX_Audio FxBackwards(SDL_IOStream *file, MIX_Mixer *mixer);

#endif  // AUDIOFX_H
