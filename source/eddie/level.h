#ifndef LEVEL_H
#define LEVEL_H

#include "jamultypes.h"
#include "coro.h"

// these are the worlds
#define WORLD_EARTH	1
#define WORLD_RIGEL 2
#define WORLD_AGUA	3
#define WORLD_PROMETHEUS 4
#define WORLD_TARTARUS 5
#define WORLD_ASTEROID 6
#define WORLD_VARAKKIS 7
#define WORLD_JURASSIC 8
#define WORLD_GLOBULUS 9
#define WORLD_FINAL	   10
#define NUM_WORLDS	   10

// these are the events that gameplay may tell the level about
#define EVT_MONSDIE		0	// a monster died
#define EVT_FIRE		1	// player fired a shot
#define EVT_GET			2   // player got a present
#define EVT_MONSGONE	3	// a monster left the level without dying

// these are the level modes
#define LVL_BATTLE		0	// the level ends when all GF_VICTORY creatures are dead
#define LVL_TIMED		1	// the level ends when the timer runs out (you win then)

// these are the bonus level types
#define BNS_ASTEROID	1
#define BNS_HORDE		2
#define BNS_HOSTILE		3

typedef void (*levelInitProc)(void);

typedef struct levelDef_t
{
	char backgd[32];
	byte backscroll;
	byte gravity;
	dword lvlclock;		// how long, if it's timed.  If 0, it's a battle level
	char cent1x;
	char cent2x;		// x-coords of centipedes to be added
	int  cent1speed;
	int  cent2speed;
	byte cent1size;
	byte cent2size;		// size of 0 means no pede
	byte numShrooms;
	byte shroomType;
	levelInitProc levelInit;	// if non-NULL, gets called to do special initting
	int  clock[4];
	byte value[2];
} levelDef_t;

typedef struct worldDef_t
{
	char name[32];
	levelDef_t level[5];
} worldDef_t;

TASK(void) SetupLevel(byte lvl);
void UpdateLevel(void);
void TellLevelAboutIt(byte event,byte value);
byte IsBonusStage(void);
byte LevelMode(void);
byte TimeIsUp(void);
void StartTicking(int clock);
byte GetLIndex(void);

// special inits
// these are the bonus stages
void InitAsteroidLevel(void);
void InitHordeLevel(void);
void InitTerritoryLevel(void);
// these are regular stages that require special inits
void InitPrometheusLevel(void);
void InitTartarusLevel(void);
void InitOtherAsteroidLevel(void);
void InitJurassicLevel(void);
void InitGlobLevel(void);
void InitAlienQueen(void);

#endif
