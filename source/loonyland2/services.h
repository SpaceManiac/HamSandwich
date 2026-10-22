#ifndef SERVICES_H
#define SERVICES_H

#include "mgldraw.h"

#define SERVICES		(50)
#define SVC_SHARPEN		(50)
#define SVC_MAGICSMITH	(51)
#define SVC_BALLOON		(52)
#define SVC_ALCHEMY		(53)
#define SVC_ARENA		(54)
#define SVC_ONION		(55)	// make your choice with the onion
#define SVC_JUNKSMITH	(56)
#define SVC_CLOCKWORK	(57)
#define SVC_ALLUS		(58)

void RenderService(MGLDraw *mgl);
byte UpdateService(MGLDraw *mgl);
void InitService(byte type);

void MadcapArena(void);

#endif
