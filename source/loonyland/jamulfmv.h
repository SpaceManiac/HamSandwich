#ifndef JAMULFMV_H
#define JAMULFMV_H

#include "jamultypes.h"
#include "mgldraw.h"
#include <stdio.h>

typedef byte (*FlicCallBack)(int);

void FLI_play(char *name,byte loop,word wait,MGLDraw *mgl,FlicCallBack callback);

#endif
