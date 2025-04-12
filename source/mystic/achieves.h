#ifndef ACHIEVES_H
#define ACHIEVES_H

#include "mgldraw.h"

enum class Achievement :byte
{
	WINCLASSIC,
	WINMODERN,
	WINBRUTAL,
	WINMADCAP,
	MASTER_MADCAP,
	WIN_CHALLENGE,
	MASTER_CHALLENGE,
	MAX_RUNES,
	ALL_FAIRIES,
	GETSWORD,
	LEVEL50,
	ENERGYBARRAGE,
	DRAGONFLAME,
	SEEKER,
	ICEBLAST,
	INFERNO,
	SUMMON,
	STONESKIN,
	BERSERK,
	HEAL,
	CHEF,
	SKILLS,
	WHACKAZOID,
	DELIVERY,
	NUM_ACHIEVES,
	NONE,
};

enum class CheatStone :byte
{
	HAPPYSTICK,	// replace all overworld enemies with happy sticks
	STAMANA,	// mana regenerates really fast
	PICKPOCKET,	// get cash by hitting guys
	NONRANDOM,	// no random battles!
	FIREUP,		// start with fireballs in every level
	REBIRTH,	// unlimited resurrection
	NUM_CHEATSTONES,
};

enum
{
	CHEATSTONE_LOCKED,	// haven't earned it
	CHEATSTONE_OFF,
	CHEATSTONE_ON,
};

class AchieveDef
{
	public:
		char name[32];
		byte icon;
		char hint[128];
		char desc[128];
};

class CheatStoneDef
{
	public:
		char name[32];
		char desc[128];
};

void InitAchieves(void);
void ExitAchieves(void);
TASK(void) AchieveMenu(MGLDraw *mgl);

void InitAchieveSystem(void);
void EarnAchieve(Achievement n);
void UpdateAchieves(void);
void RenderAchieves(void);
bool CheatStoneCheck(void);

#endif
