#ifndef INTFACE_H
#define INTFACE_H

#include "winpch.h"
#include "jamulspr.h"
#include "display.h"

void InitInterface(void);
void ExitInterface(void);
void ResetInterface(void);
void ShowEnemyLife(char *name,byte formerLife,byte life,byte alive);
void RenderInterface(byte life,byte hmrFlags,byte hammers,int brains,int score,byte wpn,byte ammo,
					 byte hamSpeed,MGLDraw *mgl);
void UpdateInterface(byte life,byte hmrFlags,byte hammers,int brains,int score,byte wpn,byte ammo,
					 byte hamSpeed);

#endif