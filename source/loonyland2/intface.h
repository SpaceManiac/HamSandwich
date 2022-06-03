#ifndef INTFACE_H
#define INTFACE_H

#include "winpch.h"
#include "jamulspr.h"
#include "display.h"
#include "mgldraw.h"

void InitInterface(void);
void ExitInterface(void);
void ResetInterface(void);
void ShowEnemyLife(char *name,byte formerLife,byte life,byte alive);
void UpdateInterface(void);
void RenderInterface(MGLDraw *mgl);
void RenderIntfaceSprite(int x,int y,int spr,char bright,MGLDraw *mgl);
sprite_t *GetIntfaceSprite(int spr);

#endif
