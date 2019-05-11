#ifndef INTFACE_H
#define INTFACE_H

#include "jamulspr.h"
#include "display.h"
#include "mgldraw.h"

void InitInterface(void);
void ExitInterface(void);
void ResetInterface(void);
void ShowEnemyLife(char *name,byte formerLife,byte life,byte alive);
void UpdateInterface(Map *map);
void RenderInterface(MGLDraw *mgl);
void RenderCollectedStuff(int x,int y,MGLDraw *mgl);

#endif
