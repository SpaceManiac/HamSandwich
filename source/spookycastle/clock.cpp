#include "clock.h"

dword timeStart=0,timeEnd=0;

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