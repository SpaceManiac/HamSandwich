#ifndef BLOWOUT_H
#define BLOWOUT_H

#include "mgldraw.h"
#include "map.h"

void InitBlowout(Map *m);

void UpdateBlowout(void);
void RenderBlowout(void);

byte BlowoutMenu(MGLDraw *mgl);

#endif