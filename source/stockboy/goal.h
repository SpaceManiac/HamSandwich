#ifndef GOAL_H
#define GOAL_H

#include "mgldraw.h"

// level goals are defined like this:
// GOALTYPE X GOALMODIFIER
//
// e.g.
// push barrels, 10, red
//   - means you must get 10 red barrels onto red colored spaces to finish

// level goals
#define GOAL_NONE		0	// no goal (use this for anti-goal - if you use it for goal, you win instantly!)
#define GOAL_LIVE		1	// just make it until time runs out, and don't do the antigoal
#define GOAL_PUSH		2	// push objects onto colored spaces
#define GOAL_EXPLODE	3	// destroy objects
#define	GOAL_KILL		4	// kill monsters
#define GOAL_EXIT		5	// get to the exit
#define GOAL_SAVE		6	// get monsters to exit
#define GOAL_SCORE		7	// earn points
#define GOAL_DUMP		8	// send objects into pits or holes
#define GOAL_DUMPMONS	9	// send monsters into pits or holes
#define GOAL_PENCIL		10	// collect pencils
#define GOAL_NOPENCIL	11	// destroy pencils
// add goal_combo!
#define NUM_GOALS		12

// goal modifiers
#define GM_ALL			0	// must do 'all' (destroy all objects, kill all monsters, etc)
#define	GM_MONS1		1	// for kill monsters only: just type 1 monsters
#define	GM_MONS2		2	// for kill monsters only: just type 2 monsters
#define	GM_MONS3		3	// for kill monsters only: just type 3 monsters
#define	GM_MONS4		4	// for kill monsters only: just type 4 monsters
#define GM_COLOR0		5	// rather than all, just referring to all of color 0
#define GM_COLOR1		6	// rather than all, just referring to all of color 1
#define GM_COLOR2		7	// rather than all, just referring to all of color 2
#define GM_COLOR3		8	// rather than all, just referring to all of color 3
#define GM_COLOR4		9	// rather than all, just referring to all of color 4
#define GM_COLOR5		10  // rather than all, just referring to all of color 5
#define GM_COLOR6		11	// rather than all, just referring to all of color 6
#define GM_COLOR7		12	// rather than all, just referring to all of color 7
#define GM_X1			13	// for score goals, earn score X 1
#define GM_X10			14	// for score goals, earn score X 10
#define GM_X100			15	// for score goals, earn score X 100
#define GM_X1000		16	// for score goals, earn score X 1000

// modifier types
#define GMT_NONE		0
#define GMT_MONSTER		1
#define GMT_OBJECT		2
#define GMT_POINTS		3

typedef struct goal_t
{
	byte type;
	byte number;
	byte modifier;
} goal_t;

const char *GoalColorName(byte mod);
const char *CreateGoalString(goal_t goal);
const char *EditGoalName(goal_t goal);
const char *EditGoalEnding(goal_t goal);
byte GoalModType(goal_t goal);
byte GoalHasNumber(goal_t goal);
void ChangeGoalType(goal_t *goal,byte noneok);
void ChangeGoalMod(goal_t *goal);
void PrintGoal(goal_t goal,goal_t antigoal,int timer);

class Map;

byte GoalIsMet(goal_t *goal,Map *map);
byte GoalProgress(goal_t *goal,Map *map);

#endif
