#ifndef PAUSE_H
#define PAUSE_H

#include "display.h"

#define PAUSE_CONTINUE	0
#define PAUSE_WORLDSEL	1
#define PAUSE_GIVEUP	2
#define PAUSE_RETRY		3
#define PAUSE_PAUSED	4
#define PAUSE_EXIT		5
#define PAUSE_SHOP		6

#define INVT_PANTS		0
#define INVT_HAMMERS	1
#define INVT_MYSTIC		2
#define INVT_BRAINS		3
#define INVT_CANDLES	4
#define INVT_JOURNAL	5
#define INVT_HFLAGS		6
#define INVT_ABILITY	7
#define INVT_KEYS		8
#define INVT_LVAR		9
#define INVT_GVAR		10

// Inventory stuff
typedef struct invItem_t {
	byte position;
	byte item;

	byte type, subtype;
	int max;

	char name[32];
	char desc[64];

	int dx, dy;
} invItem_t;

struct player_t;

void UpdateUnpaused(void);
void RenderUnpaused(void);

void RenderPauseMenu(void);
byte UpdatePauseMenu(MGLDraw *mgl);
void InitPauseMenu(void);
void ExitPauseMenu(void);

void ChangeWeapon(char dir);

#endif
