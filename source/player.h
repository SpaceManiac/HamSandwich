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
#include "progress.h"

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
#define WPN_TORCH	  16
#define WPN_SCANNER	  17
#define WPN_MINISUB	  18
#define WPN_FREEZE	  19
#define WPN_STOPWATCH 20

#define MAX_WEAPONS	  21

// vehicles you could be on
#define VE_NONE		0
#define VE_MINECART 1
#define VE_RAFT		2
#define VE_YUGO		3

typedef struct player_t
{
	// per-world value
	char worldName[32];
	worldData_t *worldProg;
	// values reset for each level
	levelData_t *levelProg;
	byte shield;
	byte levelNum;
	byte keys[4];
	int  boredom;
	byte hammers;
	byte hamSpeed;
	byte weapon;
	byte lastWeapon;
	int  ammo;
	byte reload;
	byte wpnReload;
	int life;
	int brains;
	int coins;
	int candles;
	byte pushPower;	// for pushing pushy blocks
	byte hammerFlags;
	byte vehicle;
	byte garlic;
	byte speed;	// accelerated
	byte rageClock;
	word rage;
	byte invisibility;
	byte jetting;
	byte lavaTimer;
	byte stealthy;
	byte perfect;
	byte timeStop;
	byte ammoCrate;
	byte torch;
	byte cheesePower;
	byte gotRecords;
	byte spotted;
	int oxygen;
	int comboClock;
	int clock;
	int kills;
	int damageTaken;
	int bestCombo;
	int combo;
	int totalEnemies;
	int enemiesSlain;
	byte cheated;
	dword score;
	int var[8];	// local vars, cleared each new level
	byte ability[6];
	int brainX,brainY,candleX,candleY;
	word brainTime,candleTime;
	byte playAs;
} player_t;

extern player_t player;

void InitPlayer(byte level,char *fname);
void ExitPlayer(void);
void PlayerControlMe(Guy *me,mapTile_t *mapTile,world_t *world);
void PlayerControlPowerArmor(Guy *me,mapTile_t *mapTile,world_t *world);
void PlayerControlMiniSub(Guy *me,mapTile_t *mapTile,world_t *world);
byte PlayerHasHammer(void);
void PlayerWinLevel(byte isSecret);
void PlayerRenderInterface(MGLDraw *mgl);
int  PlayerBrains(void);
void SetPlayerHP(int hp);
byte PlayerKeyChain(byte w);
byte PlayerKeys(byte w);
void PlayerLoseKey(byte w);
void PlayerHeal(byte amt);
void PlayerLoadGame(byte which);
void PlayerSaveGame(byte which);
byte PlayerShield(void);
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

int WeaponMaxAmmo(byte wpn);
byte PlayerGetWeapon(byte wpn,int x,int y);
byte PlayerPowerup(char powerup);
void PlayerRadioactiveFood(void);
void PlayerChineseFood(void);
void KeyChainAllCheck(void);
void PlayerGetBrain(int amt);
void PlayerGetCandle(int amt);
void PlayerGetCoin(int amt);

void SetPlayerStart(int x,int y);
void PutPlayerAtStart(Guy *g);

byte ControlCheck(byte c);

void ShouldCheckControls(byte n);

#endif
