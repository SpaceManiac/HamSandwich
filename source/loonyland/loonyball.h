#ifndef LOONYBALL_H
#define LOONYBALL_H

#include "game.h"
#include "map.h"
#include "bullet.h"
#include "guy.h"

void InitLoonyball(Map *map);
void ScoreGoal(bullet_t *me,Map *map);
void UpdateLoonyball(void);
void GoToPenaltyBox(Guy *me,Map *map);

#endif
