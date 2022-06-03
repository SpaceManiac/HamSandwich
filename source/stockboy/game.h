#ifndef GAME_H
#define GAME_H

#include "winpch.h"
#include "mgldraw.h"
#include "display.h"
#include "clock.h"
#include "monster.h"
#include "sound.h"
#include "guy.h"
#include "player.h"
#include "items.h"
#include "world.h"
#include "bullet.h"
#include "message.h"
#include "particle.h"
#include "intface.h"
#include "pause.h"
#include "cheat.h"

#define CONTINUE 0
#define QUITGAME 1

#define TIME_PER_FRAME (1000/30)

#define GAMEMODE_PLAY  0
#define GAMEMODE_MENU  1
#define GAMEMODE_PIC   2
#define GAMEMODE_CHAT  3

#define GAME_TRAINING	0
#define GAME_BLOWOUT	1
#define GAME_STOCKROOM	2
#define GAME_CLEARANCE	3
#define GAME_PESTCONTROL 4
#define GAME_ADDON		5
#define GAME_PARALLEL	6
#define NUM_GAMES	7

// these are the messages you can send to the game
#define MSG_NONE	  0
#define MSG_GOTOMAP	  1
#define MSG_WINLEVEL  2
#define MSG_RESET	  3
#define MSG_LOADGAME  4
#define MSG_WINGAME	  5
#define MSG_NEWFEATURE 6

// these are the possible outcomes of a level
// if you die, the level just starts over, so that isn't included
// playing isn't an outcome, it's just used to keep the level going
#define LEVEL_ABORT   0
#define LEVEL_WIN	  1
#define LEVEL_PLAYING 2
#define LEVEL_RESET	  3
#define LEVEL_LOADING 4

// these are world outcomes
#define WORLD_ABORT 5
#define WORLD_WIN	6
#define WORLD_PLAYING 7
#define WORLD_LOAD	8
#define WORLD_QUITGAME 9
#define WORLD_MENUAGAIN 10

extern Map	*curMap;
extern world_t curWorld;

// these are the major inits, just at the beginning and ending of a whole game
void LunaticInit(MGLDraw *mgl);
void LunaticExit(void);

// these are the minor inits, called every time you enter a new map
byte InitLevel(byte map);
void ExitLevel(void);

byte GameStartup(void);

void EnterStatusScreen(void);
void EnterPictureDisplay(void);
void EnterChatMode(void);
void ExitChatMode(void);

byte LunaticRun(int *lastTime);
TASK(void) LunaticDraw(void);

TASK(void) TestLevel(byte map);

TASK(byte) PlayALevel(byte map);
byte GameType(void);

TASK(byte) LunaticWorld(byte world,const char *worldName);
TASK(void) LunaticGame(MGLDraw *mgl,byte load,byte mode);

void SendMessageToGame(byte msg,int content);
void AddGarbageTime(dword t);

void SetGameIdle(byte b);
byte GetGameIdle(void);
void GameIdle(void);
byte WindingDown(void);
byte WindingUp(void);
void StopWindingUp(void);
byte TestingLevel(void);
void SetGameMGL(MGLDraw *mgl);

#endif
