#ifndef CUSTOMWORLD_H
#define CUSTOMWORLD_H

#include "mgldraw.h"
#include "player.h"

byte IsCustomWorld(void);	// 0=hollow.shw, 1=other
void InitCustomWorld(void); // load stuff

const char* CustomWorldTitle(const char* fname); // NULL for current world
const char* CustomWorldFname(void);
char* CustomJournalPage(int p);
char* CustomGoalInfo(byte goal, byte info); // 0=name, 1=description

#endif // CUSTOMWORLD_H
