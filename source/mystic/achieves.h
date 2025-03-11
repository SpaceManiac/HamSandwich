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
	ACH8,
	ACH9,
	ACH10,
	ACH11,
	ACH12,
	ACH13,
	ACH14,
	ACH15,
	ACH16,
	ACH17,
	ACH18,
	ACH19,
	ACH20,
	NUM_ACHIEVES,
};

enum class Progress :byte
{
	ONE_SHOT,	// you just get it when you do the thing

};

class AchieveDef
{
	public:
		char name[32];
		char hint[128];
		char desc[128];

		Progress progMethod;
};

void InitAchieves(void);
void ExitAchieves(void);
TASK(void) AchieveMenu(MGLDraw *mgl);

#endif
