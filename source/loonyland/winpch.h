#ifndef WINPCH_H
#define WINPCH_H

// this header is just meant to be precompiled for compilation speed reasons.
// mainly for the purposes of making it fast when windows.h is involved.
#define WIN32_LEAN_AND_MEAN
#pragma pack(1)
#pragma warning( disable : 4800 4103 )

#include <windows.h>
#include "jamultypes.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h>

#endif
