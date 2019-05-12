#ifndef SCANNER_H
#define SCANNER_H

#include "mgldraw.h"
#include "guy.h"

#define MAX_SCAN 16

void ScanGuy(Guy *g);

byte UpdateScan(MGLDraw *mgl);
void RenderScan(MGLDraw *mgl);

void Bestiary(MGLDraw *mgl);

float ScanPercent(void);

#endif
