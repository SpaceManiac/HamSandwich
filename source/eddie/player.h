#ifndef PLAYER_H
#define PLAYER_H

#include "jamultypes.h"

#define WPN_NONE    0
#define WPN_MISSILE 1
#define WPN_CHAINGUN 2
#define WPN_SHOTGUN 3
#define WPN_LASER   4
#define WPN_NUKE	5

void InitPlayer(void);
void ExitPlayer(void);

void UpdateMouse(void);
void GetMouse(int *x,int *y);

byte GetLives(void);
int GetScore(void);
byte GetPower(void);
void PlayerGetPoints(int p);
byte GetInvincible(void);
void SetInvincible(byte b);
byte PlayerWinLevel(void);
byte GetLevel(void);
byte GetWorld(void);
void PlayerDeathReset(void);
void BoostPower(void);
void UseAmmo(byte amt);
byte GetWeapon(void);
void SetWeapon(byte wpn,byte ammo);
byte GetAmmo(void);
void SetShield(byte v);
byte GetShield(void);
void SetWingmen(byte v);
byte GetWingmen(void);
void PlayerSkipWorld(void);
byte AddLife(char v);
void ContinuePlayer(void);

void UpdatePlayer(void);

#endif