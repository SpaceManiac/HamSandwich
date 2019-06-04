#ifndef JAMULSOUND_H
#define JAMULSOUND_H

#include "jamultypes.h"

// external fun sound playing flags for everyone to use
enum {
	SND_CUTOFF = 1,			// cut off same sound if needed
	SND_MAXPRIORITY = 2,	// ignore priority value, this sound is a must-have
	SND_ONE = 4,			// only one copy may play at once
	SND_PLAYING = 8,		// well, it's not for everyone, but it goes here
	SND_LOOPING = 16,
	SND_SUSTAIN = 32,		// only used by instruments
	SND_RANDOM = 64, 		// randomize pitch up or down 10%
	SND_BACKWARDS = 128,
	SND_DOUBLESPEED = 256,
};

const int MAX_SNDPRIORITY = 65536;

bool JamulSoundInit(int numBuffers);
void JamulSoundExit(void);

// --------------------------------
// here's the fun and easy sound manager
// it assumes there is a subdirectory "\sounds" that contains snd000.wav - sndXXX.wav,
// for as many sounds as you'll try to play.  It will load them if they aren't in memory already.

// call this fairly often to free up unused buffers, otherwise no new sounds can be played
void JamulSoundUpdate(void);

// call this to wipe the sounds from memory
void JamulSoundPurge(void);

// call this a lot, it plays sounds
void GoPlaySound(int num, long pan, long vol, int flags, int priority);

// pass in the sound # you're sick of hearing
bool JamulSoundStop(int which);

void JamulSoundVolume(int v);

#endif  // JAMULSOUND_H
