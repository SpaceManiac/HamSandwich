#ifndef GALLERY_H
#define GALLERY_H

#include "jamultypes.h"
#include "mgldraw.h"
#pragma pack(1)

#define GALLERY_PIX	(12)

extern byte picToDrop;

void Gallery(MGLDraw *mgl);
byte HaveGallery(void);

#endif
