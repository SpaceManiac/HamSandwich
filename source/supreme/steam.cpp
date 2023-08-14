#include "steam.h"
#include <memory>

#ifdef HAS_STEAM_API
#include <steam/steam_api.h>
#include <vector>
#include <string>
#include <map>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "appdata.h"
#include "vanilla_extract.h"
#include "game.h"
#include "log.h"
#include "erase_if.h"

std::map<std::string, int32_t> GetStats()
{
	return {
		// GoalTimeDist
		{ "totalTime", profile.progress.totalTime },
		{ "footDistance", profile.progress.footDistance },
		{ "raftDistance", profile.progress.raftDistance },
		{ "driveDistance+cartDistance", profile.progress.driveDistance + profile.progress.cartDistance },
		{ "underwaterTime", profile.progress.underwaterTime },
	};
}

class SteamManagerImpl : public SteamManager
{
public:
	std::vector<PublishedFileId_t> subscribedItemIds;

	~SteamManagerImpl()
	{
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
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SteamUserStats::RequestCurrentStats() failed");
		}
	}

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
	}

	const char* DescribeEdition() override
	{
		return "Steam edition";
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
	const static word STATS_STORE_COOLDOWN = 2 * 60 * 30;  // 2 minutes
	bool steam_stats_ready = false;
	bool game_profile_ready = false;
	bool stats_need_store = false;
	word stats_store_cooldown = 0;

	STEAM_CALLBACK(SteamManagerImpl, on_stats_received, UserStatsReceived_t)
	{
		if (pParam->m_eResult == k_EResultOK && !steam_stats_ready)
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
			for (int goal = 0; goal < 100; ++goal)
			{
				if (profile.progress.goal[goal])
				{
					CompleteGoal(goal);
				}
			}

			auto stats = GetStats();
			for (const auto& pair : stats)
			{
				int32_t steamValue;
				if (SteamUserStats()->GetStat(pair.first.c_str(), &steamValue) && steamValue != pair.second && SteamUserStats()->SetStat(pair.first.c_str(), pair.second))
				{
					stats_need_store = true;
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
		sprintf(name, "goal_%d", goal);

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
	// Workshop upload
	bool CanUploadToWorkshop() override
	{
		return true;
	}

	// ------------------------------------------------------------------------
	// Workshop download
	void MountWorkshopContent()
	{
		vanilla::VfsStack& vfs_stack = AppdataVfs();

		// Unmount all existing Workshop content and remount it in the loop below.
		erase_if(vfs_stack.mounts, [](const vanilla::Mount& mount) { return mount.meta.steamWorkshopId != 0; });

		uint32_t subscribedCount = SteamUGC()->GetNumSubscribedItems();
		subscribedItemIds.resize(subscribedCount);
		subscribedCount = SteamUGC()->GetSubscribedItems(subscribedItemIds.data(), subscribedCount);
		subscribedItemIds.resize(subscribedCount);

		for (PublishedFileId_t fileId : subscribedItemIds)
		{
			uint32_t state = SteamUGC()->GetItemState(fileId);
			if (state & k_EItemStateNeedsUpdate)
			{
				if (SteamUGC()->DownloadItem(fileId, false))
				{
					SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "workshop download %llu started", fileId);
				}
				else
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "workshop download %llu failed to start", fileId);
				}
			}
			else if ((state & k_EItemStateInstalled) && (state & k_EItemStateSubscribed) && !(state & k_EItemStateDownloadPending))
			{
				char dirname[1024];
				if (SteamUGC()->GetItemInstallInfo(fileId, nullptr, dirname, SDL_arraysize(dirname), nullptr))
				{
					SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "workshop: %s", dirname);
					vfs_stack.push_back(vanilla::open_stdio(dirname), "", { vanilla::VfsSourceKind::Addon, fileId });
				}
				else
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "workshop %llu: GetItemInstallInfo failed", fileId);
				}
			}
			else
			{
				SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "workshop %llu: state is %" PRIu32, fileId, state);
			}
		}
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "workshop end");
	}

	STEAM_CALLBACK(SteamManagerImpl, on_subscriptions_change, UserSubscribedItemsListChanged_t)
	{
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "subscriptions changed for %u", pParam->m_nAppID);
		// TODO: check that it's our appid?
		MountWorkshopContent();
	}

	STEAM_CALLBACK(SteamManagerImpl, on_item_downloaded, DownloadItemResult_t)
	{
		// TODO: check that it's our appid?
		if (pParam->m_eResult == k_EResultOK)
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "workshop download %llu succeeded", pParam->m_nPublishedFileId);
			MountWorkshopContent();
		}
		else
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "workshop download %llu failed: %d", pParam->m_nPublishedFileId, pParam->m_eResult);
		}
	}
};

#endif  // HAS_STEAM_API

static std::unique_ptr<SteamManager> gSteamManager = nullptr;

SteamManager* SteamManager::Init()
{
#ifdef HAS_STEAM_API
	if (SteamAPI_Init())
	{
		gSteamManager = std::make_unique<SteamManagerImpl>();
	}
#endif  // HAS_STEAM_API

	if (!gSteamManager)
	{
		gSteamManager = std::make_unique<SteamManager>();
	}
	return gSteamManager.get();
}

SteamManager* SteamManager::Get()
{
	return gSteamManager.get();
}

void SteamManager::Quit()
{
	gSteamManager.reset();
}
