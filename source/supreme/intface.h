#ifndef INTFACE_H
#define INTFACE_H

#include "jamulspr.h"
#include "display.h"
#include "mgldraw.h"

void InitInterface(void);
void ExitInterface(void);
void ResetInterface(void);
void RenderRage(byte size,MGLDraw *mgl);
void ShowEnemyLife(char *name,byte formerLife,byte life,byte alive);
void SetBossBar(char *name);
void UpdateInterface(Map *map);
void RenderInterface(MGLDraw *mgl);
void RenderCollectedStuff(int x,int y,MGLDraw *mgl);

#endif
