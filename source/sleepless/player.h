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

#define PLYR_ACCEL	(FIXAMT)
#define PLYR_DECEL	(FIXAMT*3/4)
#define PLYR_MAXSPD (FIXAMT*6)

// secondary weapon defines
#define WPN_NONE     0
#define WPN_FLAME	 1
#define WPN_REFLECT	 2
#define WPN_PORTAL	 3
#define WPN_SPARKS	 4
#define WPN_SONIC	 5
#define WPN_BONEHEAD 6

#define MAX_WEAPONS	  7

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
	int xp,needXP;
	byte level;
	byte cheated;
	dword score;
	int var[8];	// local vars, cleared each new level
	byte ability[6];
	int brainX,brainY,candleX,candleY;
	word brainTime,candleTime;
	byte playAs;

	// Uses of journal array:
	// 0 - default journal page
	// 1-19 - journal page of level (N-1)
	// 20 - the fishing rod
	// [30-39] hold how many of each fish type you have caught
	// [40-49] show the largest size of each type
	// 50,51,52,53 are used when fishing
	// 54 is 1 if you have caught the unwanted goldfish
	// 55 is your fishing skill
	// 61 - got the yellow key in tomb of bones
	// 62 - mr. candle from ledger
	// 63 - won the game

	byte beenThere[MAX_MAPS];
	byte journal[64];
	byte jpage;
	short enterX,enterY;
	byte weaponLvl[6];
	word wpnXP[6];
	word hammerCharge;
	byte mapFrom;	// when you go to The Dumb Side, it needs to know which map you came from in case you fail
} player_t;

extern player_t player;
extern byte oldControls,newControls,checkTheControls;

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
void PlayerGetXP(int amt);

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
void ColorizeBouapha(void);
byte PlayerWpnColor(void);
void PlayerGetWeaponXP(byte wpn,word amt);

void DoPlayerFacing(byte c,Guy *me);
char *WeaponName(byte n);
char *WeaponDesc(byte n);

word NeedWeaponXP(byte wpn);

byte PlayerMaxLife(void);
word GetHammerCharge(void);
byte GetAmmoCost(byte wpn);
void UpdateGoodBone(void);
void PlayerCalcScore(void);

byte FakeGetControls(void);
byte PlayerPercent(void);

#endif
