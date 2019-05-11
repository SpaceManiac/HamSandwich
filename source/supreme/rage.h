#ifndef RAGE_H
#define RAGE_H

#include "display.h"
#include "guy.h"

void ShowRage(MGLDraw *mgl);
byte UpdateRage(MGLDraw *mgl);
void StartRaging(void);
void DoRage(Guy *me);
void SetRageFace(void);

#endif
