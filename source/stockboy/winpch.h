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

const int SCRWID = 640;
const int SCRHEI = 480;
const int FIXSHIFT = 8;
const int FIXAMT = 256;

const int MAX_RUNS = 3;

#endif
