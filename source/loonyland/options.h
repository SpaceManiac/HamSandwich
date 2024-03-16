#ifndef OPTIONS_H
#define OPTIONS_H

#include "jamultypes.h"
#include "mgldraw.h"
#include "highscore.h"

#define DIFF_BEGINNER	0
#define DIFF_NORMAL		1
#define DIFF_CHALLENGE	2
#define DIFF_MAD		3
#define DIFF_LOONY		4

#define DIFF_HARD		5

// special cheats
#define CH_LIGHT	0		// always see everything
#define CH_DISCO	1		// disco mode
#define CH_TERROR   2		// terror mode!
#define CH_SURPRISE 3		// surprise mode (random enemies)
#define CH_QUICK	4		// quick mode (double speed player)
#define CH_GEMS		5		// infinite gems
#define CH_SIDEKICK 6		// sidekick (farley always present)
#define CH_HOMING   7		// homing shots
#define CH_MAXPOWER 8		// ultimate firepower cheat
#define CH_VINTAGE	9		// vintage mode
#define CH_BONKULA	10		// play as bonkula
#define CH_TOAD		11		// play as toad
#define CH_CRYSTAL	12		// crystal ball
#define CH_RADAR	13		// radar
#define CH_HEAVYHIT 14		// heavy hitter (all powerups boost damage only)
#define CH_RAPIDFIRE 15		// rapid fire (start with all arrows)
#define CH_DEATH	16		// touch of death (instant kill always)
#define CH_ALLACCESS 17		// all access pass (all keys, etc)
#define CH_SWAMPDOG 18		// play as swampdog
#define CH_HARDCORE 19		// hardcore mode (can't save, die and your game is gone)
#define CH_SAVEANY  20		// save anywhere
#define CH_GUIDED	21		// guided shots
#define CH_SLOMO	22		// slow monsters
#define CH_CORPSE	23		// exploding corpses!
#define CH_FROGWPN  24		// frog weapon!
#define CH_WITCH	25		// play as stick witch
#define CH_WEREWOLF 26		// play as werewolf
#define CH_RESPAWN	27		// respawning enemies
#define CH_KICKCAT	28		// kick the cat!
#define CH_RNDSURVIVAL 29	// Infinite Survival
#define CH_SUMMON	30		// play as summony
#define CH_THIEF	31		// play as thief
#define CH_WALLWALK	32		// walk through walls
#define CH_SUPERSURV 33		// ultimate survival mode
#define CH_BEND		34		// bend it like bouapha
#define CH_STINKY	35		// stinky farley
#define CH_ULTRAWEAPON 36	// ultra weapons
#define CH_THORNS	37		// thorns aura
#define CH_REGEN	38		// monster regeneration
#define CH_NOFARLEY 39		// no farley

enum PlayerCharacterType : int
{
	PC_Loony = 0,
	PC_Bonkula = 1,
	PC_Toad = 2,
	PC_Swampdog = 3,
	PC_Witch = 4,
	PC_Werewolf = 5,
	PC_Summon = 6,
	PC_Thief = 7,
	PC_MAX
};

typedef struct options_t
{
	byte control[2][6];	// key scancodes
	byte joyCtrl[2];	// joystick 'codes' for the buttons
	byte sound,music;
	byte cheats[40];	// which special cheats are on
	byte meritBadge[40];
	byte difficulty;
	byte modes[5];
	byte helpOn;
	byte wpnLock;
	byte bossDead[10];	// have killed each boss
	byte expando[9];	// room for expansion

	// best scores
	highScore_t score[9][15];	// high scores in different modes
} options_t;

extern options_t opt;

void LoadOptions(void);
void SaveOptions(void);

TASK(void) OptionsMenu(MGLDraw *mgl);
void KilledBoss(byte boss);

bool IsAnyCharacterUnlocked();
void NextCharacter();
void PrevCharacter();
void ResetCharacterCheats();
PlayerCharacterType GetCurrentPC();

#endif
