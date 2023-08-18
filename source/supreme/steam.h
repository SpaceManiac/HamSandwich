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
	virtual void Update() {}
	virtual bool IsSteamEdition() { return false; }
	virtual const char* DescribeEdition() { return ""; }
	virtual void ProfileReady() {}
	virtual void CompleteGoal(int goal) {}

	virtual bool CanUploadToWorkshop() { return false; }

	virtual void OpenURLOverlay(const char* url);
};

#endif
