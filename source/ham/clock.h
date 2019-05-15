#ifndef CLOCK_H
#define CLOCK_H

typedef unsigned long dword;

#ifdef WIN32
#include <windows.h>
#else
dword timeGetTime();
#endif  // WIN32

void StartClock(void);
void EndClock(void);
dword TimeLength(void);
void ResetClock(dword amt);

#endif
