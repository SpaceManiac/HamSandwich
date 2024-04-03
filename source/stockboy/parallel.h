#ifndef PARALLEL_H
#define PARALLEL_H

#include "mgldraw.h"
#include "map.h"

void InitParallel(Map *m);

void UpdateParallel(void);
void RenderParallel(void);

TASK(byte) ParallelMenu(MGLDraw *mgl);

char *RvsNameLevel(void);

#endif
