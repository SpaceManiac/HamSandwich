#ifndef CLOCK_H
#define CLOCK_H

#include "mgldraw.h"

void StartClock(void);
void EndClock(void);
dword TimeLength(void);

#ifndef WIN32
#include <time.h>
inline dword timeGetTime()
{
        struct timespec tm;
        clock_gettime(CLOCK_REALTIME, &tm);
        return tm.tv_sec*1000 + tm.tv_nsec/1000;
}
#endif

#endif
