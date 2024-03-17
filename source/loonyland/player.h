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

#define MAX_MONEY	250
#define COMBO_TIME	30*2

// initializing constants (pass to InitPlayer)
#define INIT_GAME  2
#define INIT_WORLD 1
#define INIT_LEVEL 0

// var constants to keep things clear
	// powerups
#define VAR_HEART		0	// 0-19 = super hearts
#define VAR_LIGHTNING	20	// 20-29 = lightning bolts
#define VAR_ARROW		30	// 30-39 = arrows
#define VAR_PANTS		40	// 40-49 = pants
	// quests
#define VAR_QUESTASSIGN 50	// 50-69 = quest (N-50) has been assigned
#define VAR_QUESTDONE	70	// 70-89 = quest (N-70) is complete
	// inventory
#define VAR_MUSHROOM	90	// 90-99 = mushrooms
#define VAR_MYSORB		100 // 100-103 = have mystery orb
#define VAR_WEAPON		104	// 104-110 = have access to weapon
#define VAR_KEY			111	// 111-113 = have the three special keys
#define VAR_BOOTS		114	// 114 = have the rubber booties
#define VAR_STICK		115	// 115 = have the stick
#define VAR_TORCH		116	// 116 = the stick is lit
#define VAR_FERTILIZER	117	// 117 = have fertilizer
#define VAR_SILVER		118 // 118 = have the silver
#define VAR_DAISY		119 // 119 = have doom daisy
#define VAR_POTION		120 // 120 = have ghost potion
#define VAR_VAMPBUST	121 // 121-128 = have vamp bust
#define VAR_CAT			129	// 129 = have cat
#define VAR_GEM			130 // 130-135 = super gems
	// special stuff
#define VAR_ORBSTAND	150 // 150-153 = have installed orb
#define VAR_BOOTSALES	154	// 154 = have talked to boot salesman once
#define VAR_WITCHTALK	155	// 155 = number of times you've annoyed the witch
#define VAR_BRIDGEOPEN	156	// 156 = the Underground Tunnel bridge has been opened
#define VAR_ZOMBIE		157	// 157-159 = each crypt has been cleaned out
#define VAR_ZOMBIEREWARD 160	// 160 = have received the zombie reward money
#define VAR_TRIPLEFIRE	161	// 161 = have triple-fire crystal
#define VAR_TALISMAN	162	// 162 = have happy stick talisman
#define VAR_VAMPYWALL	163	// 163 = wall to vampy land opened
#define VAR_BATSTATUE	164	// 164-167 = have each bat statue
#define VAR_BATSTAND	168 // 168-171 = bat statue is on stand
#define VAR_VAMPSTAND	172 // 172-179 = vamp bust is on stand
#define VAR_KNOWWOLVES	180	// 180 = have already talked to self about werewolves
#define VAR_DOLL		181	// 181-188 = have doll
#define VAR_REFLECT		189	// 189 = have reflect gem
#define VAR_DOLLGIVEN	190	// 190-197 = have given doll to girl
#define VAR_TOWNOPEN	198 // 198 = have opened zombiton
#define VAR_LEFTBOMB	199	// 199 = have left bomb room with bombs (so remove the powerups)
#define VAR_HELPERBAT	200	// 200 = have the helper bat
#define VAR_TALKBAT		201	// 201 = have talked to the bat once
#define VAR_TALKBRIDGE	202	// 202 = have talked to the bridge guy
#define VAR_PAIDBRIDGE	203	// 203 = have paid the bridge guy
#define VAR_PORTALOPEN	204	// 204 = the portal is officially open
#define VAR_GAVEDAISY	205	// 205 = gave daisy to witch
#define VAR_DEADELDER	206	// 206-209 = elder has been beaten
#define VAR_GATEOUT		210	// 210 = mystery lake electrogate is shorted out
#define VAR_DEADBONK	211	// 211 = bonkula is dead
#define VAR_SEERTALK	212	// 212 = has talked to seer
#define VAR_MINERTALK	213	// 213 = have talked to miner
#define VAR_HPSALES		214	// 214 = have talked to hot pants salesman
#define VAR_DEALERTALK	215	// 215 = have talked to arms dealer
#define VAR_HIPUZZLE	216	// 216 = the HI puzzle is solved in Remix mode

	// temp variables
#define VAR_TEMP		240	// 240-255 = temporary variables - cleared when leaving a level

// player cheat flags (for whole-game-only cheats)
#define PC_TERROR	1
#define PC_MAXPOWER	2
#define PC_PLAYTOAD 4	// play as toad
#define PC_PLAYBONK 8	// play as bonkula
#define PC_PLAYDOG	16	// play as swampdog
#define PC_HARDCORE 32	// hardcore mode
#define PC_ALLACCESS 64	// all access pass
#define PC_INFSURV	128	// infinite survival mode
#define PC_PLAYWITCH 256	// play as the witch
#define PC_PLAYWOLF	512		// play as wolf-loony
#define PC_PLAYSUMMON 1024	// play as summony the elder
#define PC_PLAYTHIEF  2048	// play as the thief

typedef struct player_t
{
	int money;
	int maxMoney;
	byte shield;
	byte worldNum;
	byte levelNum;
	byte keys[4];
	int  boredom;

	byte hearts;
	byte maxHearts;
	byte firePower;
	byte fireRate;
	byte fireFlags;
	byte fireRange;

	byte weapon;
	byte wpnLevel;
	byte wpnCost;
	byte reload;
	byte wpnReload;
	byte pushPower;	// for pushing pushy blocks
	byte chatClock;
	byte saveClock;
	byte stone;

	byte lastSave;	// which save slot was used last, for when you die
	int  invinc;
	int  poison;
	int  destx,desty;	// where the player should be put, used between levels, and for
						// saved games
	byte speed;	// accelerated
	char areaName[32];

	byte difficulty;
	dword monsterPoints;
	dword batLevel;
	int  monsType;
	word maxMaxHearts;
	word startHearts;
	dword cheatsOn;		// flags for which whole-game-only cheats are on in this game
	dword xtraVar;		// an extra variable for some characters
	byte crystalBall;
	byte xtraByte;		// an extra byte for some characters
	byte clockRunning;	// if the time limit clock should be going

	// witch's spell XP
	word spellXP[8];
	byte oob;			// out-of-bounds counter for loonyball
	byte expando[20];	// expansion bytes

	dword combo;
	byte  comboClock;
	dword timeLimit;	// time passed for certain short-term timed things, like boss-killing
	// end-game stats
	dword timeToFinish;
	dword bestCombo;
	dword shotsFired;
	dword specialShotsFired;
	dword guysKilled;
	dword gemsGotten;
	dword numSaves;
	dword hitsTaken;
	byte  rank;
	// vars
	byte  var[256];
} player_t;

extern player_t player;

void InitPlayer(byte initWhat,byte world,byte level);
void ExitPlayer(void);
void PlayerControlMe(Guy *me,Map *map,mapTile_t *mapTile,world_t *world);

void DoPlayerFacing(byte c,Guy *me);
byte PlayerHasHammer(void);
byte PlayerGetItem(byte itm,int x,int y);
void PlayerWinLevel(byte w,byte l,byte isSecret);
void SetPlayerHP(int hp);
byte PlayerShield(void);
void SetPlayerGlow(byte v);
byte GetPlayerGlow(void);
byte PlayerPushMore(void);
void PoisonPlayer(byte amt);
byte PlayerIsPoisoned(void);
void SetTportClock(byte tp);
byte GetTportClock(void);
float CalcPercent(const player_t *p);
void PlayerSetVar(int v,int val);
void PlayerClearTempVars(void);
void PlayerGetWeapon(byte wpn);
byte ShroomCount(void);
byte OrbCount(void);
byte BatCount(void);
byte VampCount(void);
byte OrbsUsed(void);
byte BatsUsed(void);
byte VampsUsed(void);
void PlayerGetMonsterPoints(int level);
void PlayerFireWeapon(Guy *me);
void SetAreaName(world_t *world);
void CallFarley(Guy *me);
void HandlePoison(Guy *me);
byte WeaponCost(byte wpn,byte level);
void PlayerFireUltraWeapon(Guy *me);

void DescribeSave(char* buf, size_t n, const player_t* player);

#endif
