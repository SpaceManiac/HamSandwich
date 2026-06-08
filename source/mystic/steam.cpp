#define __STDC_FORMAT_MACROS
#include "steam.h"
#include <memory>

// Here to anchor the vtable for linking.
SteamManager::~SteamManager() {}

#ifdef HAS_STEAM_API
#include <stdlib.h>
#include <inttypes.h>
#include <sstream>
#include <string>
#include <vector>
#include <steam/steam_api.h>
#include "extern.h"
#include "jamultypes.h"
#include "control.h"
#include "options.h"
#include "log.h"
#include "string_extras.h"
#include "game.h"
#include "challenge.h"
#include "steam_utils.inl"

static void SteamAfterFlip()
{
	Steam()->Update();
}

class SteamManagerImpl : public SteamManager
{
public:
	~SteamManagerImpl()
	{
		g_HamExtern.AfterFlip = nullptr;
		SteamAPI_Shutdown();
	}

	SteamManagerImpl()
	{
		if (!SteamUserStats()->RequestCurrentStats())
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SteamUserStats::RequestCurrentStats() failed");
		}

		g_HamExtern.AfterFlip = SteamAfterFlip;
	}

	dword oldGamepad = ~0;
	void Update() override
	{
		SteamAPI_RunCallbacks();

		if (stats_store_cooldown > 0)
		{
			--stats_store_cooldown;
		}
		else if (stats_need_store)
		{
			StoreStats();
		}

		dword gamepad = GetGamepadButtons();
		if (gamepad & ~oldGamepad & (1 << SDL_GAMEPAD_BUTTON_GUIDE))
		{
			SteamFriends()->ActivateGameOverlay(nullptr);
		}
		oldGamepad = gamepad;
	}

	// ------------------------------------------------------------------------
	// Automatically pause on overlay opening
	STEAM_CALLBACK(SteamManagerImpl, on_overlay_activated, GameOverlayActivated_t)
	{
		if (pParam->m_bActive)
		{
			PauseGame();
		}
	}

	// ------------------------------------------------------------------------
	// Achievements & statistics
	static const word STATS_STORE_COOLDOWN = 2 * 60 * 30; // 2 minutes
	bool steam_stats_ready = false;
	bool game_profile_ready = false;
	bool stats_need_store = false;
	word stats_store_cooldown = 0;

	STEAM_CALLBACK(SteamManagerImpl, on_stats_received, UserStatsReceived_t)
	{
		if (pParam->m_eResult != k_EResultOK)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UserStatsReceived: %d", (int)pParam->m_eResult);
			return;
		}
		if (!steam_stats_ready)
		{
			steam_stats_ready = true;
			if (game_profile_ready)
			{
				SyncStats();
				stats_store_cooldown = 0;
			}
		}
	}

	void ProfileReady() override
	{
		if (!game_profile_ready)
		{
			game_profile_ready = true;
			if (steam_stats_ready)
			{
				SyncStats();
				stats_store_cooldown = 0;
			}
		}
	}

	void SyncStats()
	{
		if (steam_stats_ready && game_profile_ready)
		{
			for (int goal = 0; goal < (int)Achievement::NUM_ACHIEVES; ++goal)
			{
				if (opt.achieve[goal] == ACHIEVE_GOT)
				{
					CompleteGoal(goal);
				}
			}
		}
	}

	void CompleteGoal(int goal) override
	{
		if (!steam_stats_ready)
		{
			return;
		}

		char name[64];
		ham_sprintf(name, "goal_%d", goal);

		bool steamAchieved;
		if (SteamUserStats()->GetAchievement(name, &steamAchieved) && !steamAchieved)
		{
			if (SteamUserStats()->SetAchievement(name))
			{
				LogDebug("SetAchievement(%s) OK", name);
				stats_need_store = true;
				// Store achievements immediately so the message is triggered.
				stats_store_cooldown = 2;
			}
		}
	}

	void StoreStats()
	{
		if (SteamUserStats()->StoreStats())
		{
			LogDebug("StoreStats() OK");
		}
		stats_need_store = false;
		stats_store_cooldown = STATS_STORE_COOLDOWN;
	}

	// ------------------------------------------------------------------------
	// Challenge mode leaderboards
	void UploadChallengeScore(Difficulty difficulty, const challenge_t* definitions, size_t definitionsLen, const chalData_t* data) override
	{
		const char* difficultyName;
		switch (difficulty)
		{
			case Difficulty::CLASSIC:
				difficultyName = "classic";
				break;
			case Difficulty::MODERN:
				difficultyName = "modern";
				break;
			case Difficulty::BRUTAL_CLASSIC:
				difficultyName = "brutal_classic";
				break;
			case Difficulty::BRUTAL_MODERN:
				difficultyName = "brutal_modern";
				break;
			default:
				// Invalid, unknown, or non-leaderboarded difficulty.
				return;
		}

		std::unique_ptr<LeaderboardUploadJob> scoreJob = std::make_unique<LeaderboardUploadJob>();
		std::unique_ptr<LeaderboardUploadJob> timeJob = std::make_unique<LeaderboardUploadJob>();
		std::unique_ptr<LeaderboardUploadJob> comboJob = std::make_unique<LeaderboardUploadJob>();

		timeJob->sortMethod = k_ELeaderboardSortMethodAscending;
		timeJob->displayType = k_ELeaderboardDisplayTypeTimeMilliSeconds;

		int32_t metadetail = 0;  // Placeholder for overall stats like items?
		scoreJob->AddDetail(metadetail);
		timeJob->AddDetail(metadetail);
		comboJob->AddDetail(metadetail);

		int incomplete = 0;
		for (int i = 0; i < MAX_CHALLENGE && i < definitionsLen; ++i)
		{
			// Skip menu items that aren't levels.
			if (definitions[i].chapter > 8 || definitions[i].goal[0].type != GOAL_FINISH)
			{
				continue;
			}
			// Skip rune pouch levels and Beginnerton in Classic mode.
			if ((difficulty == Difficulty::CLASSIC || difficulty == Difficulty::BRUTAL_CLASSIC) &&
				((definitions[i].chapter == 0 && definitions[i].level == 20) || // swamp stew
				 (definitions[i].chapter == 0 && definitions[i].level == 21) || // beginnerton
				 (definitions[i].chapter == 1 && definitions[i].level == 19) || // haunted woods
				 (definitions[i].chapter == 2 && definitions[i].level == 24) || // hot to go
				 (definitions[i].chapter == 3 && definitions[i].level == 19)	// horkboxes
				 ))
			{
				continue;
			}

			SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%d = %s, score=%d, combo=%d, time=%d", i, definitions[i].name, data->topScore[i], data->topCombo[i], data->topTime[i]);
			scoreJob->score += data->topScore[i];
			scoreJob->AddDetail(data->topScore[i]);
			comboJob->score += data->topCombo[i];
			comboJob->AddDetail(data->topCombo[i]);
			if (data->topTime[i] == UINT32_MAX)
			{
				incomplete += 1;
			}
			else
			{
				timeJob->score += data->topTime[i];
			}
			timeJob->AddDetail(data->topTime[i]);
		}
		if (incomplete)
		{
			// missed 0 maps = your real total time, which is <
			// missed 1/10 maps = MAX-9, which is <
			// missed 2/10 maps = MAX-8, so on
			timeJob->score = INT32_MAX - 1024 + incomplete;
		}
		else
		{
			// score is now total time in frames - convert it to milliseconds for Steam's benefit
			timeJob->score = timeJob->score * 1000 / 30;
		}
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "total score=%d, combo=%d, time=%d, incomplete=%d", scoreJob->score, comboJob->score, timeJob->score, incomplete);

		char name[64];
		ham_sprintf(name, "challenge/%s/score", difficultyName);
		LeaderboardUploadJob::Send(name, std::move(scoreJob));
		ham_sprintf(name, "challenge/%s/time", difficultyName);
		LeaderboardUploadJob::Send(name, std::move(timeJob));
		ham_sprintf(name, "challenge/%s/combo", difficultyName);
		LeaderboardUploadJob::Send(name, std::move(comboJob));
	}
};

#endif // HAS_STEAM_API

static std::unique_ptr<SteamManager> gSteamManager = nullptr;

SteamManager *SteamManager::Init(const char *appId)
{
#ifdef HAS_STEAM_API
	if (appId && !getenv("SteamAppId"))
	{
		// setenv is wildly non-threadsafe, so do this before spawning threads
#ifdef _WIN32
		_putenv_s("SteamAppId", appId);
#else
		setenv("SteamAppId", appId, 0);
#endif
	}

	if (SteamAPI_Init())
	{
		gSteamManager = std::make_unique<SteamManagerImpl>();
	}
	else
	{
		// Compensate for error message not having newline.
		fwrite("\n", 1, 1, stderr);
	}
#endif // HAS_STEAM_API

	if (!gSteamManager)
	{
		gSteamManager = std::make_unique<SteamManager>();
	}
	return gSteamManager.get();
}

SteamManager *Steam()
{
	return gSteamManager.get();
}

void SteamManager::Quit()
{
	gSteamManager.reset();
}
