#include "leaderboard.h"

#ifdef HAS_STEAM_API
#include <steam/steam_api.h>
#include "steam.h"
#include "world.h"
#include "control.h"

constexpr int NUM_SCORES = 20;

// Matches Pack* in steam.cpp
static void UnpackWorldProgress(int32_t packed, byte* keychains, float* percentage)
{
	*keychains = (packed & 0xff);
	int32_t percentage10x = (packed & 0x3ff00) >> 8;
	*percentage = float(percentage10x) / 10.0f;
}
static void UnpackMapScore(int32_t packed, dword* score, byte* playAs, byte* difficulty)
{
	*score = packed & 0x7ffffff;
	*difficulty = (packed & 0x18000000) >> 27;
	*playAs = (packed & 0xe0000000) >> 29;
}

struct Score
{
	CSteamID steamId;
	int32_t globalRank;
	int32_t score;
	byte keychains;
	float percentage;
};

struct LeaderboardDownload
{
	CCallResult<LeaderboardDownload, LeaderboardScoresDownloaded_t> scoresDownloadCall;
	std::string status;
	std::vector<Score> scores;

	explicit LeaderboardDownload(SteamLeaderboard_t id)
	{
		status = "Downloading...";
		scoresDownloadCall.Set(SteamUserStats()->DownloadLeaderboardEntries(
			id,
			k_ELeaderboardDataRequestGlobal,
			1,
			NUM_SCORES
		), this, &LeaderboardDownload::ScoresCallback);
	}

	void ScoresCallback(LeaderboardScoresDownloaded_t* result, bool ioError)
	{
		if (!ioError)
		{
			status = "";

			scores.reserve(result->m_cEntryCount);
			for (int i = 0; i < result->m_cEntryCount; ++i)
			{
				LeaderboardEntry_t entry;
				int32_t details[64];
				SteamUserStats()->GetDownloadedLeaderboardEntry(result->m_hSteamLeaderboardEntries, i, &entry, details, SDL_arraysize(details));

				Score score;
				score.steamId = entry.m_steamIDUser;
				score.globalRank = entry.m_nGlobalRank;
				score.score = entry.m_nScore;
				if (entry.m_cDetails > 0)
				{
					UnpackWorldProgress(details[0], &score.keychains, &score.percentage);
				}
				scores.push_back(score);
			}

			if (result->m_cEntryCount == 0)
			{
				status = "No scores!";
			}
		}
		else
		{
			status = "Error!";
		}
	}
};

TASK(void) ViewLevelScore(MGLDraw *mgl)
{
}

TASK(void) ViewWorldLeaderboard(MGLDraw *mgl, world_t* world)
{
	char buf[256];

	// Init
	mgl->LoadBMP("graphics/profmenu.bmp");
	byte backgd[640*480];
	for (int i=0; i<480; i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	sprite_set_t pauseJsp { "graphics/pause.jsp" };

	LeaderboardDownload download { SteamManager::Get()->WorldHasLeaderboard() };

	// Main
	GetTaps();
	while (true)
	{
		// Update
		if (mgl->LastKeyPressed() || mgl->MouseTap() || (GetTaps() & CONTROL_B1))
			break;

		// Render
		for (int i=0; i<480; i++)
			memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

		int y = 20;
		PrintGlow(20, y, world->map[0]->name, 0, 2);
		PrintGlow(460 - GetStrLength("*WIP*", 2), y, "*WIP*", 0, 2);
		y += 30;

		PrintGlow(45, y, "Player", 0, 2);
		PrintGlow(400 - GetStrLength("Score", 2), y, "Score", 0, 2);
		PrintGlow(500 - GetStrLength("Complete", 2), y, "Complete", 0, 2);
		y += 30;

		PrintGlow(20, y, download.status.c_str(), 0, 2);
		for (const auto& score : download.scores)
		{
			sprintf(buf, "%d.", score.globalRank);
			PrintGlow(40 - GetStrLength(buf, 2), y, buf, 0, 2);
			PrintGlow(45, y, SteamFriends()->GetFriendPersonaName(score.steamId), 0, 2);

			sprintf(buf, "%d", score.score);
			PrintGlow(400 - GetStrLength(buf, 2), y, buf, 0, 2);

			sprintf(buf, "%0.1f%%", score.percentage);
			PrintGlow(500 - GetStrLength(buf, 2), y, buf, 0, 2);

			y += 18;
		}

		if (/*mouseMode*/ true)
		{
			SetSpriteConstraints(13,13,627,467);
			int msx, msy;
			mgl->GetMouse(&msx, &msy);
			msx = std::clamp(msx, 13, 622);
			msy = std::clamp(msy, 13, 462);
			pauseJsp.GetSprite(0)->DrawBright(msx,msy,mgl,/*msBright/2*/0);
			ClearSpriteConstraints();
		}

		// Flip
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN;
	}

	// Exit
	CO_RETURN;
}

#else  // HAS_STEAM_API

TASK(void) ViewWorldLeaderboard(MGLDraw *mgl)
{
}

#endif  // HAS_STEAM_API
