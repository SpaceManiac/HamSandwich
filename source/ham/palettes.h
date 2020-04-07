#ifndef PALETTES_H
#define PALETTES_H

#include "jamultypes.h"

class MGLDraw;

void WaterPalette(MGLDraw *mgl);
void LavaPalette(MGLDraw *mgl);
void DumbSidePalette(MGLDraw *mgl);
void GreyPalette(MGLDraw *mgl);
void RedPalette(MGLDraw *mgl);
void DarkPalette(MGLDraw *mgl);
void CyclePalette(MGLDraw *mgl, int c);

void GammaCorrect(MGLDraw *mgl, byte gamma);

#endif // PALETTES_H
