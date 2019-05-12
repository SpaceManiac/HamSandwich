#ifndef WINPCH_H
#define WINPCH_H

// this header is just meant to be precompiled for compilation speed reasons.
// mainly for the purposes of making it fast when windows.h is involved.
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include "jamultypes.h"
#include "jamulsound.h"
#include "control.h"
#include <stdlib.h>
#include "config.h"
#include "debuggo.h"

#endif
