#ifndef CLOCK_H
#define CLOCK_H

#include "winpch.h"
#pragma pack(1)

void StartClock(void);
void EndClock(void);
dword TimeLength(void);
void ResetClock(dword amt);

#endif
