#ifndef VARS_H
#define VARS_H

#include "mgldraw.h"

#define VAR_GLOBAL	0	// 8 global vars, 0-7
#define VAR_LOCAL	8	// 8 local vars, 8-15

#define NUM_VARS	8	// 8 of each type

char *VarName(int v);
void SetVar(byte v,int value);
int GetVar(byte v);
int GetSpecialVar(char c);
int GetSpecialVarT(char c);

byte CompareVar(byte v,byte flags,int value);
byte VarMath(byte finalV,char *func);

#endif
