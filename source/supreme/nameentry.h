#ifndef NAMEENTRY_H
#define NAMEENTRY_H

#include "mgldraw.h"

TASK(void) NameEntry(MGLDraw *mgl,byte makeNew);
const char *GetEnteredName(void);

#endif
