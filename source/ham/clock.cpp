#include "clock.h"

#ifndef _WIN32
#include <time.h>

dword timeGetTime()
{
	struct timespec tm;
	clock_gettime(CLOCK_REALTIME, &tm);
	return tm.tv_sec*1000 + tm.tv_nsec/1000000;
}
#endif  // _WIN32

static dword timeStart=0, timeEnd=0;

void StartClock(void)
{
	timeStart=timeGetTime();
	timeEnd=timeStart;
}

void EndClock(void)
{
	timeEnd=timeGetTime();
}

dword TimeLength(void)
{
	return (timeEnd-timeStart);
}

void ResetClock(dword amt)
{
	timeStart=timeGetTime();
	timeEnd=timeGetTime();
}
