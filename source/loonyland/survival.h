#ifndef SURVIVAL_H
#define SURVIVAL_H

#include "jamultypes.h"
#include "guy.h"

void InitSurvival(void);
void AddSurvivalMapGuys(Map *map);
void SurvivalBeginMsg(void);
byte SurvivalWin(Map *map);
byte SurvivalOver(void);
byte GetSurvLevel(void);

#endif
