#ifndef RAGE_H
#define RAGE_H

#include "winpch.h"
#include "display.h"
#include "guy.h"

void ShowRage(MGLDraw *mgl);
byte UpdateRage(MGLDraw *mgl);
void StartRaging(void);
void DoRage(Guy *me);

#endif
