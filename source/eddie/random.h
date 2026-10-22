#ifndef RANDOM_H
#define RANDOM_H

#include "jamultypes.h"

void SeedRand(void);
int	 GetRand(int range);
dword GetLRand(dword range);
dword GetTime(void);

#endif