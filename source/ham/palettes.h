#ifndef PALETTES_H
#define PALETTES_H

#include "jamultypes.h"

class MGLDraw;

HAM_API void WaterPalette(MGLDraw *mgl);
HAM_API void LavaPalette(MGLDraw *mgl);
HAM_API void DumbSidePalette(MGLDraw *mgl);
HAM_API void GreyPalette(MGLDraw *mgl);
HAM_API void DarkPalette(MGLDraw *mgl);
HAM_API void CyclePalette(MGLDraw *mgl, int c);
HAM_API void ReversePalette(MGLDraw *mgl);

HAM_API void GammaCorrect(MGLDraw *mgl, byte gamma);

#endif // PALETTES_H
