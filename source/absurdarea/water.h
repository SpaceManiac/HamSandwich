#ifndef WATER_H
#define WATER_H

#include "mgldraw.h"
#pragma pack(1)

void InitWater(void);
void ExitWater(void);
void UpdateWater(void);
byte WaterPixel(int x,int y);
void WaterRipple(int x,int y,short amt);
void WaterBlop(byte x,byte y,byte width);
void SetupWater(void);

#endif
