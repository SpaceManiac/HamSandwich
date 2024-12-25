#ifndef ARCHIPELAGO_H
#define ARCHIPELAGO_H

#include <mgldraw.h>
#include <string_view>

// The interface from the rest of the game to the Archipelago module.
class Archipelago
{
	Archipelago(const Archipelago&) = delete;
	Archipelago(Archipelago&&) = delete;
	Archipelago& operator=(const Archipelago&) = delete;
	Archipelago& operator=(Archipelago&&) = delete;
public:
	Archipelago() {}

	std::string_view Status();
	void Update();

	bool HasCheckedLocation(int chapter, int levelNum);
	byte MysticItemAtLocation(int chapter, int levelNum);
	std::string_view ItemNameAtLocation(int chapter, int levelNum);

	void PickupItem(int chapter, int levelNum);
	void CompleteChapter(int chapter);
};
class Archipelago* Archipelago();

TASK(void) ArchipelagoMenu(MGLDraw* mgl);

#endif
