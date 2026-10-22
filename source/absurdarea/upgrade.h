#ifndef UPGRADE_H
#define UPGRADE_H

#include "winpch.h"

#pragma pack(1)

// ABSURD AREA NUMS
// trial numbers
#define TRIAL_DUEL	(0)
#define TRIAL_GLAD	(1)
#define TRIAL_MARKS	(2)
#define TRIAL_ENCH	(3)
#define TRIAL_SORCE	(4)

//objective nums
#define OBJ_CELLAR		(0)		// mickey's cellar
#define OBJ_MAGICHAT	(1)		// frigid forest magichat
#define OBJ_TOYMOTRON	(2)		// frigid forest lone toymotron
#define OBJ_KLONK		(3)		// brisk bog klonk
#define OBJ_MOUSE		(4)		// infested woods
#define OBJ_SOCKCRAB	(5)		// brisk bog sockcrab
#define OBJ_FRUITPUNCH	(6)		// isle of misfit puppets
#define OBJ_UBERGHOST	(7)		// creepy copse cabin ghost
#define OBJ_BERSERKER	(8)		// bear lair berserker
#define OBJ_GRIZZLY		(9)		// bear lair grizzly
#define OBJ_TRIGUN		(10)	// army camp trigun
#define OBJ_GENERAL		(11)	// army camp general
#define OBJ_KLUNK		(12)	// river pirates
#define OBJ_GLADIATOR	(13)	// gladiator level 5
#define OBJ_BUSH		(14)	// feral farms burning bush
#define OBJ_GORILLA		(15)	// monkey grotto
#define OBJ_LEVEL25		(16)	// kinyova
#define OBJ_ARMOYOFONE	(17)	// *frostburn castle second floor
#define OBJ_ICEBOX		(18)	// drafty den
#define OBJ_FACTORY		(19)	// factory core
#define OBJ_REDBARON	(20)	// *parched pits wandering boss
#define OBJ_RUMBLE		(21)	// battlefield rumble quest
#define OBJ_STRIFEBOSS	(22)	// capitan or junksmith
#define OBJ_DOOMDAISY	(23)	// homicidal hillside
#define OBJ_GORKAS		(24)	// bodzha nest
#define OBJ_GOURDZILLA	(25)	// in underworld
#define OBJ_FURNACE		(26)	// *blow up furnace
#define OBJ_MELTON		(27)	// *frostburn castle
#define OBJ_JACK		(28)	// *titanic treasury

#define MAX_OBJECTIVES	(29)	// total number of objectives

// upgrade nums
#define UPG_EXPLORER	(0)
#define UPG_MERCHANT	(1)
#define UPG_CRAFTSMAN	(2)
#define UPG_EXPERT		(3)
#define UPG_SCHOLAR		(4)
#define UPG_DUELIST1	(5)
#define UPG_DUELIST2	(6)
#define UPG_DUELIST3	(7)
#define UPG_DUELIST4	(8)
#define UPG_DUELIST5	(9)
#define UPG_GLADIATOR1	(10)
#define UPG_GLADIATOR2	(11)
#define UPG_GLADIATOR3	(12)
#define UPG_GLADIATOR4	(13)
#define UPG_GLADIATOR5	(14)
#define UPG_MARKSMAN1	(15)
#define UPG_MARKSMAN2	(16)
#define UPG_MARKSMAN3	(17)
#define UPG_MARKSMAN4	(18)
#define UPG_MARKSMAN5	(19)
#define UPG_ENCHANT1	(20)
#define UPG_ENCHANT2	(21)
#define UPG_ENCHANT3	(22)
#define UPG_ENCHANT4	(23)
#define UPG_ENCHANT5	(24)
#define UPG_SORCERY1	(25)
#define UPG_SORCERY2	(26)
#define UPG_SORCERY3	(27)
#define UPG_SORCERY4	(28)
#define UPG_SORCERY5	(29)

#define MAX_UPGRADES	(30)	// total number of upgrades


typedef struct upgrade_t
{
	char name[16];
	byte level;
	byte req[2];
	char perktext[4][64];
} upgrade_t;

void		PrintUpgradeName(byte n, char* s);
void		PrintRequirements(byte n, char* s);
upgrade_t*	GetUpgrade(byte n);

#endif
