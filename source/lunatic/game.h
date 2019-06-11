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
#include "music.h"
#include "bullet.h"
#include "message.h"
#include "particle.h"
#include "intface.h"
#include "pause.h"
#include "cheat.h"

enum {
    CONTINUE = 0,
    QUITGAME
};

const int TIME_PER_FRAME = 1000 / 30;

enum {
    GAMEMODE_PLAY = 0,
    GAMEMODE_MENU,
    GAMEMODE_PIC,
    GAMEMODE_RAGE
};

// these are the messages you can send to the game
enum {
    MSG_NONE = 0,
    MSG_GOTOMAP,
    MSG_WINLEVEL,
    MSG_RESET,
    MSG_LOADGAME,
    MSG_WINGAME,
    MSG_NEWFEATURE
};

// these are the possible outcomes of a level
// if you die, the level just starts over, so that isn't included
// playing isn't an outcome, it's just used to keep the level going
enum {
    LEVEL_ABORT = 0,
    LEVEL_WIN,
    LEVEL_PLAYING,
    LEVEL_RESET,
    LEVEL_LOADING
};

// these are world outcomes
enum {
    WORLD_ABORT = 5,
    WORLD_WIN,
    WORLD_PLAYING,
    WORLD_LOAD,
    WORLD_QUITGAME
};

extern world_t curWorld;
extern FILE *logFile;

// these are the major inits, just at the beginning and ending of a whole game
void LunaticInit(MGLDraw *mgl);
void LunaticExit(void);

Map* GameCurrentMap(); // replaces extern Map *curMap

// these are the minor inits, called every time you enter a new map
byte InitLevel(byte map);
void ExitLevel(void);

byte GameStartup(void);

void EnterStatusScreen(void);
void EnterRage(void);
void EnterPictureDisplay(void);

byte LunaticRun(int *lastTime);
void LunaticDraw(void);

byte PlayALevel(byte map);

byte LunaticWorld(byte world, const char *worldName);
void LunaticGame(MGLDraw *mgl, byte load);
void TrainingGame(MGLDraw *mgl);

void SendMessageToGame(byte msg, int content);
void AddGarbageTime(dword t);

void SetGameIdle(bool b);
byte GetGameIdle(void);
void GameIdle(void);
byte GetCurSong(void);

byte WorldPauseRun(int *lastTime);
void WorldPauseDraw(void);

#endif
