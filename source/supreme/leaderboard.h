#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "display.h"

enum class WorldLeaderboardKind
{
	Score,
	Time,
};

struct world_t;
TASK(void) ViewWorldLeaderboard(MGLDraw *mgl, const world_t *world, WorldLeaderboardKind kind, uint64_t id);

#endif
