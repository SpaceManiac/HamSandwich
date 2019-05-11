#ifndef JAMULFMV_H
#define JAMULFMV_H

#include "jamultypes.h"
#include "mgldraw.h"
#include <stdio.h>

byte FLI_play(char *name,byte loop,word wait,MGLDraw *mgl);
void FLI_play_callback(char *name,byte loop,word wait,void (*callback)(int),MGLDraw *mgl);

#endif
