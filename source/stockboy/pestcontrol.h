#ifndef PESTCONTROL_H
#define PESTCONTROL_H

#include "mgldraw.h"
#include "map.h"

void InitPestControl(Map *m);

void UpdatePestControl(void);
void RenderPestControl(void);

byte PestControlMenu(MGLDraw *mgl);

#endif