#ifndef STEAM_H
#define STEAM_H

#include <stdint.h>
#include <stddef.h>

enum class Difficulty : uint8_t;
struct challenge_t;
struct chalData_t;

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
	virtual void UploadChallengeScore(Difficulty difficulty, const challenge_t* definitions, size_t definitionsLen, const chalData_t* data) {}
};
SteamManager* Steam();

#endif
