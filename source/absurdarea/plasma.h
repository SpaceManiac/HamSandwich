#ifndef PLASMA_H
#define PLASMA_H

#include "game.h"
#pragma pack(1)

#define PLASMA_WIDTH 320
#define PLASMA_HEIGHT 30

void InitPlasma(byte col);
void ExitPlasma(void);
void UpdatePlasma(void);
void UpdatePlasma2(void);
void RenderPlasma(MGLDraw *mgl,byte bottom);
void RenderPlasma2(MGLDraw *mgl);
void RenderPlasma3(MGLDraw *mgl,byte bottom);
void PlasmaColor(byte c);

#endif
