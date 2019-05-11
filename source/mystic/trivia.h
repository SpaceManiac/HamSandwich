#ifndef TRIVIA_H
#define TRIVIA_H

#include "mgldraw.h"

// trivia conditions
#define TC_NONE		0	// ignore this condition
#define TC_TIMED	1	// execute after N seconds
#define TC_KILL		2	// after killing monster type N
#define TC_VAR		3	// if variable N is set
#define TC_TIMED2	4	// if the subtimer is elapsed (a separate timer which gets started by other trivias)
#define TC_NOTVAR	5	// variable N is not set
#define TC_STEP		6	// step within 2 tiles of N%256,N/256

// trivia effects
#define TE_NONE		0	// no effect
#define TE_VAR		1	// set var N
#define TE_SUBTIME	2	// start the subtimer going with N seconds

typedef struct trivia_t
{
	byte chapter,level;
	byte condition[4];	// all 4 conditions must be met for it to appear
	int condValue[4];
	byte effect[2];		// what effects this has
	int effectVal[2];
	char text[4][48];
} trivia_t;

void InitTrivia(void);
void ExitTrivia(void);
void UpdateTrivia(void);
void RenderTrivia(MGLDraw *mgl);

#endif
