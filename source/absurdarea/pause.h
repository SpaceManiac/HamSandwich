#ifndef PAUSE_H
#define PAUSE_H

#include "winpch.h"
#include "display.h"
#pragma pack(1)

#define PAUSE_RESUME	(0)
#define PAUSE_ITEMS		(1)
#define PAUSE_SKILLS	(2)
#define PAUSE_TALENTS	(3)
#define PAUSE_QUESTS	(4)
#define PAUSE_MESSAGES	(5)
#define PAUSE_MADCAP	(6)
#define PAUSE_TIPS		(7)
#define PAUSE_OPTIONS	(8)
#define PAUSE_QUIT		(9)
#define PAUSE_CHOICES	(10)

#define PAUSE_PAUSED	(255)

#define SUBMODE_NONE	 (0)
#define SUBMODE_SLOTPICK (1)
#define SUBMODE_EQUIP	 (2)
#define SUBMODE_SKILL	 (3)
#define SUBMODE_QUEST	 (4)
#define SUBMODE_TALENT	 (5)
#define SUBMODE_EQUIPYESNO (6)
#define SUBMODE_MADCAP	(7)
#define SUBMODE_OPTIONS	(8)
#define SUBMODE_SETKEY	(9)
#define SUBMODE_SETBTN	(10)
#define SUBMODE_TIPS	(11)

struct player_t;

void RenderPauseMenu(MGLDraw *mgl);
byte UpdatePauseMenu(MGLDraw *mgl);
void InitPauseMenu(void);
void SetSubCursor(byte s);
void LoadGame(void);
void SaveGame(void);

void PauseBox(int x,int y,int x2,int y2,byte c);
void RenderInvItem(int x,int y,byte type,byte count,byte on);
void RenderUnusableInvItem(int x,int y,byte type,byte count,byte on);

#endif
