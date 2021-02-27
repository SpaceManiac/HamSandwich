#ifndef Randomizer_H
#define Randomizer_H

#include "jamultypes.h"
#include "mgldraw.h"
#include "highscore.h"
#include <string>
#include <set>


TASK(void) RandomizerMenu(MGLDraw *mgl);


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
	std::string mapName;
	int mapId, xcoord, ycoord;
	int s1, s2;
	std::string description;
	std::function<bool(std::set<int> inv)> requirements;
	rItem item;
	//item newItem;
};



void PlaceItems(const std::vector<location>& locs);

bool HaveLightSource(const std::set<int>& inv);

bool HaveAnyBigGem(const std::set<int>& inv);

bool HaveAllOrbs(const std::set<int>& inv);

bool HaveAllBats(const std::set<int>& inv);

bool HaveAllVamps(const std::set<int>& inv);

bool HaveAnySpecialWeapon(const std::set<int>& inv);

bool HaveAllMushrooms(const std::set<int>& inv);

bool CanCleanseCrypts(const std::set<int>& inv);

std::vector<location> RandomFill();

bool CheckBeatable( std::vector<location> remaining);

#endif
