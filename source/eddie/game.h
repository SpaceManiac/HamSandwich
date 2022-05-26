#ifndef GAME_H
#define GAME_H

#include "jamultypes.h"
#include "mgldraw.h"
#include "display.h"
#include "control.h"
#include "player.h"
#include "guy.h"
#include "backgd.h"
#include "particle.h"

#define TIME_PER_FRAME (1000/30)

void CentipedeInit(MGLDraw *mgl);
void CentipedeExit(void);

void GameLevelSetup(char *name,byte level);

TASK(byte) CentipedeGame(void);
byte CentipedeRun(int *lastTime);
TASK(byte) CentipedeDraw(void);

void StopPausing(void);
void ExitGame(void);
void GameOver(void);

void CheatWin(void);
byte NaughtyTime(void);

const int FIXSHIFT = 8;
const int FIXAMT = 256;

class MGLDraw;
extern MGLDraw* mainmgl;

#endif
