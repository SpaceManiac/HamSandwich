#ifndef BADGE_H
#define BADGE_H

#include "mgldraw.h"
#include "game.h"
#include "map.h"

#define BADGE_EVILIZER	0
#define BADGE_BOWLING	1
#define BADGE_GRAVES	2
#define BADGE_GEMS		3
#define BADGE_MONSTERPTS 4
#define BADGE_MONSTERPTS2 5
#define BADGE_POLTERGUY	6
#define BADGE_DARKNESS	7
#define BADGE_BESTRANK	8
#define BADGE_LOONYBALL	9
#define BADGE_BOSSBASH	10
#define BADGE_BONKULA	11
#define BADGE_SNEAK		12
#define BADGE_WITCH		13
#define BADGE_WOLFDEN	14
#define BADGE_ANNOY		15
#define BADGE_RESCUE	16
#define BADGE_WINGAME	17
#define BADGE_COMBO		18
#define BADGE_ROCKS		19
#define BADGE_HIDDEN	20
#define BADGE_TERRORSRV	21
#define BADGE_KILLALL	22
#define BADGE_HUNDREDPCT 23
#define BADGE_SURVIVAL	24
#define BADGE_LARRY		25
#define BADGE_TERROR	26
#define BADGE_FRANKENJ	27
#define BADGE_SURVCOMBO	28
#define BADGE_CATTAILS	29
#define BADGE_REMIX		30
#define BADGE_WITCHCRAFT 31
#define BADGE_THIEF		32
#define BADGE_SURV100	33
#define BADGE_RIOT		34
#define BADGE_SWAMPDOG	35
#define BADGE_SCORE		36
#define BADGE_BRAWL		37
#define BADGE_HIDDEN2	38
#define BADGE_MASTER	39

#define NUM_BADGES		40

#define RULE_NEWGAME	1	// cheat affects new games only
#define RULE_ADVENTURE	2	// cheat only affects adventure mode
#define RULE_PLAYAS		4	// "only one play as can be on at once"


// events that can trigger badges
#define BE_KILL		0	// killed a monster
#define BE_DESTROY	1	// smashed a breakable item
#define BE_GETITEM	2	// got an item
#define BE_ENDGAME	3	// finished a game
#define BE_GEMSUP	4	// got some gems
#define BE_COMBO	5	// when you get a combo
#define	BE_VARSET	6	// any variable gets set
#define BE_GOTOMAP	7	// change maps

typedef struct badge_t
{
	char name[20];
	char howGet[2][55];
	byte cheatNum;
	char cheatName[40];
	char cheatDesc[8][55];
	byte rules;
} badge_t;

void BadgeMenu(MGLDraw *mgl);
void BadgeCheck(byte event,int value,Map *map);
void ShowBadge(byte b);
void ShowGameMode(byte mode,byte numBadges);
void EarnBadge(byte b);

#endif
