#define __STDC_FORMAT_MACROS
#include "steam.h"
#include <memory>
#include <SDL3/SDL_misc.h>

void SteamManager::OpenURLOverlay(const char *url)
{
	(void)SDL_OpenURL(url);
}

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
#include "appdata.h"
#include "vanilla_extract.h"
#include "highscore.h"
#include "string_extras.h"
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
		if (AppdataIsInit())
		{
			MountWorkshopContent();
		}

		if (!SteamUserStats()->RequestCurrentStats())
		{
			LogError("SteamUserStats::RequestCurrentStats() failed");
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

	bool IsSteamEdition() override
	{
		return true;
	}

	void OpenURLOverlay(const char *url) override
	{
		SteamFriends()->ActivateGameOverlayToWebPage(url);
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
	static const word STATS_STORE_COOLDOWN = 2 * 60 * 30;  // 2 minutes
	bool steam_stats_ready = false;
	bool game_profile_ready = false;
	bool stats_need_store = false;
	word stats_store_cooldown = 0;

	STEAM_CALLBACK(SteamManagerImpl, on_stats_received, UserStatsReceived_t)
	{
		if (pParam->m_eResult != k_EResultOK)
		{
			LogError("UserStatsReceived: %d", (int)pParam->m_eResult);
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
			for (int goal = 0; goal < 40; ++goal)
			{
				if (opt.meritBadge[goal] == MERIT_EARNED)
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
		ham_sprintf(name, "badge_%d", goal);

		bool steamAchieved;
		if (SteamUserStats()->GetAchievement(name, &steamAchieved) && !steamAchieved)
		{
			if (SteamUserStats()->SetAchievement(name))
			{
				LogInfo("SetAchievement(%s) OK", name);
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
			LogInfo("StoreStats() OK");
		}
		stats_need_store = false;
		stats_store_cooldown = STATS_STORE_COOLDOWN;
	}

	// ------------------------------------------------------------------------
	// Workshop download
	std::string workshopStatus;
	std::vector<PublishedFileId_t> subscribedItemIds;

	const char* DescribeWorkshopStatus() override
	{
		return workshopStatus.c_str();
	}

	void MountWorkshopContent()
	{
		vanilla::VfsStack* vfs_stack = Vfs();

		// Unmount all existing Workshop content and remount it in the loop below.
		std::erase_if(vfs_stack->mounts, [](const vanilla::Mount& mount) { return mount.meta.steamWorkshopId != 0; });

		uint32_t subscribedCount = SteamUGC()->GetNumSubscribedItems();
		subscribedItemIds.resize(subscribedCount);
		subscribedCount = SteamUGC()->GetSubscribedItems(subscribedItemIds.data(), subscribedCount);
		subscribedItemIds.resize(subscribedCount);

		int numBusy = 0;
		for (PublishedFileId_t fileId : subscribedItemIds)
		{
			uint32_t state = SteamUGC()->GetItemState(fileId);

			if (state & (k_EItemStateNeedsUpdate | k_EItemStateDownloading | k_EItemStateDownloadPending))
			{
				numBusy++;
			}

			if (state & k_EItemStateNeedsUpdate)
			{
				if (SteamUGC()->DownloadItem(fileId, false))
				{
					LogInfo("workshop download %llu started", fileId);
				}
				else
				{
					LogError("workshop download %llu failed to start", fileId);
				}
			}
			else if ((state & k_EItemStateInstalled) && (state & k_EItemStateSubscribed) && !(state & k_EItemStateDownloadPending))
			{
				char dirname[1024];
				if (SteamUGC()->GetItemInstallInfo(fileId, nullptr, dirname, std::size(dirname), nullptr))
				{
					LogInfo("workshop: %s", dirname);
					vfs_stack->push_back(vanilla::open_stdio(dirname), "", { vanilla::VfsSourceKind::Addon, fileId });
				}
				else
				{
					LogError("workshop %llu: GetItemInstallInfo failed", fileId);
				}
			}
			else
			{
				LogInfo("workshop %llu: state is %" PRIu32, fileId, state);
			}
		}

		if (numBusy == 0)
		{
			workshopStatus = "";
		}
		else
		{
			std::ostringstream status;
			status << "Updating " << numBusy << " Workshop add-on" << (numBusy == 1 ? "" : "s") << "...";
			workshopStatus = status.str();
		}

		LogDebug("workshop end");
	}

	STEAM_CALLBACK(SteamManagerImpl, on_subscriptions_change, UserSubscribedItemsListChanged_t)
	{
		LogDebug("subscriptions changed for %u", pParam->m_nAppID);
		if (pParam->m_nAppID == SteamUtils()->GetAppID())
		{
			MountWorkshopContent();
		}
	}

	STEAM_CALLBACK(SteamManagerImpl, on_item_downloaded, DownloadItemResult_t)
	{
		if (pParam->m_eResult == k_EResultOK)
		{
			LogInfo("workshop download %llu succeeded", pParam->m_nPublishedFileId);
			if (pParam->m_unAppID == SteamUtils()->GetAppID())
			{
				MountWorkshopContent();
			}
		}
		else
		{
			LogError("workshop download %llu failed: %d", pParam->m_nPublishedFileId, pParam->m_eResult);
		}
	}

	// ------------------------------------------------------------------------
	// Upload standard high scores
	struct HighScoreUploadJob : public LeaderboardUploadJob
	{
		explicit HighScoreUploadJob(const highScore_t* highScore)
		{
			score = highScore->score;
			AddDetail(highScore->monsterPoints);
			AddDetail(highScore->time);
			AddDetail(highScore->bestCombo);
			AddDetail(highScore->shotsFired);
			AddDetail(highScore->specialShotsFired);
			AddDetail(highScore->guysKilled);
			AddDetail(highScore->gemsGotten);
			AddDetail(highScore->numSaves);
			AddDetail(highScore->hitsTaken);
			AddDetail(highScore->spclValue);
		}
	};

	void UploadHighScore(const highScore_t* score, uint8_t subMode) override
	{
		char buffer[64];
		std::span<char> dest = buffer;
		dest = ham_sprintf(dest, "highscore/%d", score->mode);
		if (subMode != 255)
		{
			dest = ham_sprintf(dest, "/%d", subMode);
		}

		LeaderboardUploadJob::Send(buffer, std::make_unique<HighScoreUploadJob>(score));
	}

	// ------------------------------------------------------------------------
	// Upload space game scores
	struct SpaceGameScoreJob : public LeaderboardUploadJob
	{
		explicit SpaceGameScoreJob(int score)
		{
			this->score = score;
		}
	};
	struct SpaceGameTimeJob : public LeaderboardUploadJob
	{
		explicit SpaceGameTimeJob(int timeInFrames)
		{
			// Not a speedrun. Higher survival times are better.
			displayType = k_ELeaderboardDisplayTypeTimeMilliSeconds;  // INT_MAX is about 596:31:23.645

			// score is now total time in frames - convert it to milliseconds for Steam's benefit
			this->score = timeInFrames * 1000 / 30;
		}
	};

	void UploadSpaceGameScore(int score, int time) override
	{
		LeaderboardUploadJob::Send("spacegame/score", std::make_unique<SpaceGameScoreJob>(score));
		LeaderboardUploadJob::Send("spacegame/time", std::make_unique<SpaceGameTimeJob>(time));
	}
};

#endif  // HAS_STEAM_API

static std::unique_ptr<SteamManager> gSteamManager = nullptr;

SteamManager* SteamManager::Init(const char* appId)
{
#ifdef HAS_STEAM_API
	if (appId && !getenv("SteamAppId"))
	{
		// setenv is wildly non-threadsafe, so do this before spawning threads
		SDL_setenv_unsafe("SteamAppId", appId, 0);
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
#endif  // HAS_STEAM_API

	if (!gSteamManager)
	{
		gSteamManager = std::make_unique<SteamManager>();
	}
	return gSteamManager.get();
}

SteamManager* Steam()
{
	return gSteamManager.get();
}

void SteamManager::Quit()
{
	gSteamManager.reset();
}
