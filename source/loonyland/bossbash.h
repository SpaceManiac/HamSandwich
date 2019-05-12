#ifndef BOSSBASH_H
#define BOSSBASH_H

#include "game.h"

// boss numbers for tracking if they can be selected in bash
#define BOSS_FRANK	0
#define BOSS_LARRY	1
#define BOSS_GHOST  2
#define BOSS_ELDER1	3
#define BOSS_ELDER2	4
#define BOSS_ELDER3	5
#define BOSS_ELDER4	6
#define BOSS_BONKULA 7
#define BOSS_EVILIZER 8
#define BOSS_WITCH	9

byte BossMenu(MGLDraw *mgl);
byte BashPower(void);
void UpdateBash(void);
char *BashPowerName(byte lvl);
char *BashLevelName(byte lvl);

#endif
