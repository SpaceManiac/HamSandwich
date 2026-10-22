#ifndef TALLY_H
#define TALLY_H

#include "mgldraw.h"
#include "map.h"

void SetupTally(Map *map);
void ExitTally(void);
byte UpdateTally(void);
void RenderTally(MGLDraw *mgl);
void SetTallyMsg(const char *msg,byte winpic);

#endif
