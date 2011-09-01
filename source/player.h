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
#define WPN_PWRARMOR 5
#define WPN_BIGAXE	 6
#define WPN_LIGHTNING 7
#define WPN_SPEAR	 8
#define WPN_MACHETE  9
#define WPN_MINES	 10
#define WPN_TURRET	 11
#define WPN_MINDCONTROL 12
#define WPN_REFLECTOR 13
#define WPN_JETPACK	  14
#define WPN_SWAPGUN	  15

// initializing constants (pass to InitPlayer)
#define INIT_GAME  2
#define INIT_WORLD 1
#define INIT_LEVEL 0

// vehicles you could be on
#define VE_NONE		0
#define VE_MINECART 1
#define VE_RAFT		2

typedef struct player_t
{
	// values for the overall game
	byte musicSettings;
	int prevScore;	// so you can lose all your points when you die
	int score;
	byte levelPassed[64][MAX_MAPS];
	byte keychain[64][4];
		// total completion is how many "points" the world has in it
	int  totalCompletion[64];
		// complete is how many of those points the player has, to create a percentage complete display
	int  complete[64];
	char customName[64][32];
	byte lunacyKey[64];
	// values reset for each world
	byte levelsPassed;
	byte worldNum;
	// values reset for each level
	byte shield;
	byte levelNum;
	byte keys[4];
	int  boredom;
	byte hammers;
	byte hamSpeed;
	byte weapon;
	int  ammo;
	byte reload;
	byte wpnReload;
	byte life;
	int brains;
	byte pushPower;	// for pushing pushy blocks
	byte hammerFlags;
	byte vehicle;
	byte garlic;
	byte speed;	// accelerated
	byte rageClock;
	word rage;
	byte invisibility;
	byte jetting;
} player_t;

extern player_t player;

void InitPlayer(byte initWhat,byte world,byte level);
void ExitPlayer(void);
void PlayerControlMe(Guy *me,mapTile_t *mapTile,world_t *world);
void PlayerControlPowerArmor(Guy *me,mapTile_t *mapTile,world_t *world);
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
void PoisonVictim(Guy *me,byte amt);
byte PlayerIsPoisoned(void);
void ToggleWaterwalk(void);
byte PlayerCanWaterwalk(void);
void SetTportClock(byte tp);
byte GetTportClock(void);
byte StealWeapon(void);

#endif
