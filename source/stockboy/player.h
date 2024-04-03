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
#define COMBO_TIME	10

// initializing constants (pass to InitPlayer)
#define INIT_GAME  2
#define INIT_WORLD 1
#define INIT_LEVEL 0

// player equipment
#define PE_NONE		0
#define PE_BOMB		1
#define PE_KEY		2
#define PE_DETONATE 3
#define PE_ACID		4
#define PE_LASER	5

// sound effects
#define SE_STEP		0
#define SE_PUSH		1
#define SE_BUMP		2
#define SE_DIE		3
#define SE_WIN		4
#define SE_BORED	5

typedef struct player_t
{
	byte item[9];
	int  score;
	int  totalScore;

	int rescueBonus;
	int dumpBonus;
	int deliverBonus;
	int timeBonus;
	int escapeBonus;
	int destructionBonus;
	int pencilBonus;
	dword combo;
	byte  comboClock;

	int blownUp[8];	// how many of each color have been blown up
	int blownUpMons[8][4];	// how many of each type and color monster have been blown up
	int rescueMons[8][4];	// # of rescues of each type and color of monster
	int dumpedMons[8][4];	// how many monsters have been dropped in holes
	int dumpedObj[8];		// how many objects have been dropped in holes
	dword bestCombo;
	byte worldNum;
	byte levelNum;
	int  boredom,maxBoredom;

	byte key[8];

	byte wantBomb;
	int clock,miniClock;
	byte reload;
	byte wpnReload;
	byte pushPower;	// for pushing pushy blocks
	int  invinc;
	int  monsType;
	int  pencils[8];
	int	 deadPencils[8];
	char songName[32];
	byte songNum;
	byte noPushAnim;
} player_t;

extern player_t player;

void InitPlayer(byte initWhat,byte world,byte level);
void ExitPlayer(void);
void PlayerControlMe(Guy *me,Map *map,mapTile_t *mapTile,world_t *world);
byte DropBomb(int x,int y,Map *map);

void DoPlayerFacing(byte c,Guy *me);
void PlayerGetItem(mapTile_t *m,int x,int y);
void PlayerWinLevel(byte w,byte l,byte isSecret);
void SetPlayerGlow(byte v);
byte GetPlayerGlow(void);
byte PlayerPushMore(void);
void SetTportClock(byte tp);
byte GetTportClock(void);
float CalcPercent(player_t *p);
void PlayerGetWeapon(byte wpn);
void PlayerGetMonsterPoints(int level);
void PlayerFireWeapon(Guy *me);
byte PlayerMoveTo(Guy *me,int x,int y);
void PlayerExplode(Map *map);
void PlayerSoundEffect(byte fx);
void SetPlayerNoBomb(void);

#endif