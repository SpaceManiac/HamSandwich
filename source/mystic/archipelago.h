#ifndef ARCHIPELAGO_H
#define ARCHIPELAGO_H

#include <mgldraw.h>
#include <string_view>

// The interface from the rest of the game to the Archipelago module.
class ArchipelagoManager
{
	ArchipelagoManager(const ArchipelagoManager&) = delete;
	ArchipelagoManager(ArchipelagoManager&&) = delete;
	ArchipelagoManager& operator=(const ArchipelagoManager&) = delete;
	ArchipelagoManager& operator=(ArchipelagoManager&&) = delete;
public:
	ArchipelagoManager() {}

	std::string_view Status();
};
ArchipelagoManager* Archipelago();

TASK(void) ArchipelagoMenu(MGLDraw* mgl);

#endif
