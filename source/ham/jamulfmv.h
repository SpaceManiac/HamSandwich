#ifndef JAMULFMV_H
#define JAMULFMV_H

#include "jamultypes.h"
#include "coro.h"

class MGLDraw;

typedef byte (*FlicCallBack)(int);

TASK(byte) FLI_play(const char *name, byte loop, word wait, MGLDraw *mgl, FlicCallBack callback = nullptr);

#endif
