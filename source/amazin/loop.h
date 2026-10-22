#ifndef MAIN_H
#define MAIN_H

#include "coro.h"

typedef bool (*updateFunc)();
typedef void (*renderFunc)();

void SetUpdate(updateFunc update);
void SetRender(renderFunc render);
TASK(void) LoopMain();

#endif  // MAIN_H
