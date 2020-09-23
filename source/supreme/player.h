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
#define WPN_NONE	 0
#define WPN_MISSILES 1 // 20
#define WPN_AK8087	 2 // 99
#define WPN_BOMBS	 3 // 5
#define WPN_FLAME	 4 // 50
#define WPN_PWRARMOR 5 // 1000
#define WPN_BIGAXE	 6 // 15
#define WPN_LIGHTNING 7 // 40
#define WPN_SPEAR	 8 // 20
#define WPN_MACHETE  9 // 30
#define WPN_MINES	 10 // 8
#define WPN_TURRET	 11 // 3
#define WPN_MINDCONTROL 12 // 1
#define WPN_REFLECTOR 13 // 40
#define WPN_JETPACK	  14 // 10
#define WPN_SWAPGUN	  15 // 3
#define WPN_TORCH	  16 // 50
#define WPN_SCANNER	  17 // 5
#define WPN_MINISUB	  18 // 1000
#define WPN_FREEZE	  19 // 5
#define WPN_STOPWATCH 20 // 1
#define WPN_BOOMERANG 	21 // 15
#define WPN_CACTUS 		22 // 50
#define WPN_ROCKETS 	23 // 5
#define WPN_WATERGUN 	24 // 10
#define WPN_MEGAPHONE	25 // 10
#define WPN_PUMPKIN 	26 // 8
#define WPN_DEATHRAY 	27 // 1
#define WPN_WHOOPIE 	28 // 10
#define WPN_BLACKHOLE 	29 // 3
#define WPN_MEDICKIT 	30 // 5
#define WPN_WAND 		31 // 8
#define WPN_BURNINATOR 	32 // 5
#define WPN_BFG 		33 // 8 
#define WPN_DOUBLEGUN 	34 // 10
#define WPN_SNOWBLOWER 	35 // 8
#define WPN_CONFUSE 	36 // 5
#define WPN_GRENADIER 	37 // 8

#define MAX_WEAPONS	  38

// vehicles you could be on
#define VE_NONE		0
#define VE_MINECART 1
#define VE_RAFT		2
#define VE_YUGO		3
#define VE_LOG		4

typedef struct player_t
{
	// per-world value
	char worldName[32];
	worldData_t *worldProg;
	// values reset for each level
	levelData_t *levelProg;
	byte shield;
	byte waterwalk;
	byte levelNum;
	byte keys[6];
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
	int timer; //new!
	byte pushPower;	// for pushing pushy blocks
	byte hammerFlags;
	byte cheatrFlags;
	byte vehicle;
	byte garlic;
	byte speed;	// accelerated
	byte rageClock;
	word rage;
	word varbar, varbarMax;
	byte varbarColor;
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
	byte timedout; //new!
	int curBossHP,maxBossHP;
	char curBossName[32];
	int c1_dym,c2_dym;
	Guy *boss;
	Guy *focus;
	int oxygen;
	int comboClock;
	int clock;
	int kills;
	int damageTaken;
	int bestCombo;
	int combo;
	int totalEnemies;
	int enemiesSlain;
	dword wand[4];
	byte cheated;
	dword score;
	int var[8];	// local vars, cleared each new level
	byte ability[6];
	int brainX,brainY,candleX,candleY;
	word brainTime,candleTime;
	byte playAs;
	bool camera;
	char *debugString;
} player_t;

extern player_t player;

void InitPlayer(byte level,const char *fname);
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
byte PlayerShieldWater(void);
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
void DoPlayerFacing(byte c,Guy *me);
byte FakeGetControls(void);

void SetPlayerStart(int x,int y);
void PutPlayerAtStart(Guy *g);

//new!
void IgniteVictim(Guy *me,byte amt);
void WeakenVictim(Guy *me,byte amt);
void ConfuseVictim(Guy *me,byte amt);
void HealVictim(Guy *me,byte amt);

byte ControlCheck(byte c);

void ShouldCheckControls(byte n);

#endif
