#ifndef SCANNER_H
#define SCANNER_H

#include "mgldraw.h"
#include "guy.h"

void ScanGuy(Guy *g);

byte UpdateScan(MGLDraw *mgl);
void RenderScan(MGLDraw *mgl);

TASK(void) Bestiary(MGLDraw *mgl);

float ScanPercent(void);

#endif
