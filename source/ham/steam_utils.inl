// Common utilities for <game>/steam.cpp.
// An inline rather than a normal header so that ham doesn't have to link
// steamapi and infect games that don't actually have a Steam mode.

#include <stdint.h>
#include <SDL.h>
#include <steam/steam_api.h>

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
