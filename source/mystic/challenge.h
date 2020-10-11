#ifndef CHALLENGE_H
#define CHALLENGE_H

#include "mgldraw.h"
#include "player.h"

#define GOAL_NONE	0
#define GOAL_FINISH	1	// complete before time is up
#define GOAL_FINISH2 2	// complete in less than N time (some amount shorter than the time limit)
#define GOAL_KILL	3	// kill all badguys
#define GOAL_KILLSOME 4	// kill all of type N
#define GOAL_KILLBUT 5	// kill all except type N
#define GOAL_SCORE	6	// achieve a score of N
#define GOAL_MYSTIC	7	// spell MYSTIC
#define GOAL_NOSHOOT 8	// complete without firing fireballs
#define GOAL_NOSPELL 9	// complete without casting spells
#define GOAL_NOHIT	10	// complete without getting hit
#define GOAL_COMBO	11	// get a combo of N
#define GOAL_CRYSTALS 12	// get all challenge crystals
#define GOAL_NOKILL	13	// don't kill any of monster N
#define GOAL_GETALL 14	// get all the other goals in one run

// challenge events
#define CE_NONE		0	// nothing happened
#define CE_KILL		1	// killed a badguy of type N
#define CE_SHOOT	2	// fire a fireball (N=# of fireballs at once)
#define CE_SPELL	3	// cast a spell for N mana
#define CE_OUCH		4	// get for N damage
#define CE_HIT		5	// hit an enemy for N damage
#define CE_DIE		6	// die
#define CE_WIN		7	// finish the level
#define CE_GET		8	// get an item #N
#define CE_POINTS	9	// earn N XP
#define CE_QUIT		10	// chose to quit the game, so don't tally or even go back to challenge menu

typedef struct goal_t
{
	byte type;
	dword n;
} goal_t;

typedef struct challenge_t
{
	byte chapter,level;
	byte starCost;
	char name[32];
	dword time;
	goal_t goal[5];
} challenge_t;

typedef struct chalData_t
{
	dword topScore[64];
	dword topTime[64];
	word topCombo[64];
	byte goal[64][5];
	byte bought[64];
	byte stars,totalStars;
	player_t player;
} chalData_t;

typedef struct attempt_t
{
	int score;
	dword time;
	dword kills;
	byte comboClock;
	word curCombo,bestCombo;
	byte curLetter;
	dword letterClock;
	byte goalOk[5];
	dword letterMaxClock;
	byte quit;
} attempt_t;

TASK(byte) ChallengeMenu(MGLDraw *mgl);
byte Challenging(void);
void ChallengeEvent(byte type,int n);
byte ChallengeUpdate(void);
void ChallengeRender(MGLDraw *mgl);
TASK(void) ChallengeTally(MGLDraw *mgl);
byte CurrentLetter(void);
byte DoTrivia(void);

void CheatChal(void);


#endif
