#include "leaderboard.h"

#ifdef HAS_STEAM_API
#include <algorithm>
#include <steam/steam_api.h>
#include "steam.h"
#include "world.h"
#include "control.h"
#include "progress.h"
#include "dialogbits.h"

constexpr int NUM_SCORES = 20;
constexpr int BTN_HEIGHT = 19;

constexpr int SPR_LOONYKEY = 50;
constexpr int SPR_KEYCH = 52;

static const char diffName[][16]={"Normal","Hard","Lunatic"};
static const char charName[][16]={"Bouapha","Happy Stick Man","Dr. Lunatic","Shtupid Shroom","LunaChick","MechaBouapha"};

// Matches Pack* in steam.cpp
static void UnpackWorldProgress(int32_t packed, byte* keychains, float* percentage)
{
	// 8 bits keychains (5 used).
	// 10 bits for progress out of 1000.
	// 14 bits reserved.
	*keychains = (packed & 0xff);
	int32_t percentage10x = (packed & 0x3ff00) >> 8;
	*percentage = float(percentage10x) / 10.0f;
}
static void UnpackMapScore(int32_t packed, dword* score, byte* playAs, byte* difficulty)
{
	// 24 bits score. Maxes at 16,777,215.
	// 1 bit reserved.
	// 2 bits difficulty. Normal, Hard, Lunatic, unused.
	// 3 bits playAs.
	// 2 bits reserved.
	*score = packed & 0xffffff;
	*difficulty = (packed & 0x6000000) >> 25;
	*playAs = (packed & 0x38000000) >> 27;
}

struct Score
{
	CSteamID steamId;
	int32_t globalRank;
	int32_t score;
	byte keychains;
	float percentage;

	int32_t details[k_cLeaderboardDetailsMax];
	int numDetails;
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
				Score score;
				SteamUserStats()->GetDownloadedLeaderboardEntry(result->m_hSteamLeaderboardEntries, i, &entry, score.details, SDL_arraysize(score.details));
				score.steamId = entry.m_steamIDUser;
				score.globalRank = entry.m_nGlobalRank;
				score.score = entry.m_nScore;
				score.numDetails = entry.m_cDetails;
				if (entry.m_cDetails > 0)
				{
					UnpackWorldProgress(score.details[0], &score.keychains, &score.percentage);
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

static void RenderButton(MGLDraw *mgl, int x, int y, int wid, std::string_view txt, bool hovered)
{
	if (hovered)
	{
		mgl->Box(x,y,x+wid,y+BTN_HEIGHT,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+BTN_HEIGHT-1,32+8);
	}
	else
		mgl->Box(x,y,x+wid,y+BTN_HEIGHT,32+16);
	PrintGlow(x+2, y+2, txt, 0, 2);
}

static TASK(void) ViewDetails(MGLDraw *mgl, const byte* backgd, const sprite_set_t& pauseJsp, const world_t* world, const Score& score, WorldLeaderboardKind kind)
{
	// Init
	constexpr int ENTRIES_PER_SCREEN = 20;
	char buf[256];

	struct Entry
	{
		int num;
		const char* name;
		dword score;
		byte playAs;
		byte difficulty;
	};
	std::vector<Entry> entries;
	size_t detailsIdx = 1;
	for (int i = 0; i < world->numMaps && detailsIdx < SDL_arraysize(score.details); ++i)
	{
		if (world->map[i]->flags & MAP_HUB)
			continue;

		int32_t details = score.details[detailsIdx++];
		if (details == (kind == WorldLeaderboardKind::Time ? ~0 : 0))
			continue;

		Entry entry;
		entry.num = i;
		entry.name = world->map[i]->name;
		UnpackMapScore(details, &entry.score, &entry.playAs, &entry.difficulty);
		entries.push_back(entry);
	}

	enum ButtonId
	{
		None,
		Back,
	} curButton = ButtonId::None;
	int msz = mgl->mouse_z, offset = 0;
	int msx, msy;

	// Main
	GetTaps();
	mgl->GetMouse(&msx, &msy);
	while (true)
	{
		// Update
		int oldmsx = msx, oldmsy = msy;
		mgl->GetMouse(&msx, &msy);
		bool mb = mgl->MouseTap();
		if (mb || oldmsx != msx || oldmsy != msy)
		{
			curButton = ButtonId::None;
			int y = 18;
			if (PointInRect(msx, msy, 640-20-100, y-2, 640-20, y-2+BTN_HEIGHT))
			{
				curButton = ButtonId::Back;
			}
		}

		offset = std::max(0, std::min(offset + msz - mgl->mouse_z, int(entries.size()) - ENTRIES_PER_SCREEN));
		msz = mgl->mouse_z;

		byte taps = GetTaps() | GetArrowTaps();
		if (taps & CONTROL_UP)
		{
			if (offset > 0)
				offset--;
		}
		if (taps & CONTROL_DN)
		{
			if (offset < int(entries.size()) - ENTRIES_PER_SCREEN)
				offset++;
		}
		if (mb || (taps & CONTROL_B1))
		{
			if (curButton == ButtonId::Back)
				break;
		}
		if (taps & CONTROL_B2)
			break;

		if (mgl->LastKeyPressed() == 27)
			break;

		if (taps)
			curButton = ButtonId::Back;

		// Render
		for (int i=0; i<480; i++)
			memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

		int y = 18;
		snprintf(buf, sizeof(buf), "%s - %d. %s", world->map[0]->name, score.globalRank, SteamFriends()->GetFriendPersonaName(score.steamId));
		PrintGlow(20, y, buf, 0, 2);
		RenderButton(mgl, 640-20-100, y-2, 100, "Back", curButton == ButtonId::Back);
		y += 24;

		PrintGlow(45, y, "Level", 0, 2);
		std::string_view score_title = kind == WorldLeaderboardKind::Time ? "Level Time" : "Level Score";
		PrintGlow(390 - 1 - GetStrLength(score_title, 2), y, score_title, 0, 2);
		PrintGlow(530 - GetStrLength("Character", 2), y, "Character", 0, 2);
		PrintGlow(600 - GetStrLength("Difficulty", 2), y, "Difficulty", 0, 2);
		y += 24;

		if (offset > 0)
			PrintGlow(610, y, "^", 0, 2);
		for (int i = 0; i < ENTRIES_PER_SCREEN && offset + i < int(entries.size()); ++i)
		{
			const Entry& entry = entries[offset + i];

			snprintf(buf, sizeof(buf), "%d.", entry.num);
			PrintGlow(40 - GetStrLength(buf, 2), y, buf, 0, 2);
			PrintGlow(45, y, entry.name, 0, 2);

			if (kind == WorldLeaderboardKind::Time)
			{
				float seconds = (entry.score % 1800) / 30.0f;
				int minutes = entry.score / 1800;
				snprintf(buf, sizeof(buf), "%d:%05.2f", minutes, seconds);
			}
			else
			{
				snprintf(buf, sizeof(buf), "%d", entry.score);
			}
			PrintGlow(390 - GetStrLength(buf, 2), y, buf, 0, 2);

			if (entry.playAs < SDL_arraysize(charName))
			{
				PrintGlow(530 - GetStrLength(charName[entry.playAs], 2), y, charName[entry.playAs], 0, 2);
			}

			if (entry.difficulty < SDL_arraysize(diffName))
			{
				PrintGlow(600 - GetStrLength(diffName[entry.difficulty], 2), y, diffName[entry.difficulty], 0, 2);
			}

			y += 20;
		}
		if (offset < int(entries.size()) - ENTRIES_PER_SCREEN)
			PrintGlow(610, y - 20, "v", 0, 2);

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
		if (!mgl->Process())
			CO_RETURN;
	}
}

TASK(void) ViewWorldLeaderboard(MGLDraw *mgl, const world_t* world, WorldLeaderboardKind kind, uint64_t id)
{
	char buf[256];

	// Init
	mgl->LoadBMP("graphics/profmenu.bmp");
	byte backgd[640*480];
	for (int i=0; i<480; i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	sprite_set_t pauseJsp { "graphics/pause.jsp" };
	sprite_set_t intfaceSpr { "graphics/intface.jsp" };

	LeaderboardDownload download { id };

	enum ButtonId
	{
		None,
		Back,
		Details,
		Details_End = Details + NUM_SCORES,
	} curButton = ButtonId::None;
	int msx, msy;

	// Main
	GetTaps();
	mgl->GetMouse(&msx, &msy);

	while (true)
	{
		// Update
		byte taps = GetTaps() | GetArrowTaps();
		bool mb = mgl->MouseTap();

		int oldmsx = msx, oldmsy = msy;
		mgl->GetMouse(&msx, &msy);
		if (mb || msx != oldmsx || msy != oldmsy)
		{
			curButton = ButtonId::None;
			int y = 18;
			if (PointInRect(msx, msy, 640-20-100, y-2, 640-20, y-2+BTN_HEIGHT))
			{
				curButton = ButtonId::Back;
			}
			y += 24;
			y += 24;
			for (size_t i = 0; i < download.scores.size(); ++i)
			{
				if (PointInRect(msx, msy, 640-20-80, y-2, 640-20, y-2+BTN_HEIGHT))
				{
					curButton = ButtonId(ButtonId::Details + i);
				}
				y += 20;
			}
		}

		if (taps & CONTROL_UP)
		{
			if (curButton == ButtonId::None || curButton == ButtonId::Back)
				curButton = ButtonId(ButtonId::Details + download.scores.size() - 1);  // becomes Back if size=0
			else if (curButton == ButtonId::Details)
				curButton = ButtonId::Back;
			else if (curButton >= ButtonId::Details && curButton < ButtonId::Details + download.scores.size())
				curButton = ButtonId(curButton - 1);
		}

		if (taps & CONTROL_DN)
		{
			if (curButton == ButtonId::None || curButton == ButtonId::Details + download.scores.size() - 1)
				curButton = ButtonId::Back;
			else if (curButton == ButtonId::Back && !download.scores.empty())
				curButton = ButtonId::Details;
			else if (curButton >= ButtonId::Details && curButton < ButtonId::Details + download.scores.size())
				curButton = ButtonId(curButton + 1);
		}

		if (mb || (taps & CONTROL_B1))
		{
			if (curButton == ButtonId::Back)
			{
				break;
			}
			else if (curButton >= ButtonId::Details && curButton < ButtonId::Details + download.scores.size())
			{
				AWAIT ViewDetails(mgl, backgd, pauseJsp, world, download.scores[curButton - ButtonId::Details], kind);
				continue;
			}
		}

		if (taps & CONTROL_B2)
			break;

		if (mgl->LastKeyPressed() == 27)
			break;

		// Render
		for (int i=0; i<480; i++)
			memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

		int y = 18;
		snprintf(buf, sizeof(buf), "%s", world->map[0]->name);
		PrintGlow(20, y, buf, 0, 2);
		RenderButton(mgl, 640-20-100, y-2, 100, "Back", curButton == ButtonId::Back);
		y += 24;

		PrintGlow(45, y, "Player", 0, 2);
		std::string_view score_title = kind == WorldLeaderboardKind::Time ? "World Time" : "World Score";
		PrintGlow(390 - 1 - GetStrLength(score_title, 2), y, score_title, 0, 2);
		PrintGlow(530 - GetStrLength("Complete", 2), y, "Complete", 0, 2);
		y += 24;

		PrintGlow(20, y, download.status.c_str(), 0, 2);
		for (size_t i = 0; i < download.scores.size(); ++i)
		{
			const auto& score = download.scores[i];
			snprintf(buf, sizeof(buf), "%d.", score.globalRank);
			PrintGlow(40 - GetStrLength(buf, 2), y, buf, 0, 2);
			PrintGlow(45, y, SteamFriends()->GetFriendPersonaName(score.steamId), 0, 2);

			if (kind == WorldLeaderboardKind::Time)
			{
				if (score.score >= INT32_MAX - MAX_MAPS)
				{
					int mapsMissed = -(INT32_MAX - score.score - world->numMaps);
					int totalMaps = 0;
					for (int j = 0; j < world->numMaps; ++j)
						if (!(world->map[j]->flags & MAP_HUB))
							++totalMaps;
					snprintf(buf, sizeof(buf), "(%d to go)", mapsMissed);
				}
				else
				{
					float seconds = (score.score % 60000) / 1000.0f;  // largest legal value is 59.97
					int minutes = score.score / 60000;
					snprintf(buf, sizeof(buf), "%d:%05.2f", minutes, seconds);
				}
			}
			else
			{
				snprintf(buf, sizeof(buf), "%d", score.score);
			}
			PrintGlow(390 - GetStrLength(buf, 2), y, buf, 0, 2);

			snprintf(buf, sizeof(buf), "%0.1f%%", score.percentage);
			PrintGlow(530 - GetStrLength(buf, 2), y, buf, 0, 2);

			if (score.keychains & KC_LOONY)
				intfaceSpr.GetSprite(SPR_LOONYKEY)->Draw(457,y+1,mgl);
			if (score.keychains & KC_KEYCH1)
				intfaceSpr.GetSprite(SPR_KEYCH+3)->Draw(429,y-13,mgl);
			if (score.keychains & KC_KEYCH2)
				intfaceSpr.GetSprite(SPR_KEYCH)->Draw(408,y,mgl);
			if (score.keychains & KC_KEYCH3)
				intfaceSpr.GetSprite(SPR_KEYCH+2)->Draw(419,y-13,mgl);
			if (score.keychains & KC_KEYCH4)
				intfaceSpr.GetSprite(SPR_KEYCH+1)->Draw(418,y,mgl);

			RenderButton(mgl, 640-20-80, y-2, 80, "Details >>", curButton == ButtonId::Details + i);

			y += 20;
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
		if (!mgl->Process())
			CO_RETURN;
	}

	// Exit
	CO_RETURN;
}

#else  // HAS_STEAM_API

TASK(void) ViewWorldLeaderboard(MGLDraw *mgl, const world_t* world)
{
	CO_RETURN;
}

#endif  // HAS_STEAM_API
