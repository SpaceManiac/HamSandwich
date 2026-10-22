#ifndef WINPCH_H
#define WINPCH_H

// this header is just meant to be precompiled for compilation speed reasons.
// mainly for the purposes of making it fast when windows.h is involved.
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#endif

#include "jamultypes.h"
#include "mgldraw.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const int FIXSHIFT = 16;
const int FIXAMT = 65536;

#endif
