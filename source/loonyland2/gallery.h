#ifndef GALLERY_H
#define GALLERY_H

#include "jamultypes.h"
#include "mgldraw.h"

#define GALLERY_PIX	(12)

extern byte picToDrop;

TASK(void) Gallery(MGLDraw *mgl);
byte HaveGallery(void);

#endif
