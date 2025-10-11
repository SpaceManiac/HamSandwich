#ifndef WORLDSTITCH_H
#define WORLDSTITCH_H

#include "mgldraw.h"
#include "world.h"

void SetStitchError(const char *txt);
const char *GetStitchError(void);
byte AddWorldIn(world_t *world1,const char *fname);

#endif
