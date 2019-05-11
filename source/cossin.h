#ifndef COSSIN_H
#define COSSIN_H

#include "jamultypes.h"

void InitCosSin(void);
int Cosine(int angle);
int Sine(int angle);
void Dampen(int *value,int amt);
void Clamp(int *value,int amt);

byte AngleFrom(int fromx,int fromy,int tox,int toy);
byte TurnToward(byte faceNow,byte newFace,byte spd);

#endif
