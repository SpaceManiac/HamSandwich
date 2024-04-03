#ifndef FLOCK_H
#define FLOCK_H

#include "mgldraw.h"
#include "jamulspr.h"

typedef struct flock_t
{
	int x,y;
	int dx,dy;
	byte type;
	byte color;
	byte angle;
	char angAdj;
	char bright;
	char brightChg;
} flock_t;

typedef struct circle_t
{
	byte angle;
	char angAdj;
	int width;
	int height;
	int dWidth;
	int dHeight;
} circle_t;

void InitFlock(sprite_set_t *spr);
void UpdateFlock(void);
void RenderFlock(MGLDraw *mgl);

#endif