#ifndef ALLUSIONARY_H
#define ALLUSIONARY_H

#include "mgldraw.h"

#define ALL_NAMELEN	(48)
#define ALL_DESCLEN	(600)

#define NUM_ALLUSIONS	(99)

typedef struct allusion_t
{
	char name[ALL_NAMELEN];
	char desc[ALL_DESCLEN];
} allusion_t;

char *All_EntryName(byte n);
char *All_EntryDesc(byte n);

#endif
