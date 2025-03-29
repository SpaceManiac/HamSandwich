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
		if (gamepad & ~oldGamepad & (1 << SDL_CONTROLLER_BUTTON_GUIDE))
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
