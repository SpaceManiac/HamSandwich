#ifndef Randomizer_H
#define Randomizer_H

#include "jamultypes.h"
#include "mgldraw.h"
#include "highscore.h"
#include <string>
#include <set>


TASK(void) RandomizerMenu(MGLDraw *mgl);
TASK(void) AssumedFill();


struct rItem
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
	std::function<bool(std::set<int> inv)> requirements;
	rItem item;
	//item newItem;
};

void PlaceItems(std::vector<location> locs);

bool HaveLightSource(std::set<int> inv);

bool HaveAnyBigGem(std::set<int> inv);

bool HaveAllOrbs(std::set<int> inv);

bool HaveAllBats(std::set<int> inv);

bool HaveAllVamps(std::set<int> inv);

bool HaveAnySpecialWeapon(std::set<int> inv);

bool HaveAllMushrooms(std::set<int> inv);

bool CanCleanseCrypts(std::set<int> inv);

bool CheckBeatable();

#endif
