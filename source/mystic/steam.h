#ifndef STEAM_H
#define STEAM_H

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
	virtual ~SteamManager();
	static SteamManager* Init(const char* appId);
	static void Quit();

	// Useful stuff
	virtual void Update() {}
	virtual void ProfileReady() {}
	virtual void CompleteGoal(int goal) {}
};
SteamManager* Steam();

#endif
