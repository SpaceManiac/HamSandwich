#ifndef MAPDIALOG_H
#define MAPDIALOG_H

#include "display.h"
#include "world.h"

void InitMapDialog(world_t *wrld,byte currentMap);
void ExitMapDialog(void);
void RenderMapDialog(int msx,int msy,MGLDraw *mgl);
byte MapDialogKey(char key);
byte MapDialogClick(int msx,int msy);

#endif
