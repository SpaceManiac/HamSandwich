#ifndef Randomizer_H
#define Randomizer_H

#include "jamultypes.h"
#include "mgldraw.h"
#include "highscore.h"
#include <string>
#include <set>

#define MAX_SEED_LENGTH 11
#define R_NUM_LOCATIONS 105

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


void RandomizeSeed();

void PlaceItems(std::vector<location>& loc);

bool HaveLightSource(const std::set<int>& inv);

bool HaveAnyBigGem(const std::set<int>& inv);

bool HaveAllOrbs(const std::set<int>& inv);

bool HaveAllBats(const std::set<int>& inv);

bool HaveAllVamps(const std::set<int>& inv);

bool HaveSpecialWeaponDamage(const std::set<int>& inv);

bool HaveSpecialWeaponBullet(const std::set<int>& inv);

bool HaveSpecialWeaponRangeDamage(const std::set<int>& inv);

bool HaveSpecialWeaponThroughWalls(const std::set<int>& inv);

bool HaveAllMushrooms(const std::set<int>& inv);

bool CanEnterLoonyton(const std::set<int>& inv);

bool CanCleanseCrypts(const std::set<int>& inv);

bool CanEnterRockyCliffs(const std::set<int>& inv);

bool CanEnterVampy(const std::set<int>& inv);

bool CanEnterVampyII(const std::set<int>& inv);

bool CanEnterVampyIII(const std::set<int>& inv);

bool CanEnterVampyIV(const std::set<int>& inv);


int RandomFill(std::vector<location>& locs);

bool CheckBeatable(std::vector<location>& locs);

std::string GetSeed();

#endif
