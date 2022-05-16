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

// initializing constants (pass to InitPlayer)
#define INIT_GAME  2
#define INIT_WORLD 1
#define INIT_LEVEL 0

#define MUSIC_OFF  0
#define MUSIC_ON   1
#define MUSIC_RAND 2

typedef struct player_t
{
	// values for the overall game
	byte musicSettings;
	int prevScore;	// so you can lose all your points when you die
	int score;
	byte levelPassed[6][MAX_MAPS];
	byte keychain[6][4];
		// total completion is how many "points" the world has in it
	int  totalCompletion[6];
		// complete is how many of those points the player has, to create a percentage complete display
	int  complete[6];
	char customName[32];
	byte lunacyKey[4];
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
	byte ammo;
	byte reload;
	byte wpnReload;
	byte life;
	int brains;
	byte pushPower;	// for pushing pushy blocks
	byte hammerFlags;
	byte poison;
} player_t;

extern player_t player;

void InitPlayer(byte initWhat,byte world,byte level);
void ExitPlayer(void);
void PlayerControlMe(Guy *me,mapTile_t *mapTile,world_t *world);
byte PlayerHasHammer(void);
byte PlayerGetItem(byte itm);
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

#endif
