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

// secondary weapon defines
#define WPN_NONE     0
#define WPN_MISSILES 1
#define WPN_AK8087	 2
#define WPN_BOMBS	 3
#define WPN_FLAME	 4

#define HAMMER_UP_AMOUNT 3
#define HAMMER_MIN_SPEED 12

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
	byte musicSettings;
	int prevScore;	// so you can lose all your points when you die
	int score;
	byte levelPassed[6][MAX_MAPS];
	byte keychain[4];
		// total completion is how many "points" the world has in it
	int  totalCompletion[6];
		// complete is how many of those points the player has, to create a percentage complete display
	int  complete[6];
	char customName[32];
	byte lunacyKey[4];
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
	byte chaseFairy[4];
	// values reset for each level
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
	byte expando[252];	// just in case!
} player_t;

extern player_t player;
extern byte beenReborn;

void InitPlayer(byte initWhat,byte world,byte level);
void ExitPlayer(void);
void PlayerControlMe(Guy *me,mapTile_t *mapTile,world_t *world);
byte PlayerHasHammer(void);
byte PlayerGetItem(byte itm,int x,int y);
void PlayerWinLevel(byte w,byte l,byte isSecret);
void PlayerRenderInterface(MGLDraw *mgl);
int  PlayerBrains(void);
void SetPlayerHP(int hp);
byte PlayerLevelsPassed(void);
byte PlayerPassedLevel(byte world,byte map);
byte PlayerKeyChain(byte w);
byte PlayerKeys(byte w);
void PlayerLoseKey(byte w);
void PlayerSetWorldWorth(byte world,int amt);
float PlayerGetPercent(byte world);
float PlayerGetGamePercent(void);
void SetCustomName(const char *name);
char *GetCustomName(void);
byte PlayerHasLunacyKey(byte w);
void PlayerHeal(byte amt);
void PlayerLoadGame(byte which);
void PlayerSaveGame(byte which);
byte GetPlayerWorld(void);
byte PlayerShield(void);
void PlayerGetPoints(int amt);
void PlayerResetScore(void);
byte PlayerGetMusicSettings(void);
void PlayerSetMusicSettings(byte m);
void SetPlayerGlow(byte v);
byte GetPlayerGlow(void);
byte PlayerPushMore(void);
void PoisonPlayer(byte amt);
byte PlayerIsPoisoned(void);
void ToggleWaterwalk(void);
byte PlayerCanWaterwalk(void);
void SetTportClock(byte tp);
byte GetTportClock(void);
void DoPlayerFacing(byte c,Guy *me);
void SetPlayerDefense(void);
void SetPlayerDamage(void);
void SetPlayerSpeed(void);
byte PlayerHasSword(void);

byte PlayerHasSpell(void);
void BeginArmageddon(void);
void ArmageddonUpdate(Map *map);
byte KeyChainAllCheck(void);
byte SpellBookForThisLevel(byte level,byte chapter);
byte FairyForThisLevel(word lvl);
byte GetVampyClock(void);
void ResetPlayerLevels(void);
void CheckForAllSecrets(void);
byte PlayerHasAllSecrets(byte chapter);
void SetChallengeCrystals(int amt);
int GetChallengeCrystals(void);

#endif
