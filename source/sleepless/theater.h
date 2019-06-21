#ifndef THEATER_H
#define THEATER_H

#include "mgldraw.h"

TASK(void) Theater(MGLDraw *mgl);

void SeeMovie(char *fname);
byte AllMoviesSeen(void);

#endif
