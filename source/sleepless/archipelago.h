#ifndef ARCHIPELAGO_H
#define ARCHIPELAGO_H

#include <string_view>
#include "mgldraw.h"

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

	const char* SaveName();
};
class Archipelago* Archipelago();

TASK(void) ArchipelagoMenu(MGLDraw* mgl);

#endif
