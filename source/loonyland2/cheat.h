#ifndef CHEAT_H
#define CHEAT_H

#include "winpch.h"
#include "sound.h"
#include "display.h"
#include "player.h"
#include "message.h"
#include "bullet.h"
#pragma pack(1)

void InitCheater(void);
void CheatKey(char c);
void DoCheat(byte w);

#endif
