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
#define GOAL_COOK	15 // cook at least N% quality
#define GOAL_KILLONE 16 // kill one enemy of type N
#define GOAL_ECTOPLASM 17 // collect N ectoplasm at once
#define GOAL_GHOSTSNOSPELLS 18 // catch all ghosts without spells

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
#define CE_SMASHBALL 11 // hit a wall in smashball
#define CE_COOK		12 // cooked at quality level N%
#define CE_ECTOPLASM 13 // collected N ectoplasm

#define HOURGLASS_BONUS (20)	// percent of time the hourglass grants you

#define MAX_CHALLENGE (80)
#define TRIVIA_SLOT		(79)	// this slot is where we store whether trivia is turned on or not

#define MAX_COMBO_TIME (60)

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
	word flags;
	dword time;
	goal_t goal[5];
} challenge_t;

typedef struct chalData_t
{
	dword topScore[MAX_CHALLENGE];
	dword topTime[MAX_CHALLENGE];
	word topCombo[MAX_CHALLENGE];
	byte goal[MAX_CHALLENGE][5];
	byte bought[MAX_CHALLENGE];
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
void PauseChallengeRender(int offX);
void CheatChal(void);
void LoadChallengePercents(byte pcts[4]);

#endif
