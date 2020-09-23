#ifndef SCANNER_H
#define SCANNER_H

#include "mgldraw.h"
#include "guy.h"

#define MAX_SCAN 16

void ScanGuy(Guy *me, Guy *g);

byte UpdateScan(MGLDraw *mgl);
byte UpdateCard(MGLDraw *mgl);
void RenderScan(MGLDraw *mgl);
void RenderCard(MGLDraw *mgl);

void Bestiary(MGLDraw *mgl);

float ScanPercent(void);
float CardPercent(void);

void GetGuyCard(Guy *g);

#endif
