#ifndef RADAR_H
#define RADAR_H

#include "mgldraw.h"

#define MAX_RADAR	(256)

#pragma pack(push, 1)
typedef struct radar_t
{
	int x,y;
	byte size;
	byte color;
} radar_t;
#pragma pack(pop)

void Radar_Init(void);
void Radar_Exit(void);
void Radar_Update(void);
void Radar_Render(MGLDraw *mgl);

#endif
