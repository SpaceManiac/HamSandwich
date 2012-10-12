#ifndef CUSTOMWORLD_H
#define CUSTOMWORLD_H

#include "mgldraw.h"
#include "player.h"
#include "pause.h"

byte IsCustomWorld(void);	// 0=hollow.shw, 1=other
void InitCustomWorld(void); // load stuff

const char* CustomWorldTitle(const char* fname); // NULL for current world
const char* CustomWorldFname(void);
char* CustomJournalPage(int p);
char* CustomGoalInfo(byte goal, byte info); // 0=name, 1=description
invItem_t* CustomInventory(); // NULL if none specified

#endif // CUSTOMWORLD_H
