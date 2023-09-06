#define __STDC_FORMAT_MACROS
#include "steam.h"
#include <memory>
#include "openurl.h"

void SteamManager::OpenURLOverlay(const char *url)
{
	(void)SDL_OpenURL(url);
}

#ifdef HAS_STEAM_API
#include <steam/steam_api.h>
#include <vector>
#include <string>
#include <map>
#include <inttypes.h>
#include <sstream>
#include "appdata.h"
#include "vanilla_extract.h"
#include "game.h"
#include "log.h"
#include "erase_if.h"
#include "shop.h"
#include "theater.h"
#include "hiscore.h"
#include "progress.h"

// Matches Unpack* in leaderboard.cpp
static int32_t PackWorldProgress(worldData_t* worldProgress)
{
	// 8 bits keychains (5 used).
	// 10 bits for progress out of 1000.
	// 14 bits reserved.
	int32_t percentage10x = int32_t(std::clamp(worldProgress->percentage * 10.0f, 0.0f, 1000.0f));
	return (worldProgress->keychains & 0xff)
		| ((percentage10x & 0x3ff) << 8);
}
static int32_t PackMapScore(score_t* score)
{
	// 24 bits score. Maxes at 16,777,215.
	// 1 bit reserved.
	// 2 bits difficulty. Normal, Hard, Lunatic, unused.
	// 3 bits playAs.
	// 2 bits reserved.
	return (std::min(score->score, 0xffffffu))
		| ((score->difficulty & 0x3) << 25)
		| (std::min(score->playAs, byte(0x7)) << 27);
}

static std::map<std::string, int32_t> GetStats()
{
	int totalKills = 0, distinctKills = 0;
	for (int i = 2; i < NUM_PROFILE_MONSTERS; i++) // skip Bouapha
	{
		totalKills += profile.progress.kills[i];

		if(!(MonsterFlags(i,i)&(MF_INVINCIBLE|MF_NOHIT)) && !(MonsterTheme(i)&(MT_GOOD|MT_BITS)) &&
			(MonsterTheme(i)))
		{
			// only count those that can be hurt, aren't body parts, and aren't in the 'good' group
			distinctKills += (profile.progress.kills[i] ? 1 : 0);
		}
	}

	int scanned = 0;
	for (int i = 0; i < NUM_PROFILE_MONSTERS; i++)
	{
		if (MonsterTheme(i) && profile.progress.scanned[i])
		{
			++scanned;
		}
	}

	int totalGoals = 0;
	for (int i = 0; i < 100; i++)
		totalGoals += (profile.progress.goal[i] ? 1 : 0);

	CalcFinishedWorlds();

	return {
		// GoalTimeDist
		{ "totalTime", profile.progress.totalTime },
		{ "footDistance", profile.progress.footDistance },
		{ "raftDistance", profile.progress.raftDistance },
		{ "driveDistance+cartDistance", profile.progress.driveDistance + profile.progress.cartDistance },
		{ "underwaterTime", profile.progress.underwaterTime },
		// GoalKilledSomebody
		{ "deaths", profile.progress.kills[MONS_BOUAPHA] },
		{ "killsSvenBjorn", (profile.progress.kills[MONS_SVEN] ? 1 : 0) + (profile.progress.kills[MONS_BJORN] ? 1 : 0) },
		{ "killsStickman", profile.progress.kills[MONS_STICKMAN] },
		{ "killsEvilClone", profile.progress.kills[MONS_EVILCLONE] },
		{ "killsVampire", profile.progress.kills[MONS_VAMPIRE] + profile.progress.kills[MONS_DARKVAMP] + profile.progress.kills[MONS_COUNTESS] + profile.progress.kills[MONS_SPIKEY] },
		{ "runOver", profile.progress.runOver },
		{ "totalKills", totalKills },
		{ "distinctKills", distinctKills },
		// GoalTallyPage
		{ "totalCoins", profile.progress.totalCoins },
		{ "coinsInHand", profile.progress.totalCoins - profile.progress.coinsSpent },
		// GoalPurchase
		{ "purchasedWorlds", NumOfTypePurchased(SHOP_WORLD) },
		{ "purchasedAbilities", NumOfTypePurchased(SHOP_ABILITY) },
		{ "purchasedMajors", NumOfTypePurchased(SHOP_MAJOR) },
		{ "purchasedCheats", NumOfTypePurchased(SHOP_CHEAT) },
		{ "moviesSeen", NumMoviesSeen() },
		{ "lockersOpened", NumLockersOpen() },
		{ "purchasedTotal", NumPurchased() },
		// shop.cpp
		{ "purchasedDonation", profile.progress.purchase[157] },
		// GoalWinLevel
		{ "levelsPassed", CountLevelsPassed() },
		{ "finishedWorlds", profile.progress.finishedWorlds },
		// GoalFire
		{ "hammersThrown", profile.progress.hammersThrown },
		{ "rages", profile.progress.rages },
		{ "shotsFired", profile.progress.shotsFired },
		// items.cpp
		{ "loonyKeys", profile.progress.loonyKeys },
		{ "keysFound", profile.progress.keysFound },
		{ "doorsOpened", profile.progress.doorsOpened },
		{ "grassChopped", profile.progress.grassChopped },
		// player.cpp
		{ "totalBrains", profile.progress.totalBrains },
		{ "totalCandles", profile.progress.totalCandles },
		// guy.cpp
		{ "calories", profile.progress.calories },
		// scanner.cpp
		{ "scanned", scanned },
		// and finally...
		{ "totalGoals", totalGoals },
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

	const char* DescribeEdition() override
	{
		return "Steam edition";
	}

	void OpenURLOverlay(const char *url) override
	{
		SteamFriends()->ActivateGameOverlayToWebPage(url);
	}

	// ------------------------------------------------------------------------
	// Prefill your Steam name when creating your profile
	void GetUsername(char* buffer, size_t size) override
	{
		const char* personaName = SteamFriends()->GetPersonaName();
		size_t pos = 0;
		while (pos < size - 1)
		{
			char c = *personaName++;
			if (!c) break;
			if ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'))
			{
				buffer[pos++] = c;
			}
		}
		buffer[pos] = 0;
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
	std::string workshopStatus;

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
	// Workshop playtime tracking
	PublishedFileId_t playtimeTrackingId = 0;

	void StartPlaytimeTracking(const char* fullFilename) override
	{
		// nullptr = use whatever the last thing was (for unpausing)
		vanilla::VfsMeta meta;
		if (fullFilename && AppdataVfs().query_bottom(fullFilename, &meta))
		{
			playtimeTrackingId = meta.steamWorkshopId;
		}
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "StartPlaytimeTracking(%s) = %llu", fullFilename ? fullFilename : "nullptr", playtimeTrackingId);
		if (playtimeTrackingId)
		{
			SteamUGC()->StartPlaytimeTracking(&playtimeTrackingId, 1);
		}
	}

	void StopPlaytimeTracking() override
	{
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "StopPlaytimeTracking");
		SteamUGC()->StopPlaytimeTrackingForAllItems();
	}

	// ------------------------------------------------------------------------
	// Steam Friends rich presence
	void SetPresenceNone() override
	{
		SteamFriends()->ClearRichPresence();
	}

	void SetPresenceWorld(const char* worldName) override
	{
		// TODO: set up a connect command?
		SteamFriends()->SetRichPresence("steam_display", "#Status_PlayingWorld");
		SteamFriends()->SetRichPresence("world_name", worldName);
	}

	void SetPresenceEditor() override
	{
		SteamFriends()->SetRichPresence("steam_display", "#Status_Editing");
		SteamFriends()->SetRichPresence("status", nullptr);
		SteamFriends()->SetRichPresence("connect", nullptr);
	}

	void SetPresenceShopping() override
	{
		SteamFriends()->SetRichPresence("steam_display", "#Status_Shopping");
		SteamFriends()->SetRichPresence("status", nullptr);
		SteamFriends()->SetRichPresence("connect", nullptr);
	}

	// ------------------------------------------------------------------------
	// Per-world score leaderboard upload
	class UploadWorldScoreJob
	{
		int32_t totalScore;
		int32_t extraData[k_cLeaderboardDetailsMax];
		int extraDataCount;

		CCallResult<UploadWorldScoreJob, LeaderboardFindResult_t> findLeaderboardCall;
		CCallResult<UploadWorldScoreJob, LeaderboardScoreUploaded_t> uploadScoreCall;

	public:
		UploadWorldScoreJob(const char* fullFilename, world_t* world, worldData_t* worldProgress)
		{
			// Pack keychain flags and NNN.N% completion into extra data for display
			extraData[0] = PackWorldProgress(worldProgress);
			extraDataCount = 1;

			totalScore = 0;
			for (int i = 0; i < world->numMaps; ++i)
			{
				int32_t packedMapScore = 0;
				Map* map = world->map[i];
				score_t winners[3];
				// Take the top score on this machine, regardless of profile.
				if (GetTopScores(winners, map) > 0)
				{
					packedMapScore = PackMapScore(&winners[0]);
					totalScore += winners[0].score;
				}

				// Pack per-level top score, character, and difficulty into extra slots.
				// HUB levels are skipped, but unscored levels get explicit zeroes.
				if (!(map->flags & MAP_HUB) && extraDataCount < k_cLeaderboardDetailsMax)
				{
					// 4 bits for playable character
					// 2 bits for difficulty
					// leaving 29 bits for
					extraData[extraDataCount] = packedMapScore;
					++extraDataCount;
				}
			}

			SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "FindOrCreateLeaderboard(%s)", fullFilename);
			findLeaderboardCall.Set(SteamUserStats()->FindOrCreateLeaderboard(
				fullFilename,
				k_ELeaderboardSortMethodDescending,
				k_ELeaderboardDisplayTypeNumeric
			), this, &UploadWorldScoreJob::FindLeaderboardCallback);
		}

	private:
		void FindLeaderboardCallback(LeaderboardFindResult_t* result, bool ioError)
		{
			if (!ioError && result->m_bLeaderboardFound && result->m_hSteamLeaderboard)
			{
				SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "UploadLeaderboardScore(%llu, %d, ...[%d])",
					result->m_hSteamLeaderboard, totalScore, extraDataCount);
				uploadScoreCall.Set(SteamUserStats()->UploadLeaderboardScore(
					result->m_hSteamLeaderboard,
					k_ELeaderboardUploadScoreMethodForceUpdate,
					totalScore,
					extraData,
					extraDataCount
				), this, &UploadWorldScoreJob::UploadScoreCallback);
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

	void UploadWorldScore() override
	{
		const char* worldFilename = player.worldName;

		std::string fullFilename = "worlds/";
		fullFilename.append(worldFilename);

		world_t world;
		if (!LoadWorld(&world, fullFilename.c_str()))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UploadWorldScore(%s): bad LoadWorld", worldFilename);
			return;
		}

		worldData_t* progress = GetWorldProgressNoCreate(worldFilename);
		if (!progress)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UploadWorldScore(%s): no progress", worldFilename);
			return;
		}

		// raw new() so the job can delete itself when its callbacks complete
		new UploadWorldScoreJob(fullFilename.c_str(), &world, progress);
	}

	// ------------------------------------------------------------------------
	// Per-world score leaderboard display
	CCallResult<SteamManagerImpl, LeaderboardFindResult_t> findLeaderboardCall;
	SteamLeaderboard_t worldLeaderboardId = 0;

	void PrepWorldLeaderboard(const char* fullFilename) override
	{
		worldLeaderboardId = 0;
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "PrepWorldLeaderboard(%s)", fullFilename);
		findLeaderboardCall.Set(SteamUserStats()->FindLeaderboard(fullFilename), this, &SteamManagerImpl::FindLeaderboardCallback);
	}

	void FindLeaderboardCallback(LeaderboardFindResult_t* result, bool ioError)
	{
		if (!ioError && result->m_bLeaderboardFound && result->m_hSteamLeaderboard)
		{
			worldLeaderboardId = result->m_hSteamLeaderboard;
		}
		else
		{
			worldLeaderboardId = 0;
			//SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FindLeaderboard failed");
		}
	}

	uint64_t WorldHasLeaderboard() override
	{
		return worldLeaderboardId;
	}
	// Remainder in leaderboard.cpp
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
