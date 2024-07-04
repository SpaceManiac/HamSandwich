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

// these are the different worlds to play
#define WORLD_NORMAL   0
#define WORLD_SURVIVAL 1
#define WORLD_SLINGSHOT 2
#define WORLD_LOONYBALL 3
#define WORLD_BOWLING	4
#define WORLD_BOSSBASH	5
#define WORLD_INFSURV	6
#define WORLD_REMIX		7
#define WORLD_RANDOMIZER	8

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
void LunaticDraw(void);

TASK(byte) PlayALevel(byte map);

TASK(byte) LunaticWorld(byte world,const char *worldName);
TASK(void) LunaticGame(MGLDraw *mgl,byte load,byte mode);

void SendMessageToGame(byte msg,int content);
void AddGarbageTime(dword t);

byte WindingDown(void);

void UndoWindDown(void);

void PauseGame();

#endif
