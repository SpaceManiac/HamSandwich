#ifndef HELP_H
#define HELP_H

#include "game.h"

#define TIP_BEGIN	1	// the game started
#define TIP_QUEST	2	// first quest obtained
#define TIP_GO		3	// went to a new map N
#define TIP_SKILL	4	// gained a skill
#define TIP_DIE		5	// died
#define TIP_THROWING 6	// put points into Throwing
#define TIP_STAMINA 7	// ran low on stamina
#define TIP_EQUIP	8	// got equipment
#define TIP_POTION  9	// got a potion
#define TIP_QUIT	10	// save & exit (you paused)
#define TIP_GO2		11	// enter town
#define TIP_MAGIC	12	// magic item
#define TIP_SHOVEL	13	// got the shovel
#define TIP_GLASSES 14	// got magic glasses
#define TIP_LEVELUP 15	// gained a level
#define TIP_SPELL	16	// put points in a spell
#define TIP_CLOCKWORK 17
#define TIP_JUNKSMITH 18
#define TIP_SYNERGY 19	// have 2 spells of same school
#define TIP_ENDING	20	// entered peak cave

void TipEvent(byte eventType);
byte UpdateHelp(MGLDraw *mgl);
void RenderHelp(MGLDraw *mgl);

void PauseShowTip(byte help,int x,int y);
char *PauseTipName(byte tip);

#endif
