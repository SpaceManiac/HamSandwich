#ifndef ARENA_H
#define ARENA_H

#include "mgldraw.h"
#include "map.h"
#pragma pack(1)

// types of battles
#define AR_BRAWL	(0)	// just a bunch of monsters, kill them all
#define AR_WAVES	(1)	// once you kill all monsters, more come.  Up to 10 waves.  Kill them all
#define AR_SURVIVE  (2)	// wave 1 appears initially, then the enemies of wave 2 come endlessly, a new set every spawnRate.  Stay alive for timeLimit
#define AR_PRESSURE (3)	// same as Waves, but the waves come once every spawnRate, no matter what.  So kill fast or you will
						// be overwhelmed
#define AR_COLLECT	(4)	// collect all the Souvenir Swords.  Waves arrive Pressure style.  If time doesn't run out, waves wrap around
#define AR_SLAY		(5)	// kill as many guys as you can, you get the prize amount for each kill.  Same rules as survive otherwise

// flags
#define AF_RANDOM	(1)	// enemies randomly pick spawn points.  If false, they are added in a linear order
#define AF_PERFECT	(2)	// you lose if you ever get hit at all
#define AF_POISON	(4)	// you spend the entire match poisoned
#define AF_HEALING	(8)	// enemies heal 1 health every 1/2 second
#define AF_TEAM		(16)	// the last two slots in each wave's list will be on your team instead of enemy team
#define AF_PROTECT	(32)	// adds an Arena Crystal, and if it dies, you lose

#define MAX_PER_WAVE (5)
#define MAX_WAVES	(10)

#ifdef DIRECTORS
#define MAX_MATCHES	 (60)
#else
#define MAX_MATCHES	 (55)
#endif
#pragma pack(1)

typedef struct waveDef_t
{
	byte count;
	byte type;
} waveDef_t;

typedef struct arenaMatch_t
{
	char name[32];
	char desc[2][64];
	byte rank;	// gladiator rank required to enter
	word var;	// variable that must be true for this fight to be playable
	word cost;
	word prize;	// cost to enter, prize for winning
	byte rules;
	byte map;
	word flags;
	word timeLimit;
	word spawnRate;	// how often new waves appear, or new guys, depending on mode
	byte waveCnt;	// just for display purposes
	waveDef_t waves[MAX_WAVES][MAX_PER_WAVE];
} arenaMatch_t;

extern arenaMatch_t arenaMatch[MAX_MATCHES];

void SetupArena(Map *map);
void UpdateArena(void);
void PrintArenaInfo(int y);
void LoseArena(void);
void ArenaKill(byte type,int x,int y);

#endif
