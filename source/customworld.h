#ifndef CUSTOMWORLD_H
#define CUSTOMWORLD_H

#include "mgldraw.h"
#include "player.h"
#include "pause.h"

byte IsCustomWorld(void);	// 0=hollow.shw, 1=other
void InitCustomWorld(void); // load stuff
byte VerifyHollowShw(void); // make sure hollow.shw hasn't been tampered with

const char* CustomWorldTitle(const char* fname); // NULL for current world
const char* CustomWorldFname(void);
char* CustomJournalPage(int p);
char* CustomGoalInfo(byte goal, byte info); // 0=name, 1=description
invItem_t* CustomInventory(); // NULL if none specified

char* CustomFishName(byte type);
float CustomFishSize(byte type);
byte CustomFishEffect(byte type);
byte CustomFishWhich(byte map);
byte CustomFishDiff(byte map);

#endif // CUSTOMWORLD_H
