#ifndef GALLERY_H
#define GALLERY_H

#include "mgldraw.h"
#include "map.h"

typedef struct galpic_t
{
	char fname[32];
	char title[48];
	char artist[16];
	char desc[1024];
	byte x,y;
} galpic_t;

void InitGallery(Map *map);
void ExitGallery(void);

byte GalleryBump(int x,int y);
TASK(void) ShowGalleryPic(void);
void SetupGalPix(Map *map);

#endif
