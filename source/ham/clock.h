#ifndef CLOCK_H
#define CLOCK_H

#include "jamultypes.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#else  // _WIN32
HAM_API dword timeGetTime();
#endif  // _WIN32

HAM_API void StartClock(void);
HAM_API void EndClock(void);
HAM_API dword TimeLength(void);
HAM_API void ResetClock(dword amt);

#endif
