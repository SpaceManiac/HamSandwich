#ifndef FLOOR_H
#define FLOOR_H

#include "map.h"

#define NOT_AN_ARROW	8
#define ANY_COLOR		8
#define NOT_PRESSURE_PLATE 0
#define PRESSURE_UP		1
#define PRESSURE_DOWN	2

// floor classes
#define FLR_OPEN	0
#define FLR_WALL	1
#define FLR_ARROW	2
#define FLR_SPIKE	3
#define FLR_COLOR	4
#define FLR_PRESSURE 5
#define FLR_HOLE	6
#define FLR_PIT		7
#define FLR_DOOR	8
#define FLR_TELEPORT 9
#define FLR_SUMMON1	10
#define FLR_SUMMON2	11
#define FLR_SUMMON3	12
#define FLR_SUMMON4	13
#define FLR_EXIT	14
#define FLR_MAX		15

byte FloorIsSolid(word flr);
byte FloorArrowFacing(word flr);
byte FloorOnlyColor(word flr);
byte PressurePlateState(word flr);
byte FloorIsHole(word flr);
byte FloorIsPit(word flr);
byte FloorCanWalk(word flr);
byte FloorIsTeleporter(word flr);
byte FloorIsSpikes(word flr);
byte FloorIsSummoner(word flr);
byte FloorIsExit(word flr);

word GetTileNumber(byte flrClass,byte color,byte facing);
void FloorTurnArrow(word *flr);

#endif