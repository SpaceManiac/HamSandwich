#ifndef PUSHING_H
#define PUSHING_H

#include "items.h"
#include "map.h"
#include "guy.h"

byte CanBePushedTo(int fromX,int fromY,int toX,int toY,Map *map);
void BlowUpNeighbors(Map *map,int x,int y,byte color);
byte TurnCrank(Map *map,mapTile_t *me);
byte PushSwitch(Map *map,mapTile_t *me);
byte PushObject(int x,int y,Guy *me,mapTile_t *m,Map *curMap);
void LaunchBall(int x,int y,int dx,int dy);
void DetonateBomb(int x,int y,Map *map);
byte DetonateAllBombs(byte color,Map *map);
void FireLaser(int x,int y,byte facing,byte color,Map *map);

char GetDX(int facing);
char GetDY(int facing);

#endif