#include "random.h"
#include "clock.h"
#include "mgldraw.h"

void SeedRand(void)
{
	MGL_srand(timeGetTime());
}

int	 GetRand(int range)
{
	return MGL_random(range);
}

dword GetLRand(dword range)
{
	return MGL_randoml(range);
}

dword GetTime(void)
{
	return timeGetTime();
}
