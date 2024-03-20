#define __STDC_FORMAT_MACROS
#include "steam.h"
#include <memory>
#include "openurl.h"

void SteamManager::OpenURLOverlay(const char *url)
{
	(void)SDL_OpenURL(url);
}

#ifdef HAS_STEAM_API
#include <stdlib.h>
#include <sstream>
#include <inttypes.h>
#include <steam/steam_api.h>
#include "extern.h"
#include "jamultypes.h"
#include "control.h"
#include "options.h"
#include "log.h"
#include "appdata.h"
#include "vanilla_extract.h"
#include "erase_if.h"
#include "highscore.h"

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
	const static word STATS_STORE_COOLDOWN = 2 * 60 * 30;  // 2 minutes
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
			for (int goal = 0; goal < 40; ++goal)
			{
				if (opt.meritBadge[goal])
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
		sprintf(name, "badge_%d", goal);

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
	// Workshop download
	std::string workshopStatus;
	std::vector<PublishedFileId_t> subscribedItemIds;

	const char* DescribeWorkshopStatus() override
	{
		return workshopStatus.c_str();
	}

	void MountWorkshopContent()
	{
		vanilla::VfsStack& vfs_stack = AppdataVfs();

		// Unmount all existing Workshop content and remount it in the loop below.
		erase_if(vfs_stack.mounts, [](const vanilla::Mount& mount) { return mount.meta.steamWorkshopId != 0; });

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

	// ------------------------------------------------------------------------
	// Leaderboard upload base
	class LeaderboardUploadJob
	{
	protected:
		int32_t score = 0;
		ELeaderboardSortMethod sortMethod = k_ELeaderboardSortMethodDescending;
		ELeaderboardDisplayType displayType = k_ELeaderboardDisplayTypeNumeric;
		ELeaderboardUploadScoreMethod uploadScoreMethod = k_ELeaderboardUploadScoreMethodKeepBest;
	private:
		int32_t extraData[k_cLeaderboardDetailsMax];
		int extraDataCount = 0;

		CCallResult<LeaderboardUploadJob, LeaderboardFindResult_t> findLeaderboardCall;
		CCallResult<LeaderboardUploadJob, LeaderboardScoreUploaded_t> uploadScoreCall;

	protected:
		LeaderboardUploadJob() {}

		// Try to add a detail. Returns false if you're out of space.
		bool AddDetail(int32_t detail)
		{
			if (extraDataCount < k_cLeaderboardDetailsMax)
			{
				extraData[extraDataCount] = detail;
				++extraDataCount;
				return true;
			}
			return false;
		}

	public:
		static void Send(const char* leaderboardId, std::unique_ptr<LeaderboardUploadJob> job)
		{
			// std::unique_ptr type enforces that we can call .release() and `delete` it later.
			job.release()->Send2(leaderboardId);
		}

	private:
		// Safety: `this` must be on the heap and this object is now responsible for `delete`ing itself.
		void Send2(const char* leaderboardId)
		{
			SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "FindOrCreateLeaderboard(%s)", leaderboardId);
			findLeaderboardCall.Set(SteamUserStats()->FindOrCreateLeaderboard(
				leaderboardId,
				sortMethod,
				displayType
			), this, &LeaderboardUploadJob::FindLeaderboardCallback);
		}

		void FindLeaderboardCallback(LeaderboardFindResult_t* result, bool ioError)
		{
			if (!ioError && result->m_bLeaderboardFound && result->m_hSteamLeaderboard)
			{
				SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "UploadLeaderboardScore(%llu, %d, ...[%d])",
					result->m_hSteamLeaderboard, score, extraDataCount);
				uploadScoreCall.Set(SteamUserStats()->UploadLeaderboardScore(
					result->m_hSteamLeaderboard,
					uploadScoreMethod,
					score,
					extraData,
					extraDataCount
				), this, &LeaderboardUploadJob::UploadScoreCallback);
			}
			else
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FindOrCreateLeaderboard failed");
				delete this;  // living on the edge
			}
		}

		void UploadScoreCallback(LeaderboardScoreUploaded_t* result, bool ioError)
		{
			if (!ioError && result->m_bSuccess)
			{
				SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "UploadLeaderboardScore OK: changed=%d, rank %d -> %d",
					result->m_bScoreChanged, result->m_nGlobalRankPrevious, result->m_nGlobalRankNew);
			}
			else
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UploadLeaderboardScore failed");
			}
			delete this;
		}
	};

	// ------------------------------------------------------------------------
	// Upload standard high scores
	struct HighScoreUploadJob : public LeaderboardUploadJob
	{
		HighScoreUploadJob(const highScore_t* highScore)
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
		span<char> dest = buffer;
		dest = ham_sprintf(dest, "highscore/%d", score->mode);
		if (subMode != 255)
		{
			dest = ham_sprintf(dest, "/%d", subMode);
		}

		LeaderboardUploadJob::Send(buffer, std::make_unique<HighScoreUploadJob>(score));
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
