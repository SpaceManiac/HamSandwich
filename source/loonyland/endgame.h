#ifndef ENDGAME_H
#define ENDGAME_H

#include "mgldraw.h"
#include "player.h"
#include "highscore.h"

TASK(void) EndGameTally(MGLDraw *mgl);
void ShowHighScore(int x,int y,int score,byte on,byte col,MGLDraw *mgl);
void ShowScoreStats(int x,int y,highScore_t *me,MGLDraw *mgl);

#endif
