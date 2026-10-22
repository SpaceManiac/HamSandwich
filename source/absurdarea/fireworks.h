#ifndef FIREWORKS_H
#define FIREWORKS_H

#include "mgldraw.h"

// types
#define FWORK_NONE		0	// unused
#define FWORK_NORMAL	1	// basic firework
#define FWORK_CRACKLE	3	// sputters out fireworks as it flies
#define FWORK_SHELL	4	// the shell that goes up

// explosion type
#define FE_SINGLE	0	// basic normal explosion, single color
#define FE_RANDOM	1	// random color
#define FE_SHELLS	2	// explodes into more shells
#define FE_DUD		3	// no good
#define FE_CRACKLER	4	// explode into crackle fireworks

#define MAX_FWORK	1024

typedef struct fwork_t
{
	byte type;
	byte exType;

	byte color;
	int	 bright;

	int  power;

	int tag;
	int timer;
	int x,y;
	int dx,dy;
} fwork_t;

void InitFireworks(void);
void ExitFireworks(void);
void LaunchShell(byte rank);
void UpdateFireworks(void);
void RenderFireworks(MGLDraw *mgl);

#endif
