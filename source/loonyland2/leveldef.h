#ifndef LEVELDEF_H
#define LEVELDEF_H

#include "winpch.h"

// the levels
#define LVL_EISBURG		(0)
#define LVL_COZYCABIN	(1)
#define LVL_MICKEYCABIN	(2)
#define LVL_MICKEYCELLAR (3)
#define LVL_WOODS		(4)
#define LVL_WOODYCABIN	(5)
#define LVL_TUNDRA		(6)
#define LVL_TUNDRACAVE	(7)
#define LVL_AXESRUS		(8)
#define LVL_GURUCABIN	(9)
#define LVL_COZYCAVE	(10)
#define LVL_STORAGECABIN (11)
#define LVL_BLACKMARKET (12)
#define LVL_SOUTHWOOD	(13)
#define LVL_KLONKCABIN	(14)
#define LVL_ARENALOBBY	(15)
#define LVL_ARENA1		(16)	// small
#define LVL_ARENA2		(17)	// maze
#define LVL_ICYLAKE		(18)
#define LVL_ROCKMAZE	(19)
#define LVL_BASKERVILLE (20)
#define LVL_BASKERHOUSE (21)	// hank's cabin
#define LVL_FJORD		(22)
#define LVL_EASTWOOD	(23)
#define LVL_BASKERHOUSE2 (24)	// empty cabin
#define LVL_BASKERHOUSE3 (25)	// abandoned cabin
#define LVL_BASKERCAVE	(26)	// under Baskerville
#define LVL_HISTORY		(27)
#define LVL_LAKE		(28)
#define LVL_CLIFFS		(29)
#define LVL_ROPECABIN	(30)
#define LVL_TOYFACTORY	(31)
#define LVL_TOYSTORY	(32)
#define LVL_TOYOFFICE	(33)
#define LVL_TOYBASEMENT (34)
#define LVL_ARMSCAVE	(35)
#define LVL_CLIFFCAVE	(36)
#define LVL_CRAMPEDCAVE	(37)
#define LVL_ARENA3		(38)	// icy arena
#define LVL_ARENA4		(39)	// wet arena
#define LVL_ARENA5		(40)	// 'capital' arena
#define LVL_WESTWOOD	(41)
#define LVL_ONIONCAMP	(42)
#define LVL_HIGHWAY		(43)
#define LVL_NORTHWOOD	(44)
#define LVL_CLEARING	(45)
#define LVL_SNUGGLYHQ	(46)
#define LVL_CAPTAIN		(47)
#define LVL_GEYSER		(48)
#define LVL_SHROOMCAMP	(49)
#define LVL_CRYSTAL		(50)
#define LVL_GARDEN		(51)
#define LVL_FOREST		(52)
#define LVL_PIT			(53)
#define LVL_FURNACE		(54)
#define LVL_TITANS		(55)
#define LVL_GORGE		(56)
#define LVL_MTNCABIN	(57)
#define LVL_CASTLE1		(58)
#define LVL_THRONE		(59)
#define LVL_CASTLE2		(60)
#define LVL_ASCENT		(61)
#define LVL_PEAK		(62)	// town before final fight
#define LVL_LOOKDOWN	(63)

#define NUM_LEVELS		(64)

// level flags
#define LF_TOWN		(1)
#define LF_ARENA	(2)	// Arena rules - enemies don't drop items, just money

#pragma pack(push, 1)
typedef struct levelDef_t
{
	dword flags;
	byte badguyType[5];
	byte badguyCount;
	byte herbCount[7];
	byte song;
} levelDef_t;
#pragma pack(pop)

extern levelDef_t levelDef[NUM_LEVELS];

void ResetLevelDefs(void);
void LoadLevelDefs(char *name);
char *LevelError(void);

#endif
