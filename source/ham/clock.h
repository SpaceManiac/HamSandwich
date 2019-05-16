#ifndef CLOCK_H
#define CLOCK_H

#include "hamtypes.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#else  // _WIN32
dword timeGetTime();
#endif  // _WIN32

void StartClock(void);
void EndClock(void);
dword TimeLength(void);
void ResetClock(dword amt);

#endif
