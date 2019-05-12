#ifndef LEVELDIALOG_H
#define LEVELDIALOG_H

#include "display.h"
#include "world.h"

void InitLevelDialog(world_t *wrld,byte currentMap);
void ExitLevelDialog(void);
void RenderLevelDialog(int msx,int msy,MGLDraw *mgl);
byte LevelDialogKey(char key);
byte LevelDialogClick(int msx,int msy);
byte LevelDialogRClick(int msx,int msy);

#endif
