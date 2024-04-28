#ifndef Randomizer_H
#define Randomizer_H

#include "jamultypes.h"
#include "mgldraw.h"
#include "highscore.h"
#include <string>
#include <set>

#define MAX_SEED_LENGTH 11

TASK(void) RandomizerMenu(MGLDraw *mgl);
const std::string& GetSeed();
void ClearSeed();

struct RandoItem
{
	/* data */
	//int randId;
	int itemId;
	int playerVarId;
	const char* itemName;
};

#endif
