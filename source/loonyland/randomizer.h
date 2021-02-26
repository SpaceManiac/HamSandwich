#ifndef Randomizer_H
#define Randomizer_H

#include "jamultypes.h"
#include "mgldraw.h"
#include "highscore.h"
#include <string>

TASK(void) RandomizerMenu(MGLDraw *mgl);
TASK(void) AssumedFill();


struct item
{
	/* data */
	//int randId;
	int itemId;
	int playerVarId;
	std::string itemName;
};

struct location
{
	/* data */
	//int randId;
	bool isQuest;
	int mapId, xcoord, ycoord;
	int s1, s2;
	std::string description;
	std::function<bool()> requirements;
};

bool HaveLightSource();

bool HaveAnyBigGem();

bool HaveAllOrbs();

bool HaveAllBats();

bool HaveAllVamps();

bool HaveAnySpecialWeapon();

bool HaveAllMushrooms();

bool CanCleanseCrypts();

#endif
