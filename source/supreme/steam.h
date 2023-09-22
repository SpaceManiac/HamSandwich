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
	static SteamManager* Init();
	static SteamManager* Get();
	static void Quit();

	// Useful stuff
	virtual void Update() {}
	virtual bool IsSteamEdition() { return false; }
	virtual const char* DescribeEdition() { return ""; }
	virtual void ProfileReady() {}
	virtual void CompleteGoal(int goal) {}

	virtual const char* DescribeWorkshopStatus() { return ""; }
	virtual bool CanUploadToWorkshop() { return false; }
	virtual void StartPlaytimeTracking(const char* fullFilename) {}
	virtual void StopPlaytimeTracking() {}

	virtual void OpenURLOverlay(const char* url);

	virtual void UploadWorldScore() {}
	virtual void PrepWorldLeaderboard(const char* fullFilename) {}
	virtual uint64_t LeaderboardIdScore() { return 0; }
	virtual uint64_t LeaderboardIdTime() { return 0; }

	virtual void GetUsername(char* buffer, size_t size) {}

	virtual void SetPresenceNone() {}
	virtual void SetPresenceWorld(const char* worldName) {}
	virtual void SetPresenceEditor() {}
	virtual void SetPresenceShopping() {}
};

#endif
