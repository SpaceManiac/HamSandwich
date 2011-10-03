#ifndef JAMULSOUND_H
#define JAMULSOUND_H

#include "winpch.h"
#include "jamultypes.h"

// internal sound playing flags
#define SOUND_LOOP	 1	// loop the sound indefinitely
#define SOUND_CUTOFF 2	// if the copy of the sound is busy, cut it off and restart

// external fun sound playing flags for everyone to use
#define SND_CUTOFF		1	// cut off same sound if needed
#define SND_MAXPRIORITY 2	// ignore priority value, this sound is a must-have
#define SND_ONE			4	// only one copy may play at once
#define SND_PLAYING		8	// well, it's not for everyone, but it goes here
#define SND_FEW			16	// only allow MAX_FEW_SOUNDS copies to play at once

#define MAX_SOUNDS_AT_ONCE	16


#define MAX_SNDPRIORITY	65536

typedef struct sound_t
{
	int dsHandle;	// directsound handle
	int soundNum;	// which game sound number
	int priority;
	long pan,vol;
	byte flags;
} sound_t;



bool JamulSoundInit(HINSTANCE hInst,const char *wndName, int numBuffers);
void JamulSoundExit(void);

// pass in the sound # you wish to play
bool JamulSoundPlay(int which,long pan,long vol,byte playFlags);
// pass in the sound # you're sick of hearing
bool JamulSoundStop(int which);
// if you're REALLY sick of hearing a certain sound
void JamulSoundDestroyBuffer(int which);

bool JamulSoundRewind(int which);
// returns the number of the position in which it stored the sound (later use JamulSoundPlay(n) to play
// sound #n.
// -1 means it failed.
int  JamulSoundLoad(char *fname);

// lets you know if the given sound is currently playing
bool JamulSoundIsPlaying(int which);

// copies the sound buffer specified (doesn't actually make a copy, so you're not
// wasting space).  Returns the index of the copy.
int JamulSoundCopy(int src);

// these two are companion functions.  LoadToMemory loads in a sound, but it does not
// load it into any playable space.  It simply allocates memory and loads the sound into
// the memory in the following format:
//
// dword size;			// size of the sound data itself
// WAVEFORMATEX wfmtx;  // information about the sound that dsound needs
// byte data[size];     // the sound data itself
//
// This is useful in storing many sounds together in one file, or similar actions.
// Then later, you load the single file in, and...
byte *JamulSoundLoadToMemory(char *fname);
// JamulSoundGetFromMemory can be used to obtain a playable sound buffer from
// the sound data pointed to.
int JamulSoundGetFromMemory(byte *buf);


//--------------------------------
// here's the fun and easy sound manager
// it assumes there is a subdirectory "\sounds" that contains snd000.wav - sndXXX.wav,
// for as many sounds as you'll try to play.  It will load them if they aren't in memory already.

// call this fairly often to free up unused buffers, otherwise no new sounds can be played
void JamulSoundUpdate(void);
// call this to wipe the sounds from memory
void JamulSoundPurge(void);
// call this a lot, it plays sounds
void GoPlaySound(int num,long pan,long vol,byte flags,int priority);

#endif
