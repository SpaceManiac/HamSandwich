#ifndef PLAYER_H
#define PLAYER_H

#include "jamultypes.h"
#include "guy.h"
#include "control.h"
#include "game.h"
#include "display.h"
#include "bullet.h"
#include "world.h"
#include "intface.h"
#include "skills.h"
#include "runes.h"

#define SAVE_VERSION (1)

// level passed bitflags
#define LP_PASSED (1)
#define LP_GOTRUNE (2)
#define LP_GOTSPELL (4)
#define LP_GOTFAIRY (8)
#define LP_GOTSWORD (16)
#define LP_GOTRUNEPOUCH (32)
#define LP_GOTSKILLSHARD (64)

#define WATER_DAMAGE (10)	// damage taken when you fall in the water, in modern mode
#define MAX_RUNESTONES (9999)

enum class Difficulty :byte
{
	CLASSIC,
	MODERN,
	BRUTAL_CLASSIC,
	BRUTAL_MODERN,
	// ironman?
	UNUSED,	// for a save slot that isn't used
};

#define HAMMER_UP_AMOUNT 3
#define HAMMER_MIN_SPEED 12

#define MAX_PLAYERLEVEL (50)

// initializing constants (pass to InitPlayer)
#define INIT_GAME  2
#define INIT_WORLD 1
#define INIT_LEVEL 0

#define MUSIC_OFF  0

// items you can purchase (flags)
#define GEAR_HEART   (1)
#define GEAR_MOON    (2)
#define GEAR_LAMP    (4)
#define GEAR_SOUL    (8)
#define GEAR_SUN     (16)
#define GEAR_WISDOM  (32)
#define GEAR_POINTY  (64)
#define GEAR_MAGNET  (128)
#define GEAR_BOUNCY  (256)
#define GEAR_COMPASS (512)
#define GEAR_FEATHER (1024)
#define GEAR_SOCKS	 (2048)

typedef struct player_t
{
	// values for the overall game
	byte saveVersion;
	char saveCode[4];
	int score;
	byte levelPassed[6][MAX_MAPS-6];
	byte swordPiece[4];
	int  UNUSED1[6];		
	int  UNUSED2[6];
	char UNUSED3[32];
	byte UNUSED4[3];
	byte bobbyDoorOpen;
	byte prevLevel;	// in case you die
	word prevExp;	// in case you die
	word prevMoney;
	byte level;
	word experience;
	word needExp;
	byte spell[10];	// spells known (1=normal version, 2=enhanced, 0=unknown)
	byte boots,hat,staff;
	word money;
	int  maxSpeed;
	byte defense;
	byte damage;
	byte maxLife,maxMana;
	dword gear;
	dword gameClock;
	// values reset for each world
	byte levelsPassed;
	byte worldNum;
	int overworldX,overworldY;
	word chaseFairy;
	// values reset for each level
	byte usedFireballs;
	byte seekerKills;
	byte shield;
	byte levelNum;
	byte keys[4];
	int  boredom;
	byte hammers;
	byte hamSpeed;
	byte weapon;
	byte ammo;
	byte reload;
	byte wpnReload;
	byte life;
	byte mana;
	int brains;
	byte pushPower;	// for pushing pushy blocks
	byte hammerFlags;
	byte poison;
	byte curSpell;
	byte casting;
	word berserk;
	word stoneskin;
	byte rechargeClock;
	word haveFairy;	// bit flags for each fairy
	word fairyOn;	// which fairy is active
	// special new puzzle values
	byte templePuzzleDone;

	// real new stuff
	byte nightmare;
	byte gotSpell[20];	// specific tracking of which spellbooks you got, so you can't cheat
	byte powerStones;
	byte shieldStones;
	byte spellStones;
	byte vaultOpened;
	Difficulty difficulty;
	byte skillPts;
	byte skill[MAX_SKILLS];
	byte downgradeSpell[9];
	byte disableSword;
	byte disableThorns;
	byte disableMoveNShoot;
	byte disableDmgNumbers;
	byte timeSinceLastInferno;
	byte summonDmgBoost;
	byte taunted;
	byte parry;
	word bigMoney;
	word prevBigMoney;
	byte skillStones;
	byte runes[(int)Rune::NUM_RUNES];
	word runeStones;
	byte farley;
	byte barrier;
	byte storedFlames;
	byte flameCounter;
	byte infernoKills;
	byte summonKills;
	byte usedSpells[10];
	byte totalKills;	// capped at 255 of course
	word cumulativeBerserk;
	byte levelPassed2[6][6];	// the last 6 levels have to hide here to prevent old saves from breaking
	byte puzzleVar[3];	// special vars for an in-level puzzle
	byte runePouches;
	byte expando[166-MAX_SKILLS-(int)Rune::NUM_RUNES];	// just in case!
} player_t;

extern player_t player;
extern byte beenReborn;

void InitPlayer(byte initWhat,byte world,byte level);
void ExitPlayer(void);
bool PlayerHasTier2Spell(void);
void PlayerControlMe(Guy *me,mapTile_t *mapTile,world_t *world);
byte PlayerHasHammer(void);
byte PlayerGetItem(byte itm,int x,int y);
void PlayerWinLevel(byte w,byte l,byte isSecret);
void PlayerRenderInterface(MGLDraw *mgl);
int  PlayerBrains(void);
void SetPlayerHP(int hp);
byte PlayerLevelsPassed(void);
byte PlayerPassedLevel(byte world,byte map);
byte PlayerHasSwordPiece(byte w);
byte PlayerKeys(byte w);
void PlayerLoseKey(byte w);
int CalcGamePercent(player_t* p);
void PlayerHeal(byte amt);
void PlayerLoadGame(byte which);
void PlayerSaveGame(byte which);
byte GetPlayerWorld(void);
byte PlayerShield(void);
void PlayerGetPoints(int amt);
void PlayerResetScore(void);
void SetPlayerGlow(byte v);
byte GetPlayerGlow(void);
byte PlayerPushMore(void);
void PoisonPlayer(byte amt);
byte PlayerIsPoisoned(void);
void ToggleWaterwalk(void);
byte PlayerCanWaterwalk(void);
void SetTportClock(byte tp);
byte GetTportClock(void);
void DoPlayerFacing(Guy *me);
void SetPlayerDefense(void);
void SetPlayerDamage(void);
void SetPlayerSpeed(void);
byte PlayerHasSword(void);
void PlayerUpdateLife(void);
byte PlayerHasSpell(void);
void BeginArmageddon(void);
void ArmageddonUpdate(Map *map);
byte SwordAllCheck(void);
byte SpellBookForThisLevel(byte level,byte chapter);
byte FairyForThisLevel(word lvl);
byte GetVampyClock(void);
void ResetPlayerLevels(void);
void CheckForAllSecrets(void);
byte PlayerHasAllSecrets(byte chapter);
void SetChallengeCrystals(int amt);
int GetChallengeCrystals(void);
void BeginCast(Guy* me,byte spell);
void AddToRestorationBuffer(float amt);
bool ArmageddonIsUnderway(void);
void GainMoney(int amt);
bool CanAffordMoney(int amt);
int TotalMoney(void);
byte HighestWorldReached(void);
void PlayerHealWithSpell(byte amt);

void GetRunePouchInLevel(void);
void GetRuneInLevel(void);
void GetFairyBellInLevel(void);
void GetSwordInLevel(void);
void GetSpellInLevel(void);
void GetSkillShardInLevel(void);
bool GotRuneInLevel(byte world, byte level);
bool GotFairyBellInLevel(byte world, byte level);
bool GotSpellInLevel(byte world, byte level);
bool GotSwordInLevel(byte world, byte level);
bool GotRunePouchInLevel(byte world, byte level);
bool GotSkillShardInLevel(byte world, byte level);

void ResetLevelPassedFlags(int chapter, int level, bool resetAll);
byte* GetLevelPassedFlag(int chapter, int level);

void PickUpRune(void);

bool BrutalMode(void);
bool ClassicMode(void);

#endif
