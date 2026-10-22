#ifndef MAPSCREEN_H
#define MAPSCREEN_H

#include "winpch.h"
#include "display.h"
#include "world.h"

void InitMapScreen(world_t* wrld, byte currentMap);
void ExitMapScreen(void);
void RenderMapScreen(int msx, int msy, MGLDraw* mgl);
byte MapScreenKey(char key);
byte MapScreenClick(int msx, int msy);

#endif
