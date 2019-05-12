#ifndef CH_SUMMON_H
#define CH_SUMMON_H

#include "player.h"

void InitSummons(void);
void SummonControlMe(Guy *me,mapTile_t *mapTile,world_t *world);
void UpdateSummons(void);

void GrabSummons(void);
void DeploySummons(Guy *goodguy);
void AddToSummoned(Guy *g);
char *SummonName(byte spl);
byte SummonCost(byte spl);

void RenderSummonLife(MGLDraw *mgl);

#endif
