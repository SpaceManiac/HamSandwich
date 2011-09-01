#ifndef INTFACE_H
#define INTFACE_H

#include "winpch.h"
#include "jamulspr.h"
#include "display.h"
#include "mgldraw.h"

void InitInterface(void);
void ExitInterface(void);
void ResetInterface(void);
void RenderRage(byte size,MGLDraw *mgl);
void ShowEnemyLife(char *name,byte formerLife,byte life,byte alive);
void RenderInterface(byte life,byte rage,byte hmrFlags,byte hammers,int brains,int score,byte wpn,int ammo,
					 byte hamSpeed,MGLDraw *mgl);

#endif
