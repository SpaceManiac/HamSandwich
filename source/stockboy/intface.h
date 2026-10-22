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
void UpdateInterface(byte life);
void RenderInterface(MGLDraw *mgl);
char *GetAreaName(byte a);
void RenderIntfaceSprite(int x,int y,int spr,char bright,MGLDraw *mgl);
void AddRadarBlip(int x,int y,byte c,Map *map);
void CleanRadar(void);
sprite_t *GetIntfaceSprite(int spr);
void SetBossHP(int hp,int max);

#endif