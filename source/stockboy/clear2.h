#ifndef CLEAR2_H
#define CLEAR2_H

#include "mgldraw.h"
#include "map.h"

void InitClear2(Map *m);

void UpdateClear2(void);
void RenderClear2(void);

void Clear2Wait(void);

byte Clear2Menu(MGLDraw *mgl);

#endif