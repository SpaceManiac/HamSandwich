#ifndef PROGRESS_H
#define PROGRESS_H

#include "mgldraw.h"
#include "monster.h"

// level progress flags
#define LF_PASSED		2	// have passed it
#define LF_CANDLES		4	// have collected all candles

// keychain bitflags
#define KC_KEYCH1		1
#define KC_KEYCH2		2
#define KC_KEYCH3		4
#define KC_KEYCH4		8
#define KC_LOONY		16

#define NUM_PLAYLISTS	4
#define SONGNAME_LEN	128

// level percentage works like this:
// 40% for passing it
// 20% for candles
// 20% for bonus goal
// 20% for secrets
// if a level lacks any of those, you just get percentage credit for them instantly
// (but still zero percent until you pass the level!)

// world percentage is:
// 100 points per level (you get the level percentage's worth of those 100)
// 25 points per keychain (so all four equals 1 level)
// 100 points for loonykey (get free credit for it, and instantly get it, if you enter a world
//							that lacks one)

// total percentage is:
// 1000 points per world (you get the world's percentage of that)
// N points per item in the shop you've bought (N=price of item, includes worlds)
// 10 points per monster in bestiary (must get them all!)
// 250 points per weapon in armory (must get them all!)
// 20 points per star (50 total, 1000 points)


typedef struct levelData_t	// contains your scores, etc for one level
{
	byte levelNum;		// because only passed levels are stored
	byte flags;
	float percentage;
	float recordDestroy;
	int   recordCombo;
	dword recordBaseScore;
} levelData_t;

typedef struct worldData_t	// contains your progress for one world
{
	char filename[32];	// the world's filename
	int  var[8];		// the 'global' variables for this world
	byte keychains;		// bitflags for each one, and the loonykey
	float percentage;
	byte levelOn;		// which level of this world you're on
	byte levels;		// how many levels are stored here - it only stores ones you've either
						// passed, or accomplished something in
	levelData_t *level;	// progress info for each level that HAS been passed only.
} worldData_t;

enum class HudChoice : byte  // SERIALIZED in the player profile.
{
	// Supreme With Cheese, and the default choice.
	Supreme  = 0,
	// Like classic Dr. Lunatic. See `RenderInterfaceOld` for details.
	Classic  = 1,
	// Supreme, plus weapon lock indicator and speedrun timer.
	Advanced = 2,
};

#define EXPANSION_SIZE	(1018)

// WARNING: changing this struct's contents may break save compatibility or require modifications in Load/SaveProfile.
typedef struct progress_t
{
	// total values for stats
	dword totalCandles;
	dword totalBrains;
	dword totalTime;
	dword totalCoins,coinsSpent;
	dword totalWorlds;
	dword loonyKeys,loonyKeysUsed;
	float totalPercent;	// percentage of ALL worlds/levels/etc you have completed

	dword hammersThrown;
	dword damageDone,damageTaken;
	dword shotsFired;
	dword rages;
	dword runOver;
	word finishedWorlds;	// worlds you've 100%ed
	dword footDistance,raftDistance,cartDistance,driveDistance;
	dword underwaterTime;
	dword grassChopped;
	dword keysFound,doorsOpened;
	dword calories,calsBurned;
	word  bestCombo;

	int num_worlds;
	worldData_t *world;
	word kills[NUM_PROFILE_MONSTERS];	// how many times you've killed each type
	byte scanned[NUM_PROFILE_MONSTERS];	// has each type been scanned?
	byte purchase[256];			// which things you have purchased
	byte movie[20];				// which movies you've seen for theater purposes
	byte goal[100];				// whether you've done each of the 100 goals

	dword cheats;				// how often you've cheated
	byte wpnLock;				// weapon lock
	HudChoice hudChoice;
	byte expansion[EXPANSION_SIZE];		// unused space for possible future expansion
} progress_t;

typedef struct playList_t
{
	byte numSongs;
	char *song;
} playList_t;

// Difficulty levels. SERIALIZED in profile, specials, and leaderboards.
enum : byte
{
	DIFFICULTY_NORMAL  = 0,
	DIFFICULTY_HARD    = 1,
	DIFFICULTY_LUNATIC = 2,

	MAX_DIFFICULTY  // End marker. Add new entries above this.
};
const char* GetDifficultyName(int difficulty);

// WARNING: changing this struct's contents may break save compatibility or require modifications in Load/SaveProfile.
typedef struct profile_t
{
	char name[16];
	// important stuff
	byte control[2][6];	// key scancodes
	byte joyCtrl[2];	// joystick 'codes' for the buttons
	byte sound;			// sound volume
	byte music;			// music volume
	byte musicMode;		// music playing mode

	char lastWorld[32];	// name of the last world visited
	playList_t playList[NUM_PLAYLISTS];	// song playlists
	byte difficulty;
	byte playAs;
	byte moveNShoot,candleRadar,brainRadar;
	byte nameVerified;
	progress_t progress;
	char motd[1024];	// message of the day
} profile_t;

extern profile_t profile;
extern byte modeShopNum[10];

void InitProfile(void);
void SaveProfile(void);
void LoadProfile(const char *name);
void FreeProfile(void);
void DefaultControls(void);
void DefaultProfile(const char *name);

void SetFirstTime();
byte FirstTime(void);
void EraseWorldProgress(const char *fname);
worldData_t *GetWorldProgress(const char *fname);
worldData_t *GetWorldProgressNoCreate(const char *fname);
levelData_t *GetLevelProgress(const char *fname,byte levelNum);
byte LevelsPassed(worldData_t *world);
byte LevelIsPassed(worldData_t *world,byte level);
void ClearTestProgress(void);

void StoreWorldResults(worldData_t *me,world_t *world);

dword CountLevelsPassed(void);
void CalcFinishedWorlds(void);

void ApplyControlSettings();

#endif
