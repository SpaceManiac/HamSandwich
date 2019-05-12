#ifndef MUSIC_H
#define MUSIC_H

#include "winpch.h"

// these are CD audio modes for the CDPlayerUpdate
enum {
	CD_OFF = 0,
	CD_LOOPTRACK,	// continuously loop the current track
	CD_INTROLOOP,	// plays the chosen track, then loops the next one
	CD_RANDOM,		// after current track, jump to any other at random
	CD_NORMAL		// just keep playing the tracks in order, loops at end of CD to beginning
};

byte MusicInit(void);
void MusicExit(void);

// CD audio functions
byte CDLoaded(void);
void CDPlay(int track); // Play given track
void CDStop(void);
void CDPlayerUpdate(byte mode);
void CDNeedsUpdating(void);

#endif
