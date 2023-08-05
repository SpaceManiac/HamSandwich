#ifndef STEAM_H
#define STEAM_H

// The interface from the rest of the game to the Steam module.
class SteamManager
{
public:
	// Singleton and setup
	virtual ~SteamManager() {}
	static SteamManager* Init();
	static SteamManager* Get();
	static void Quit();

	// Useful stuff
	virtual const char* DescribeEdition() { return ""; }
};

#endif
