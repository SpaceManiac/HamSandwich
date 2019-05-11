#ifndef WORLDSTITCH_H
#define WORLDSTITCH_H

#include "mgldraw.h"
#include "world.h"

extern int stitchTileOffset,stitchSoundOffset,stitchItemOffset;

void SetStitchError(char *txt);
char *GetStitchError(void);
byte AddWorldIn(world_t *world1,char *fname);

#endif
