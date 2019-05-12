#ifndef BOWLING_H
#define BOWLING_H

#include "game.h"
#include "bullet.h"

void InitBowling(Map *map);
void SetupLane(Map *map);
void BowlingBallIsDone(void);
void ThrowBowlingBall(void);
void RenderBowlingIntface(MGLDraw *mgl);
void UpdateBowling(void);
void GutterBall(void);

#endif
