#ifndef CH_LOONY_H
#define CH_LOONY_H

#include "player.h"
#pragma pack(1)

void LoonyControlMe(Guy *me,mapTile_t *mapTile,world_t *world);
void LoonyControlStunned(Guy *me,mapTile_t *mapTile,world_t *world);

void LoonyHitGuyThrow(float factor,word dmg,Guy *me,int n,world_t *world);
void CastSpell(Guy *me);
float SpellDamageBoost(byte fire);
byte CalcShockRate(byte oldRate);

#endif
