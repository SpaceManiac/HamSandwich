#ifndef COSSIN_H
#define COSSIN_H

#include "winpch.h"

void InitCosSin(void);
int Cosine(int angle);
int Sine(int angle);
void Dampen(int *value,int amt);
void Clamp(int *value,int amt);
byte PointInRect(int x,int y,int rx,int ry,int rx2,int ry2);

#endif