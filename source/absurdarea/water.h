#ifndef WATER_H
#define WATER_H

#include "mgldraw.h"

void InitWater(void);
void ExitWater(void);
void UpdateWater(void);
byte WaterPixel(int x,int y);
byte LavaPixel(int x, int y);	// AA add
void WaterRipple(int x,int y,short amt);
void WaterBlop(byte x,byte y,byte width);
void SetupWater(void);

#endif
