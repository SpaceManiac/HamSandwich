#ifndef STEAM_H
#define STEAM_H

#include <stddef.h>
#include <stdint.h>

// The interface from the rest of the game to the Steam module.
class SteamManager
{
	SteamManager(const SteamManager&) = delete;
	SteamManager(SteamManager&&) = delete;
	SteamManager& operator=(const SteamManager&) = delete;
	SteamManager& operator=(SteamManager&&) = delete;
public:
	// Singleton and setup
	SteamManager() {}
	virtual ~SteamManager() {}
	static SteamManager* Init(const char* appId);
	static void Quit();

	// Useful stuff
	virtual void Update() {}
	virtual bool IsSteamEdition() { return false; }
	virtual void ProfileReady() {}
	virtual void CompleteGoal(int goal) {}

	virtual const char* DescribeWorkshopStatus() { return ""; }

	virtual void OpenURLOverlay(const char* url);
};
SteamManager* Steam();

#endif
